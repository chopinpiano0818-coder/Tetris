#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "types.hpp"
#include "settings.hpp"

class Game;
class UI;
class Progression;
class StageManager;

// ============================================================
// TETRIS COMPLETE
// renderer.hpp
// ============================================================

class Renderer
{
public:
    Renderer(
        SDL_Window* window,
        SDL_Renderer* renderer,
        Settings& settings
    );

    ~Renderer();

    void applySettings(
        const Settings& settings
    );

    void beginFrame();

    void render(
        AppState state,
        RenderStyle selectedStyle,
        GameMode selectedMode,
        int selectedStage,
        const Game& game,
        const UI& ui,
        const Progression& progression,
        const StageManager& stageManager
    );

    void endFrame();

private:
    // --------------------------------------------------------
    // Screens
    // --------------------------------------------------------

    void renderHome(
        RenderStyle selectedStyle,
        const UI& ui,
        const Progression& progression
    );

    void renderModeSelect(
        GameMode selectedMode,
        const UI& ui
    );

    void renderStageSelect(
        int selectedStage,
        const UI& ui,
        const StageManager& stageManager,
        const Progression& progression
    );

    void renderPlaying(
        const Game& game
    );

    void renderResult(
        const UI& ui
    );

    void renderSettings(
        const UI& ui
    );

    // --------------------------------------------------------
    // 2D Tetris
    // --------------------------------------------------------

    void renderBoard2D(
        const Game& game
    );

    void renderSidePanel(
        const Game& game
    );

    void renderMiniPiece(
        TetrominoType type,
        int centerX,
        int centerY,
        int cellSize,
        Uint8 alpha = 255
    );

    void drawBlock(
        int x,
        int y,
        int size,
        TetrominoType type,
        Uint8 alpha = 255,
        bool outline = true
    );

    // --------------------------------------------------------
    // Background
    // --------------------------------------------------------

    void renderStageBackground(
        int stage
    );

    // --------------------------------------------------------
    // UI helpers
    // --------------------------------------------------------

    void drawText(
        const std::string& text,
        int x,
        int y,
        int size,
        bool centered = false,
        Uint8 alpha = 255
    );

    void drawPanel(
        int x,
        int y,
        int w,
        int h,
        Uint8 alpha = 180
    );

    void drawMenuItem(
        const std::string& text,
        int centerX,
        int y,
        bool selected,
        float pulse
    );

    void getOutputSize(
        int& width,
        int& height
    ) const;

private:
    SDL_Window* mWindow = nullptr;
    SDL_Renderer* mRenderer = nullptr;

    Settings* mSettings = nullptr;

    TTF_Font* mFontSmall = nullptr;
    TTF_Font* mFontMedium = nullptr;
    TTF_Font* mFontLarge = nullptr;
    TTF_Font* mFontTitle = nullptr;

    bool mFullscreenApplied = false;
};
