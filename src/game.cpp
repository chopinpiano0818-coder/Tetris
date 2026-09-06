#include "game.hpp"
#include "input.hpp"

#include <algorithm>
#include <chrono>

// ============================================================
// Constructor
// ============================================================

Game::Game(const Settings& settings)
    : mSettings(&settings)
{
    const auto seed =
        static_cast<unsigned>(
            std::chrono::high_resolution_clock::now()
                .time_since_epoch()
                .count()
        );

    mRandom.seed(seed);

    clearBoard();
}

// ============================================================
// Start / Restart
// ============================================================

void Game::start(
    RenderStyle style,
    GameMode mode,
    int stage,
    int playerLevel
)
{
    mStyle = style;
    mMode = mode;
    mStage = stage;
    mPlayerLevel = std::max(1, playerLevel);

    mLastStyle = style;
    mLastMode = mode;
    mLastStage = stage;
    mLastPlayerLevel = mPlayerLevel;

    clearBoard();

    mCurrent = Tetromino();

    mHold = TetrominoType::None;
    mHoldUsed = false;

    mNextQueue.clear();
    mBag.clear();

    mScore = 0;
    mLines = 0;
    mGameLevel = 1;

    mCombo = -1;
    mMaxCombo = 0;
    mTetrisCount = 0;

    mPlayTime = 0.0f;
    mGravityTimer = 0.0f;

    mPaused = false;
    mFinished = false;

    mFinishReason =
        FinishReason::None;

    mRemainingTime = 0.0f;

    if (mMode == GameMode::Runner)
    {
        mRemainingTime = RUNNER_TIME;
    }
    else if (mMode == GameMode::Ultra)
    {
        mRemainingTime = ULTRA_TIME;
    }

    mCamera.reset();

    mLastClearedLines = 0;
    mLineClearAnimation = 0.0f;
    mHardDropAnimation = 0.0f;
    mLandAnimation = 0.0f;

    ensureNextQueue();
    spawnPiece();
}

void Game::restart()
{
    start(
        mLastStyle,
        mLastMode,
        mLastStage,
        mLastPlayerLevel
    );
}

// ============================================================
// Update
// ============================================================

void Game::update(
    float deltaTime,
    InputManager& input
)
{
    if (mFinished)
    {
        return;
    }

    updateAnimations(deltaTime);

    if (mPaused)
    {
        return;
    }

    mPlayTime += deltaTime;

    processInput(
        deltaTime,
        input
    );

    if (mFinished)
    {
        return;
    }

    updateMode(deltaTime);

    if (mFinished)
    {
        return;
    }

    // --------------------------------------------------------
    // Gravity
    // --------------------------------------------------------

    mGravityTimer += deltaTime;

    const float interval =
        gravityInterval();

    while (mGravityTimer >= interval)
    {
        mGravityTimer -= interval;

        if (!tryMove(0, 1))
        {
            lockPiece();
            break;
        }
    }
}

// ============================================================
// Input
// ============================================================

void Game::processInput(
    float deltaTime,
    InputManager& input
)
{
    // 左右はInputManager側でDAS/ARR処理
    if (input.moveLeftTriggered())
    {
        tryMove(-1, 0);
    }

    if (input.moveRightTriggered())
    {
        tryMove(1, 0);
    }

    if (input.softDropTriggered())
    {
        if (tryMove(0, 1))
        {
            // 手動ソフトドロップには少量加点
            mScore += 1;
        }
    }

    // L
    if (input.rotateLeftPressed())
    {
        tryRotate(-1);
    }

    // R
    if (input.rotateRightPressed())
    {
        tryRotate(1);
    }

    // A = 180度回転
    if (input.flipPressed())
    {
        tryFlip();
    }

    // B
    if (input.hardDropPressed())
    {
        hardDrop();
    }

    // X
    if (input.holdPressed())
    {
        hold();
    }

    // --------------------------------------------------------
    // 3D camera
    // --------------------------------------------------------

    if (mStyle == RenderStyle::ThreeD)
    {
        const float x =
            input.cameraX();

        const float y =
            input.cameraY();

        mCamera.yaw +=
            x *
            mSettings->cameraSensitivity *
            deltaTime;

        mCamera.pitch +=
            y *
            mSettings->cameraSensitivity *
            deltaTime;

        mCamera.yaw =
            std::clamp(
                mCamera.yaw,
                mSettings->cameraMinYaw,
                mSettings->cameraMaxYaw
            );

        mCamera.pitch =
            std::clamp(
                mCamera.pitch,
                mSettings->cameraMinPitch,
                mSettings->cameraMaxPitch
            );

        // R3
        if (input.cameraResetPressed())
        {
            mCamera.reset();
        }
    }
}

