#pragma once

#include <SDL2/SDL.h>

#include "settings.hpp"

// ============================================================
// TETRIS COMPLETE
// input.hpp
// ============================================================

class InputManager
{
public:
    explicit InputManager(
        const Settings& settings
    );

    // 毎フレーム最初
    void beginFrame();

    // SDLイベント
    void handleEvent(
        const SDL_Event& event
    );

    // 長押し/DAS/ARRなど
    void update(
        float deltaTime
    );

    // 設定変更反映
    void applySettings(
        const Settings& settings
    );

    // 終了処理
    void shutdown();

    // --------------------------------------------------------
    // Global
    // --------------------------------------------------------

    bool quitRequested() const;

    bool pausePressed() const;

    bool restartTriggered() const;

    bool backToMenuPressed() const;

    // --------------------------------------------------------
    // Game
    // --------------------------------------------------------

    bool moveLeftTriggered() const;
    bool moveRightTriggered() const;
    bool softDropTriggered() const;

    bool rotateLeftPressed() const;
    bool rotateRightPressed() const;

    bool flipPressed() const;

    bool hardDropPressed() const;

    bool holdPressed() const;

    // --------------------------------------------------------
    // 3D camera
    // --------------------------------------------------------

    float cameraX() const;
    float cameraY() const;

    bool cameraResetPressed() const;

    // --------------------------------------------------------
    // Menu
    // --------------------------------------------------------

    bool menuUpPressed() const;
    bool menuDownPressed() const;
    bool menuLeftPressed() const;
    bool menuRightPressed() const;

    bool confirmPressed() const;
    bool cancelPressed() const;

private:
    void openController(
        int joystickIndex
    );

    void closeController();

    void updateControllerAxes();

    void updateRepeat(
        float deltaTime
    );

    bool keyDown(
        SDL_Scancode code
    ) const;

private:
    const Settings* mSettings = nullptr;

    SDL_GameController*
        mController = nullptr;

    SDL_JoystickID
        mControllerInstance = -1;

    // ========================================================
    // One-frame actions
    // ========================================================

    bool mQuit = false;

    bool mPausePressed = false;

    bool mRestartTriggered = false;

    bool mBackToMenu = false;

    bool mMoveLeft = false;
    bool mMoveRight = false;
    bool mSoftDrop = false;

    bool mRotateLeft = false;
    bool mRotateRight = false;

    bool mFlip = false;

    bool mHardDrop = false;

    bool mHold = false;

    bool mCameraReset = false;

    bool mMenuUp = false;
    bool mMenuDown = false;
    bool mMenuLeft = false;
    bool mMenuRight = false;

    bool mConfirm = false;
    bool mCancel = false;

    // ========================================================
    // Stick state
    // ========================================================

    float mLeftStickX = 0.0f;
    float mLeftStickY = 0.0f;

    float mRightStickX = 0.0f;
    float mRightStickY = 0.0f;

    // ========================================================
    // DAS / ARR
    // ========================================================

    bool mLeftHeld = false;
    bool mRightHeld = false;
    bool mDownHeld = false;

    float mLeftHoldTime = 0.0f;
    float mRightHoldTime = 0.0f;
    float mDownHoldTime = 0.0f;

    float mLeftRepeatTimer = 0.0f;
    float mRightRepeatTimer = 0.0f;
    float mDownRepeatTimer = 0.0f;

    // ========================================================
    // Restart long hold
    // ========================================================

    bool mRestartHeld = false;

    float mRestartHoldTime = 0.0f;

    bool mRestartAlreadyTriggered = false;

    static constexpr float
        RESTART_HOLD_SECONDS = 1.0f;
};
