#include "ui.hpp"
#include "input.hpp"

#include <algorithm>
#include <cmath>

// ============================================================
// Constructor
// ============================================================

UI::UI(
    Progression& progression,
    StageManager& stageManager,
    Settings& settings
)
    : mProgression(&progression),
      mStageManager(&stageManager),
      mSettings(&settings)
{
}

// ============================================================
// Common animation
// ============================================================

void UI::updateCommonAnimations(
    float deltaTime
)
{
    mTitleAnimation =
        std::min(
            1.0f,
            mTitleAnimation +
            deltaTime * 2.0f
        );

    mTransitionAnimation =
        std::min(
            1.0f,
            mTransitionAnimation +
            deltaTime * 4.0f
        );

    mPulseTime += deltaTime;

    mSelectionPulse =
        0.5f +
        0.5f *
        std::sin(
            mPulseTime * 5.0f
        );
}

// ============================================================
// HOME
//
// 0 = 2D
// 1 = 3D
// 2 = SETTINGS
// 3 = QUIT
// ============================================================

UIAction UI::updateHome(
    InputManager& input,
    float deltaTime,
    RenderStyle& selectedStyle
)
{
    updateCommonAnimations(
        deltaTime
    );

    constexpr int ITEM_COUNT = 4;

    if (input.menuUpPressed())
    {
        --mHomeSelection;

        if (mHomeSelection < 0)
        {
            mHomeSelection =
                ITEM_COUNT - 1;
        }
    }

    if (input.menuDownPressed())
    {
        ++mHomeSelection;

        if (mHomeSelection >= ITEM_COUNT)
        {
            mHomeSelection = 0;
        }
    }

    if (input.confirmPressed())
    {
        switch (mHomeSelection)
        {
            case 0:
                selectedStyle =
                    RenderStyle::TwoD;

                mModeSelection = 0;

                return
                    UIAction::OpenModeSelect;

            case 1:
                selectedStyle =
                    RenderStyle::ThreeD;

                mModeSelection = 0;

                return
                    UIAction::OpenModeSelect;

            case 2:
                mSettingsSelection = 0;

                return
                    UIAction::OpenSettings;

            case 3:
                return
                    UIAction::Quit;
        }
    }

    return UIAction::None;
}

// ============================================================
// MODE SELECT
//
// 0 = ENDLESS
// 1 = RUNNER
// 2 = SPRINT
// 3 = ULTRA
// ============================================================

UIAction UI::updateModeSelect(
    InputManager& input,
    float deltaTime,
    GameMode& selectedMode
)
{
    updateCommonAnimations(
        deltaTime
    );

    constexpr int MODE_COUNT = 4;

    if (
        input.menuLeftPressed() ||
        input.menuUpPressed()
    )
    {
        --mModeSelection;

        if (mModeSelection < 0)
        {
            mModeSelection =
                MODE_COUNT - 1;
        }
    }

    if (
        input.menuRightPressed() ||
        input.menuDownPressed()
    )
    {
        ++mModeSelection;

        if (
            mModeSelection >=
            MODE_COUNT
        )
        {
            mModeSelection = 0;
        }
    }

    switch (mModeSelection)
    {
        case 0:
            selectedMode =
                GameMode::Endless;
            break;

        case 1:
            selectedMode =
                GameMode::Runner;
            break;

        case 2:
            selectedMode =
                GameMode::Sprint;
            break;

        case 3:
            selectedMode =
                GameMode::Ultra;
            break;
    }

    if (input.confirmPressed())
    {
        mStageSelection = 0;

        return
            UIAction::OpenStageSelect;
    }

    if (input.cancelPressed())
    {
        return
            UIAction::Back;
    }

    return UIAction::None;
}

// ============================================================
// STAGE SELECT
// ============================================================

UIAction UI::updateStageSelect(
    InputManager& input,
    float deltaTime,
    int& selectedStage
)
{
    updateCommonAnimations(
        deltaTime
    );

    const int count =
        mStageManager->stageCount();

    if (count <= 0)
    {
        selectedStage = 0;
        return UIAction::Back;
    }

    if (
        input.menuLeftPressed() ||
        input.menuUpPressed()
    )
    {
        --mStageSelection;

        if (mStageSelection < 0)
        {
            mStageSelection =
                count - 1;
        }
    }

    if (
        input.menuRightPressed() ||
        input.menuDownPressed()
    )
    {
        ++mStageSelection;

        if (mStageSelection >= count)
        {
            mStageSelection = 0;
        }
    }

    selectedStage =
        mStageSelection;

    if (input.confirmPressed())
    {
        if (
            mStageManager->isUnlocked(
                mStageSelection
            )
        )
        {
            selectedStage =
                mStageSelection;

            return
                UIAction::StartGame;
        }
    }

    if (input.cancelPressed())
    {
        return
            UIAction::Back;
    }

    return UIAction::None;
}

// ============================================================
// RESULT
//
// 0 = RETRY
// 1 = STAGE SELECT
// 2 = HOME
// ============================================================

