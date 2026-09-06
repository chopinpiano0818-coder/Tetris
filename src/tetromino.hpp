#pragma once

#include <array>

#include "types.hpp"

// ============================================================
// 1マス分の相対座標
// ============================================================

struct BlockCell
{
    int x = 0;
    int y = 0;
};

// ============================================================
// Tetromino
// ============================================================

class Tetromino
{
public:
    Tetromino();
    explicit Tetromino(TetrominoType type);

    void setType(TetrominoType type);

    TetrominoType type() const;

    // --------------------------------------------------------
    // Position
    // --------------------------------------------------------

    int x() const;
    int y() const;

    void setPosition(int x, int y);
    void move(int dx, int dy);

    // --------------------------------------------------------
    // Rotation
    // --------------------------------------------------------

    int rotation() const;

    void setRotation(int rotation);

    void rotateLeft();
    void rotateRight();

    // Aボタン用
    // 現在の向きを180度変える
    void flip();

    // --------------------------------------------------------
    // Cells
    // --------------------------------------------------------

    const std::array<BlockCell, 4>& cells() const;

    // 任意回転状態のセル
    const std::array<BlockCell, 4>& cellsAt(
        int rotation
    ) const;

    // --------------------------------------------------------
    // Utility
    // --------------------------------------------------------

    static const char* name(
        TetrominoType type
    );

private:
    TetrominoType mType =
        TetrominoType::None;

    int mX = 3;
    int mY = 0;

    int mRotation = 0;
};
