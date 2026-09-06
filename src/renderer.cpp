#include "renderer.hpp"

#include "game.hpp"
#include "ui.hpp"
#include "progression.hpp"
#include "stages.hpp"
#include "tetromino.hpp"
#include "renderer3d.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

// ============================================================
// 日本語フォント
// ============================================================

static constexpr const char* FONT_PATH =
    "/usr/share/fonts/opentype/noto/NotoSansCJK-Bold.ttc";

// ============================================================
// Constructor
// ============================================================

Renderer::Renderer(
    SDL_Window* window,
    SDL_Renderer* renderer,
    Settings& settings
)
    : mWindow(window),
      mRenderer(renderer),
      mSettings(&settings)
{
    mFontSmall =
        TTF_OpenFont(FONT_PATH, 18);

    mFontMedium =
        TTF_OpenFont(FONT_PATH, 26);

    mFontLarge =
        TTF_OpenFont(FONT_PATH, 40);

    mFontTitle =
        TTF_OpenFont(FONT_PATH, 68);

    applySettings(settings);
}

// ============================================================
// Destructor
// ============================================================

Renderer::~Renderer()
{
    if (mFontSmall)
        TTF_CloseFont(mFontSmall);

    if (mFontMedium)
        TTF_CloseFont(mFontMedium);

    if (mFontLarge)
        TTF_CloseFont(mFontLarge);

    if (mFontTitle)
        TTF_CloseFont(mFontTitle);
}

// ============================================================
// Settings
// ============================================================

void Renderer::applySettings(
    const Settings& settings
)
{
    mSettings =
        const_cast<Settings*>(&settings);

    if (
        mFullscreenApplied !=
        settings.fullscreen
    )
    {
        SDL_SetWindowFullscreen(
            mWindow,
            settings.fullscreen
                ? SDL_WINDOW_FULLSCREEN_DESKTOP
                : 0
        );

        mFullscreenApplied =
            settings.fullscreen;
    }
}

// ============================================================
// Frame
// ============================================================

void Renderer::beginFrame()
{
    SDL_SetRenderDrawColor(
        mRenderer,
        5, 6, 12, 255
    );

    SDL_RenderClear(
        mRenderer
    );
}

void Renderer::endFrame()
{
    SDL_RenderPresent(
        mRenderer
    );
}

// ============================================================
// Main render
// ============================================================

void Renderer::render(
    AppState state,
    RenderStyle selectedStyle,
    GameMode selectedMode,
    int selectedStage,
    const Game& game,
    const UI& ui,
    const Progression& progression,
    const StageManager& stageManager
)
{
    switch (state)
    {
        case AppState::Home:
            renderHome(
                selectedStyle,
                ui,
                progression
            );
            break;

        case AppState::ModeSelect:
            renderModeSelect(
                selectedMode,
                ui
            );
            break;

        case AppState::StageSelect:
            renderStageSelect(
                selectedStage,
                ui,
                stageManager,
                progression
            );
            break;

        case AppState::Playing:
            renderPlaying(game);
            break;

        case AppState::Result:
            renderResult(ui);
            break;

        case AppState::Settings:
            renderSettings(ui);
            break;
    }
}

// ============================================================
// HOME
// ============================================================

void Renderer::renderHome(
    RenderStyle,
    const UI& ui,
    const Progression& progression
)
{
    int w, h;
    getOutputSize(w, h);

    renderStageBackground(0);

    const float intro =
        ui.titleAnimation();

    const int titleY =
        static_cast<int>(
            85 -
            (1.0f - intro) *
            65.0f
        );

    drawText(
        "テトリス",
        w / 2,
        titleY,
        68,
        true
    );

    drawText(
        "COMPLETE",
        w / 2,
        titleY + 72,
        26,
        true,
        220
    );

    std::ostringstream levelText;

    levelText
        << "プレイヤーレベル "
        << progression.playerLevel()
        << "   XP "
        << progression.currentXP()
        << "/"
        << progression.xpForNextLevel();

    drawText(
        levelText.str(),
        w / 2,
        205,
        18,
        true
    );

    const char* items[] =
    {
        "2Dで遊ぶ",
        "3Dで遊ぶ",
        "設定",
        "終了"
    };

    for (int i = 0; i < 4; ++i)
    {
        drawMenuItem(
            items[i],
            w / 2,
            300 + i * 65,
            ui.homeSelection() == i,
            ui.selectionPulse()
        );
    }

    drawText(
        "十字キー / 左スティック：移動   A / Enter：決定",
        w / 2,
        h - 45,
        18,
        true,
        170
    );
}

