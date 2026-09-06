#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <string>
#include <algorithm>

using namespace std;

// ============================================================
// 設定
// ============================================================

const int WINDOW_W = 700;
const int WINDOW_H = 760;

const int COLS = 10;
const int ROWS = 20;

const int CELL = 30;

const int BOARD_X = 60;
const int BOARD_Y = 80;

const int DROP_INTERVAL_START = 700;

// ============================================================
// SDL
// ============================================================

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_GameController* controller = nullptr;

TTF_Font* fontLarge = nullptr;
TTF_Font* fontMedium = nullptr;
TTF_Font* fontSmall = nullptr;

// ============================================================
// ゲーム
// ============================================================

int board[ROWS][COLS] = {};

bool running = true;
bool paused = false;
bool gameOver = false;

int score = 0;
int lines = 0;
int level = 1;

Uint32 lastDropTime = 0;

// ============================================================
// 乱数
// ============================================================

random_device rd;
mt19937 rng(rd());

// ============================================================
// テトリミノ
// ============================================================

enum PieceType
{
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

struct Piece
{
    PieceType type;
    int rotation;
    int x;
    int y;
};

Piece currentPiece;

bool hasHold = false;
PieceType holdPiece;
bool holdUsed = false;

vector<PieceType> nextQueue;

// ============================================================
// 色
// ============================================================

SDL_Color pieceColor(PieceType type)
{
    switch (type)
    {
        case I: return {0, 220, 220, 255};
        case O: return {240, 220, 0, 255};
        case T: return {170, 70, 220, 255};
        case S: return {60, 200, 80, 255};
        case Z: return {230, 60, 60, 255};
        case J: return {60, 90, 220, 255};
        case L: return {240, 150, 30, 255};
    }

    return {255, 255, 255, 255};
}

// ============================================================
// ミノ形状
// 4x4
// ============================================================

const int SHAPES[7][4][4][4] =
{
    // I
    {
        {
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0}
        },
        {
            {0,0,0,0},
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0}
        }
    },

    // O
    {
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        }
    },

    // T
    {
        {
            {0,1,0,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {1,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },

    // S
    {
        {
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0}
        },
        {
            {1,0,0,0},
            {1,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },

    // Z
    {
        {
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {1,1,0,0},
            {1,0,0,0},
            {0,0,0,0}
        }
    },

    // J
    {
        {
            {1,0,0,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {1,1,0,0},
            {0,0,0,0}
        }
    },

    // L
    {
        {
            {0,0,1,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {1,0,0,0},
            {0,0,0,0}
        },
        {
            {1,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    }
};

// ============================================================
// 描画
// ============================================================

void drawRect(SDL_Rect rect, SDL_Color color)
{
    SDL_SetRenderDrawColor(
        renderer,
        color.r,
        color.g,
        color.b,
        color.a
    );

    SDL_RenderFillRect(renderer, &rect);
}

void drawText(
    const string& text,
    int x,
    int y,
    TTF_Font* font,
    SDL_Color color
)
{
    SDL_Surface* surface =
        TTF_RenderUTF8_Blended(
            font,
            text.c_str(),
            color
        );

    if (!surface)
        return;

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            renderer,
            surface
        );

    if (!texture)
    {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect =
    {
        x,
        y,
        surface->w,
        surface->h
    };

    SDL_RenderCopy(
        renderer,
        texture,
        nullptr,
        &rect
    );

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void drawCell(
    int x,
    int y,
    SDL_Color color,
    bool ghost = false
)
{
    SDL_Rect rect =
    {
        BOARD_X + x * CELL + 1,
        BOARD_Y + y * CELL + 1,
        CELL - 2,
        CELL - 2
    };

    if (ghost)
    {
        SDL_SetRenderDrawBlendMode(
            renderer,
            SDL_BLENDMODE_BLEND
        );

        color.a = 70;
    }

    drawRect(rect, color);
}

void drawMiniPiece(
    PieceType type,
    int startX,
    int startY
)
{
    SDL_Color color = pieceColor(type);

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (SHAPES[type][0][y][x])
            {
                SDL_Rect rect =
                {
                    startX + x * 20,
                    startY + y * 20,
                    18,
                    18
                };

                drawRect(rect, color);
            }
        }
    }
}

// ============================================================
// 衝突判定
// ============================================================

bool isValid(
    const Piece& piece,
    int offsetX = 0,
    int offsetY = 0,
    int rotationOffset = 0
)
{
    int rot =
        (piece.rotation + rotationOffset + 4) % 4;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (!SHAPES[piece.type][rot][y][x])
                continue;

            int boardX =
                piece.x + x + offsetX;

            int boardY =
                piece.y + y + offsetY;

            if (
                boardX < 0 ||
                boardX >= COLS ||
                boardY >= ROWS
            )
            {
                return false;
            }

            if (
                boardY >= 0 &&
                board[boardY][boardX] != 0
            )
            {
                return false;
            }
        }
    }

    return true;
}

// ============================================================
// NEXT
// ============================================================

PieceType randomPiece()
{
    uniform_int_distribution<int> dist(0, 6);

    return static_cast<PieceType>(
        dist(rng)
    );
}

void fillNextQueue()
{
    while (nextQueue.size() < 5)
    {
        nextQueue.push_back(
            randomPiece()
        );
    }
}

// ============================================================
// 新しいミノ
// ============================================================

void spawnPiece()
{
    fillNextQueue();

    currentPiece.type =
        nextQueue.front();

    nextQueue.erase(
        nextQueue.begin()
    );

    fillNextQueue();

    currentPiece.rotation = 0;
    currentPiece.x = 3;
    currentPiece.y = -1;

    holdUsed = false;

    if (!isValid(currentPiece))
    {
        gameOver = true;
    }
}

// ============================================================
// ライン消去
// ============================================================

void clearLines()
{
    int cleared = 0;

    for (int y = ROWS - 1; y >= 0; y--)
    {
        bool full = true;

        for (int x = 0; x < COLS; x++)
        {
            if (board[y][x] == 0)
            {
                full = false;
                break;
            }
        }

        if (full)
        {
            cleared++;

            for (int yy = y; yy > 0; yy--)
            {
                for (int x = 0; x < COLS; x++)
                {
                    board[yy][x] =
                        board[yy - 1][x];
                }
            }

            for (int x = 0; x < COLS; x++)
            {
                board[0][x] = 0;
            }

            y++;
        }
    }

    if (cleared > 0)
    {
        lines += cleared;

        switch (cleared)
        {
            case 1:
                score += 100 * level;
                break;

            case 2:
                score += 300 * level;
                break;

            case 3:
                score += 500 * level;
                break;

            case 4:
                score += 800 * level;
                break;
        }

        level =
            1 + lines / 10;
    }
}

// ============================================================
// 固定
// ============================================================

void lockPiece()
{
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (
                SHAPES
                [currentPiece.type]
                [currentPiece.rotation]
                [y]
                [x]
            )
            {
                int bx =
                    currentPiece.x + x;

                int by =
                    currentPiece.y + y;

                if (by < 0)
                {
                    gameOver = true;
                    return;
                }

                board[by][bx] =
                    currentPiece.type + 1;
            }
        }
    }

    clearLines();

    spawnPiece();
}

// ============================================================
// 移動
// ============================================================

void moveHorizontal(int dx)
{
    if (
        isValid(
            currentPiece,
            dx,
            0
        )
    )
    {
        currentPiece.x += dx;
    }
}

void softDrop()
{
    if (
        isValid(
            currentPiece,
            0,
            1
        )
    )
    {
        currentPiece.y++;
        score++;
    }
    else
    {
        lockPiece();
    }
}

// ============================================================
// ハードドロップ
// ============================================================

void hardDrop()
{
    int dropped = 0;

    while (
        isValid(
            currentPiece,
            0,
            1
        )
    )
    {
        currentPiece.y++;
        dropped++;
    }

    score += dropped * 2;

    lockPiece();
}

// ============================================================
// 回転
// ============================================================

void rotatePiece(int direction)
{
    if (
        isValid(
            currentPiece,
            0,
            0,
            direction
        )
    )
    {
        currentPiece.rotation =
            (
                currentPiece.rotation +
                direction +
                4
            ) % 4;

        return;
    }

    // 簡単な壁キック
    if (
        isValid(
            currentPiece,
            -1,
            0,
            direction
        )
    )
    {
        currentPiece.x--;

        currentPiece.rotation =
            (
                currentPiece.rotation +
                direction +
                4
            ) % 4;

        return;
    }

    if (
        isValid(
            currentPiece,
            1,
            0,
            direction
        )
    )
    {
        currentPiece.x++;

        currentPiece.rotation =
            (
                currentPiece.rotation +
                direction +
                4
            ) % 4;
    }
}

// ============================================================
// HOLD
// ============================================================

void holdCurrent()
{
    if (holdUsed)
        return;

    if (!hasHold)
    {
        holdPiece =
            currentPiece.type;

        hasHold = true;

        spawnPiece();
    }
    else
    {
        PieceType temp =
            currentPiece.type;

        currentPiece.type =
            holdPiece;

        holdPiece = temp;

        currentPiece.rotation = 0;
        currentPiece.x = 3;
        currentPiece.y = -1;
    }

    holdUsed = true;
}

// ============================================================
// ゴースト
// ============================================================

Piece getGhostPiece()
{
    Piece ghost =
        currentPiece;

    while (
        isValid(
            ghost,
            0,
            1
        )
    )
    {
        ghost.y++;
    }

    return ghost;
}

// ============================================================
// 描画
// ============================================================

void drawGame()
{
    SDL_SetRenderDrawColor(
        renderer,
        20,
        20,
        26,
        255
    );

    SDL_RenderClear(renderer);

    // タイトル
    drawText(
        "TETRIS",
        60,
        20,
        fontLarge,
        {240, 240, 240, 255}
    );

    // 盤面背景
    SDL_Rect boardRect =
    {
        BOARD_X,
        BOARD_Y,
        COLS * CELL,
        ROWS * CELL
    };

    drawRect(
        boardRect,
        {40, 40, 48, 255}
    );

    // グリッド
    SDL_SetRenderDrawColor(
        renderer,
        60,
        60,
        70,
        255
    );

    for (int x = 0; x <= COLS; x++)
    {
        SDL_RenderDrawLine(
            renderer,
            BOARD_X + x * CELL,
            BOARD_Y,
            BOARD_X + x * CELL,
            BOARD_Y + ROWS * CELL
        );
    }

    for (int y = 0; y <= ROWS; y++)
    {
        SDL_RenderDrawLine(
            renderer,
            BOARD_X,
            BOARD_Y + y * CELL,
            BOARD_X + COLS * CELL,
            BOARD_Y + y * CELL
        );
    }

    // 固定ブロック
    for (int y = 0; y < ROWS; y++)
    {
        for (int x = 0; x < COLS; x++)
        {
            if (board[y][x] != 0)
            {
                PieceType type =
                    static_cast<PieceType>(
                        board[y][x] - 1
                    );

                drawCell(
                    x,
                    y,
                    pieceColor(type)
                );
            }
        }
    }

    if (!gameOver)
    {
        // ゴースト
        Piece ghost =
            getGhostPiece();

        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                if (
                    SHAPES
                    [ghost.type]
                    [ghost.rotation]
                    [y]
                    [x]
                )
                {
                    int bx =
                        ghost.x + x;

                    int by =
                        ghost.y + y;

                    if (by >= 0)
                    {
                        drawCell(
                            bx,
                            by,
                            pieceColor(
                                ghost.type
                            ),
                            true
                        );
                    }
                }
            }
        }

        // 現在のミノ
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                if (
                    SHAPES
                    [currentPiece.type]
                    [currentPiece.rotation]
                    [y]
                    [x]
                )
                {
                    int bx =
                        currentPiece.x + x;

                    int by =
                        currentPiece.y + y;

                    if (by >= 0)
                    {
                        drawCell(
                            bx,
                            by,
                            pieceColor(
                                currentPiece.type
                            )
                        );
                    }
                }
            }
        }
    }

    // 右側情報
    drawText(
        "SCORE",
        410,
        100,
        fontSmall,
        {180, 180, 190, 255}
    );

    drawText(
        to_string(score),
        410,
        125,
        fontMedium,
        {255, 255, 255, 255}
    );

    drawText(
        "LINES",
        410,
        175,
        fontSmall,
        {180, 180, 190, 255}
    );

    drawText(
        to_string(lines),
        410,
        200,
        fontMedium,
        {255, 255, 255, 255}
    );

    drawText(
        "LEVEL",
        410,
        250,
        fontSmall,
        {180, 180, 190, 255}
    );

    drawText(
        to_string(level),
        410,
        275,
        fontMedium,
        {255, 255, 255, 255}
    );

    drawText(
        "HOLD",
        410,
        335,
        fontSmall,
        {180, 180, 190, 255}
    );

    if (hasHold)
    {
        drawMiniPiece(
            holdPiece,
            410,
            365
        );
    }

    drawText(
        "NEXT",
        535,
        335,
        fontSmall,
        {180, 180, 190, 255}
    );

    if (!nextQueue.empty())
    {
        drawMiniPiece(
            nextQueue[0],
            525,
            365
        );
    }

    drawText(
        "Keyboard",
        405,
        490,
        fontSmall,
        {220, 220, 220, 255}
    );

    drawText(
        "Arrow : Move",
        405,
        520,
        fontSmall,
        {170, 170, 180, 255}
    );

    drawText(
        "Z/X : Rotate",
        405,
        545,
        fontSmall,
        {170, 170, 180, 255}
    );

    drawText(
        "Space : Hard Drop",
        405,
        570,
        fontSmall,
        {170, 170, 180, 255}
    );

    drawText(
        "C : Hold",
        405,
        595,
        fontSmall,
        {170, 170, 180, 255}
    );

    drawText(
        "P : Pause",
        405,
        620,
        fontSmall,
        {170, 170, 180, 255}
    );

    if (paused)
    {
        drawText(
            "PAUSED",
            410,
            665,
            fontLarge,
            {255, 220, 80, 255}
        );
    }

    if (gameOver)
    {
        drawText(
            "GAME OVER",
            390,
            665,
            fontLarge,
            {255, 80, 80, 255}
        );

        drawText(
            "Press R",
            430,
            710,
            fontSmall,
            {255, 255, 255, 255}
        );
    }

    SDL_RenderPresent(renderer);
}

