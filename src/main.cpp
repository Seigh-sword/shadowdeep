#include "shadowdeep/app/cli.hpp"
#include "shadowdeep/app/app.hpp"

int main(int argc, char** argv) {
    auto opts = shadowdeep::parseCli(argc, argv);

    if (opts.showHelp || opts.showVersion || opts.showAbout || opts.showLicense || opts.checkUpdate || opts.doUpdate) {
        if (opts.showHelp) shadowdeep::printHelp(argv[0]);
        else if (opts.showVersion) shadowdeep::printVersion();
        else if (opts.showAbout) shadowdeep::printAbout();
        else if (opts.showLicense) shadowdeep::printLicense();
        else {
            shadowdeep::App app(opts);
            return app.run();
        }
        return 0;
    }

    shadowdeep::App app(opts);
    return app.run();
}