// ============================================================
// MODE SELECT
// ============================================================

void Renderer::renderModeSelect(
    GameMode,
    const UI& ui
)
{
    int w, h;
    getOutputSize(w, h);

    renderStageBackground(1);

    drawText(
        "モード選択",
        w / 2,
        65,
        40,
        true
    );

    const char* modes[] =
    {
        "エンドレス",
        "ランナー",
        "スプリント",
        "ウルトラ"
    };

    const char* descriptions[] =
    {
        "ゲームオーバーまで挑戦",
        "120秒で高得点を狙う",
        "40ライン消去でクリア",
        "180秒のスコアアタック"
    };

    for (int i = 0; i < 4; ++i)
    {
        const int y =
            180 + i * 105;

        const bool selected =
            ui.modeSelection() == i;

        if (selected)
        {
            drawPanel(
                w / 2 - 260,
                y - 15,
                520,
                80,
                220
            );
        }

        drawText(
            modes[i],
            w / 2,
            y,
            26,
            true
        );

        drawText(
            descriptions[i],
            w / 2,
            y + 38,
            18,
            true,
            selected ? 235 : 150
        );
    }

    drawText(
        "B / ESC：戻る",
        w / 2,
        h - 40,
        18,
        true,
        160
    );
}

// ============================================================
// STAGE SELECT
// ============================================================

void Renderer::renderStageSelect(
    int,
    const UI& ui,
    const StageManager& stageManager,
    const Progression& progression
)
{
    int w, h;
    getOutputSize(w, h);

    const int selected =
        ui.stageSelection();

    renderStageBackground(selected);

    drawText(
        "ステージ選択",
        w / 2,
        45,
        40,
        true
    );

    const auto& stages =
        stageManager.stages();

    constexpr int CARD_W = 200;
    constexpr int CARD_H = 105;
    constexpr int GAP = 22;

    const int columns = 3;

    const int totalWidth =
        CARD_W * columns +
        GAP * (columns - 1);

    const int startX =
        (w - totalWidth) / 2;

    for (
        int i = 0;
        i < static_cast<int>(
            stages.size()
        );
        ++i
    )
    {
        const int col =
            i % columns;

        const int row =
            i / columns;

        const int x =
            startX +
            col *
            (CARD_W + GAP);

        const int y =
            130 +
            row *
            (CARD_H + GAP);

        const bool isSelected =
            i == selected;

        drawPanel(
            x,
            y,
            CARD_W,
            CARD_H,
            isSelected ? 235 : 165
        );

        if (
            stages[i].unlocked
        )
        {
            drawText(
                stages[i].name,
                x + CARD_W / 2,
                y + 22,
                18,
                true
            );

            drawText(
                "解放済み",
                x + CARD_W / 2,
                y + 63,
                18,
                true,
                190
            );
        }
        else
        {
            drawText(
                "未解放",
                x + CARD_W / 2,
                y + 20,
                18,
                true,
                150
            );

            drawText(
                "必要レベル " +
                std::to_string(
                    stages[i].requiredLevel
                ),
                x + CARD_W / 2,
                y + 62,
                18,
                true,
                150
            );
        }
    }

    drawText(
        "現在のプレイヤーレベル "
        + std::to_string(
            progression.playerLevel()
        ),
        w / 2,
        h - 42,
        18,
        true
    );
}

// ============================================================
// PLAYING
// ============================================================

void Renderer::renderPlaying(
    const Game& game
)
{
    renderStageBackground(
        game.stage()
    );

    if (
        game.renderStyle() ==
        RenderStyle::ThreeD
    )
    {
        renderGame3D(
            mRenderer,
            game,
            *mSettings
        );

        renderSidePanel(game);

        return;
    }

    renderBoard2D(game);
    renderSidePanel(game);
}