// ============================================================
// リスタート
// ============================================================

void restartGame()
{
    for (int y = 0; y < ROWS; y++)
    {
        for (int x = 0; x < COLS; x++)
        {
            board[y][x] = 0;
        }
    }

    score = 0;
    lines = 0;
    level = 1;

    gameOver = false;
    paused = false;

    hasHold = false;
    holdUsed = false;

    nextQueue.clear();

    fillNextQueue();
    spawnPiece();

    lastDropTime =
        SDL_GetTicks();
}

// ============================================================
// プロコン初期化
// ============================================================

void openController()
{
    for (
        int i = 0;
        i < SDL_NumJoysticks();
        i++
    )
    {
        if (
            SDL_IsGameController(i)
        )
        {
            controller =
                SDL_GameControllerOpen(i);

            if (controller)
            {
                cout
                    << "Controller connected: "
                    << SDL_GameControllerName(
                        controller
                    )
                    << endl;

                return;
            }
        }
    }

    cout
        << "Controller not found."
        << endl;
}

// ============================================================
// SDL初期化
// ============================================================

bool init()
{
    if (
        SDL_Init(
            SDL_INIT_VIDEO |
            SDL_INIT_GAMECONTROLLER
        ) < 0
    )
    {
        cout
            << "SDL error: "
            << SDL_GetError()
            << endl;

        return false;
    }

    if (
        TTF_Init() < 0
    )
    {
        cout
            << "TTF error: "
            << TTF_GetError()
            << endl;

        return false;
    }

    window =
        SDL_CreateWindow(
            "Tetris",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_W,
            WINDOW_H,
            SDL_WINDOW_SHOWN
        );

    if (!window)
    {
        return false;
    }

    renderer =
        SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC
        );

    if (!renderer)
    {
        renderer =
            SDL_CreateRenderer(
                window,
                -1,
                SDL_RENDERER_SOFTWARE
            );
    }

    if (!renderer)
    {
        return false;
    }

    const char* fontPath =
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";

    fontLarge =
        TTF_OpenFont(
            fontPath,
            36
        );

    fontMedium =
        TTF_OpenFont(
            fontPath,
            26
        );

    fontSmall =
        TTF_OpenFont(
            fontPath,
            16
        );

    if (
        !fontLarge ||
        !fontMedium ||
        !fontSmall
    )
    {
        cout
            << "Font error: "
            << TTF_GetError()
            << endl;

        return false;
    }

    openController();

    return true;
}

