#include "input.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

// ============================================================
// Constructor
// ============================================================

InputManager::InputManager(
    const Settings& settings
)
    : mSettings(&settings)
{
    // すでに接続されているコントローラーを探す
    for (int i = 0;
         i < SDL_NumJoysticks();
         ++i)
    {
        if (SDL_IsGameController(i))
        {
            openController(i);
            break;
        }
    }
}

// ============================================================
// Frame reset
// ============================================================

void InputManager::beginFrame()
{
    mPausePressed = false;

    mRestartTriggered = false;

    mBackToMenu = false;

    mMoveLeft = false;
    mMoveRight = false;
    mSoftDrop = false;

    mRotateLeft = false;
    mRotateRight = false;

    mFlip = false;

    mHardDrop = false;

    mHold = false;

    mCameraReset = false;

    mMenuUp = false;
    mMenuDown = false;
    mMenuLeft = false;
    mMenuRight = false;

    mConfirm = false;
    mCancel = false;
}

// ============================================================
// Event
// ============================================================

void InputManager::handleEvent(
    const SDL_Event& event
)
{
    // --------------------------------------------------------
    // Keyboard
    // --------------------------------------------------------

    if (event.type == SDL_KEYDOWN &&
        !event.key.repeat)
    {
        switch (
            event.key.keysym.scancode
        )
        {
            case SDL_SCANCODE_ESCAPE:
                mBackToMenu = true;
                mCancel = true;
                break;

            case SDL_SCANCODE_P:
                mPausePressed = true;
                break;

            case SDL_SCANCODE_R:
                mRestartTriggered = true;
                break;

            case SDL_SCANCODE_LEFT:
                mMoveLeft = true;
                mMenuLeft = true;
                break;

            case SDL_SCANCODE_RIGHT:
                mMoveRight = true;
                mMenuRight = true;
                break;

            case SDL_SCANCODE_DOWN:
                mSoftDrop = true;
                mMenuDown = true;
                break;

            case SDL_SCANCODE_UP:
                mRotateRight = true;
                mMenuUp = true;
                break;

            case SDL_SCANCODE_Z:
                mRotateLeft = true;
                break;

            case SDL_SCANCODE_X:
                mRotateRight = true;
                break;

            case SDL_SCANCODE_A:
                mFlip = true;
                break;

            case SDL_SCANCODE_SPACE:
                mHardDrop = true;
                mConfirm = true;
                break;

            case SDL_SCANCODE_C:
                mHold = true;
                break;

            case SDL_SCANCODE_RETURN:
                mConfirm = true;
                break;

            case SDL_SCANCODE_BACKSPACE:
                mCancel = true;
                break;

            default:
                break;
        }
    }

    // --------------------------------------------------------
    // Controller connected
    // --------------------------------------------------------

    if (event.type ==
        SDL_CONTROLLERDEVICEADDED)
    {
        if (!mController)
        {
            openController(
                event.cdevice.which
            );
        }
    }

    // --------------------------------------------------------
    // Controller removed
    // --------------------------------------------------------

    if (event.type ==
        SDL_CONTROLLERDEVICEREMOVED)
    {
        if (
            event.cdevice.which ==
            mControllerInstance
        )
        {
            closeController();
        }
    }

    // --------------------------------------------------------
    // Controller buttons
    // --------------------------------------------------------

    if (event.type ==
        SDL_CONTROLLERBUTTONDOWN)
    {
        const auto button =
            static_cast<
                SDL_GameControllerButton
            >(
                event.cbutton.button
            );

        switch (button)
        {
            // A = 180度回転
            case SDL_CONTROLLER_BUTTON_A:
                mFlip = true;
                mConfirm = true;
                break;

            // B = Hard Drop
            case SDL_CONTROLLER_BUTTON_B:
                mHardDrop = true;
                mCancel = true;
                break;

            // X = HOLD
            case SDL_CONTROLLER_BUTTON_X:
                mHold = true;
                break;

            // Y = 予備
            case SDL_CONTROLLER_BUTTON_Y:
                break;

            // L = 左回転
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                mRotateLeft = true;
                break;

            // R = 右回転
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                mRotateRight = true;
                break;

            // Plus / Start
            case SDL_CONTROLLER_BUTTON_START:
                mPausePressed = true;
                break;

            // Minus / Back
            case SDL_CONTROLLER_BUTTON_BACK:
                mRestartHeld = true;
                mRestartHoldTime = 0.0f;
                mRestartAlreadyTriggered = false;
                break;

            // R3
            case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                mCameraReset = true;
                break;

            // D-pad
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                mMoveLeft = true;
                mMenuLeft = true;
                break;

            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                mMoveRight = true;
                mMenuRight = true;
                break;

            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                mSoftDrop = true;
                mMenuDown = true;
                break;

            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                mMenuUp = true;
                break;

            default:
                break;
        }
    }

    // --------------------------------------------------------
    // Controller button released
    // --------------------------------------------------------

    if (event.type ==
        SDL_CONTROLLERBUTTONUP)
    {
        const auto button =
            static_cast<
                SDL_GameControllerButton
            >(
                event.cbutton.button
            );

        if (button ==
            SDL_CONTROLLER_BUTTON_BACK)
        {
            mRestartHeld = false;
            mRestartHoldTime = 0.0f;
            mRestartAlreadyTriggered = false;
        }
    }
}

