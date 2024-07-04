#include "utils.h"

#include <Windows.h>
#include <iostream>

namespace utils {
    void create_console() {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    }

    void destroy_console() {
        fclose(stdout);
        fclose(stderr);
        FreeConsole();
    }

    void kill_process() {
        destroy_console();
        HANDLE curr_proc = GetCurrentProcess();
        TerminateProcess(curr_proc, NULL);
        CloseHandle(curr_proc);
    }
}