#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    const char *sources[] = {
        "main.cpp",
        "window.cpp",
        "server.cpp",
    };
    size_t sources_count = sizeof(sources) / sizeof(sources[0]);

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "c++");
    for (size_t i = 0; i < sources_count; ++i) nob_cmd_append(&cmd, sources[i]);
    nob_cmd_append(&cmd, "-o", "main");
    nob_cmd_append(&cmd, "-lm", "-l:libraylib.a", "-L./raylib5.5/lib");

    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    nob_cmd_append(&cmd, "./main", "-server");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    return 0;
}
