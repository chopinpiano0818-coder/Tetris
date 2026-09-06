#include "renderer3d.hpp"

#include <algorithm>
#include <cmath>

// ============================================================
// 内部用
// ============================================================

namespace
{
    constexpr float PI =
        3.14159265358979323846f;

    struct Point2
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Point3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    // --------------------------------------------------------
    // 色
    // --------------------------------------------------------

    SDL_Color pieceColor(
        TetrominoType type,
        Uint8 alpha = 255
    )
    {
        SDL_Color color
        {
            220,
            220,
            220,
            alpha
        };

        switch (type)
        {
            case TetrominoType::I:
                color = {50, 220, 240, alpha};
                break;

            case TetrominoType::O:
                color = {245, 220, 50, alpha};
                break;

            case TetrominoType::T:
                color = {180, 80, 220, alpha};
                break;

            case TetrominoType::S:
                color = {70, 210, 100, alpha};
                break;

            case TetrominoType::Z:
                color = {230, 65, 70, alpha};
                break;

            case TetrominoType::J:
                color = {65, 100, 230, alpha};
                break;

            case TetrominoType::L:
                color = {240, 145, 45, alpha};
                break;

            default:
                break;
        }

        return color;
    }

    Uint8 darker(
        Uint8 value,
        float factor
    )
    {
        return static_cast<Uint8>(
            std::clamp(
                static_cast<int>(
                    value * factor
                ),
                0,
                255
            )
        );
    }

    Uint8 brighter(
        Uint8 value,
        int amount
    )
    {
        return static_cast<Uint8>(
            std::clamp(
                static_cast<int>(value) +
                amount,
                0,
                255
            )
        );
    }

    // --------------------------------------------------------
    // 3D回転
    // --------------------------------------------------------

    Point3 rotatePoint(
        Point3 p,
        float yawDegrees,
        float pitchDegrees
    )
    {
        const float yaw =
            yawDegrees *
            PI /
            180.0f;

        const float pitch =
            pitchDegrees *
            PI /
            180.0f;

        // Y軸回転
        const float x1 =
            p.x * std::cos(yaw) -
            p.z * std::sin(yaw);

        const float z1 =
            p.x * std::sin(yaw) +
            p.z * std::cos(yaw);

        p.x = x1;
        p.z = z1;

        // X軸回転
        const float y1 =
            p.y * std::cos(pitch) -
            p.z * std::sin(pitch);

        const float z2 =
            p.y * std::sin(pitch) +
            p.z * std::cos(pitch);

        p.y = y1;
        p.z = z2;

        return p;
    }

    // --------------------------------------------------------
    // 投影
    // --------------------------------------------------------

    Point2 project(
        const Point3& point,
        int centerX,
        int centerY,
        float scale
    )
    {
        // 軽い遠近感
        const float perspective =
            1.0f /
            std::max(
                0.45f,
                1.0f +
                point.z * 0.035f
            );

        Point2 result;

        result.x =
            centerX +
            point.x *
            scale *
            perspective;

        result.y =
            centerY +
            point.y *
            scale *
            perspective;

        return result;
    }

    // --------------------------------------------------------
    // ポリゴン塗りつぶし
    //
    // SDL_RenderGeometryを使用
    // --------------------------------------------------------

    void fillQuad(
        SDL_Renderer* renderer,
        const Point2& a,
        const Point2& b,
        const Point2& c,
        const Point2& d,
        SDL_Color color
    )
    {
#if SDL_VERSION_ATLEAST(2,0,18)

        SDL_Vertex vertices[4];

        vertices[0].position =
        {
            a.x,
            a.y
        };

        vertices[1].position =
        {
            b.x,
            b.y
        };

        vertices[2].position =
        {
            c.x,
            c.y
        };

        vertices[3].position =
        {
            d.x,
            d.y
        };

        for (auto& vertex : vertices)
        {
            vertex.color = color;

            vertex.tex_coord =
            {
                0.0f,
                0.0f
            };
        }

        const int indices[6] =
        {
            0, 1, 2,
            0, 2, 3
        };

        SDL_RenderGeometry(
            renderer,
            nullptr,
            vertices,
            4,
            indices,
            6
        );

#else

        // 古いSDL2用フォールバック
        SDL_SetRenderDrawColor(
            renderer,
            color.r,
            color.g,
            color.b,
            color.a
        );

        SDL_Point points[5] =
        {
            {
                static_cast<int>(a.x),
                static_cast<int>(a.y)
            },
            {
                static_cast<int>(b.x),
                static_cast<int>(b.y)
            },
            {
                static_cast<int>(c.x),
                static_cast<int>(c.y)
            },
            {
                static_cast<int>(d.x),
                static_cast<int>(d.y)
            },
            {
                static_cast<int>(a.x),
                static_cast<int>(a.y)
            }
        };

        SDL_RenderDrawLines(
            renderer,
            points,
            5
        );

#endif
    }

