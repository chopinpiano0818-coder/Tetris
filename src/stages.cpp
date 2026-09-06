#include "stages.hpp"

#include <algorithm>

// ============================================================
// Constructor
// ============================================================

StageManager::StageManager(
    Progression& progression
)
    : mProgression(&progression)
{
    createStages();
    refreshUnlocks();

    // 起動直後に大量の「新規解禁」が出ないようにする
    mNewlyUnlocked.clear();
}

// ============================================================
// Stage definitions
// ============================================================

void StageManager::createStages()
{
    mStages.clear();

    // --------------------------------------------------------
    // id
    // 名前
    // 必要プレイヤーレベル
    // --------------------------------------------------------

    mStages.push_back(
        {
            0,
            "CLASSIC",
            1,
            false
        }
    );

    mStages.push_back(
        {
            1,
            "NEON CITY",
            3,
            false
        }
    );

    mStages.push_back(
        {
            2,
            "SUNSET",
            5,
            false
        }
    );

    mStages.push_back(
        {
            3,
            "ARCTIC",
            8,
            false
        }
    );

    mStages.push_back(
        {
            4,
            "VOLCANO",
            12,
            false
        }
    );

    mStages.push_back(
        {
            5,
            "CYBER GRID",
            16,
            false
        }
    );

    mStages.push_back(
        {
            6,
            "SPACE",
            22,
            false
        }
    );

    mStages.push_back(
        {
            7,
            "VOID",
            30,
            false
        }
    );

    mStages.push_back(
        {
            8,
            "GALAXY",
            40,
            false
        }
    );

    mStages.push_back(
        {
            9,
            "MASTER",
            50,
            false
        }
    );
}

// ============================================================
// Unlock
// ============================================================

void StageManager::refreshUnlocks()
{
    if (!mProgression)
    {
        return;
    }

    mNewlyUnlocked.clear();

    const int level =
        mProgression->playerLevel();

    for (auto& stage : mStages)
    {
        const bool wasUnlocked =
            stage.unlocked;

        const bool shouldUnlock =
            level >= stage.requiredLevel;

        stage.unlocked =
            shouldUnlock;

        if (
            !wasUnlocked &&
            shouldUnlock
        )
        {
            mNewlyUnlocked.push_back(
                stage.id
            );
        }
    }
}

// ============================================================
// Getters
// ============================================================

const std::vector<StageInfo>&
StageManager::stages() const
{
    return mStages;
}

const StageInfo&
StageManager::stage(
    int id
) const
{
    if (
        id < 0 ||
        id >=
            static_cast<int>(
                mStages.size()
            )
    )
    {
        return mStages.front();
    }

    return mStages[id];
}

int StageManager::stageCount() const
{
    return static_cast<int>(
        mStages.size()
    );
}

bool StageManager::isUnlocked(
    int id
) const
{
    if (
        id < 0 ||
        id >=
            static_cast<int>(
                mStages.size()
            )
    )
    {
        return false;
    }

    return mStages[id].unlocked;
}

const std::vector<int>&
StageManager::newlyUnlockedStages() const
{
    return mNewlyUnlocked;
}

void StageManager::clearNewlyUnlocked()
{
    mNewlyUnlocked.clear();
}