// ============================================================
// Board
// ============================================================

void Game::clearBoard()
{
    for (auto& row : mBoard)
    {
        row.fill(
            TetrominoType::None
        );
    }
}

const Game::Board& Game::board() const
{
    return mBoard;
}

// ============================================================
// Spawn
// ============================================================

void Game::spawnPiece()
{
    ensureNextQueue();

    const TetrominoType type =
        mNextQueue.front();

    mNextQueue.pop_front();

    ensureNextQueue();

    mCurrent.setType(type);
    mCurrent.setPosition(3, 0);
    mCurrent.setRotation(0);

    mHoldUsed = false;

    if (!canPlace(
            mCurrent,
            mCurrent.x(),
            mCurrent.y(),
            mCurrent.rotation()
        ))
    {
        finish(
            FinishReason::GameOver
        );
    }
}

// ============================================================
// Collision
// ============================================================

bool Game::canPlace(
    const Tetromino& piece,
    int x,
    int y,
    int rotation
) const
{
    const auto& cells =
        piece.cellsAt(rotation);

    for (const auto& cell : cells)
    {
        const int boardX =
            x + cell.x;

        const int boardY =
            y + cell.y;

        if (boardX < 0 ||
            boardX >= BOARD_WIDTH)
        {
            return false;
        }

        if (boardY >= BOARD_HEIGHT)
        {
            return false;
        }

        // 盤面より上は許可
        if (boardY < 0)
        {
            continue;
        }

        if (mBoard[boardY][boardX] !=
            TetrominoType::None)
        {
            return false;
        }
    }

    return true;
}

// ============================================================
// Movement
// ============================================================

bool Game::tryMove(
    int dx,
    int dy
)
{
    const int newX =
        mCurrent.x() + dx;

    const int newY =
        mCurrent.y() + dy;

    if (!canPlace(
            mCurrent,
            newX,
            newY,
            mCurrent.rotation()
        ))
    {
        return false;
    }

    mCurrent.move(dx, dy);

    return true;
}

// ============================================================
// Rotation
// ============================================================

bool Game::tryRotate(
    int direction
)
{
    int newRotation =
        mCurrent.rotation() + direction;

    newRotation %= 4;

    if (newRotation < 0)
    {
        newRotation += 4;
    }

    // --------------------------------------------------------
    // 簡易Wall Kick
    //
    // 中央
    // 左1
    // 右1
    // 左2
    // 右2
    // 上1
    // --------------------------------------------------------

    static constexpr int kicks[][2] =
    {
        { 0,  0 },
        {-1,  0 },
        { 1,  0 },
        {-2,  0 },
        { 2,  0 },
        { 0, -1 }
    };

    for (const auto& kick : kicks)
    {
        const int newX =
            mCurrent.x() + kick[0];

        const int newY =
            mCurrent.y() + kick[1];

        if (canPlace(
                mCurrent,
                newX,
                newY,
                newRotation
            ))
        {
            mCurrent.setPosition(
                newX,
                newY
            );

            mCurrent.setRotation(
                newRotation
            );

            return true;
        }
    }

    return false;
}

// ============================================================
// A button = 180°
// ============================================================

bool Game::tryFlip()
{
    const int newRotation =
        (mCurrent.rotation() + 2) % 4;

    static constexpr int kicks[][2] =
    {
        { 0,  0 },
        {-1,  0 },
        { 1,  0 },
        {-2,  0 },
        { 2,  0 },
        { 0, -1 },
        { 0, -2 }
    };

    for (const auto& kick : kicks)
    {
        const int newX =
            mCurrent.x() + kick[0];

        const int newY =
            mCurrent.y() + kick[1];

        if (canPlace(
                mCurrent,
                newX,
                newY,
                newRotation
            ))
        {
            mCurrent.setPosition(
                newX,
                newY
            );

            mCurrent.setRotation(
                newRotation
            );

            return true;
        }
    }

    return false;
}

// ============================================================
// Hard Drop
// ============================================================

void Game::hardDrop()
{
    int distance = 0;

    while (tryMove(0, 1))
    {
        ++distance;
    }

    mScore +=
        distance * 2;

    mHardDropAnimation = 1.0f;

    lockPiece();
}

// ============================================================
// Ghost
// ============================================================

int Game::ghostY() const
{
    int y =
        mCurrent.y();

    while (canPlace(
        mCurrent,
        mCurrent.x(),
        y + 1,
        mCurrent.rotation()
    ))
    {
        ++y;
    }

    return y;
}

// ============================================================
// HOLD
// ============================================================

