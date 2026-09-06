#pragma once

#include <string>

#include "progression.hpp"
#include "settings.hpp"

// ============================================================
// TETRIS COMPLETE
// save.hpp
//
// セーブ / ロード管理
// ============================================================

class SaveManager
{
public:
    explicit SaveManager(
        const std::string& path
    );

    bool exists() const;

    bool load(
        Progression& progression,
        Settings& settings
    );

    bool save(
        const Progression& progression,
        const Settings& settings
    );

private:
    std::string mPath;
};
