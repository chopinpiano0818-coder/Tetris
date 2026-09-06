#include "renderer.hpp"

#include "game.hpp"
#include "ui.hpp"
#include "progression.hpp"
#include "stages.hpp"
#include "tetromino.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

// renderer3d.cpp で実装
#include "renderer3d.hpp"

// ============================================================
// Font
// ============================================================

static constexpr const char* FONT_PATH =
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";

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
        7, 9, 18, 255
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
// Main render switch
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
            90 - (1.0f - intro) * 70.0f
        );

    drawText(
        "TETRIS",
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
        210
    );

    std::ostringstream levelText;

    levelText
        << "PLAYER LV."
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
        true,
        220
    );

    const char* items[] =
    {
        "PLAY 2D",
        "PLAY 3D",
        "SETTINGS",
        "QUIT"
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
        "ARROWS / LEFT STICK : MOVE     A / ENTER : SELECT",
        w / 2,
        h - 45,
        18,
        true,
        150
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
        "SELECT MODE",
        w / 2,
        70,
        40,
        true
    );

    const char* modes[] =
    {
        "ENDLESS",
        "RUNNER",
        "SPRINT",
        "ULTRA"
    };

    const char* descriptions[] =
    {
        "PLAY UNTIL GAME OVER",
        "SCORE ATTACK - 120 SEC",
        "CLEAR 40 LINES",
        "SCORE ATTACK - 180 SEC"
    };

    for (int i = 0; i < 4; ++i)
    {
        const int y =
            190 + i * 105;

        const bool selected =
            ui.modeSelection() == i;

        if (selected)
        {
            drawPanel(
                w / 2 - 240,
                y - 18,
                480,
                78,
                200
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
            y + 35,
            18,
            true,
            selected ? 220 : 120
        );
    }

    drawText(
        "B / ESC : BACK",
        w / 2,
        h - 40,
        18,
        true,
        150
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

    renderStageBackground(
        selected
    );

    drawText(
        "SELECT STAGE",
        w / 2,
        50,
        40,
        true
    );

    const auto& stages =
        stageManager.stages();

    constexpr int CARD_W = 190;
    constexpr int CARD_H = 100;
    constexpr int GAP = 20;

    const int columns = 3;

    const int totalWidth =
        CARD_W * columns +
        GAP * (columns - 1);

    const int startX =
        (w - totalWidth) / 2;

    for (
        int i = 0;
        i < static_cast<int>(stages.size());
        ++i
    )
    {
        const int col =
            i % columns;

        const int row =
            i / columns;

        const int x =
            startX +
            col * (CARD_W + GAP);

        const int y =
            135 +
            row * (CARD_H + GAP);

        const bool isSelected =
            i == selected;

        drawPanel(
            x,
            y,
            CARD_W,
            CARD_H,
            isSelected ? 230 : 150
        );

        if (stages[i].unlocked)
        {
            drawText(
                stages[i].name,
                x + CARD_W / 2,
                y + 20,
                18,
                true
            );

            drawText(
                "UNLOCKED",
                x + CARD_W / 2,
                y + 58,
                18,
                true,
                170
            );
        }
        else
        {
            drawText(
                "LOCKED",
                x + CARD_W / 2,
                y + 18,
                18,
                true,
                140
            );

            drawText(
                "LV." +
                std::to_string(
                    stages[i].requiredLevel
                ),
                x + CARD_W / 2,
                y + 57,
                18,
                true,
                140
            );
        }
    }

    drawText(
        "PLAYER LV." +
        std::to_string(
            progression.playerLevel()
        ),
        w / 2,
        h - 45,
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

    // Board background
    SDL_SetRenderDrawColor(
        mRenderer,
        5, 8, 15, 225
    );

    SDL_Rect background
    {
        originX - 4,
        originY - 4,
        boardW + 8,
        boardH + 8
    };

    SDL_RenderFillRect(
        mRenderer,
        &background
    );

    // Grid
    SDL_SetRenderDrawColor(
        mRenderer,
        40, 45, 60, 90
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

    // Locked blocks
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

    // Ghost
    if (mSettings->showGhost)
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

    // Current piece
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

    // Line-clear flash
    if (
        mSettings->lineClearFlash &&
        game.lineClearAnimation() > 0.0f
    )
    {
        const Uint8 alpha =
            static_cast<Uint8>(
                90.0f *
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
        170
    );

    drawText(
        "SCORE",
        panelX + 20,
        70,
        18
    );

    drawText(
        std::to_string(
            game.score()
        ),
        panelX + 20,
        95,
        26
    );

    drawText(
        "LINES  " +
        std::to_string(
            game.lines()
        ),
        panelX + 20,
        145,
        18
    );

    drawText(
        "LEVEL  " +
        std::to_string(
            game.gameLevel()
        ),
        panelX + 20,
        175,
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
            "TIME",
            panelX + 20,
            215,
            18
        );

        drawText(
            timer.str(),
            panelX + 20,
            240,
            26
        );
    }
    else if (
        game.gameMode() ==
        GameMode::Sprint
    )
    {
        drawText(
            "TARGET 40",
            panelX + 20,
            220,
            18
        );
    }

    if (mSettings->showHold)
    {
        drawText(
            "HOLD",
            panelX + 20,
            295,
            18
        );

        if (game.hasHoldPiece())
        {
            renderMiniPiece(
                game.holdPiece(),
                panelX + 100,
                350,
                18
            );
        }
    }

    if (mSettings->showNext)
    {
        drawText(
            "NEXT",
            panelX + 20,
            400,
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
                450 + index * 55,
                12
            );

            ++index;
        }
    }

    if (game.isPaused())
    {
        drawPanel(
            w / 2 - 150,
            h / 2 - 60,
            300,
            120,
            235
        );

        drawText(
            "PAUSED",
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
        w / 2 - 280,
        70,
        560,
        h - 140,
        220
    );

    drawText(
        "RESULT",
        w / 2,
        100,
        40,
        true
    );

    const RunResult& result =
        ui.lastRunResult();

    const ProgressionResult& progress =
        ui.lastProgressionResult();

    drawText(
        "SCORE  " +
        std::to_string(result.score),
        w / 2,
        185,
        26,
        true
    );

    drawText(
        "LINES  " +
        std::to_string(result.lines),
        w / 2,
        230,
        26,
        true
    );

    drawText(
        "TETRIS  " +
        std::to_string(result.tetrises),
        w / 2,
        275,
        18,
        true
    );

    drawText(
        "MAX COMBO  " +
        std::to_string(result.maxCombo),
        w / 2,
        310,
        18,
        true
    );

    drawText(
        "XP +" +
        std::to_string(progress.xpEarned),
        w / 2,
        360,
        26,
        true
    );

    if (progress.levelUp)
    {
        drawText(
            "LEVEL UP!  LV." +
            std::to_string(
                progress.newLevel
            ),
            w / 2,
            405,
            26,
            true
        );
    }

    const char* items[] =
    {
        "RETRY",
        "STAGE SELECT",
        "HOME"
    };

    for (int i = 0; i < 3; ++i)
    {
        drawMenuItem(
            items[i],
            w / 2,
            480 + i * 55,
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

    drawText(
        "SETTINGS",
        w / 2,
        35,
        40,
        true
    );

    const Settings& s =
        *mSettings;

    const std::string items[] =
    {
        "MASTER VOLUME   " +
            std::to_string(
                static_cast<int>(
                    s.masterVolume * 100
                )
            ),

        "MUSIC VOLUME    " +
            std::to_string(
                static_cast<int>(
                    s.musicVolume * 100
                )
            ),

        "SOUND VOLUME    " +
            std::to_string(
                static_cast<int>(
                    s.soundVolume * 100
                )
            ),

        std::string("ANIMATION       ") +
            (s.animationsEnabled
                ? "ON"
                : "OFF"),

        "CAMERA SPEED    " +
            std::to_string(
                static_cast<int>(
                    s.cameraSensitivity
                )
            ),

        "STICK DEADZONE  " +
            std::to_string(
                s.leftStickDeadzone
            ),

        "DAS             " +
            std::to_string(
                static_cast<int>(
                    s.dasDelay * 1000
                )
            ) + "ms",

        "ARR             " +
            std::to_string(
                static_cast<int>(
                    s.arrDelay * 1000
                )
            ) + "ms",

        std::string("GHOST           ") +
            (s.showGhost ? "ON" : "OFF"),

        std::string("VIBRATION       ") +
            (s.vibrationEnabled
                ? "ON"
                : "OFF"),

        std::string("FULLSCREEN      ") +
            (s.fullscreen ? "ON" : "OFF"),

        "BACK"
    };

    for (int i = 0; i < 12; ++i)
    {
        drawMenuItem(
            items[i],
            w / 2,
            105 + i * 47,
            ui.settingsSelection() == i,
            ui.selectionPulse()
        );
    }

    drawText(
        "LEFT / RIGHT : CHANGE",
        w / 2,
        h - 35,
        18,
        true,
        150
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

    for (const auto& cell : cells)
    {
        minX =
            std::min(minX, cell.x);

        maxX =
            std::max(maxX, cell.x);

        minY =
            std::min(minY, cell.y);

        maxY =
            std::max(maxY, cell.y);
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
        minX * cellSize;

    const int startY =
        centerY -
        height / 2 -
        minY * cellSize;

    for (const auto& cell : cells)
    {
        drawBlock(
            startX + cell.x * cellSize,
            startY + cell.y * cellSize,
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
            r = 50;  g = 220; b = 240;
            break;

        case TetrominoType::O:
            r = 245; g = 220; b = 50;
            break;

        case TetrominoType::T:
            r = 180; g = 80;  b = 220;
            break;

        case TetrominoType::S:
            r = 70;  g = 210; b = 100;
            break;

        case TetrominoType::Z:
            r = 230; g = 65;  b = 70;
            break;

        case TetrominoType::J:
            r = 65;  g = 100; b = 230;
            break;

        case TetrominoType::L:
            r = 240; g = 145; b = 45;
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
        r, g, b, alpha
    );

    SDL_RenderFillRect(
        mRenderer,
        &rect
    );

    // 光ってる上辺
    SDL_SetRenderDrawColor(
        mRenderer,
        std::min(255, r + 35),
        std::min(255, g + 35),
        std::min(255, b + 35),
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
// Background
// ============================================================

void Renderer::renderStageBackground(
    int stage
)
{
    int w, h;
    getOutputSize(w, h);

    // ステージごとに背景の雰囲気を変える
    Uint8 r = 8;
    Uint8 g = 12;
    Uint8 b = 24;

    switch (stage)
    {
        case 1:
            r = 15; g = 5;  b = 35;
            break;

        case 2:
            r = 45; g = 20; b = 20;
            break;

        case 3:
            r = 10; g = 30; b = 45;
            break;

        case 4:
            r = 45; g = 8;  b = 5;
            break;

        case 5:
            r = 4;  g = 25; b = 25;
            break;

        case 6:
            r = 4;  g = 5;  b = 25;
            break;

        case 7:
            r = 3;  g = 3;  b = 8;
            break;

        case 8:
            r = 20; g = 5;  b = 35;
            break;

        case 9:
            r = 30; g = 20; b = 5;
            break;

        default:
            break;
    }

    SDL_SetRenderDrawColor(
        mRenderer,
        r, g, b, 255
    );

    SDL_Rect background
    {
        0, 0, w, h
    };

    SDL_RenderFillRect(
        mRenderer,
        &background
    );

    // 薄い横線
    SDL_SetRenderDrawColor(
        mRenderer,
        255, 255, 255, 12
    );

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
        10, 14, 25,
        alpha
    );

    SDL_Rect rect
    {
        x, y, w, h
    };

    SDL_RenderFillRect(
        mRenderer,
        &rect
    );

    SDL_SetRenderDrawColor(
        mRenderer,
        110, 130, 180,
        std::min<int>(
            255,
            alpha + 20
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
                pulse * 8.0f
            );

        drawPanel(
            centerX - 180 - extra,
            y - 10,
            360 + extra * 2,
            48,
            210
        );
    }

    drawText(
        selected
            ? "> " + text + " <"
            : text,
        centerX,
        y,
        selected ? 26 : 18,
        true,
        selected ? 255 : 165
    );
}

// ============================================================
// TEXT
// ============================================================

void Renderer::drawText(
    const std::string& text,
    int x,
    int y,
    int size,
    bool centered,
    Uint8 alpha
)
{
    TTF_Font* font =
        mFontSmall;

    if (size >= 55)
        font = mFontTitle;
    else if (size >= 34)
        font = mFontLarge;
    else if (size >= 23)
        font = mFontMedium;

    if (!font)
    {
        return;
    }

    SDL_Color color
    {
        245,
        247,
        255,
        alpha
    };

    SDL_Surface* surface =
        TTF_RenderUTF8_Blended(
            font,
            text.c_str(),
            color
        );

    if (!surface)
    {
        return;
    }

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            mRenderer,
            surface
        );

    if (!texture)
    {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_SetTextureAlphaMod(
        texture,
        alpha
    );

    SDL_Rect destination
    {
        centered
            ? x - surface->w / 2
            : x,
        y,
        surface->w,
        surface->h
    };

    SDL_RenderCopy(
        mRenderer,
        texture,
        nullptr,
        &destination
    );

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// ============================================================
// Output size
// ============================================================

void Renderer::getOutputSize(
    int& width,
    int& height
) const
{
    SDL_GetRendererOutputSize(
        mRenderer,
        &width,
        &height
    );
}