// ============================================================
// 2D BOARD
// ============================================================

void Renderer::renderBoard2D(
    const Game& game
)
{
    int w, h;
    getOutputSize(w, h);

    const int maxCellH =
        (h - 70) /
        Game::BOARD_HEIGHT;

    const int cell =
        std::max(
            16,
            std::min(
                32,
                maxCellH
            )
        );

    const int boardW =
        Game::BOARD_WIDTH *
        cell;

    const int boardH =
        Game::BOARD_HEIGHT *
        cell;

    const int originX =
        w / 2 -
        boardW / 2;

    const int originY =
        (h - boardH) / 2;

    SDL_SetRenderDrawColor(
        mRenderer,
        3, 5, 12, 230
    );

    SDL_Rect background
    {
        originX - 5,
        originY - 5,
        boardW + 10,
        boardH + 10
    };

    SDL_RenderFillRect(
        mRenderer,
        &background
    );

    // 外枠
    SDL_SetRenderDrawColor(
        mRenderer,
        180, 190, 230, 230
    );

    SDL_RenderDrawRect(
        mRenderer,
        &background
    );

    // Grid
    SDL_SetRenderDrawColor(
        mRenderer,
        90, 100, 130, 70
    );

    for (
        int x = 0;
        x <= Game::BOARD_WIDTH;
        ++x
    )
    {
        SDL_RenderDrawLine(
            mRenderer,
            originX + x * cell,
            originY,
            originX + x * cell,
            originY + boardH
        );
    }

    for (
        int y = 0;
        y <= Game::BOARD_HEIGHT;
        ++y
    )
    {
        SDL_RenderDrawLine(
            mRenderer,
            originX,
            originY + y * cell,
            originX + boardW,
            originY + y * cell
        );
    }

    const auto& board =
        game.board();

    for (
        int y = 0;
        y < Game::BOARD_HEIGHT;
        ++y
    )
    {
        for (
            int x = 0;
            x < Game::BOARD_WIDTH;
            ++x
        )
        {
            if (
                board[y][x] !=
                TetrominoType::None
            )
            {
                drawBlock(
                    originX + x * cell,
                    originY + y * cell,
                    cell,
                    board[y][x]
                );
            }
        }
    }

    if (
        mSettings->showGhost
    )
    {
        const Tetromino& piece =
            game.currentPiece();

        const int ghost =
            game.ghostY();

        for (
            const auto& block :
            piece.cells()
        )
        {
            const int x =
                piece.x() +
                block.x;

            const int y =
                ghost +
                block.y;

            if (y >= 0)
            {
                drawBlock(
                    originX + x * cell,
                    originY + y * cell,
                    cell,
                    piece.type(),
                    65,
                    true
                );
            }
        }
    }

    const Tetromino& piece =
        game.currentPiece();

    for (
        const auto& block :
        piece.cells()
    )
    {
        const int x =
            piece.x() +
            block.x;

        const int y =
            piece.y() +
            block.y;

        if (y >= 0)
        {
            drawBlock(
                originX + x * cell,
                originY + y * cell,
                cell,
                piece.type()
            );
        }
    }

    if (
        mSettings->lineClearFlash &&
        game.lineClearAnimation() > 0.0f
    )
    {
        const Uint8 alpha =
            static_cast<Uint8>(
                100.0f *
                game.lineClearAnimation() *
                mSettings->animationIntensity
            );

        SDL_SetRenderDrawColor(
            mRenderer,
            255, 255, 255,
            alpha
        );

        SDL_Rect flash
        {
            originX,
            originY,
            boardW,
            boardH
        };

        SDL_RenderFillRect(
            mRenderer,
            &flash
        );
    }
}

// ============================================================
// SIDE PANEL
// ============================================================

