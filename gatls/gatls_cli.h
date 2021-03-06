//
// Created by dc on 6/11/16.
//

#ifndef GAR_SUIL_ARGS_H
#define GAR_SUIL_ARGS_H

#include <string>
#include <wait.h>
#include <unistd.h>
#include <sys/types.h>

namespace gatls {

    extern int create(int argc, char *argv[]);
    extern void showCreateHelp();
    extern int build(int argc, char* argv[]);
    extern void showBuildHelp();
    extern int clean(int argc, char* argv[]);
    extern void showCleanHelp();
    extern int run(int argc, char* argv[]);
    extern void showRunHelp();
    extern int remote(int argc, char* argv[]);
    extern void showRemoteHelp();
    extern int help(int argc, char* argv[]);
    extern void showHelp();
    extern void showVersionInfo();

    struct CliCommand {
        const char    *cmd;
        int (*fn)(int, char**);
        void (*help)(void);
    };

    /**
     * Helper functions
     */
    static bool   spawnCommand(void (*fn)(void *), void *arg) {
        pid_t       pid;
        int         status = 0;

        pid = ::fork();
        switch (pid) {
            case -1:
                fprintf(stderr, "ERROR: spawnCommand::fork() failed - %s\n", strerror(errno));
                status = -1;
                break;
            case 0:
                fn(arg);
                fprintf(stderr, "spawnCommand - %s\n", strerror(errno));
                break;
            default:
                break;
        }

        if (status == -1) {
            return false;
        }

        // Wait for process
        if (waitpid(pid, &status, 0) == -1) {
            fprintf(stderr, "ERROR: waiting for command process %d failed\n", pid);
            return false;
        }

        if (WEXITSTATUS(status) || WTERMSIG(status) || WCOREDUMP(status)) {
            fprintf(stderr, "ERROR: command execution failed, see output for more info\n");
            return false;
        }

        return true;
    }
}
#endif //GAR_SUIL_ARGS_H
