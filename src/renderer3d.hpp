#pragma once

#include <SDL2/SDL.h>

#include "game.hpp"
#include "settings.hpp"

// ============================================================
// TETRIS COMPLETE
// renderer3d.hpp
//
// SDL_Rendererだけで描く軽量3D風テトリス
// ============================================================

void renderGame3D(
    SDL_Renderer* renderer,
    const Game& game,
    const Settings& settings
);
