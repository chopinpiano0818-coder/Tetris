#pragma once

// ============================================================
// TETRIS COMPLETE
// types.hpp
//
// 全ファイルで共通して使う型・列挙型
// ============================================================

// ------------------------------------------------------------
// アプリの現在画面
// ------------------------------------------------------------
enum class AppState
{
    Home,           // ホーム
    ModeSelect,     // モード選択
    StageSelect,    // ステージ選択
    Playing,        // ゲーム中
    Result,         // リザルト
    Settings        // 設定
};

// ------------------------------------------------------------
// 2D / 3D
// ------------------------------------------------------------
enum class RenderStyle
{
    TwoD,
    ThreeD
};

// ------------------------------------------------------------
// ゲームモード
// ------------------------------------------------------------
enum class GameMode
{
    Endless,    // ゲームオーバーまで
    Runner,     // 制限時間内でスコアを稼ぐ
    Sprint,     // 40ラインのタイムアタック
    Ultra       // 3分間スコアアタック
};

// ------------------------------------------------------------
// UIからmain.cppへ返す命令
// ------------------------------------------------------------
enum class UIAction
{
    None,

    OpenHome,
    OpenModeSelect,
    OpenStageSelect,
    OpenSettings,

    StartGame,
    Restart,

    Back,
    Quit
};

// ------------------------------------------------------------
// テトリミノ種類
// ------------------------------------------------------------
enum class TetrominoType
{
    I,
    O,
    T,
    S,
    Z,
    J,
    L,
    None
};

// ------------------------------------------------------------
// ゲーム終了理由
// ------------------------------------------------------------
enum class FinishReason
{
    None,

    GameOver,       // ブロックが上まで積み上がった
    TimeUp,         // Runner / Ultra
    SprintClear,    // Sprint 40ライン達成
    Quit
};

// ------------------------------------------------------------
// 1プレイの結果
// ------------------------------------------------------------
struct RunResult
{
    GameMode mode = GameMode::Endless;
    RenderStyle style = RenderStyle::TwoD;

    int stage = 0;

    int score = 0;
    int lines = 0;

    // ゲーム内レベル
    int gameLevel = 1;

    // プレイ時間（秒）
    float playTime = 0.0f;

    // 4ライン消し回数
    int tetrises = 0;

    // 最大コンボ
    int maxCombo = 0;

    // このプレイで獲得するXP
    int earnedXP = 0;

    FinishReason finishReason =
        FinishReason::None;
};

// ------------------------------------------------------------
// XP適用後の結果
// ------------------------------------------------------------
struct ProgressionResult
{
    int xpEarned = 0;

    int oldLevel = 1;
    int newLevel = 1;

    bool levelUp = false;

    // 今回新しくアンロックされたステージ数
    int stagesUnlocked = 0;
};

// ------------------------------------------------------------
// 3Dカメラ
// ------------------------------------------------------------
struct Camera3D
{
    // 左右
    float yaw = 0.0f;

    // 上下
    float pitch = 15.0f;

    // 拡大率
    float zoom = 1.0f;

    void reset()
    {
        yaw = 0.0f;
        pitch = 15.0f;
        zoom = 1.0f;
    }
};

// ------------------------------------------------------------
// 2D座標
// ------------------------------------------------------------
struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;
};

// ------------------------------------------------------------
// 3D座標
// ------------------------------------------------------------
struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

// ------------------------------------------------------------
// アニメーション種類
// ------------------------------------------------------------
enum class AnimationType
{
    None,

    MenuTransition,
    PieceSpawn,
    PieceMove,
    PieceRotate,
    HardDrop,
    PieceLand,

    LineClear,
    TetrisClear,

    LevelUp,
    StageUnlock,

    GameOver,
    ResultCountUp
};

// ------------------------------------------------------------
// ステージ情報
// ------------------------------------------------------------
struct StageInfo
{
    int id = 0;

    const char* name = "CLASSIC";

    // このプレイヤーレベルで解禁
    int requiredLevel = 1;

    bool unlocked = false;
};

// ------------------------------------------------------------
// 入力方向
// ------------------------------------------------------------
enum class Direction
{
    None,
    Left,
    Right,
    Down,
    Up
};
