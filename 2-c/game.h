#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_TURNS 100
#define DEBUG 0

#define OK 0
#define ERR -1

#if DEBUG
#define printf_dbg(...) printf(__VA_ARGS__)
#else
#define printf_dbg(...) /* nothing */
#endif 

typedef struct turn_t {
    int red;
    int green;
    int blue;
} turn_t;

typedef struct game_t {
    turn_t turns[MAX_TURNS];
    int turn_count;
} game_t;

int parse_turn(int len, char *turn_text, turn_t *out) {
    if (out == NULL)
        return ERR;
    // Zero-out struct
    *out = (turn_t) { 0 };

    // Copy string so we don't destroy the string for subsequent turns.
    char copy[100];
    strncpy(copy, turn_text, sizeof(copy) - 1);

    // Iterate segments e.g. 'n blue'
    char *token;
    char *rest = copy;
    while ((token = strtok_r(rest, ",", &rest))) {
        int n;
        char color[20];
        // Scan in segment e.g. "8 green"
        if (sscanf(token, "%d %s", &n, color) != 2)
            return ERR;
        // Copy 'n' to the correct color.
        if (strcmp(color, "red") == 0) {
            out->red = n;
        } else if (strcmp(color, "green") == 0) {
            out->green = n;
        } else if (strcmp(color, "blue") == 0) {
            out->blue = n;
        }
    }

    printf_dbg("Parsed: %i red, %i green, %i blue\n", out->red, out->green, out->blue);

    return OK;
}

int parse_game(int len, char *line, game_t *out) {
    if (out == NULL)
        return ERR;
    // Zero-out struct
    *out = (game_t) { 0 };

    // Skip 'Game: '
    char *str;
    str = strtok(line, ":");
    str = strtok(NULL, ":");

    // We now have a semi-colon separated list of turns.
    char *token;
    while ((token = strtok_r(str, ";", &str))) {
        if (out->turn_count > MAX_TURNS)
            break;
        parse_turn(strlen(token), token, &out->turns[out->turn_count++]);
    }

    printf_dbg("Parsed %i game turns.\n", out->turn_count);

    return OK;
}