UIAction UI::updateResult(
    InputManager& input,
    float deltaTime
)
{
    updateCommonAnimations(
        deltaTime
    );

    constexpr int ITEM_COUNT = 3;

    if (input.menuUpPressed())
    {
        --mResultSelection;

        if (mResultSelection < 0)
        {
            mResultSelection =
                ITEM_COUNT - 1;
        }
    }

    if (input.menuDownPressed())
    {
        ++mResultSelection;

        if (
            mResultSelection >=
            ITEM_COUNT
        )
        {
            mResultSelection = 0;
        }
    }

    if (input.confirmPressed())
    {
        switch (mResultSelection)
        {
            case 0:
                return
                    UIAction::Restart;

            case 1:
                return
                    UIAction::OpenStageSelect;

            case 2:
                return
                    UIAction::OpenHome;
        }
    }

    if (input.cancelPressed())
    {
        return
            UIAction::OpenHome;
    }

    return UIAction::None;
}

// ============================================================
// SETTINGS
//
// 0 Master Volume
// 1 Music Volume
// 2 Sound Volume
// 3 Animation
// 4 Camera Sensitivity
// 5 Stick Deadzone
// 6 DAS
// 7 ARR
// 8 Ghost
// 9 Vibration
// 10 Fullscreen
// 11 Back
// ============================================================

UIAction UI::updateSettings(
    InputManager& input,
    float deltaTime,
    Settings& settings
)
{
    updateCommonAnimations(
        deltaTime
    );

    constexpr int ITEM_COUNT = 12;

    if (input.menuUpPressed())
    {
        --mSettingsSelection;

        if (mSettingsSelection < 0)
        {
            mSettingsSelection =
                ITEM_COUNT - 1;
        }
    }

    if (input.menuDownPressed())
    {
        ++mSettingsSelection;

        if (
            mSettingsSelection >=
            ITEM_COUNT
        )
        {
            mSettingsSelection = 0;
        }
    }

    const bool left =
        input.menuLeftPressed();

    const bool right =
        input.menuRightPressed();

    const float direction =
        right ? 1.0f :
        left  ? -1.0f :
        0.0f;

    switch (mSettingsSelection)
    {
        // Master Volume
        case 0:
        {
            if (direction != 0.0f)
            {
                settings.masterVolume +=
                    direction * 0.05f;
            }

            break;
        }

        // Music
        case 1:
        {
            if (direction != 0.0f)
            {
                settings.musicVolume +=
                    direction * 0.05f;
            }

            break;
        }

        // SE
        case 2:
        {
            if (direction != 0.0f)
            {
                settings.soundVolume +=
                    direction * 0.05f;
            }

            break;
        }

        // Animation intensity
        case 3:
        {
            if (direction != 0.0f)
            {
                settings.animationIntensity +=
                    direction * 0.1f;
            }

            if (input.confirmPressed())
            {
                settings.animationsEnabled =
                    !settings.animationsEnabled;
            }

            break;
        }

        // Camera sensitivity
        case 4:
        {
            if (direction != 0.0f)
            {
                settings.cameraSensitivity +=
                    direction * 10.0f;
            }

            break;
        }

        // Deadzone
        case 5:
        {
            if (direction != 0.0f)
            {
                settings.leftStickDeadzone +=
                    static_cast<int>(
                        direction * 1000.0f
                    );

                settings.rightStickDeadzone =
                    settings.leftStickDeadzone;
            }

            break;
        }

        // DAS
        case 6:
        {
            if (direction != 0.0f)
            {
                settings.dasDelay +=
                    direction * 0.01f;
            }

            break;
        }

        // ARR
        case 7:
        {
            if (direction != 0.0f)
            {
                settings.arrDelay +=
                    direction * 0.005f;
            }

            break;
        }

        // Ghost
        case 8:
        {
            if (input.confirmPressed())
            {
                settings.showGhost =
                    !settings.showGhost;
            }

            break;
        }

        // Vibration
        case 9:
        {
            if (input.confirmPressed())
            {
                settings.vibrationEnabled =
                    !settings.vibrationEnabled;
            }

            break;
        }

        // Fullscreen
        case 10:
        {
            if (input.confirmPressed())
            {
                settings.fullscreen =
                    !settings.fullscreen;
            }

            break;
        }

        // Back
        case 11:
        {
            if (input.confirmPressed())
            {
                settings.validate();

                return
                    UIAction::Back;
            }

            break;
        }
    }

    settings.validate();

    if (input.cancelPressed())
    {
        return
            UIAction::Back;
    }

    return UIAction::None;
}

// ============================================================
// Result data
// ============================================================

void UI::showResult(
    const RunResult& result,
    const ProgressionResult&
        progressionResult
)
{
    mLastRunResult =
        result;

    mLastProgressionResult =
        progressionResult;

    mResultSelection = 0;

    mTransitionAnimation = 0.0f;
}

// ============================================================
// Getters
// ============================================================

int UI::homeSelection() const
{
    return mHomeSelection;
}

int UI::modeSelection() const
{
    return mModeSelection;
}

int UI::stageSelection() const
{
    return mStageSelection;
}

int UI::resultSelection() const
{
    return mResultSelection;
}

int UI::settingsSelection() const
{
    return mSettingsSelection;
}

const RunResult&
UI::lastRunResult() const
{
    return mLastRunResult;
}

const ProgressionResult&
UI::lastProgressionResult() const
{
    return mLastProgressionResult;
}

float UI::titleAnimation() const
{
    return mTitleAnimation;
}

float UI::selectionPulse() const
{
    return mSelectionPulse;
}

float UI::transitionAnimation() const
{
    return mTransitionAnimation;
}
