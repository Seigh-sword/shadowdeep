#include "shadowdeep/app/cli.hpp"
#include "shadowdeep/version.hpp"
#include "shadowdeep/world/dungeon.hpp"
#include "shadowdeep/game/session.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

namespace shadowdeep {

CliOptions parseCli(int argc, char** argv) {
    CliOptions opts;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") opts.showHelp = true;
        else if (arg == "--version" || arg == "-v") opts.showVersion = true;
        else if (arg == "--about") opts.showAbout = true;
        else if (arg == "--license") opts.showLicense = true;
        else if (arg == "--check-update") opts.checkUpdate = true;
        else if (arg == "--update") opts.doUpdate = true;
        else if (arg == "--auto-update") opts.autoUpdate = true;
        else if (arg == "--run-tests") opts.runTests = true;
        else if (arg == "--seed" && i + 1 < argc) {
            try {
                opts.seed = static_cast<uint32_t>(std::stoul(argv[++i]));
            } catch (...) {
                opts.seed = 0;
            }
        } else if (arg.rfind("--seed=", 0) == 0) {
            try {
                opts.seed = static_cast<uint32_t>(std::stoul(arg.substr(7)));
            } catch (...) {}
        } else if (arg == "--entry" && i + 1 < argc) {
            opts.entryToLoad = argv[++i];
        } else {
            opts.extraArgs.push_back(arg);
        }
    }
    return opts;
}

void printHelp(const char* exe) {
    std::cout << "SHADOWDEEP " << kGameVersion << " - A roguelike dungeon crawler (Infinite Depths)\n\n";
    std::cout << "Usage: " << exe << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help            Show this help\n";
    std::cout << "  --version         Show version\n";
    std::cout << "  --about           Show about information\n";
    std::cout << "  --license         Show license\n";
    std::cout << "  --seed <N>        Set RNG seed for deterministic runs\n";
    std::cout << "  --check-update    Check for updates (API + direct URL fallback)\n";
    std::cout << "  --update          Download update (staged)\n";
    std::cout << "  --auto-update     Download and auto-install update if possible\n";
    std::cout << "  --entry <id>      Load specific entry directly\n";
    std::cout << "\nDirect download pattern:\n";
    std::cout << "  https://github.com/Seigh-sword/shadowdeep/releases/download/(version)/(file)\n";
    std::cout << "  Example: https://github.com/Seigh-sword/shadowdeep/releases/download/Zv1/shadowdeep-Zv1-linux-x86_64.tar.gz\n";
    std::cout << "\nControls:\n";
    std::cout << "  Arrow keys / hjkl / yubn   Move or attack\n";
    std::cout << "  g / ,                      Pick up\n";
    std::cout << "  i                          Inventory (side panel)\n";
    std::cout << "  c                          Codex / Guides / Lore\n";
    std::cout << "  q                          Quaff potion\n";
    std::cout << "  r                          Read scroll\n";
    std::cout << "  >                          Descend (infinite beyond 30)\n";
    std::cout << "  <                          Ascend\n";
    std::cout << "  z / .                      Wait\n";
    std::cout << "  ?                          Help (essentials)\n";
    std::cout << "  Q                          Quit\n";
}

void printVersion() {
    std::cout << kGameVersion << "\n";
}

void printAbout() {
    std::cout << "SHADOWDEEP " << kGameVersion << " (" << kVersionHuman << ") - Infinite Depths\n";
    std::cout << "A roguelike dungeon crawler written in C++20\n\n";
    std::cout << "Made by: " << kAuthor << "\n";
    std::cout << "Repository: " << kRepositoryUrl << "\n";
    std::cout << "Releases: " << kRepositoryUrl << "/releases\n";
    std::cout << "Direct: " << kRepositoryUrl << "/releases/download/(version)/(file)\n";
    std::cout << "License: " << kLicenseId << "\n";
    std::cout << "\nSave schema: " << kSaveSchemaVersion << "\n";
    std::cout << "Config schema: " << kConfigSchemaVersion << "\n";
    std::cout << "Build revision: " << kBuildRevision << "\n";
    std::cout << "Map: " << kMapW << "x" << kMapH << " Viewport: " << kViewportW << "x" << kViewportH << "\n";
    std::cout << "Max Depth: " << kMaxDepth << " (Win at " << kOriginalMaxDepth << ", infinite beyond)\n";
}

void printLicense() {
    std::cout << "ISC License\n\n";
    std::cout << "Copyright (c) 2026 Seigh-sword\n\n";
    std::cout << "Permission to use, copy, modify, and/or distribute this software for any\n";
    std::cout << "purpose with or without fee is hereby granted, provided that the above\n";
    std::cout << "copyright notice and this permission notice appear in all copies.\n\n";
    std::cout << "THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES\n";
    std::cout << "WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF\n";
    std::cout << "MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR\n";
    std::cout << "ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES\n";
    std::cout << "WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN\n";
    std::cout << "ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF\n";
    std::cout << "OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.\n";
}

}