void Renderer::renderSidePanel(
    const Game& game
)
{
    int w, h;
    getOutputSize(w, h);

    const int panelX =
        std::min(
            w - 230,
            w / 2 + 210
        );

    drawPanel(
        panelX,
        45,
        200,
        h - 90,
        190
    );

    drawText(
        "スコア",
        panelX + 20,
        70,
        18
    );

    drawText(
        std::to_string(
            game.score()
        ),
        panelX + 20,
        98,
        26
    );

    drawText(
        "ライン  " +
        std::to_string(
            game.lines()
        ),
        panelX + 20,
        150,
        18
    );

    drawText(
        "レベル  " +
        std::to_string(
            game.gameLevel()
        ),
        panelX + 20,
        180,
        18
    );

    if (
        game.gameMode() ==
            GameMode::Runner ||
        game.gameMode() ==
            GameMode::Ultra
    )
    {
        std::ostringstream timer;

        timer
            << std::fixed
            << std::setprecision(1)
            << game.remainingTime();

        drawText(
            "残り時間",
            panelX + 20,
            220,
            18
        );

        drawText(
            timer.str(),
            panelX + 20,
            248,
            26
        );
    }
    else if (
        game.gameMode() ==
        GameMode::Sprint
    )
    {
        drawText(
            "目標 40ライン",
            panelX + 20,
            225,
            18
        );
    }

    if (
        mSettings->showHold
    )
    {
        drawText(
            "ホールド",
            panelX + 20,
            300,
            18
        );

        if (
            game.hasHoldPiece()
        )
        {
            renderMiniPiece(
                game.holdPiece(),
                panelX + 100,
                355,
                18
            );
        }
    }

    if (
        mSettings->showNext
    )
    {
        drawText(
            "ネクスト",
            panelX + 20,
            405,
            18
        );

        const auto& queue =
            game.nextQueue();

        int index = 0;

        for (
            TetrominoType type :
            queue
        )
        {
            if (
                index >=
                Game::NEXT_COUNT
            )
            {
                break;
            }

            renderMiniPiece(
                type,
                panelX + 100,
                455 + index * 55,
                12
            );

            ++index;
        }
    }

    if (
        game.isPaused()
    )
    {
        drawPanel(
            w / 2 - 170,
            h / 2 - 65,
            340,
            130,
            240
        );

        drawText(
            "ポーズ中",
            w / 2,
            h / 2 - 20,
            40,
            true
        );
    }
}

// ============================================================
// RESULT
// ============================================================

void Renderer::renderResult(
    const UI& ui
)
{
    int w, h;
    getOutputSize(w, h);

    renderStageBackground(
        ui.lastRunResult().stage
    );

    drawPanel(
        w / 2 - 290,
        65,
        580,
        h - 130,
        225
    );

    drawText(
        "リザルト",
        w / 2,
        95,
        40,
        true
    );

    const RunResult& result =
        ui.lastRunResult();

    const ProgressionResult& progress =
        ui.lastProgressionResult();

    drawText(
        "スコア  " +
        std::to_string(
            result.score
        ),
        w / 2,
        180,
        26,
        true
    );

    drawText(
        "ライン  " +
        std::to_string(
            result.lines
        ),
        w / 2,
        225,
        26,
        true
    );

    drawText(
        "テトリス  " +
        std::to_string(
            result.tetrises
        ),
        w / 2,
        270,
        18,
        true
    );

    drawText(
        "最大コンボ  " +
        std::to_string(
            result.maxCombo
        ),
        w / 2,
        305,
        18,
        true
    );

    drawText(
        "獲得XP +" +
        std::to_string(
            progress.xpEarned
        ),
        w / 2,
        355,
        26,
        true
    );

    if (
        progress.levelUp
    )
    {
        drawText(
            "レベルアップ！  LV." +
            std::to_string(
                progress.newLevel
            ),
            w / 2,
            400,
            26,
            true
        );
    }

    const char* items[] =
    {
        "もう一度",
        "ステージ選択",
        "ホーム"
    };

    for (int i = 0; i < 3; ++i)
    {
        drawMenuItem(
            items[i],
            w / 2,
            475 + i * 55,
            ui.resultSelection() == i,
            ui.selectionPulse()
        );
    }
}