    // --------------------------------------------------------
    // 立方体
    // --------------------------------------------------------

    void drawCube(
        SDL_Renderer* renderer,
        float blockX,
        float blockY,
        TetrominoType type,
        const Camera3D& camera,
        int centerX,
        int centerY,
        float scale,
        Uint8 alpha
    )
    {
        // 盤面中央を原点にする
        const float x =
            blockX -
            Game::BOARD_WIDTH /
            2.0f;

        const float y =
            blockY -
            Game::BOARD_HEIGHT /
            2.0f;

        constexpr float depth =
            0.75f;

        Point3 vertices[8] =
        {
            {x,     y,     0.0f},
            {x + 1, y,     0.0f},
            {x + 1, y + 1, 0.0f},
            {x,     y + 1, 0.0f},

            {x,     y,     depth},
            {x + 1, y,     depth},
            {x + 1, y + 1, depth},
            {x,     y + 1, depth}
        };

        Point2 projected[8];

        for (int i = 0; i < 8; ++i)
        {
            Point3 rotated =
                rotatePoint(
                    vertices[i],
                    camera.yaw,
                    camera.pitch
                );

            projected[i] =
                project(
                    rotated,
                    centerX,
                    centerY,
                    scale * camera.zoom
                );
        }

        SDL_Color base =
            pieceColor(
                type,
                alpha
            );

        SDL_Color side =
        {
            darker(base.r, 0.60f),
            darker(base.g, 0.60f),
            darker(base.b, 0.60f),
            alpha
        };

        SDL_Color top =
        {
            brighter(base.r, 30),
            brighter(base.g, 30),
            brighter(base.b, 30),
            alpha
        };

        SDL_Color front =
            base;

        // 背面
        fillQuad(
            renderer,
            projected[4],
            projected[5],
            projected[6],
            projected[7],
            side
        );

        // 左側
        fillQuad(
            renderer,
            projected[0],
            projected[4],
            projected[7],
            projected[3],
            side
        );

        // 右側
        fillQuad(
            renderer,
            projected[1],
            projected[5],
            projected[6],
            projected[2],
            side
        );

        // 上面
        fillQuad(
            renderer,
            projected[0],
            projected[1],
            projected[5],
            projected[4],
            top
        );

        // 下面
        fillQuad(
            renderer,
            projected[3],
            projected[2],
            projected[6],
            projected[7],
            side
        );

        // 正面
        fillQuad(
            renderer,
            projected[0],
            projected[1],
            projected[2],
            projected[3],
            front
        );

        // 輪郭
        SDL_SetRenderDrawColor(
            renderer,
            10,
            10,
            15,
            alpha
        );

        SDL_Point outline[5] =
        {
            {
                static_cast<int>(
                    projected[0].x
                ),
                static_cast<int>(
                    projected[0].y
                )
            },
            {
                static_cast<int>(
                    projected[1].x
                ),
                static_cast<int>(
                    projected[1].y
                )
            },
            {
                static_cast<int>(
                    projected[2].x
                ),
                static_cast<int>(
                    projected[2].y
                )
            },
            {
                static_cast<int>(
                    projected[3].x
                ),
                static_cast<int>(
                    projected[3].y
                )
            },
            {
                static_cast<int>(
                    projected[0].x
                ),
                static_cast<int>(
                    projected[0].y
                )
            }
        };

        SDL_RenderDrawLines(
            renderer,
            outline,
            5
        );
    }

    // --------------------------------------------------------
    // Board frame
    // --------------------------------------------------------

    void drawBoardFrame(
        SDL_Renderer* renderer,
        const Camera3D& camera,
        int centerX,
        int centerY,
        float scale
    )
    {
        Point3 corners[4] =
        {
            {
                -Game::BOARD_WIDTH / 2.0f,
                -Game::BOARD_HEIGHT / 2.0f,
                0.0f
            },

            {
                Game::BOARD_WIDTH / 2.0f,
                -Game::BOARD_HEIGHT / 2.0f,
                0.0f
            },

            {
                Game::BOARD_WIDTH / 2.0f,
                Game::BOARD_HEIGHT / 2.0f,
                0.0f
            },

            {
                -Game::BOARD_WIDTH / 2.0f,
                Game::BOARD_HEIGHT / 2.0f,
                0.0f
            }
        };

        Point2 projected[4];

        for (int i = 0; i < 4; ++i)
        {
            Point3 rotated =
                rotatePoint(
                    corners[i],
                    camera.yaw,
                    camera.pitch
                );

            projected[i] =
                project(
                    rotated,
                    centerX,
                    centerY,
                    scale *
                    camera.zoom
                );
        }

        SDL_SetRenderDrawColor(
            renderer,
            180,
            190,
            220,
            170
        );

        SDL_Point outline[5];

        for (int i = 0; i < 4; ++i)
        {
            outline[i] =
            {
                static_cast<int>(
                    projected[i].x
                ),
                static_cast<int>(
                    projected[i].y
                )
            };
        }

        outline[4] =
            outline[0];

        SDL_RenderDrawLines(
            renderer,
            outline,
            5
        );
    }
}

