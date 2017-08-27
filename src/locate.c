
#include <corto/corto.h>
#include <driver/tool/locate/locate.h>

void cortotool_toLibPath(char *location) {
    char *ptr, ch;
    ptr = &location[strlen(location) - 1];
    while ((ch = *ptr) && (ptr >= location)) {
        if (ch == '/') {
            *ptr = '\0';
            break;
        }
        ptr --;
    }
}

int cortomain(int argc, char *argv[]) {
    char *location;
    bool lib = FALSE, path = FALSE, env = FALSE, silent = FALSE, lib_redistr = FALSE;
    bool error_only = FALSE;

    if (argc <= 1) {
        printf("please provide a package name\n");
        goto error;
    }

    if (argc > 2) {
        int i = 0;
        for (i = 2; i < argc; i++) {
            if (!strcmp(argv[i], "--lib")) {
                lib = TRUE;
            } else if (!strcmp(argv[i], "--path")) {
                path = TRUE;
            } else if (!strcmp(argv[i], "--lib-redistr")) {
                lib_redistr = TRUE;
            } else if (!strcmp(argv[i], "--env")) {
                env = TRUE;
            } else if (!strcmp(argv[i], "--silent")) {
                silent = TRUE;
            } else if (!strcmp(argv[i], "--error_only")) {
                error_only = TRUE;
            }
            if (!strcmp(argv[i], "--verbose")) {
                corto_verbosity(CORTO_DEBUG);
            }
        }
    }

    if (!lib_redistr) {
        location = corto_locate(argv[1], NULL, CORTO_LOCATION_LIB);
    } else {
        corto_id package;
        strcpy(package, argv[1][0] == '/' ? argv[1] + 1 : argv[1]);
        char *ptr = package, ch;
        while ((ch = *ptr)) {
            if (ch == '/') {
                *ptr = '_';
            }
            ptr++;
        }
        location = corto_envparse("$CORTO_TARGET/redistr/corto/$CORTO_VERSION/lib/lib%s.so", package);
        if (!corto_fileTest(location)) {
            corto_trace("library '%s' not found", location);
            corto_dealloc(location);
            location = NULL;
        }
    }

    if (location) {
        if (env) {
            char *ptr = location;
            while (*ptr) {
                if (!memcmp(ptr, "/lib", 4)) {
                    *ptr = '\0';
                    break;
                }
                ptr++;
            }
        } else if (path && !lib) {
            cortotool_toLibPath(location);
        }

        if (lib || lib_redistr || path || env) {
            if (!silent && !error_only) printf("%s\n", location);
        } else {
            if (!silent && !error_only) printf("%s%s%s  =>  %s\n", CORTO_CYAN, argv[1], CORTO_NORMAL, location);
        }
    } else {
        if (!silent) {
            if (!error_only) {
                if (lib_redistr) {
                    printf("redistributable library for package '%s' not found", argv[1]);
                } else {
                    printf("package '%s' not located: ", argv[1]);
                }
            }
            if (corto_lastinfo()) {
                printf("%s\n", corto_lastinfo());
            } else {
                printf("\n");
            }
        }

        /* This will mute the 'cortomain failed' error */
        corto_seterr("");
        goto error;
    }

    corto_dealloc(location);

    return 0;
error:
    return -1;
}