// ============================================================
// Update
// ============================================================

void InputManager::update(
    float deltaTime
)
{
    updateControllerAxes();

    updateRepeat(deltaTime);

    // --------------------------------------------------------
    // Minus長押し
    // --------------------------------------------------------

    if (mRestartHeld)
    {
        mRestartHoldTime +=
            deltaTime;

        if (
            mRestartHoldTime >=
                RESTART_HOLD_SECONDS &&
            !mRestartAlreadyTriggered
        )
        {
            mRestartTriggered = true;

            mRestartAlreadyTriggered = true;
        }
    }
}

// ============================================================
// Controller open
// ============================================================

void InputManager::openController(
    int joystickIndex
)
{
    if (mController)
    {
        return;
    }

    if (!SDL_IsGameController(
            joystickIndex
        ))
    {
        return;
    }

    mController =
        SDL_GameControllerOpen(
            joystickIndex
        );

    if (!mController)
    {
        std::cerr
            << "Controller open failed: "
            << SDL_GetError()
            << "\n";

        return;
    }

    SDL_Joystick* joystick =
        SDL_GameControllerGetJoystick(
            mController
        );

    mControllerInstance =
        SDL_JoystickInstanceID(
            joystick
        );

    std::cout
        << "Controller connected: "
        << SDL_GameControllerName(
               mController
           )
        << "\n";
}

// ============================================================
// Controller close
// ============================================================

void InputManager::closeController()
{
    if (mController)
    {
        SDL_GameControllerClose(
            mController
        );

        mController = nullptr;
    }

    mControllerInstance = -1;

    mLeftStickX = 0.0f;
    mLeftStickY = 0.0f;

    mRightStickX = 0.0f;
    mRightStickY = 0.0f;
}

// ============================================================
// Axes
// ============================================================

void InputManager::updateControllerAxes()
{
    if (!mController)
    {
        mLeftStickX = 0.0f;
        mLeftStickY = 0.0f;

        mRightStickX = 0.0f;
        mRightStickY = 0.0f;

        return;
    }

    auto normalize =
        [](Sint16 value,
           int deadzone)
        -> float
    {
        const int raw =
            static_cast<int>(value);

        if (
            std::abs(raw) <
            deadzone
        )
        {
            return 0.0f;
        }

        float result =
            static_cast<float>(raw) /
            32767.0f;

        result =
            std::clamp(
                result,
                -1.0f,
                1.0f
            );

        return result;
    };

    const Sint16 lx =
        SDL_GameControllerGetAxis(
            mController,
            SDL_CONTROLLER_AXIS_LEFTX
        );

    const Sint16 ly =
        SDL_GameControllerGetAxis(
            mController,
            SDL_CONTROLLER_AXIS_LEFTY
        );

    const Sint16 rx =
        SDL_GameControllerGetAxis(
            mController,
            SDL_CONTROLLER_AXIS_RIGHTX
        );

    const Sint16 ry =
        SDL_GameControllerGetAxis(
            mController,
            SDL_CONTROLLER_AXIS_RIGHTY
        );

    mLeftStickX =
        normalize(
            lx,
            mSettings->leftStickDeadzone
        );

    mLeftStickY =
        normalize(
            ly,
            mSettings->leftStickDeadzone
        );

    mRightStickX =
        normalize(
            rx,
            mSettings->rightStickDeadzone
        );

    mRightStickY =
        normalize(
            ry,
            mSettings->rightStickDeadzone
        );
}

// ============================================================
// DAS / ARR
// ============================================================