// ============================================================
// SETTINGS
// ============================================================

void Renderer::renderSettings(
    const UI& ui
)
{
    int w, h;
    getOutputSize(w, h);

    renderStageBackground(5);

    drawText(
        "設定",
        w / 2,
        30,
        40,
        true
    );

    const Settings& s =
        *mSettings;

    const std::string items[] =
    {
        "全体音量        " +
            std::to_string(
                static_cast<int>(
                    s.masterVolume *
                    100
                )
            ),

        "BGM音量         " +
            std::to_string(
                static_cast<int>(
                    s.musicVolume *
                    100
                )
            ),

        "効果音量        " +
            std::to_string(
                static_cast<int>(
                    s.soundVolume *
                    100
                )
            ),

        std::string(
            "アニメーション   "
        ) +
            (
                s.animationsEnabled
                    ? "ON"
                    : "OFF"
            ),

        "3Dカメラ速度     " +
            std::to_string(
                static_cast<int>(
                    s.cameraSensitivity
                )
            ),

        "スティック遊び   " +
            std::to_string(
                s.leftStickDeadzone
            ),

        "長押し開始       " +
            std::to_string(
                static_cast<int>(
                    s.dasDelay *
                    1000
                )
            ) +
            "ms",

        "連続移動速度     " +
            std::to_string(
                static_cast<int>(
                    s.arrDelay *
                    1000
                )
            ) +
            "ms",

        std::string(
            "ゴースト表示     "
        ) +
            (
                s.showGhost
                    ? "ON"
                    : "OFF"
            ),

        std::string(
            "振動             "
        ) +
            (
                s.vibrationEnabled
                    ? "ON"
                    : "OFF"
            ),

        std::string(
            "フルスクリーン   "
        ) +
            (
                s.fullscreen
                    ? "ON"
                    : "OFF"
            ),

        "戻る"
    };

    for (int i = 0; i < 12; ++i)
    {
        drawMenuItem(
            items[i],
            w / 2,
            100 + i * 47,
            ui.settingsSelection() == i,
            ui.selectionPulse()
        );
    }

    drawText(
        "左右：変更   A / Enter：切り替え",
        w / 2,
        h - 35,
        18,
        true,
        170
    );
}

// ============================================================
// MINI PIECE
// ============================================================

void Renderer::renderMiniPiece(
    TetrominoType type,
    int centerX,
    int centerY,
    int cellSize,
    Uint8 alpha
)
{
    Tetromino piece(type);

    const auto& cells =
        piece.cells();

    int minX = 99;
    int maxX = -99;
    int minY = 99;
    int maxY = -99;

    for (
        const auto& cell :
        cells
    )
    {
        minX =
            std::min(
                minX,
                cell.x
            );

        maxX =
            std::max(
                maxX,
                cell.x
            );

        minY =
            std::min(
                minY,
                cell.y
            );

        maxY =
            std::max(
                maxY,
                cell.y
            );
    }

    const int width =
        (maxX - minX + 1) *
        cellSize;

    const int height =
        (maxY - minY + 1) *
        cellSize;

    const int startX =
        centerX -
        width / 2 -
        minX *
        cellSize;

    const int startY =
        centerY -
        height / 2 -
        minY *
        cellSize;

    for (
        const auto& cell :
        cells
    )
    {
        drawBlock(
            startX +
            cell.x *
            cellSize,

            startY +
            cell.y *
            cellSize,

            cellSize,
            type,
            alpha
        );
    }
}

// ============================================================
// BLOCK
// ============================================================

