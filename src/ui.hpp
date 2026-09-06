#pragma once

#include "types.hpp"
#include "progression.hpp"
#include "stages.hpp"
#include "settings.hpp"

class InputManager;

// ============================================================
// TETRIS COMPLETE
// ui.hpp
// ============================================================

class UI
{
public:
    UI(
        Progression& progression,
        StageManager& stageManager,
        Settings& settings
    );

    // --------------------------------------------------------
    // Update
    // --------------------------------------------------------

    UIAction updateHome(
        InputManager& input,
        float deltaTime,
        RenderStyle& selectedStyle
    );

    UIAction updateModeSelect(
        InputManager& input,
        float deltaTime,
        GameMode& selectedMode
    );

    UIAction updateStageSelect(
        InputManager& input,
        float deltaTime,
        int& selectedStage
    );

    UIAction updateResult(
        InputManager& input,
        float deltaTime
    );

    UIAction updateSettings(
        InputManager& input,
        float deltaTime,
        Settings& settings
    );

    // --------------------------------------------------------
    // Result
    // --------------------------------------------------------

    void showResult(
        const RunResult& result,
        const ProgressionResult& progressionResult
    );

    // --------------------------------------------------------
    // Selection getters
    // Rendererから使う
    // --------------------------------------------------------

    int homeSelection() const;
    int modeSelection() const;
    int stageSelection() const;
    int resultSelection() const;
    int settingsSelection() const;

    const RunResult& lastRunResult() const;
    const ProgressionResult& lastProgressionResult() const;

    // --------------------------------------------------------
    // Animations
    // --------------------------------------------------------

    float titleAnimation() const;
    float selectionPulse() const;
    float transitionAnimation() const;

private:
    void updateCommonAnimations(
        float deltaTime
    );

private:
    Progression* mProgression = nullptr;
    StageManager* mStageManager = nullptr;
    Settings* mSettings = nullptr;

    // --------------------------------------------------------
    // Selections
    // --------------------------------------------------------

    int mHomeSelection = 0;
    int mModeSelection = 0;
    int mStageSelection = 0;
    int mResultSelection = 0;
    int mSettingsSelection = 0;

    // --------------------------------------------------------
    // Result
    // --------------------------------------------------------

    RunResult mLastRunResult;
    ProgressionResult mLastProgressionResult;

    // --------------------------------------------------------
    // Animation
    // --------------------------------------------------------

    float mTitleAnimation = 0.0f;
    float mSelectionPulse = 0.0f;
    float mTransitionAnimation = 0.0f;

    float mPulseTime = 0.0f;
};