void InputManager::updateRepeat(
    float deltaTime
)
{
    const Uint8* keys =
        SDL_GetKeyboardState(
            nullptr
        );

    bool leftNow =
        keys[SDL_SCANCODE_LEFT];

    bool rightNow =
        keys[SDL_SCANCODE_RIGHT];

    bool downNow =
        keys[SDL_SCANCODE_DOWN];

    // 左スティック
    if (mLeftStickX < -0.5f)
    {
        leftNow = true;
    }

    if (mLeftStickX > 0.5f)
    {
        rightNow = true;
    }

    if (mLeftStickY > 0.5f)
    {
        downNow = true;
    }

    // D-pad held
    if (mController)
    {
        leftNow =
            leftNow ||
            SDL_GameControllerGetButton(
                mController,
                SDL_CONTROLLER_BUTTON_DPAD_LEFT
            );

        rightNow =
            rightNow ||
            SDL_GameControllerGetButton(
                mController,
                SDL_CONTROLLER_BUTTON_DPAD_RIGHT
            );

        downNow =
            downNow ||
            SDL_GameControllerGetButton(
                mController,
                SDL_CONTROLLER_BUTTON_DPAD_DOWN
            );
    }

    // --------------------------------------------------------
    // LEFT
    // --------------------------------------------------------

    if (leftNow)
    {
        if (!mLeftHeld)
        {
            mLeftHeld = true;

            mLeftHoldTime = 0.0f;
            mLeftRepeatTimer = 0.0f;

            mMoveLeft = true;
        }
        else
        {
            mLeftHoldTime +=
                deltaTime;

            if (
                mLeftHoldTime >=
                mSettings->dasDelay
            )
            {
                mLeftRepeatTimer +=
                    deltaTime;

                while (
                    mLeftRepeatTimer >=
                    mSettings->arrDelay
                )
                {
                    mLeftRepeatTimer -=
                        mSettings->arrDelay;

                    mMoveLeft = true;
                }
            }
        }
    }
    else
    {
        mLeftHeld = false;

        mLeftHoldTime = 0.0f;
        mLeftRepeatTimer = 0.0f;
    }

    // --------------------------------------------------------
    // RIGHT
    // --------------------------------------------------------

    if (rightNow)
    {
        if (!mRightHeld)
        {
            mRightHeld = true;

            mRightHoldTime = 0.0f;
            mRightRepeatTimer = 0.0f;

            mMoveRight = true;
        }
        else
        {
            mRightHoldTime +=
                deltaTime;

            if (
                mRightHoldTime >=
                mSettings->dasDelay
            )
            {
                mRightRepeatTimer +=
                    deltaTime;

                while (
                    mRightRepeatTimer >=
                    mSettings->arrDelay
                )
                {
                    mRightRepeatTimer -=
                        mSettings->arrDelay;

                    mMoveRight = true;
                }
            }
        }
    }
    else
    {
        mRightHeld = false;

        mRightHoldTime = 0.0f;
        mRightRepeatTimer = 0.0f;
    }

    // --------------------------------------------------------
    // DOWN
    // --------------------------------------------------------

    if (downNow)
    {
        if (!mDownHeld)
        {
            mDownHeld = true;

            mDownHoldTime = 0.0f;
            mDownRepeatTimer = 0.0f;

            mSoftDrop = true;
        }
        else
        {
            mDownHoldTime +=
                deltaTime;

            mDownRepeatTimer +=
                deltaTime;

            while (
                mDownRepeatTimer >=
                mSettings->softDropInterval
            )
            {
                mDownRepeatTimer -=
                    mSettings->softDropInterval;

                mSoftDrop = true;
            }
        }
    }
    else
    {
        mDownHeld = false;

        mDownHoldTime = 0.0f;
        mDownRepeatTimer = 0.0f;
    }
}

// ============================================================
// Settings
// ============================================================

void InputManager::applySettings(
    const Settings& settings
)
{
    mSettings = &settings;
}

// ============================================================
// Shutdown
// ============================================================

void InputManager::shutdown()
{
    closeController();
}

// ============================================================
// Helpers
// ============================================================

bool InputManager::keyDown(
    SDL_Scancode code
) const
{
    const Uint8* state =
        SDL_GetKeyboardState(
            nullptr
        );

    return state[code] != 0;
}

// ============================================================
// Getters
// ============================================================

bool InputManager::quitRequested() const
{
    return mQuit;
}

bool InputManager::pausePressed() const
{
    return mPausePressed;
}

bool InputManager::restartTriggered() const
{
    return mRestartTriggered;
}

bool InputManager::backToMenuPressed() const
{
    return mBackToMenu;
}

bool InputManager::moveLeftTriggered() const
{
    return mMoveLeft;
}

bool InputManager::moveRightTriggered() const
{
    return mMoveRight;
}

bool InputManager::softDropTriggered() const
{
    return mSoftDrop;
}

bool InputManager::rotateLeftPressed() const
{
    return mRotateLeft;
}

bool InputManager::rotateRightPressed() const
{
    return mRotateRight;
}

bool InputManager::flipPressed() const
{
    return mFlip;
}

bool InputManager::hardDropPressed() const
{
    return mHardDrop;
}

bool InputManager::holdPressed() const
{
    return mHold;
}

float InputManager::cameraX() const
{
    return mRightStickX;
}

float InputManager::cameraY() const
{
    // スティック上でカメラ上方向
    return -mRightStickY;
}

bool InputManager::cameraResetPressed() const
{
    return mCameraReset;
}

bool InputManager::menuUpPressed() const
{
    return mMenuUp;
}

bool InputManager::menuDownPressed() const
{
    return mMenuDown;
}

bool InputManager::menuLeftPressed() const
{
    return mMenuLeft;
}

bool InputManager::menuRightPressed() const
{
    return mMenuRight;
}

bool InputManager::confirmPressed() const
{
    return mConfirm;
}

bool InputManager::cancelPressed() const
{
    return mCancel;
}