// ============================================================
// 終了
// ============================================================

void cleanup()
{
    if (controller)
    {
        SDL_GameControllerClose(
            controller
        );
    }

    if (fontLarge)
        TTF_CloseFont(fontLarge);

    if (fontMedium)
        TTF_CloseFont(fontMedium);

    if (fontSmall)
        TTF_CloseFont(fontSmall);

    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

// ============================================================
// キーボード
// ============================================================

void handleKeyboard(SDL_Keycode key)
{
    if (key == SDLK_ESCAPE)
    {
        running = false;
        return;
    }

    if (key == SDLK_r)
    {
        restartGame();
        return;
    }

    if (key == SDLK_p)
    {
        paused = !paused;
        return;
    }

    if (
        paused ||
        gameOver
    )
    {
        return;
    }

    switch (key)
    {
        case SDLK_LEFT:
            moveHorizontal(-1);
            break;

        case SDLK_RIGHT:
            moveHorizontal(1);
            break;

        case SDLK_DOWN:
            softDrop();
            break;

        case SDLK_z:
            rotatePiece(-1);
            break;

        case SDLK_x:
        case SDLK_UP:
            rotatePiece(1);
            break;

        case SDLK_SPACE:
            hardDrop();
            break;

        case SDLK_c:
            holdCurrent();
            break;
    }
}

// ============================================================
// プロコン
// ============================================================

void handleControllerButton(
    SDL_GameControllerButton button
)
{
    if (button == SDL_CONTROLLER_BUTTON_START)
    {
        paused = !paused;
        return;
    }

    if (
        paused ||
        gameOver
    )
    {
        return;
    }

    switch (button)
    {
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            moveHorizontal(-1);
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            moveHorizontal(1);
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            softDrop();
            break;

        // B = Hard Drop
        case SDL_CONTROLLER_BUTTON_B:
            hardDrop();
            break;

        // X = Hold
        case SDL_CONTROLLER_BUTTON_X:
            holdCurrent();
            break;

        default:
            break;
    }
}

// ============================================================
// プロコン L / R
// ============================================================

void handleControllerAxisOrShoulder(
    SDL_GameControllerButton button
)
{
    if (
        paused ||
        gameOver
    )
    {
        return;
    }

    if (
        button ==
        SDL_CONTROLLER_BUTTON_LEFTSHOULDER
    )
    {
        // L = 左回転
        rotatePiece(-1);
    }

    else if (
        button ==
        SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
    )
    {
        // R = 右回転
        rotatePiece(1);
    }
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    if (!init())
    {
        return 1;
    }

    restartGame();

    while (running)
    {
        SDL_Event event;

        while (
            SDL_PollEvent(&event)
        )
        {
            if (
                event.type ==
                SDL_QUIT
            )
            {
                running = false;
            }

            else if (
                event.type ==
                SDL_KEYDOWN
            )
            {
                if (
                    event.key.repeat == 0
                )
                {
                    handleKeyboard(
                        event.key.keysym.sym
                    );
                }
            }

            else if (
                event.type ==
                SDL_CONTROLLERBUTTONDOWN
            )
            {
                SDL_GameControllerButton button =
                    static_cast<SDL_GameControllerButton>(
                        event.cbutton.button
                    );

                handleControllerButton(
                    button
                );

                handleControllerAxisOrShoulder(
                    button
                );
            }

            else if (
                event.type ==
                SDL_CONTROLLERDEVICEADDED
            )
            {
                if (!controller)
                {
                    openController();
                }
            }

            else if (
                event.type ==
                SDL_CONTROLLERDEVICEREMOVED
            )
            {
                if (controller)
                {
                    SDL_Joystick* joystick =
                        SDL_GameControllerGetJoystick(
                            controller
                        );

                    SDL_JoystickID id =
                        SDL_JoystickInstanceID(
                            joystick
                        );

                    if (
                        id ==
                        event.cdevice.which
                    )
                    {
                        SDL_GameControllerClose(
                            controller
                        );

                        controller =
                            nullptr;
                    }
                }
            }
        }

        // ====================================================
        // 自動落下
        // ====================================================

        if (
            !paused &&
            !gameOver
        )
        {
            int interval =
                DROP_INTERVAL_START -
                (level - 1) * 55;

            if (interval < 90)
            {
                interval = 90;
            }

            Uint32 now =
                SDL_GetTicks();

            if (
                now - lastDropTime >=
                static_cast<Uint32>(interval)
            )
            {
                if (
                    isValid(
                        currentPiece,
                        0,
                        1
                    )
                )
                {
                    currentPiece.y++;
                }
                else
                {
                    lockPiece();
                }

                lastDropTime = now;
            }
        }

        drawGame();

        SDL_Delay(8);
    }

    cleanup();

    return 0;
}
