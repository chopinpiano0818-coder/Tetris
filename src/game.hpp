#pragma once

#include <array>
#include <deque>
#include <random>
#include <vector>

#include "types.hpp"
#include "settings.hpp"
#include "tetromino.hpp"

// 前方宣言
class InputManager;

// ============================================================
// TETRIS COMPLETE
// game.hpp
//
// ゲーム本体
//
// ・10×20盤面
// ・7-Bag
// ・NEXT
// ・HOLD
// ・Ghost
// ・移動 / 回転 / Hard Drop
// ・ライン消去
// ・Score / Combo / Level
// ・Endless / Runner / Sprint / Ultra
// ============================================================

class Game
{
public:
    static constexpr int BOARD_WIDTH  = 10;
    static constexpr int BOARD_HEIGHT = 20;

    // NEXTに表示する個数
    static constexpr int NEXT_COUNT = 5;

    using Board =
        std::array<
            std::array<TetrominoType, BOARD_WIDTH>,
            BOARD_HEIGHT
        >;

    explicit Game(
        const Settings& settings
    );

    // --------------------------------------------------------
    // Game lifecycle
    // --------------------------------------------------------

    void start(
        RenderStyle style,
        GameMode mode,
        int stage,
        int playerLevel
    );

    void restart();

    void update(
        float deltaTime,
        InputManager& input
    );

    // --------------------------------------------------------
    // Pause
    // --------------------------------------------------------

    void setPaused(bool paused);
    bool isPaused() const;

    // --------------------------------------------------------
    // Result
    // --------------------------------------------------------

    bool hasFinished() const;

    RunResult takeResult() const;

    // --------------------------------------------------------
    // Board
    // --------------------------------------------------------

    const Board& board() const;

    // --------------------------------------------------------
    // Current piece
    // --------------------------------------------------------

    const Tetromino& currentPiece() const;

    // ゴーストのY座標
    int ghostY() const;

    // --------------------------------------------------------
    // HOLD
    // --------------------------------------------------------

    TetrominoType holdPiece() const;

    bool hasHoldPiece() const;

    // --------------------------------------------------------
    // NEXT
    // --------------------------------------------------------

    const std::deque<TetrominoType>&
    nextQueue() const;

    // --------------------------------------------------------
    // Game information
    // --------------------------------------------------------

    int score() const;
    int lines() const;
    int gameLevel() const;

    int combo() const;
    int maxCombo() const;

    int tetrisCount() const;

    float playTime() const;
    float remainingTime() const;

    RenderStyle renderStyle() const;
    GameMode gameMode() const;

    int stage() const;

    // --------------------------------------------------------
    // 3D Camera
    // --------------------------------------------------------

    const Camera3D& camera() const;
    Camera3D& camera();

    // --------------------------------------------------------
    // Animation information
    // renderer / animation.cpp から利用
    // --------------------------------------------------------

    int lastClearedLines() const;

    float lineClearAnimation() const;
    float hardDropAnimation() const;
    float landAnimation() const;

private:
    // ========================================================
    // Core
    // ========================================================

    void clearBoard();

    void spawnPiece();

    void lockPiece();

    bool canPlace(
        const Tetromino& piece,
        int x,
        int y,
        int rotation
    ) const;

    bool tryMove(
        int dx,
        int dy
    );

    bool tryRotate(
        int direction
    );

    bool tryFlip();

    void hardDrop();

    void hold();

    // --------------------------------------------------------
    // Lines
    // --------------------------------------------------------

    int clearLines();

    void applyLineScore(
        int cleared
    );

    // --------------------------------------------------------
    // Piece generation
    // --------------------------------------------------------

    void refillBag();

    TetrominoType takeNextPiece();

    void ensureNextQueue();

    // --------------------------------------------------------
    // Modes
    // --------------------------------------------------------

    void updateMode(
        float deltaTime
    );

    void finish(
        FinishReason reason
    );

    // --------------------------------------------------------
    // Input
    // --------------------------------------------------------

    void processInput(
        float deltaTime,
        InputManager& input
    );

    // --------------------------------------------------------
    // Gravity
    // --------------------------------------------------------

    float gravityInterval() const;

    // --------------------------------------------------------
    // Animation timers
    // --------------------------------------------------------

    void updateAnimations(
        float deltaTime
    );

private:
    // ========================================================
    // References
    // ========================================================

    const Settings* mSettings = nullptr;

    // ========================================================
    // Board
    // ========================================================

    Board mBoard{};

    // ========================================================
    // Pieces
    // ========================================================

    Tetromino mCurrent;

    TetrominoType mHold =
        TetrominoType::None;

    bool mHoldUsed = false;

    std::deque<TetrominoType>
        mNextQueue;

    std::vector<TetrominoType>
        mBag;

    std::mt19937 mRandom;

    // ========================================================
    // Game settings
    // ========================================================

    RenderStyle mStyle =
        RenderStyle::TwoD;

    GameMode mMode =
        GameMode::Endless;

    int mStage = 0;

    int mPlayerLevel = 1;

    // ========================================================
    // Score
    // ========================================================

    int mScore = 0;
    int mLines = 0;
    int mGameLevel = 1;

    int mCombo = -1;
    int mMaxCombo = 0;

    int mTetrisCount = 0;

    // ========================================================
    // Time
    // ========================================================

    float mPlayTime = 0.0f;

    float mRemainingTime = 0.0f;

    float mGravityTimer = 0.0f;

    // Runnerの初期制限時間
    static constexpr float RUNNER_TIME =
        120.0f;

    // Ultra
    static constexpr float ULTRA_TIME =
        180.0f;

    // Sprint
    static constexpr int SPRINT_LINES =
        40;

    // ========================================================
    // State
    // ========================================================

    bool mPaused = false;
    bool mFinished = false;

    FinishReason mFinishReason =
        FinishReason::None;

    // ========================================================
    // Camera
    // ========================================================

    Camera3D mCamera;

    // ========================================================
    // Animation state
    // ========================================================

    int mLastClearedLines = 0;

    float mLineClearAnimation = 0.0f;
    float mHardDropAnimation = 0.0f;
    float mLandAnimation = 0.0f;

    // ========================================================
    // Restart information
    // ========================================================

    RenderStyle mLastStyle =
        RenderStyle::TwoD;

    GameMode mLastMode =
        GameMode::Endless;

    int mLastStage = 0;
    int mLastPlayerLevel = 1;
};
