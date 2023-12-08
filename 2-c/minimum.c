#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "game.h"

turn_t get_minimum(game_t *game) {
    int r = 0;
    int g = 0;
    int b = 0;
    for (int i = 0; i < game->turn_count; i++) {
        turn_t *turn = &game->turns[i];
        if (turn->red > r)
            r = turn->red;
        if (turn->green > g)
            g = turn->green;
        if (turn->blue > b)
            b = turn->blue;
    }
    return (turn_t) { r, g, b };
}

int main(int argc, char **argv) {
    char *filename;

    /* Sum the power of the minimum cubes for each game. */
    /* e.g. the sum of (r_i * g_i * b_i) for all i in G. */
    int sum = 0;
    /* Print the minimum required cube counts for each game. */
    int print = 0;

    int opt;
    while ((opt = getopt(argc, argv, "sp")) != -1) {
        switch (opt) {
            case 's':
                sum = 1;
                break;
            case 'p':
                print = 1;
                break;
        }
    }
    filename = argv[optind];

    FILE *fp;
    if (strcmp(filename, "-") == 0) {
        printf("Using stdin as input.\n");
        fp = stdin;
    } else {
        printf("Using file '%s' as input.\n", filename);
        fp = fopen(filename, "r");
    }

    if (print)
        printf("Printing minimum cube counts for all games.\n");

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    int rt = 0;

    char copy[500];
    while ((read = getline(&line, &len, fp)) != -1) {
        printf_dbg("Line: %s", line);

        // Copy the line so that we don't destroy it with strtok.
        strncpy(copy, line, sizeof(copy) - 1);

        game_t game;
        parse_game(len, copy, &game);

        turn_t minimum = get_minimum(&game);
        if (print)
            printf("Game %i requires %i red cubes, %i green cubes, and %i blue cubes.\n",
                    ++i,
                    minimum.red,
                    minimum.green,
                    minimum.blue);

        rt += (minimum.red * minimum.green * minimum.blue);
    }

    if (sum) {
        printf("\n");
        printf("The sum of the powers of each game minimum is %i.\n", rt);
    }

    fclose(fp);
    return 0;
}
