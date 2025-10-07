#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    bool force = true;

    const char *sources[] = {
        "main.cpp",
        "window.cpp",
        "server.cpp",
        "game.cpp",
        "client.cpp",
    };
    size_t sources_count = sizeof(sources) / sizeof(sources[0]);

    Nob_Procs procs = {0};
    char *obj_files[sources_count];

    for (size_t i = 0; i < sources_count; ++i) {
        const char *src = sources[i];
        char *obj = nob_temp_sprintf("%.*s.o", (int)(strlen(src)-4), src);
        obj_files[i] = obj;

        if (nob_needs_rebuild1(obj, src) || force) {
            Nob_Cmd cmd = {0};
            nob_cmd_append(&cmd, "c++", "-c", src, "-o", obj, "-I./");
            nob_cmd_append(&cmd, "-ggdb");
            nob_cmd_run(&cmd, .async = &procs);
        }
    }

    if (!nob_procs_flush(&procs)) return 1;

    if (nob_needs_rebuild("main", (const char **)obj_files, sources_count)) {
        Nob_Cmd link_cmd = {0};
        nob_cmd_append(&link_cmd, "c++");
        for (size_t i = 0; i < sources_count; ++i) {
            nob_cmd_append(&link_cmd, obj_files[i]);
        }
        nob_cmd_append(&link_cmd, "-o", "main");
        nob_cmd_append(&link_cmd, "-ggdb");
        nob_cmd_append(&link_cmd, "-lm", "-l:libraylib.a", "-L./raylib5.5/lib");
        if (!nob_cmd_run(&link_cmd)) return 1;
    }

    nob_shift(argv, argc);
    if (argc >= 1) {
        Nob_Cmd run_cmd = {0};
        if (strcmp(argv[0], "-s") == 0) {
            nob_cmd_append(&run_cmd, "./main", "-server");
        } else {
            nob_cmd_append(&run_cmd, "./main");
        }
        if (!nob_cmd_run(&run_cmd)) return 1;
    }

    return 0;
}