void Renderer::drawBlock(
    int x,
    int y,
    int size,
    TetrominoType type,
    Uint8 alpha,
    bool outline
)
{
    Uint8 r = 220;
    Uint8 g = 220;
    Uint8 b = 220;

    switch (type)
    {
        case TetrominoType::I:
            r = 45;
            g = 225;
            b = 245;
            break;

        case TetrominoType::O:
            r = 250;
            g = 225;
            b = 45;
            break;

        case TetrominoType::T:
            r = 185;
            g = 75;
            b = 235;
            break;

        case TetrominoType::S:
            r = 65;
            g = 220;
            b = 95;
            break;

        case TetrominoType::Z:
            r = 240;
            g = 60;
            b = 65;
            break;

        case TetrominoType::J:
            r = 55;
            g = 100;
            b = 240;
            break;

        case TetrominoType::L:
            r = 245;
            g = 145;
            b = 35;
            break;

        default:
            break;
    }

    SDL_Rect rect
    {
        x + 1,
        y + 1,
        size - 2,
        size - 2
    };

    SDL_SetRenderDrawColor(
        mRenderer,
        r,
        g,
        b,
        alpha
    );

    SDL_RenderFillRect(
        mRenderer,
        &rect
    );

    SDL_SetRenderDrawColor(
        mRenderer,

        std::min(
            255,
            static_cast<int>(r) +
            45
        ),

        std::min(
            255,
            static_cast<int>(g) +
            45
        ),

        std::min(
            255,
            static_cast<int>(b) +
            45
        ),

        alpha
    );

    SDL_RenderDrawLine(
        mRenderer,
        x + 2,
        y + 2,
        x + size - 3,
        y + 2
    );

    if (outline)
    {
        SDL_SetRenderDrawColor(
            mRenderer,
            r / 2,
            g / 2,
            b / 2,
            alpha
        );

        SDL_RenderDrawRect(
            mRenderer,
            &rect
        );
    }
}

// ============================================================
// BACKGROUND
// ============================================================

