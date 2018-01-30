
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
    bool
        lib = FALSE,
        bin = FALSE,
        app = FALSE,
        path = FALSE,
        env = FALSE,
        etc = FALSE,
        include = FALSE,
        no_arg = FALSE;

    if (argc <= 1) {
        printf("please provide a package name\n");
        goto error;
    }

    if (argc > 2) {
        int i = 0;
        for (i = 2; i < argc; i++) {
            if (!strcmp(argv[i], "--path")) {
                path = TRUE;
            } else if (!strcmp(argv[i], "--env")) {
                env = TRUE;
            } else if (!strcmp(argv[i], "--etc")) {
                etc = TRUE;
            } else if (!strcmp(argv[i], "--include")) {
                include = TRUE;
            } else if (!strcmp(argv[i], "--lib")) {
                lib = TRUE;
            } else if (!strcmp(argv[i], "--app")) {
                app = TRUE;
            } else if (!strcmp(argv[i], "--bin")) {
                bin = TRUE;
            }
            if (!strcmp(argv[i], "--verbose")) {
                corto_log_verbositySet(CORTO_DEBUG);
            }
        }
    }

    const char *const_location = NULL;
    if (path) {
        const_location = corto_locate(argv[1], NULL, CORTO_LOCATE_PACKAGE);
    } else if (env) {
        const_location = corto_locate(argv[1], NULL, CORTO_LOCATE_ENV);
    } else if (etc) {
        const_location = corto_locate(argv[1], NULL, CORTO_LOCATE_ETC);
    } else if (include) {
        const_location = corto_locate(argv[1], NULL, CORTO_LOCATE_INCLUDE);
    } else if (lib) {
        const_location = corto_locate(argv[1], NULL, CORTO_LOCATE_LIB);
    } else if (app) {
        const_location = corto_locate(argv[1], NULL, CORTO_LOCATE_APP);
    } else if (bin) {
        const_location = corto_locate(argv[1], NULL, CORTO_LOCATE_BIN);
    } else {
        const_location = corto_locate(argv[1], NULL, CORTO_LOCATE_BIN);
        if (!const_location) {
            const_location = corto_locate(argv[1], NULL, CORTO_LOCATE_PACKAGE);
        }
        if (const_location) {
            corto_log("#[bold]%s#[grey] => %s\n", argv[1], const_location);
        }
        no_arg = TRUE;
    }

    if (!no_arg && const_location) {
        printf("%s\n", const_location);
    } else if (!const_location) {
        corto_log("#[bold]%s#[grey] => #[red]not found!\n", argv[1]);
        goto error;
    }

    return 0;
error:
    return -1;
}
