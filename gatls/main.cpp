//
// Created by dc on 6/6/16.
//
#include <cstring>
#include "gatls_cli.h"
#include "log.h"

using namespace gatls;


int main(int argc, char* argv[])
{
    int     cargc   = argc-1;
    char    **cargv = argv+1;
    if (cargc > 0) {
        int cliLen = strlen(cargv[0]);
        for(int i=0; i < sizeof(cliCmds)/sizeof(CliCommand); i++) {
            if (cliLen != strlen(cliCmds[i].cmd)) continue;
            if (strcmp(cargv[0], cliCmds[i].cmd) == 0) {
                // execute cli command
                if (!cliCmds[i].fn(cargc-1, cargv+1)) {
                    // Command failed, show command help
                    if (cliCmds[i].help) {
                        cliCmds[i].help();
                    }
                    return EXIT_FAILURE;
                }
                // Command success
                return EXIT_SUCCESS;
            }
        }

        // We shouldn't be here if it is a valid command
        GAP_Perr("Gatls", "unrecognized command '%s'", cargv[0]);
    }
    // show help
    return EXIT_SUCCESS;
}



