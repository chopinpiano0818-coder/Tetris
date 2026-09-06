#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <algorithm>
#include <iostream>

#include "types.hpp"
#include "game.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include "ui.hpp"
#include "save.hpp"
#include "progression.hpp"
#include "settings.hpp"
#include "stages.hpp"

// ------------------------------------------------------------
// TETRIS COMPLETE
// main.cpp
//
// 役割:
// ・SDL初期化
// ・ウィンドウ作成
// ・各システムを接続
// ・ホーム → モード → ステージ → ゲーム の管理
// ・セーブ / ロード
// ・ゲームループ
// ------------------------------------------------------------

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // ========================================================
    // SDL 初期化
    // ========================================================

    if (SDL_Init(
            SDL_INIT_VIDEO |
            SDL_INIT_GAMECONTROLLER |
            SDL_INIT_JOYSTICK |
            SDL_INIT_HAPTIC |
            SDL_INIT_TIMER
        ) != 0)
    {
        std::cerr
            << "SDL_Init failed: "
            << SDL_GetError()
            << std::endl;

        return 1;
    }

    if (TTF_Init() != 0)
    {
        std::cerr
            << "TTF_Init failed: "
            << TTF_GetError()
            << std::endl;

        SDL_Quit();
        return 1;
    }

    // ========================================================
    // 設定
    // ========================================================

    Settings settings;

    // 最初はデフォルト値。
    // save.dat に設定があれば後で上書きされる。
    settings.resetToDefaults();

    // ========================================================
    // プレイヤーデータ
    // ========================================================

    Progression progression;

    SaveManager saveManager(
        "save/save.dat"
    );

    // セーブデータ読み込み
    if (saveManager.exists())
    {
        if (!saveManager.load(
                progression,
                settings
            ))
        {
            std::cout
                << "Save load failed. "
                << "Using new save."
                << std::endl;
        }
    }

    // ========================================================
    // ウィンドウ作成
    // ========================================================

    Uint32 windowFlags =
        SDL_WINDOW_SHOWN |
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_ALLOW_HIGHDPI;

#if defined(__ANDROID__) || defined(__IPHONEOS__)
    windowFlags |= SDL_WINDOW_FULLSCREEN;
