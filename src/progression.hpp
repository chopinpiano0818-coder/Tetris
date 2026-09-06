#pragma once

#include "types.hpp"

// ============================================================
// TETRIS COMPLETE
// progression.hpp
//
// プレイヤーXP / レベル管理
// ============================================================

class Progression
{
public:
    Progression();

    // プレイ結果からXPを加算
    ProgressionResult applyResult(
        const RunResult& result
    );

    // --------------------------------------------------------
    // Getters
    // --------------------------------------------------------

    int playerLevel() const;
    int currentXP() const;

    // 現在Lv → 次Lvまでに必要なXP
    int xpForNextLevel() const;

    // --------------------------------------------------------
    // SaveManager用
    // --------------------------------------------------------

    void setPlayerLevel(int level);
    void setCurrentXP(int xp);

    // 初期状態へ
    void reset();

    // 指定Lvに到達するために必要なXP
    static int requiredXPForLevel(
        int level
    );

private:
    void checkLevelUp();

private:
    int mPlayerLevel = 1;

    // 現在レベル内で持っているXP
    int mCurrentXP = 0;

    static constexpr int MAX_LEVEL = 100;
};
