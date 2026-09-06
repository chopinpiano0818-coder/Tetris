#include "tetromino.hpp"

// ============================================================
// Shape table
//
// rotation:
// 0 = 初期
// 1 = 右90
// 2 = 180
// 3 = 左90
// ============================================================

using Shape =
    std::array<
        std::array<BlockCell, 4>,
        4
    >;

// ------------------------------------------------------------
// I
// ------------------------------------------------------------

static const Shape SHAPE_I =
{{
    {{
        {0, 1},
        {1, 1},
        {2, 1},
        {3, 1}
    }},
    {{
        {2, 0},
        {2, 1},
        {2, 2},
        {2, 3}
    }},
    {{
        {0, 2},
        {1, 2},
        {2, 2},
        {3, 2}
    }},
    {{
        {1, 0},
        {1, 1},
        {1, 2},
        {1, 3}
    }}
}};

// ------------------------------------------------------------
// O
// ------------------------------------------------------------

static const Shape SHAPE_O =
{{
    {{
        {1, 0},
        {2, 0},
        {1, 1},
        {2, 1}
    }},
    {{
        {1, 0},
        {2, 0},
        {1, 1},
        {2, 1}
    }},
    {{
        {1, 0},
        {2, 0},
        {1, 1},
        {2, 1}
    }},
    {{
        {1, 0},
        {2, 0},
        {1, 1},
        {2, 1}
    }}
}};

// ------------------------------------------------------------
// T
// ------------------------------------------------------------

static const Shape SHAPE_T =
{{
    {{
        {1, 0},
        {0, 1},
        {1, 1},
        {2, 1}
    }},
    {{
        {1, 0},
        {1, 1},
        {2, 1},
        {1, 2}
    }},
    {{
        {0, 1},
        {1, 1},
        {2, 1},
        {1, 2}
    }},
    {{
        {1, 0},
        {0, 1},
        {1, 1},
        {1, 2}
    }}
}};

// ------------------------------------------------------------
// S
// ------------------------------------------------------------

static const Shape SHAPE_S =
{{
    {{
        {1, 0},
        {2, 0},
        {0, 1},
        {1, 1}
    }},
    {{
        {1, 0},
        {1, 1},
        {2, 1},
        {2, 2}
    }},
    {{
        {1, 1},
        {2, 1},
        {0, 2},
        {1, 2}
    }},
    {{
        {0, 0},
        {0, 1},
        {1, 1},
        {1, 2}
    }}
}};

// ------------------------------------------------------------
// Z
// ------------------------------------------------------------

static const Shape SHAPE_Z =
{{
    {{
        {0, 0},
        {1, 0},
        {1, 1},
        {2, 1}
    }},
    {{
        {2, 0},
        {1, 1},
        {2, 1},
        {1, 2}
    }},
    {{
        {0, 1},
        {1, 1},
        {1, 2},
        {2, 2}
    }},
    {{
        {1, 0},
        {0, 1},
        {1, 1},
        {0, 2}
    }}
}};

// ------------------------------------------------------------
// J
// ------------------------------------------------------------

static const Shape SHAPE_J =
{{
    {{
        {0, 0},
        {0, 1},
        {1, 1},
        {2, 1}
    }},
    {{
        {1, 0},
        {2, 0},
        {1, 1},
        {1, 2}
    }},
    {{
        {0, 1},
        {1, 1},
        {2, 1},
        {2, 2}
    }},
    {{
        {1, 0},
        {1, 1},
        {0, 2},
        {1, 2}
    }}
}};

// ------------------------------------------------------------
// L
// ------------------------------------------------------------

static const Shape SHAPE_L =
{{
    {{
        {2, 0},
        {0, 1},
        {1, 1},
        {2, 1}
    }},
    {{
        {1, 0},
        {1, 1},
        {1, 2},
        {2, 2}
    }},
    {{
        {0, 1},
        {1, 1},
        {2, 1},
        {0, 2}
    }},
    {{
        {0, 0},
        {1, 0},
        {1, 1},
        {1, 2}
    }}
}};

// ============================================================
// Constructors
// ============================================================

Tetromino::Tetromino()
{
}

Tetromino::Tetromino(
    TetrominoType type
)
{
    setType(type);
}

// ============================================================
// Type
// ============================================================

void Tetromino::setType(
    TetrominoType type
)
{
    mType = type;
    mRotation = 0;

    // 10マス盤面の中央付近
    mX = 3;
    mY = 0;
}

TetrominoType Tetromino::type() const
{
    return mType;
}

// ============================================================
// Position
// ============================================================

int Tetromino::x() const
{
    return mX;
}

int Tetromino::y() const
{
    return mY;
}

void Tetromino::setPosition(
    int x,
    int y
)
{
    mX = x;
    mY = y;
}

void Tetromino::move(
    int dx,
    int dy
)
{
    mX += dx;
    mY += dy;
}

// ============================================================
// Rotation
// ============================================================

int Tetromino::rotation() const
{
    return mRotation;
}

void Tetromino::setRotation(
    int rotation
)
{
    rotation %= 4;

    if (rotation < 0)
    {
        rotation += 4;
    }

    mRotation = rotation;
}

void Tetromino::rotateLeft()
{
    setRotation(
        mRotation - 1
    );
}

void Tetromino::rotateRight()
{
    setRotation(
        mRotation + 1
    );
}

// Aボタン用
void Tetromino::flip()
{
    setRotation(
        mRotation + 2
    );
}

// ============================================================
// Shape access
// ============================================================

static const Shape& getShape(
    TetrominoType type
)
{
    switch (type)
    {
        case TetrominoType::I:
            return SHAPE_I;

        case TetrominoType::O:
            return SHAPE_O;

        case TetrominoType::T:
            return SHAPE_T;

        case TetrominoType::S:
            return SHAPE_S;

        case TetrominoType::Z:
            return SHAPE_Z;

        case TetrominoType::J:
            return SHAPE_J;

        case TetrominoType::L:
            return SHAPE_L;

        default:
            return SHAPE_O;
    }
}

const std::array<BlockCell, 4>&
Tetromino::cells() const
{
    return getShape(
        mType
    )[mRotation];
}

const std::array<BlockCell, 4>&
Tetromino::cellsAt(
    int rotation
) const
{
    rotation %= 4;

    if (rotation < 0)
    {
        rotation += 4;
    }

    return getShape(
        mType
    )[rotation];
}

// ============================================================
// Name
// ============================================================

const char* Tetromino::name(
    TetrominoType type
)
{
    switch (type)
    {
        case TetrominoType::I:
            return "I";

        case TetrominoType::O:
            return "O";

        case TetrominoType::T:
            return "T";

        case TetrominoType::S:
            return "S";

        case TetrominoType::Z:
            return "Z";

        case TetrominoType::J:
            return "J";

        case TetrominoType::L:
            return "L";

        default:
            return "NONE";
    }
}
