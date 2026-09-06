#include "progression.hpp"

#include <algorithm>

// ============================================================
// Constructor
// ============================================================

Progression::Progression()
{
    reset();
}

// ============================================================
// Reset
// ============================================================

void Progression::reset()
{
    mPlayerLevel = 1;
    mCurrentXP = 0;
}

// ============================================================
// XP
// ============================================================

ProgressionResult Progression::applyResult(
    const RunResult& result
)
{
    ProgressionResult progress;

    progress.oldLevel =
        mPlayerLevel;

    progress.xpEarned =
        std::max(
            0,
            result.earnedXP
        );

    mCurrentXP +=
        progress.xpEarned;

    checkLevelUp();

    progress.newLevel =
        mPlayerLevel;

    progress.levelUp =
        progress.newLevel >
        progress.oldLevel;

    // ステージ解禁数はStageManager側で更新する
    progress.stagesUnlocked = 0;

    return progress;
}

// ============================================================
// Level up
// ============================================================

void Progression::checkLevelUp()
{
    while (
        mPlayerLevel < MAX_LEVEL
    )
    {
        const int needed =
            xpForNextLevel();

        if (mCurrentXP < needed)
        {
            break;
        }

        mCurrentXP -= needed;

        ++mPlayerLevel;
    }

    if (mPlayerLevel >= MAX_LEVEL)
    {
        mPlayerLevel = MAX_LEVEL;

        // Lv100ではゲージを上限以内にする
        mCurrentXP =
            std::min(
                mCurrentXP,
                xpForNextLevel()
            );
    }
}

// ============================================================
// XP curve
// ============================================================

int Progression::requiredXPForLevel(
    int level
)
{
    level =
        std::max(
            1,
            level
        );

    // --------------------------------------------------------
    // Lv1 → 2   : 500
    // Lv2 → 3   : 650
    // Lv3 → 4   : 800
    // ...
    //
    // 徐々に必要XPが増える
    // --------------------------------------------------------

    return
        500 +
        (level - 1) * 150;
}

int Progression::xpForNextLevel() const
{
    return requiredXPForLevel(
        mPlayerLevel
    );
}

// ============================================================
// Getters
// ============================================================

int Progression::playerLevel() const
{
    return mPlayerLevel;
}

int Progression::currentXP() const
{
    return mCurrentXP;
}

// ============================================================
// Save loading
// ============================================================

void Progression::setPlayerLevel(
    int level
)
{
    mPlayerLevel =
        std::clamp(
            level,
            1,
            MAX_LEVEL
        );
}

void Progression::setCurrentXP(
    int xp
)
{
    mCurrentXP =
        std::max(
            0,
            xp
        );

    // セーブデータのXPが大きすぎても
    // 正常にレベルアップさせる
    checkLevelUp();
}