void Game::hold()
{
    if (mHoldUsed ||
        mFinished)
    {
        return;
    }

    const TetrominoType currentType =
        mCurrent.type();

    if (mHold ==
        TetrominoType::None)
    {
        mHold =
            currentType;

        ensureNextQueue();

        const TetrominoType next =
            mNextQueue.front();

        mNextQueue.pop_front();

        ensureNextQueue();

        mCurrent.setType(next);
    }
    else
    {
        const TetrominoType oldHold =
            mHold;

        mHold =
            currentType;

        mCurrent.setType(
            oldHold
        );
    }

    mCurrent.setPosition(3, 0);
    mCurrent.setRotation(0);

    mHoldUsed = true;

    if (!canPlace(
            mCurrent,
            mCurrent.x(),
            mCurrent.y(),
            mCurrent.rotation()
        ))
    {
        finish(
            FinishReason::GameOver
        );
    }
}

// ============================================================
// Lock
// ============================================================

void Game::lockPiece()
{
    const auto& cells =
        mCurrent.cells();

    bool aboveBoard = false;

    for (const auto& cell : cells)
    {
        const int x =
            mCurrent.x() + cell.x;

        const int y =
            mCurrent.y() + cell.y;

        if (y < 0)
        {
            aboveBoard = true;
            continue;
        }

        if (x >= 0 &&
            x < BOARD_WIDTH &&
            y >= 0 &&
            y < BOARD_HEIGHT)
        {
            mBoard[y][x] =
                mCurrent.type();
        }
    }

    mLandAnimation = 1.0f;

    if (aboveBoard)
    {
        finish(
            FinishReason::GameOver
        );

        return;
    }

    const int cleared =
        clearLines();

    applyLineScore(cleared);

    // Sprint
    if (mMode == GameMode::Sprint &&
        mLines >= SPRINT_LINES)
    {
        finish(
            FinishReason::SprintClear
        );

        return;
    }

    spawnPiece();
}

// ============================================================
// Clear Lines
// ============================================================

int Game::clearLines()
{
    int cleared = 0;

    for (int y =
            BOARD_HEIGHT - 1;
         y >= 0;)
    {
        bool full = true;

        for (int x = 0;
             x < BOARD_WIDTH;
             ++x)
        {
            if (mBoard[y][x] ==
                TetrominoType::None)
            {
                full = false;
                break;
            }
        }

        if (!full)
        {
            --y;
            continue;
        }

        ++cleared;

        // 上の行を全部1段下へ
        for (int moveY = y;
             moveY > 0;
             --moveY)
        {
            mBoard[moveY] =
                mBoard[moveY - 1];
        }

        mBoard[0].fill(
            TetrominoType::None
        );

        // yを減らさない。
        // 下がってきた同じ行番号を再チェックする。
    }

    mLastClearedLines =
        cleared;

    if (cleared > 0)
    {
        mLineClearAnimation =
            1.0f;
    }

    return cleared;
}

// ============================================================
// Scoring
// ============================================================

void Game::applyLineScore(
    int cleared
)
{
    if (cleared <= 0)
    {
        mCombo = -1;
        return;
    }

    ++mCombo;

    mMaxCombo =
        std::max(
            mMaxCombo,
            mCombo
        );

    int baseScore = 0;

    switch (cleared)
    {
        case 1:
            baseScore = 100;
            break;

        case 2:
            baseScore = 300;
            break;

        case 3:
            baseScore = 500;
            break;

        case 4:
            baseScore = 800;
            ++mTetrisCount;
            break;

        default:
            break;
    }

    mScore +=
        baseScore *
        mGameLevel;

    // Combo bonus
    if (mCombo > 0)
    {
        mScore +=
            50 *
            mCombo *
            mGameLevel;
    }

    mLines +=
        cleared;

    // 10ラインごとにゲーム内Lv UP
    mGameLevel =
        1 + (mLines / 10);
}

// ============================================================
// 7-Bag
// ============================================================

void Game::refillBag()
{
    mBag =
    {
        TetrominoType::I,
        TetrominoType::O,
        TetrominoType::T,
        TetrominoType::S,
        TetrominoType::Z,
        TetrominoType::J,
        TetrominoType::L
    };

    std::shuffle(
        mBag.begin(),
        mBag.end(),
        mRandom
    );
}

TetrominoType Game::takeNextPiece()
{
    if (mBag.empty())
    {
        refillBag();
    }

    const TetrominoType type =
        mBag.back();

    mBag.pop_back();

    return type;
}

void Game::ensureNextQueue()
{
    while (
        static_cast<int>(
            mNextQueue.size()
        ) < NEXT_COUNT + 1
    )
    {
        mNextQueue.push_back(
            takeNextPiece()
        );
    }
}

// ============================================================
// Modes
// ============================================================

