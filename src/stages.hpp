#pragma once

#include <vector>

#include "types.hpp"
#include "progression.hpp"

// ============================================================
// TETRIS COMPLETE
// stages.hpp
//
// ステージ管理
// ============================================================

class StageManager
{
public:
    explicit StageManager(
        Progression& progression
    );

    // プレイヤーレベルに応じて解禁状態を更新
    void refreshUnlocks();

    // --------------------------------------------------------
    // Getters
    // --------------------------------------------------------

    const std::vector<StageInfo>&
    stages() const;

    const StageInfo&
    stage(int id) const;

    int stageCount() const;

    bool isUnlocked(
        int id
    ) const;

    // 今回の更新で新しく解禁されたステージ
    const std::vector<int>&
    newlyUnlockedStages() const;

    void clearNewlyUnlocked();

private:
    void createStages();

private:
    Progression* mProgression = nullptr;

    std::vector<StageInfo>
        mStages;

    std::vector<int>
        mNewlyUnlocked;
};