#endif

    SDL_Window* window =
        SDL_CreateWindow(
            "TETRIS COMPLETE",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            settings.windowWidth,
            settings.windowHeight,
            windowFlags
        );

    if (!window)
    {
        std::cerr
            << "SDL_CreateWindow failed: "
            << SDL_GetError()
            << std::endl;

        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // ========================================================
    // SDL Renderer
    // ========================================================

    SDL_Renderer* sdlRenderer =
        SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC
        );

    // ハードウェアレンダラーが使えない場合
    if (!sdlRenderer)
    {
        std::cout
            << "Accelerated renderer unavailable. "
            << "Trying software renderer."
            << std::endl;

        sdlRenderer =
            SDL_CreateRenderer(
                window,
                -1,
                SDL_RENDERER_SOFTWARE
            );
    }

    if (!sdlRenderer)
    {
        std::cerr
            << "SDL_CreateRenderer failed: "
            << SDL_GetError()
            << std::endl;

        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(
        sdlRenderer,
        SDL_BLENDMODE_BLEND
    );

    // ========================================================
    // 各システム作成
    // ========================================================

    StageManager stageManager(
        progression
    );

    InputManager input(
        settings
    );

    Game game(
        settings
    );

    UI ui(
        progression,
        stageManager,
        settings
    );

    Renderer renderer(
        window,
        sdlRenderer,
        settings
    );

    // ========================================================
    // アプリ状態
    // ========================================================

    AppState state =
        AppState::Home;

    RenderStyle selectedStyle =
        RenderStyle::TwoD;

    GameMode selectedMode =
        GameMode::Endless;

    int selectedStage = 0;

    bool running = true;

    // ========================================================
    // 時間管理
    // ========================================================

    Uint64 previousCounter =
        SDL_GetPerformanceCounter();

    const double frequency =
        static_cast<double>(
            SDL_GetPerformanceFrequency()
        );

    // 自動セーブ用
    float autoSaveTimer = 0.0f;

    constexpr float AUTO_SAVE_INTERVAL =
        15.0f;

    // ========================================================
    // メインループ
    // ========================================================

    while (running)
    {
        // ----------------------------------------------------
        // Delta Time
        // ----------------------------------------------------

        Uint64 currentCounter =
            SDL_GetPerformanceCounter();

        float deltaTime =
            static_cast<float>(
                (currentCounter - previousCounter)
                / frequency
            );

        previousCounter =
            currentCounter;

        // ウィンドウ移動などで長時間止まった場合、
        // ゲームが一気に進まないよう制限
        deltaTime =
            std::clamp(
                deltaTime,
                0.0f,
                0.05f
            );

        // ----------------------------------------------------
        // 入力開始
        // ----------------------------------------------------

        input.beginFrame();

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }

            input.handleEvent(event);
        }

        input.update(deltaTime);

        // ----------------------------------------------------
        // 全画面共通：終了
        // ----------------------------------------------------

        if (input.quitRequested())
        {
            running = false;
        }

        // ====================================================
        // 状態ごとの更新
        // ====================================================

        switch (state)
        {
            // =================================================
            // HOME
            // =================================================

            case AppState::Home:
            {
                UIAction action =
                    ui.updateHome(
                        input,
                        deltaTime,
                        selectedStyle
                    );

                if (action == UIAction::OpenModeSelect)
                {
                    state =
                        AppState::ModeSelect;
                }
                else if (action == UIAction::OpenSettings)
                {
                    state =
                        AppState::Settings;
                }
                else if (action == UIAction::Quit)
                {
                    running = false;
                }

                break;
            }

            // =================================================
            // MODE SELECT
            // =================================================

            case AppState::ModeSelect:
            {
                UIAction action =
                    ui.updateModeSelect(
                        input,
                        deltaTime,
                        selectedMode
                    );

                if (action == UIAction::OpenStageSelect)
                {
                    state =
                        AppState::StageSelect;
                }
                else if (action == UIAction::Back)
                {
                    state =
                        AppState::Home;
                }

                break;
            }

            // =================================================
            // STAGE SELECT
            // =================================================

            case AppState::StageSelect:
            {
                UIAction action =
                    ui.updateStageSelect(
                        input,
                        deltaTime,
                        selectedStage
                    );

                if (action == UIAction::StartGame)
                {
                    // ------------------------------------------
                    // 新しいゲーム開始
                    // ------------------------------------------

                    game.start(
                        selectedStyle,
                        selectedMode,
                        selectedStage,
                        progression.playerLevel()
                    );

                    state =
                        AppState::Playing;
                }
                else if (action == UIAction::Back)
                {
                    state =
                        AppState::ModeSelect;
                }

                break;
            }

            // =================================================
            // PLAYING
            // =================================================

            case AppState::Playing:
            {
                // ＋ボタン / P
                if (input.pausePressed())
                {
                    game.setPaused(
                        !game.isPaused()
                    );
                }

                // －ボタン長押し
                if (input.restartTriggered())
                {
                    game.restart();
                }

                // ゲーム更新
                game.update(
                    deltaTime,
                    input
                );

                // ----------------------------------------------
                // ゲーム終了
                // ----------------------------------------------

                if (game.hasFinished())
                {
                    RunResult result =
                        game.takeResult();

                    // XP追加・レベルアップ判定
                    ProgressionResult progressResult =
                        progression.applyResult(
                            result
                        );

                    // ステージ解放更新
                    stageManager.refreshUnlocks();

                    // UIへ結果を渡す
                    ui.showResult(
                        result,
                        progressResult
                    );

                    // 終了時は必ず保存
                    saveManager.save(
                        progression,
                        settings
                    );

                    state =
                        AppState::Result;
                }

                // HOMEへ戻る
                if (input.backToMenuPressed())
                {
                    saveManager.save(
                        progression,
                        settings
                    );

                    state =
                        AppState::Home;
                }

                break;
            }

            // =================================================
            // RESULT
            // =================================================

            case AppState::Result:
            {
                UIAction action =
                    ui.updateResult(
                        input,
                        deltaTime
                    );

                if (action == UIAction::Restart)
                {
                    game.start(
                        selectedStyle,
                        selectedMode,
                        selectedStage,
                        progression.playerLevel()
                    );

                    state =
                        AppState::Playing;
                }
                else if (action == UIAction::OpenStageSelect)
                {
                    state =
                        AppState::StageSelect;
                }
                else if (action == UIAction::OpenHome)
                {
                    state =
                        AppState::Home;
                }

                break;
            }

            // =================================================
            // SETTINGS
            // =================================================

            case AppState::Settings:
            {
                UIAction action =
                    ui.updateSettings(
                        input,
                        deltaTime,
                        settings
                    );

                // 設定変更を入力にも反映
                input.applySettings(
                    settings
                );

                // Rendererにも反映
                renderer.applySettings(
                    settings
                );

                if (action == UIAction::Back)
                {
                    saveManager.save(
                        progression,
                        settings
                    );

                    state =
                        AppState::Home;
                }

                break;
            }
        }

        // ====================================================
        // 自動セーブ
        // ====================================================

        autoSaveTimer += deltaTime;

        if (autoSaveTimer >= AUTO_SAVE_INTERVAL)
        {
            autoSaveTimer = 0.0f;

            saveManager.save(
                progression,
                settings
            );
        }

        // ====================================================
        // 描画
        // ====================================================

        renderer.beginFrame();

        renderer.render(
            state,
            selectedStyle,
            selectedMode,
            selectedStage,
            game,
            ui,
            progression,
            stageManager
        );

        renderer.endFrame();
    }

    // ========================================================
    // 終了時セーブ
    // ========================================================

    saveManager.save(
        progression,
        settings
    );

    // ========================================================
    // 終了処理
    // ========================================================

    input.shutdown();

    SDL_DestroyRenderer(
        sdlRenderer
    );

    SDL_DestroyWindow(
        window
    );

    TTF_Quit();
    SDL_Quit();

    return 0;
}