void Game::updateMode(
    float deltaTime
)
{
    switch (mMode)
    {
        case GameMode::Endless:
        {
            break;
        }

        case GameMode::Runner:
        {
            mRemainingTime -=
                deltaTime;

            if (mRemainingTime <= 0.0f)
            {
                mRemainingTime =
                    0.0f;

                finish(
                    FinishReason::TimeUp
                );
            }

            break;
        }

        case GameMode::Sprint:
        {
            // ライン達成判定はlockPiece()
            break;
        }

        case GameMode::Ultra:
        {
            mRemainingTime -=
                deltaTime;

            if (mRemainingTime <= 0.0f)
            {
                mRemainingTime =
                    0.0f;

                finish(
                    FinishReason::TimeUp
                );
            }

            break;
        }
    }
}

// ============================================================
// Gravity
// ============================================================

float Game::gravityInterval() const
{
    // Lv1 = 約0.8秒
    // Lvが上がるほど速くなる
    float interval =
        0.80f -
        static_cast<float>(
            mGameLevel - 1
        ) * 0.055f;

    return std::max(
        0.06f,
        interval
    );
}

// ============================================================
// Finish
// ============================================================

void Game::finish(
    FinishReason reason
)
{
    if (mFinished)
    {
        return;
    }

    mFinished = true;
    mFinishReason = reason;
}

// ============================================================
// Animation timers
// ============================================================

void Game::updateAnimations(
    float deltaTime
)
{
    constexpr float SPEED =
        4.0f;

    mLineClearAnimation =
        std::max(
            0.0f,
            mLineClearAnimation -
            deltaTime * SPEED
        );

    mHardDropAnimation =
        std::max(
            0.0f,
            mHardDropAnimation -
            deltaTime * 6.0f
        );

    mLandAnimation =
        std::max(
            0.0f,
            mLandAnimation -
            deltaTime * 7.0f
        );
}

// ============================================================
// Pause
// ============================================================

void Game::setPaused(
    bool paused
)
{
    mPaused = paused;
}

bool Game::isPaused() const
{
    return mPaused;
}

// ============================================================
// Result
// ============================================================

bool Game::hasFinished() const
{
    return mFinished;
}

RunResult Game::takeResult() const
{
    RunResult result;

    result.mode =
        mMode;

    result.style =
        mStyle;

    result.stage =
        mStage;

    result.score =
        mScore;

    result.lines =
        mLines;

    result.gameLevel =
        mGameLevel;

    result.playTime =
        mPlayTime;

    result.tetrises =
        mTetrisCount;

    result.maxCombo =
        mMaxCombo;

    result.finishReason =
        mFinishReason;

    // XP計算
    //
    // ライン
    // スコア
    // TETRIS
    // Combo
    // 3Dボーナス
    int xp =
        mLines * 10;

    xp +=
        mScore / 100;

    xp +=
        mTetrisCount * 40;

    xp +=
        mMaxCombo * 10;

    if (mStyle ==
        RenderStyle::ThreeD)
    {
        xp =
            static_cast<int>(
                xp * 1.10f
            );
    }

    if (mMode ==
        GameMode::Sprint &&
        mFinishReason ==
        FinishReason::SprintClear)
    {
        xp += 250;
    }

    result.earnedXP =
        std::max(0, xp);

    return result;
}

// ============================================================
// Getters
// ============================================================

const Tetromino&
Game::currentPiece() const
{
    return mCurrent;
}

TetrominoType
Game::holdPiece() const
{
    return mHold;
}

bool Game::hasHoldPiece() const
{
    return
        mHold !=
        TetrominoType::None;
}

const std::deque<TetrominoType>&
Game::nextQueue() const
{
    return mNextQueue;
}

int Game::score() const
{
    return mScore;
}

int Game::lines() const
{
    return mLines;
}

int Game::gameLevel() const
{
    return mGameLevel;
}

int Game::combo() const
{
    return mCombo;
}

int Game::maxCombo() const
{
    return mMaxCombo;
}

int Game::tetrisCount() const
{
    return mTetrisCount;
}

float Game::playTime() const
{
    return mPlayTime;
}

float Game::remainingTime() const
{
    return mRemainingTime;
}

RenderStyle Game::renderStyle() const
{
    return mStyle;
}

GameMode Game::gameMode() const
{
    return mMode;
}

int Game::stage() const
{
    return mStage;
}

const Camera3D&
Game::camera() const
{
    return mCamera;
}

Camera3D&
Game::camera()
{
    return mCamera;
}

int Game::lastClearedLines() const
{
    return mLastClearedLines;
}

float Game::lineClearAnimation() const
{
    return mLineClearAnimation;
}

float Game::hardDropAnimation() const
{
    return mHardDropAnimation;
}

float Game::landAnimation() const
{
    return mLandAnimation;
}
