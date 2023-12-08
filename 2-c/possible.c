#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "game.h"

int check_game(game_t *game, int r, int g, int b) {
    for (int i = 0; i < game->turn_count; i++) {
        turn_t *turn = &game->turns[i];
        // If any cube count exceeds the actual counts, the game is not possible.
        // Return an error in this case.
        if (turn->red   > r ||
            turn->green > g ||
            turn->blue  > b)
            return ERR;
    }
    // The game is possible.
    return OK;
}

int main(int argc, char **argv) {
    char *filename;

    int red   = 0;
    int green = 0;
    int blue  = 0;

    /* Sum the indices of all valid games. */
    int sum = 0;
    /* Print all games which are possible given the r,g,b cube counts. */
    int print = 0;

    int opt;
    while ((opt = getopt(argc, argv, "r:g:b:sp")) != -1) {
        switch (opt) {
            case 'r':
                red = strtoimax(optarg, (char **)NULL, 10);
                break;
            case 'g':
                green = strtoimax(optarg, (char **)NULL, 10);
                break;
            case 'b':
                blue = strtoimax(optarg, (char **)NULL, 10);
                break;
            case 's':
                sum = 1;
                break;
            case 'p':
                print = 1;
                break;
        }
    }
    filename = argv[optind];

    if (red == 0 || green == 0 || blue == 0) {
        printf("ERROR: Must pass an integer value for r,g,b flags.");
        printf("");
        printf("e.g. ./possible FILENAME -r 10 -g 15 -b 13");
        exit(1);
    }

    printf("There are %i red cubes, %i green cubes, and %i blue cubes in the bag.\n",
            red,
            green,
            blue);

    FILE *fp;
    if (strcmp(filename, "-") == 0) {
        printf("Using stdin as input.\n");
        fp = stdin;
    } else {
        printf("Using file '%s' as input.\n", filename);
        fp = fopen(filename, "r");
    }

    if (print)
        printf("Printing all possible games.\n");

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
        i++;

        if (check_game(&game, red, green, blue) == OK) {
            printf_dbg("Game %i is possible.\n", i);

            if (print)
                printf("%s", line);
            
            // Keep a running total of the IDs.
            rt += i;
        }
    }

    if (sum) {
        printf("\n");
        printf("The sum of all possible game IDs is %i.\n", rt);
    }

    fclose(fp);
    return 0;
}
