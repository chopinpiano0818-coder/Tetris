#include "settings.hpp"

#include <algorithm>

// ============================================================
// デフォルト設定へ戻す
// ============================================================

void Settings::resetToDefaults()
{
    // Window
    windowWidth = 1280;
    windowHeight = 720;

    fullscreen = false;
    vsync = true;

    // Audio
    masterVolume = 1.0f;
    musicVolume = 0.75f;
    soundVolume = 1.0f;

    musicEnabled = true;
    soundEnabled = true;

    // Controller
    leftStickDeadzone = 8000;
    rightStickDeadzone = 8000;

    dasDelay = 0.16f;
    arrDelay = 0.045f;

    softDropInterval = 0.045f;

    // 3D
    cameraSensitivity = 100.0f;

    cameraMinYaw = -45.0f;
    cameraMaxYaw = 45.0f;

    cameraMinPitch = -20.0f;
    cameraMaxPitch = 35.0f;

    // Animation
    animationsEnabled = true;
    animationIntensity = 1.0f;

    screenShake = true;
    lineClearFlash = true;
    hardDropTrail = true;

    // Display
    showGhost = true;
    showNext = true;
    showHold = true;

    // Controller
    vibrationEnabled = true;
    vibrationStrength = 0.6f;

    // Mobile
    touchControls = true;
    touchOpacity = 0.65f;

    validate();
}

// ============================================================
// 設定値を安全な範囲に制限
// ============================================================

void Settings::validate()
{
    // --------------------------------------------------------
    // Window
    // --------------------------------------------------------

    windowWidth =
        std::clamp(
            windowWidth,
            640,
            3840
        );

    windowHeight =
        std::clamp(
            windowHeight,
            480,
            2160
        );

    // --------------------------------------------------------
    // Volume
    // --------------------------------------------------------

    masterVolume =
        std::clamp(
            masterVolume,
            0.0f,
            1.0f
        );

    musicVolume =
        std::clamp(
            musicVolume,
            0.0f,
            1.0f
        );

    soundVolume =
        std::clamp(
            soundVolume,
            0.0f,
            1.0f
        );

    // --------------------------------------------------------
    // Stick deadzone
    // --------------------------------------------------------

    leftStickDeadzone =
        std::clamp(
            leftStickDeadzone,
            0,
            30000
        );

    rightStickDeadzone =
        std::clamp(
            rightStickDeadzone,
            0,
            30000
        );

    // --------------------------------------------------------
    // DAS / ARR
    // --------------------------------------------------------

    dasDelay =
        std::clamp(
            dasDelay,
            0.05f,
            0.50f
        );

    arrDelay =
        std::clamp(
            arrDelay,
            0.01f,
            0.20f
        );

    softDropInterval =
        std::clamp(
            softDropInterval,
            0.01f,
            0.20f
        );

    // --------------------------------------------------------
    // Camera
    // --------------------------------------------------------

    cameraSensitivity =
        std::clamp(
            cameraSensitivity,
            20.0f,
            300.0f
        );

    cameraMinYaw =
        std::clamp(
            cameraMinYaw,
            -180.0f,
            0.0f
        );

    cameraMaxYaw =
        std::clamp(
            cameraMaxYaw,
            0.0f,
            180.0f
        );

    cameraMinPitch =
        std::clamp(
            cameraMinPitch,
            -80.0f,
            0.0f
        );

    cameraMaxPitch =
        std::clamp(
            cameraMaxPitch,
            0.0f,
            80.0f
        );

    // --------------------------------------------------------
    // Animation
    // --------------------------------------------------------

    animationIntensity =
        std::clamp(
            animationIntensity,
            0.0f,
            2.0f
        );

    // --------------------------------------------------------
    // Vibration
    // --------------------------------------------------------

    vibrationStrength =
        std::clamp(
            vibrationStrength,
            0.0f,
            1.0f
        );

    // --------------------------------------------------------
    // Touch
    // --------------------------------------------------------

    touchOpacity =
        std::clamp(
            touchOpacity,
            0.2f,
            1.0f
        );
}
