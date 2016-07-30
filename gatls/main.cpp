//
// Created by dc on 6/6/16.
//
#include <cstring>
#include "gatls_cli.h"
#include "log.h"
#include "version.h"

using namespace gatls;

#ifndef SUIL_BUILD_TAG
#ifdef SUIL_DEBUG
#define SUIL_BUILD_TAG     "Debug"
#else
#define SUIL_BUILD_TAG      "Release"
#endif
#endif

#ifndef __EXECUTABLE_NAME
#define __EXECUTABLE_NAME   "gatls"
#endif

void gatls::showVersionInfo() {
    const gap::Version& v = gap::gapVersion();
    fprintf(stdout, "%s (suil): v%d.%d %d %s\n",
            __EXECUTABLE_NAME, v.major, v.minor, v.patch,
            SUIL_BUILD_TAG);
    fprintf(stdout, "Copyright (C) 2016 suilteam\n");
    fprintf(stdout, "The MIT License (MIT)\n");
    fprintf(stdout, "This is free software: you are free to change and redistribute it.\n");
    fprintf(stdout, "There is NO WARRANTY, to the extent permitted by law.\n");
}

void gatls::showHelp() {
    fprintf(stdout, "Usage\n\n");
    fprintf(stdout, "%s [COMMAND]... [OPTION]...\n\n", __EXECUTABLE_NAME);
    fprintf(stdout, "%s create name [OPTION]... create a new gap project.\n", __EXECUTABLE_NAME);
    showCreateHelp();
    fprintf(stdout, "%s build [OPTION]... build current gap project.\n", __EXECUTABLE_NAME);
    showBuildHelp();
    fprintf(stdout, "%s clean [OPTION]... cleans current gap project.\n", __EXECUTABLE_NAME);
    showCleanHelp();
    fprintf(stdout, "%s run [OPTION]... run the project locally.\n", __EXECUTABLE_NAME);
    showRunHelp();
    fprintf(stdout, "%s remote [OPTION]... interact with remote suil server.\n", __EXECUTABLE_NAME);
    showRemoteHelp();
    fprintf(stdout, "%s help [COMMAND]... show this help\n", __EXECUTABLE_NAME);
    fprintf(stdout, "        if a command is specified, the help associated with the command will be displayed\n");
    fprintf(stdout, "        eg. %s help create - this will show create's help\n", __EXECUTABLE_NAME);
}


static int cliCmdVersion(int argc, char* argv[]) {
    gatls::showVersionInfo();
}

static const CliCommand cliCmds[] =  {
        {"create", create, nullptr},
        {"build", build, nullptr},
        {"clean", clean, nullptr},
        {"run", run, nullptr},
        {"remote", remote, nullptr},
        {"--version", cliCmdVersion, nullptr},
        {"help", help, nullptr}
};

static const inline CliCommand* getCliCommand(const char *arg) {
    size_t cliLen = strlen(arg);
    for(int i=0; i < sizeof(cliCmds)/sizeof(CliCommand); i++) {
        if (cliLen == strlen(cliCmds[i].cmd) && strcmp(arg, cliCmds[i].cmd) == 0) {
            return &cliCmds[i];
        }
    }
    return nullptr;
}

int main(int argc, char* argv[])
{
    int     cargc   = argc-1;
    char    **cargv = argv+1;
    int     status  = EXIT_SUCCESS;
    if (cargc > 0) {
        int cliLen = strlen(cargv[0]);
        const CliCommand *cmd = getCliCommand(cargv[0]);
        if (!cmd)
        {
            // We shouldn't be here if it is a valid command
            GAP_Perr("Gatls", "unrecognized command '%s'", cargv[0]);
            showHelp();
            status  = EXIT_FAILURE;
        }
        else {
            status = cmd->fn(cargc-1, cargv+1);
            if (status != EXIT_SUCCESS) {
                // Command failed, or not available
                if (cmd->help)
                {
                    cmd->help();
                }
            }
        }
    }
    else
    {
        showVersionInfo();
        showHelp();
        status  = EXIT_SUCCESS;
    }
    return status;
}

int gatls::help(int argc, char *argv[]) {
    if (argc == 0) {
        showHelp();
    } else {
        const CliCommand *cmd = getCliCommand(argv[0]);
        if (!cmd || !cmd->help) {
            showHelp();
            return EXIT_FAILURE;
        } else {
            cmd->help();
        }
    }

    return EXIT_SUCCESS;
}

int gatls::remote(int argc, char *argv[]) {
    return EXIT_FAILURE;
}