#pragma once
#include "../platform/terminal_backend.hpp"
#include "../platform/paths.hpp"
#include "../config/config.hpp"
#include "../save/save_manager.hpp"
#include "../game/session.hpp"
#include "../tui/screen.hpp"
#include "cli.hpp"
#include <memory>
#include <cstdint>

namespace shadowdeep {

class App {
public:
    explicit App(CliOptions opts);
    int run();

private:
    CliOptions opts_;
    AppPaths paths_;
    std::unique_ptr<ITerminalBackend> terminal_;
    std::unique_ptr<ConfigManager> configMgr_;
    std::unique_ptr<SaveManager> saveMgr_;
    ScreenBuffer screen_;

    bool init();
    void shutdown();

    int runHome();
    int runGameplay(GameSession& session);

    void renderHome(int selected);
    void renderEntries(const std::vector<EntryMetadata>& entries, int cursor, int scroll);
    void renderNewEntryScreen(std::string& entryName, std::string& charName, int& pclass, uint32_t& seed, int field);
    void renderGameplay(const GameSession& session);
    void renderInventory(GameSession& session, int cursor);
    void renderHelp();
    void renderCodex(GameSession& session, int tab, int cursor);
    void renderSettings();
    void renderChangelog();
    void renderCredits();
    void renderMessageLog(const GameSession& session);

    std::string promptString(const std::string& prompt, const std::string& initial = "");

    bool confirm(const std::string& msg);

    void showDeathScreen(const GameSession& session);
    void showWinScreen(const GameSession& session);

    void blit();
};

}
