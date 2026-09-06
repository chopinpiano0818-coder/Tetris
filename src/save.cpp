#include "save.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// ============================================================
// Constructor
// ============================================================

SaveManager::SaveManager(
    const std::string& path
)
    : mPath(path)
{
}

// ============================================================
// Exists
// ============================================================

bool SaveManager::exists() const
{
    return std::filesystem::exists(
        mPath
    );
}

// ============================================================
// Save
// ============================================================

bool SaveManager::save(
    const Progression& progression,
    const Settings& settings
)
{
    try
    {
        const std::filesystem::path path(
            mPath
        );

        if (
            path.has_parent_path()
        )
        {
            std::filesystem::create_directories(
                path.parent_path()
            );
        }

        std::ofstream file(
            mPath,
            std::ios::trunc
        );

        if (!file)
        {
            std::cerr
                << "Save file could not be opened: "
                << mPath
                << "\n";

            return false;
        }

        // ----------------------------------------------------
        // Version
        // ----------------------------------------------------

        file
            << "version=1\n";

        // ----------------------------------------------------
        // Progression
        // ----------------------------------------------------

        file
            << "playerLevel="
            << progression.playerLevel()
            << "\n";

        file
            << "currentXP="
            << progression.currentXP()
            << "\n";

        // ----------------------------------------------------
        // Window
        // ----------------------------------------------------

        file
            << "windowWidth="
            << settings.windowWidth
            << "\n";

        file
            << "windowHeight="
            << settings.windowHeight
            << "\n";

        file
            << "fullscreen="
            << settings.fullscreen
            << "\n";

        file
            << "vsync="
            << settings.vsync
            << "\n";

        // ----------------------------------------------------
        // Audio
        // ----------------------------------------------------

        file
            << "masterVolume="
            << settings.masterVolume
            << "\n";

        file
            << "musicVolume="
            << settings.musicVolume
            << "\n";

        file
            << "soundVolume="
            << settings.soundVolume
            << "\n";

        file
            << "musicEnabled="
            << settings.musicEnabled
            << "\n";

        file
            << "soundEnabled="
            << settings.soundEnabled
            << "\n";

        // ----------------------------------------------------
        // Controller
        // ----------------------------------------------------

        file
            << "leftStickDeadzone="
            << settings.leftStickDeadzone
            << "\n";

        file
            << "rightStickDeadzone="
            << settings.rightStickDeadzone
            << "\n";

        file
            << "dasDelay="
            << settings.dasDelay
            << "\n";

        file
            << "arrDelay="
            << settings.arrDelay
            << "\n";

        file
            << "softDropInterval="
            << settings.softDropInterval
            << "\n";

        // ----------------------------------------------------
        // 3D Camera
        // ----------------------------------------------------

        file
            << "cameraSensitivity="
            << settings.cameraSensitivity
            << "\n";

        file
            << "cameraMinYaw="
            << settings.cameraMinYaw
            << "\n";

        file
            << "cameraMaxYaw="
            << settings.cameraMaxYaw
            << "\n";

        file
            << "cameraMinPitch="
            << settings.cameraMinPitch
            << "\n";

        file
            << "cameraMaxPitch="
            << settings.cameraMaxPitch
            << "\n";

        // ----------------------------------------------------
        // Animation
        // ----------------------------------------------------

        file
            << "animationsEnabled="
            << settings.animationsEnabled
            << "\n";

        file
            << "animationIntensity="
            << settings.animationIntensity
            << "\n";

        file
            << "screenShake="
            << settings.screenShake
            << "\n";

        file
            << "lineClearFlash="
            << settings.lineClearFlash
            << "\n";

        file
            << "hardDropTrail="
            << settings.hardDropTrail
            << "\n";

        // ----------------------------------------------------
        // Display
        // ----------------------------------------------------

        file
            << "showGhost="
            << settings.showGhost
            << "\n";

        file
            << "showNext="
            << settings.showNext
            << "\n";

        file
            << "showHold="
            << settings.showHold
            << "\n";

        // ----------------------------------------------------
        // Controller extras
        // ----------------------------------------------------

        file
            << "vibrationEnabled="
            << settings.vibrationEnabled
            << "\n";

        file
            << "vibrationStrength="
            << settings.vibrationStrength
            << "\n";

        // ----------------------------------------------------
        // Touch
        // ----------------------------------------------------

        file
            << "touchControls="
            << settings.touchControls
            << "\n";

        file
            << "touchOpacity="
            << settings.touchOpacity
            << "\n";

        file.flush();

        return file.good();
    }
    catch (
        const std::exception& e
    )
    {
        std::cerr
            << "Save failed: "
            << e.what()
            << "\n";

        return false;
    }
}

// ============================================================
// Load
// ============================================================

