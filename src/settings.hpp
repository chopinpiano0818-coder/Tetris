#pragma once

// ============================================================
// TETRIS COMPLETE
// settings.hpp
//
// ゲーム全体の設定
// ============================================================

struct Settings
{
    // --------------------------------------------------------
    // Window
    // --------------------------------------------------------
    int windowWidth = 1280;
    int windowHeight = 720;

    bool fullscreen = false;
    bool vsync = true;

    // --------------------------------------------------------
    // Audio
    // 0.0 ～ 1.0
    // --------------------------------------------------------
    float masterVolume = 1.0f;
    float musicVolume = 0.75f;
    float soundVolume = 1.0f;

    bool musicEnabled = true;
    bool soundEnabled = true;

    // --------------------------------------------------------
    // Controller
    // --------------------------------------------------------

    // スティックが勝手に反応するのを防ぐ
    int leftStickDeadzone = 8000;
    int rightStickDeadzone = 8000;

    // 左右長押し
    //
    // DAS:
    // 押してから連続移動が始まるまで
    //
    // ARR:
    // 連続移動の間隔
    // --------------------------------------------------------
    float dasDelay = 0.16f;
    float arrDelay = 0.045f;

    // 下長押しの速度
    float softDropInterval = 0.045f;

    // --------------------------------------------------------
    // 3D Camera
    // --------------------------------------------------------
    float cameraSensitivity = 100.0f;

    float cameraMinYaw = -45.0f;
    float cameraMaxYaw = 45.0f;

    float cameraMinPitch = -20.0f;
    float cameraMaxPitch = 35.0f;

    // --------------------------------------------------------
    // Animation
    // --------------------------------------------------------
    bool animationsEnabled = true;

    // 0.0 = ほぼ無し
    // 1.0 = 通常
    // 2.0 = 派手
    float animationIntensity = 1.0f;

    bool screenShake = true;
    bool lineClearFlash = true;
    bool hardDropTrail = true;

    // --------------------------------------------------------
    // Gameplay display
    // --------------------------------------------------------
    bool showGhost = true;
    bool showNext = true;
    bool showHold = true;

    // --------------------------------------------------------
    // Controller extras
    // --------------------------------------------------------
    bool vibrationEnabled = true;

    float vibrationStrength = 0.6f;

    // --------------------------------------------------------
    // Mobile / Touch
    // --------------------------------------------------------
    bool touchControls = true;

    // タッチボタン透明度
    float touchOpacity = 0.65f;

    // --------------------------------------------------------
    // Default values
    // --------------------------------------------------------
    void resetToDefaults();

    // --------------------------------------------------------
    // 値がおかしくなった場合に安全な範囲へ戻す
    // --------------------------------------------------------
    void validate();
};
