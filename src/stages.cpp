#include "stages.hpp"

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

    // 起動時は「新しく解放！」扱いにしない
    mNewlyUnlocked.clear();
}

// ============================================================
// Stage definitions
// ============================================================

void StageManager::createStages()
{
    mStages.clear();

    mStages.push_back({
        0,
        "クラシック",
        1,
        false
    });

    mStages.push_back({
        1,
        "ネオンシティ",
        3,
        false
    });

    mStages.push_back({
        2,
        "サンセット",
        5,
        false
    });

    mStages.push_back({
        3,
        "アイスワールド",
        8,
        false
    });

    mStages.push_back({
        4,
        "ボルケーノ",
        12,
        false
    });

    mStages.push_back({
        5,
        "サイバーグリッド",
        16,
        false
    });

    mStages.push_back({
        6,
        "スペース",
        22,
        false
    });

    mStages.push_back({
        7,
        "ヴォイド",
        30,
        false
    });

    mStages.push_back({
        8,
        "ギャラクシー",
        40,
        false
    });

    mStages.push_back({
        9,
        "マスター",
        50,
        false
    });
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

        stage.unlocked =
            level >= stage.requiredLevel;

        if (
            !wasUnlocked &&
            stage.unlocked
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
        id >= static_cast<int>(
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
        id >= static_cast<int>(
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