bool SaveManager::load(
    Progression& progression,
    Settings& settings
)
{
    std::ifstream file(
        mPath
    );

    if (!file)
    {
        return false;
    }

    // --------------------------------------------------------
    // まずデフォルト設定
    // --------------------------------------------------------

    settings.resetToDefaults();

    int loadedLevel = 1;
    int loadedXP = 0;

    try
    {
        std::string line;

        while (
            std::getline(
                file,
                line
            )
        )
        {
            if (
                line.empty() ||
                line[0] == '#'
            )
            {
                continue;
            }

            const std::size_t split =
                line.find('=');

            if (
                split ==
                std::string::npos
            )
            {
                continue;
            }

            const std::string key =
                line.substr(
                    0,
                    split
                );

            const std::string value =
                line.substr(
                    split + 1
                );

            // ------------------------------------------------
            // Progression
            // ------------------------------------------------

            if (key == "playerLevel")
            {
                loadedLevel =
                    std::stoi(value);
            }
            else if (
                key == "currentXP"
            )
            {
                loadedXP =
                    std::stoi(value);
            }

            // ------------------------------------------------
            // Window
            // ------------------------------------------------

            else if (
                key == "windowWidth"
            )
            {
                settings.windowWidth =
                    std::stoi(value);
            }
            else if (
                key == "windowHeight"
            )
            {
                settings.windowHeight =
                    std::stoi(value);
            }
            else if (
                key == "fullscreen"
            )
            {
                settings.fullscreen =
                    std::stoi(value) != 0;
            }
            else if (
                key == "vsync"
            )
            {
                settings.vsync =
                    std::stoi(value) != 0;
            }

            // ------------------------------------------------
            // Audio
            // ------------------------------------------------

            else if (
                key == "masterVolume"
            )
            {
                settings.masterVolume =
                    std::stof(value);
            }
            else if (
                key == "musicVolume"
            )
            {
                settings.musicVolume =
                    std::stof(value);
            }
            else if (
                key == "soundVolume"
            )
            {
                settings.soundVolume =
                    std::stof(value);
            }
            else if (
                key == "musicEnabled"
            )
            {
                settings.musicEnabled =
                    std::stoi(value) != 0;
            }
            else if (
                key == "soundEnabled"
            )
            {
                settings.soundEnabled =
                    std::stoi(value) != 0;
            }

            // ------------------------------------------------
            // Controller
            // ------------------------------------------------

            else if (
                key ==
                "leftStickDeadzone"
            )
            {
                settings.leftStickDeadzone =
                    std::stoi(value);
            }
            else if (
                key ==
                "rightStickDeadzone"
            )
            {
                settings.rightStickDeadzone =
                    std::stoi(value);
            }
            else if (
                key == "dasDelay"
            )
            {
                settings.dasDelay =
                    std::stof(value);
            }
            else if (
                key == "arrDelay"
            )
            {
                settings.arrDelay =
                    std::stof(value);
            }
            else if (
                key ==
                "softDropInterval"
            )
            {
                settings.softDropInterval =
                    std::stof(value);
            }

            // ------------------------------------------------
            // Camera
            // ------------------------------------------------

            else if (
                key ==
                "cameraSensitivity"
            )
            {
                settings.cameraSensitivity =
                    std::stof(value);
            }
            else if (
                key == "cameraMinYaw"
            )
            {
                settings.cameraMinYaw =
                    std::stof(value);
            }
            else if (
                key == "cameraMaxYaw"
            )
            {
                settings.cameraMaxYaw =
                    std::stof(value);
            }
            else if (
                key == "cameraMinPitch"
            )
            {
                settings.cameraMinPitch =
                    std::stof(value);
            }
            else if (
                key == "cameraMaxPitch"
            )
            {
                settings.cameraMaxPitch =
                    std::stof(value);
            }

            // ------------------------------------------------
            // Animation
            // ------------------------------------------------

            else if (
                key ==
                "animationsEnabled"
            )
            {
                settings.animationsEnabled =
                    std::stoi(value) != 0;
            }
            else if (
                key ==
                "animationIntensity"
            )
            {
                settings.animationIntensity =
                    std::stof(value);
            }
            else if (
                key == "screenShake"
            )
            {
                settings.screenShake =
                    std::stoi(value) != 0;
            }
            else if (
                key == "lineClearFlash"
            )
            {
                settings.lineClearFlash =
                    std::stoi(value) != 0;
            }
            else if (
                key == "hardDropTrail"
            )
            {
                settings.hardDropTrail =
                    std::stoi(value) != 0;
            }

            // ------------------------------------------------
            // Display
            // ------------------------------------------------

            else if (
                key == "showGhost"
            )
            {
                settings.showGhost =
                    std::stoi(value) != 0;
            }
            else if (
                key == "showNext"
            )
            {
                settings.showNext =
                    std::stoi(value) != 0;
            }
            else if (
                key == "showHold"
            )
            {
                settings.showHold =
                    std::stoi(value) != 0;
            }

            // ------------------------------------------------
            // Controller extras
            // ------------------------------------------------

            else if (
                key ==
                "vibrationEnabled"
            )
            {
                settings.vibrationEnabled =
                    std::stoi(value) != 0;
            }
            else if (
                key ==
                "vibrationStrength"
            )
            {
                settings.vibrationStrength =
                    std::stof(value);
            }

            // ------------------------------------------------
            // Touch
            // ------------------------------------------------

            else if (
                key == "touchControls"
            )
            {
                settings.touchControls =
                    std::stoi(value) != 0;
            }
            else if (
                key == "touchOpacity"
            )
            {
                settings.touchOpacity =
                    std::stof(value);
            }
        }

        // ----------------------------------------------------
        // 安全チェック
        // ----------------------------------------------------

        settings.validate();

        progression.reset();

        progression.setPlayerLevel(
            loadedLevel
        );

        progression.setCurrentXP(
            loadedXP
        );

        return true;
    }
    catch (
        const std::exception& e
    )
    {
        std::cerr
            << "Save data is invalid: "
            << e.what()
            << "\n";

        // セーブ破損時は初期状態へ
        progression.reset();
        settings.resetToDefaults();

        return false;
    }
}