void Renderer::renderStageBackground(
    int stage
)
{
    int w, h;
    getOutputSize(w, h);

    // --------------------------------------------------------
    // 0 クラシック
    // --------------------------------------------------------

    if (stage == 0)
    {
        SDL_SetRenderDrawColor(
            mRenderer,
            6, 18, 42, 255
        );

        SDL_RenderClear(
            mRenderer
        );

        SDL_SetRenderDrawColor(
            mRenderer,
            40, 110, 210, 55
        );

        for (
            int x = 0;
            x < w;
            x += 50
        )
        {
            SDL_RenderDrawLine(
                mRenderer,
                x,
                0,
                x,
                h
            );
        }

        for (
            int y = 0;
            y < h;
            y += 50
        )
        {
            SDL_RenderDrawLine(
                mRenderer,
                0,
                y,
                w,
                y
            );
        }

        return;
    }

    // --------------------------------------------------------
    // 1 ネオンシティ
    // --------------------------------------------------------

    if (stage == 1)
    {
        SDL_SetRenderDrawColor(
            mRenderer,
            20, 5, 45, 255
        );

        SDL_RenderClear(
            mRenderer
        );

        SDL_SetRenderDrawColor(
            mRenderer,
            255, 30, 210, 90
        );

        for (
            int x = -h;
            x < w;
            x += 90
        )
        {
            SDL_RenderDrawLine(
                mRenderer,
                x,
                h,
                x + h,
                0
            );
        }

        SDL_SetRenderDrawColor(
            mRenderer,
            40, 220, 255, 70
        );

        for (
            int y = 80;
            y < h;
            y += 100
        )
        {
            SDL_RenderDrawLine(
                mRenderer,
                0,
                y,
                w,
                y
            );
        }

        return;
    }

    // --------------------------------------------------------
    // 2 サンセット
    // --------------------------------------------------------

    if (stage == 2)
    {
        for (
            int y = 0;
            y < h;
            ++y
        )
        {
            const float t =
                static_cast<float>(y) /
                static_cast<float>(h);

            const Uint8 r =
                static_cast<Uint8>(
                    230 -
                    130 *
                    t
                );

            const Uint8 g =
                static_cast<Uint8>(
                    100 -
                    70 *
                    t
                );

            const Uint8 b =
                static_cast<Uint8>(
                    80 +
                    110 *
                    t
                );

            SDL_SetRenderDrawColor(
                mRenderer,
                r,
                g,
                b,
                255
            );

            SDL_RenderDrawLine(
                mRenderer,
                0,
                y,
                w,
                y
            );
        }

        SDL_SetRenderDrawColor(
            mRenderer,
            255, 220, 120, 180
        );

        SDL_Rect sun
        {
            w / 2 - 70,
            110,
            140,
            140
        };

        SDL_RenderFillRect(
            mRenderer,
            &sun
        );

        return;
    }

    // --------------------------------------------------------
    // 3 アイス
    // --------------------------------------------------------

    if (stage == 3)
    {
        SDL_SetRenderDrawColor(
            mRenderer,
            12, 55, 85, 255
        );

        SDL_RenderClear(
            mRenderer
        );

        SDL_SetRenderDrawColor(
            mRenderer,
            210, 245, 255, 120
        );

        for (
            int y = 20;
            y < h;
            y += 70
        )
        {
            for (
                int x = 20;
                x < w;
                x += 85
            )
            {
                SDL_RenderDrawLine(
                    mRenderer,
                    x - 6,
                    y,
                    x + 6,
                    y
                );

                SDL_RenderDrawLine(
                    mRenderer,
                    x,
                    y - 6,
                    x,
                    y + 6
                );

                SDL_RenderDrawLine(
                    mRenderer,
                    x - 5,
                    y - 5,
                    x + 5,
                    y + 5
                );

                SDL_RenderDrawLine(
                    mRenderer,
                    x + 5,
                    y - 5,
                    x - 5,
                    y + 5
                );
            }
        }

        return;
    }

    // --------------------------------------------------------
    // 4 ボルケーノ
    // --------------------------------------------------------

    if (stage == 4)
    {
        SDL_SetRenderDrawColor(
            mRenderer,
            28, 3, 3, 255
        );

        SDL_RenderClear(
            mRenderer
        );

        SDL_SetRenderDrawColor(
            mRenderer,
            255, 70, 10, 130
        );

        for (
            int y = h - 50;
            y > 0;
            y -= 110
        )
        {
            SDL_RenderDrawLine(
                mRenderer,
                0,
                y,
                w,
                y - 35
            );
        }

        SDL_SetRenderDrawColor(
            mRenderer,
            255, 155, 20, 90
        );

        for (
            int x = 0;
            x < w;
            x += 120
        )
        {
            SDL_RenderDrawLine(
                mRenderer,
                x,
                h,
                x + 70,
                0
            );
        }

        return;
    }

    // --------------------------------------------------------
    // 5 サイバー
    // --------------------------------------------------------

    if (stage == 5)
    {
        SDL_SetRenderDrawColor(
            mRenderer,
            2, 18, 12, 255
        );

        SDL_RenderClear(
            mRenderer
        );

        SDL_SetRenderDrawColor(
            mRenderer,
            40, 255, 135, 70
        );

        for (
            int x = 0;
            x < w;
            x += 40
        )
        {
            SDL_RenderDrawLine(
                mRenderer,
                x,
                0,
                x,
                h
            );
        }

        for (
            int y = 0;
            y < h;
            y += 40
        )
        {
            SDL_RenderDrawLine(
                mRenderer,
                0,
                y,
                w,
                y
            );
        }

        SDL_SetRenderDrawColor(
            mRenderer,
            100, 255, 190, 120
        );

        for (
            int y = 20;
            y < h;
            y += 120
        )
        {
            SDL_RenderDrawLine(
                mRenderer,
                0,
                y,
                w,
                y
            );
        }

        return;
    }

    // --------------------------------------------------------
    // 6 スペース
    // --------------------------------------------------------

    if (stage == 6)
    {
        SDL_SetRenderDrawColor(
            mRenderer,
            2, 3, 18, 255
        );

        SDL_RenderClear(
            mRenderer
        );

        SDL_SetRenderDrawColor(
            mRenderer,
            255, 255, 255, 200
        );

        for (
            int i = 0;
            i < 140;
            ++i
        )
        {
            const int x =
                (i * 97) %
                std::max(1, w);

            const int y =
                (i * 53) %
                std::max(1, h);

            SDL_RenderDrawPoint(
                mRenderer,
                x,
                y
            );
        }

        return;
    }

    // --------------------------------------------------------
    // 7 VOID
    // --------------------------------------------------------

    if (stage == 7)
    {
        SDL_SetRenderDrawColor(
            mRenderer,
            2, 1, 6, 255
        );

        SDL_RenderClear(
            mRenderer
        );

        SDL_SetRenderDrawColor(
            mRenderer,
            95, 25, 155, 80
        );

        for (
            int i = 0;
            i < 12;
            ++i
        )
        {
            SDL_Rect rect
            {
                i * 30,
                i * 22,
                w - i * 60,
                h - i * 44
            };

            if (
                rect.w > 0 &&
                rect.h > 0
            )
            {
                SDL_RenderDrawRect(
                    mRenderer,
                    &rect
                );
            }
        }

        return;
    }

    // --------------------------------------------------------
    // 8 ギャラクシー
    // --------------------------------------------------------

    if (stage == 8)
    {
        SDL_SetRenderDrawColor(
            mRenderer,
            15, 4, 35, 255
        );

        SDL_RenderClear(
            mRenderer
        );

        for (
            int i = 0;
            i < 220;
            ++i
        )
        {
            const int x =
                (i * 71 + 35) %
                std::max(1, w);

            const int y =
                (i * 113 + 19) %
                std::max(1, h);

            if (i % 3 == 0)
            {
                SDL_SetRenderDrawColor(
                    mRenderer,
                    150, 140, 255, 210
                );
            }
            else
            {
                SDL_SetRenderDrawColor(
                    mRenderer,
                    255, 255, 255, 170
                );
            }

            SDL_RenderDrawPoint(
                mRenderer,
                x,
                y
            );
        }

        SDL_SetRenderDrawColor(
            mRenderer,
            120, 45, 220, 55
        );

        for (
            int i = 0;
            i < 10;
            ++i
        )
        {
            SDL_Rect ring
            {
                w / 2 -
                80 -
                i * 25,

                h / 2 -
                40 -
                i * 15,

                160 +
                i * 50,

                80 +
                i * 30
            };

            SDL_RenderDrawRect(
                mRenderer,
                &ring
            );
        }

        return;
    }

    // --------------------------------------------------------
    // 9 MASTER
    // --------------------------------------------------------

    SDL_SetRenderDrawColor(
        mRenderer,
        15, 10, 2, 255
    );

    SDL_RenderClear(
        mRenderer
    );

    SDL_SetRenderDrawColor(
        mRenderer,
        220, 170, 45, 85
    );

    for (
        int x = 0;
        x < w;
        x += 70
    )
    {
        SDL_RenderDrawLine(
            mRenderer,
            x,
            0,
            x,
            h
        );
    }

    for (
        int y = 0;
        y < h;
        y += 70
    )
    {
        SDL_RenderDrawLine(
            mRenderer,
            0,
            y,
            w,
            y
        );
    }

    SDL_SetRenderDrawColor(
        mRenderer,
        255, 220, 90, 130
    );

    SDL_Rect border
    {
        20,
        20,
        w - 40,
        h - 40
    };

    SDL_RenderDrawRect(
        mRenderer,
        &border
    );
}

// ============================================================
// PANEL
// ============================================================

void Renderer::drawPanel(
    int x,
    int y,
    int w,
    int h,
    Uint8 alpha
)
{
    SDL_SetRenderDrawColor(
        mRenderer,
        8, 12, 24,
        alpha
    );

    SDL_Rect rect
    {
        x,
        y,
        w,
        h
    };

    SDL_RenderFillRect(
        mRenderer,
        &rect
    );

    SDL_SetRenderDrawColor(
        mRenderer,
        160, 180, 230,
        std::min(
            255,
            static_cast<int>(
                alpha
            ) +
            25
        )
    );

    SDL_RenderDrawRect(
        mRenderer,
        &rect
    );
}

// ============================================================
// MENU ITEM
// ============================================================

void Renderer::drawMenuItem(
    const std::string& text,
    int centerX,
    int y,
    bool selected,
    float pulse
)
{
    if (selected)
    {
        const int extra =
            static_cast<int>(
                pulse *
                8.0f
            );

        drawPanel(
            centerX