// ============================================================
// Public render
// ============================================================

void renderGame3D(
    SDL_Renderer* renderer,
    const Game& game,
    const Settings& settings
)
{
    int width = 0;
    int height = 0;

    SDL_GetRendererOutputSize(
        renderer,
        &width,
        &height
    );

    const int centerX =
        width / 2 - 90;

    const int centerY =
        height / 2;

    const float scale =
        std::max(
            17.0f,
            std::min(
                30.0f,
                (height - 80.0f) /
                Game::BOARD_HEIGHT
            )
        );

    const Camera3D& camera =
        game.camera();

    // --------------------------------------------------------
    // Board frame
    // --------------------------------------------------------

    drawBoardFrame(
        renderer,
        camera,
        centerX,
        centerY,
        scale
    );

    // --------------------------------------------------------
    // Ghost
    // --------------------------------------------------------

    if (settings.showGhost)
    {
        const Tetromino& piece =
            game.currentPiece();

        const int ghostY =
            game.ghostY();

        for (
            const auto& cell :
            piece.cells()
        )
        {
            const int x =
                piece.x() +
                cell.x;

            const int y =
                ghostY +
                cell.y;

            if (y >= 0)
            {
                drawCube(
                    renderer,
                    static_cast<float>(x),
                    static_cast<float>(y),
                    piece.type(),
                    camera,
                    centerX,
                    centerY,
                    scale,
                    45
                );
            }
        }
    }

    // --------------------------------------------------------
    // 固定ブロック
    // --------------------------------------------------------

    const auto& board =
        game.board();

    // 奥側から描くため下から
    for (
        int y =
            Game::BOARD_HEIGHT - 1;
        y >= 0;
        --y
    )
    {
        for (
            int x = 0;
            x < Game::BOARD_WIDTH;
            ++x
        )
        {
            const TetrominoType type =
                board[y][x];

            if (
                type ==
                TetrominoType::None
            )
            {
                continue;
            }

            drawCube(
                renderer,
                static_cast<float>(x),
                static_cast<float>(y),
                type,
                camera,
                centerX,
                centerY,
                scale,
                255
            );
        }
    }

    // --------------------------------------------------------
    // 操作中のミノ
    // --------------------------------------------------------

    const Tetromino& current =
        game.currentPiece();

    for (
        const auto& cell :
        current.cells()
    )
    {
        const int x =
            current.x() +
            cell.x;

        const int y =
            current.y() +
            cell.y;

        if (y < 0)
        {
            continue;
        }

        drawCube(
            renderer,
            static_cast<float>(x),
            static_cast<float>(y),
            current.type(),
            camera,
            centerX,
            centerY,
            scale,
            255
        );
    }

    // --------------------------------------------------------
    // Hard Drop impact
    // --------------------------------------------------------

    if (
        settings.animationsEnabled &&
        game.hardDropAnimation() > 0.0f
    )
    {
        const float strength =
            game.hardDropAnimation() *
            settings.animationIntensity;

        const int radius =
            static_cast<int>(
                25.0f +
                60.0f *
                strength
            );

        SDL_SetRenderDrawColor(
            renderer,
            255,
            255,
            255,
            static_cast<Uint8>(
                80.0f *
                strength
            )
        );

        SDL_Rect impact
        {
            centerX - radius,
            centerY - radius,
            radius * 2,
            radius * 2
        };

        SDL_RenderDrawRect(
            renderer,
            &impact
        );
    }

    // --------------------------------------------------------
    // Line clear flash
    // --------------------------------------------------------

    if (
        settings.lineClearFlash &&
        game.lineClearAnimation() > 0.0f
    )
    {
        const Uint8 alpha =
            static_cast<Uint8>(
                60.0f *
                game.lineClearAnimation() *
                settings.animationIntensity
            );

        SDL_SetRenderDrawColor(
            renderer,
            255,
            255,
            255,
            alpha
        );

        SDL_Rect flash
        {
            0,
            0,
            width,
            height
        };

        SDL_RenderFillRect(
            renderer,
            &flash
        );
    }
}
