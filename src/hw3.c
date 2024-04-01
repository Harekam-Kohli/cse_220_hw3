#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)

GameState* initialize_game_state(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        // printf("File cannot be opened.\n");
        return NULL;
    }


    int rows = 0, cols = 0;
    char line[1024];


    // First pass: Determine the size of the board
    while (fgets(line, sizeof(line), file))
    {
        if (cols == 0)
        {
            cols = strcspn(line, "\n"); // More accurate count of cols, excluding the newline
        }
        rows++;
    }
    fclose(file); // Close and reopen the file for a second pass


    // Allocate GameState
    GameState *game = (GameState *)malloc(sizeof(GameState));
    if (!game)
    {
        printf("Memory allocation for GameState failed.\n");
        return NULL;
    }
    game->rows = rows;
    game->cols = cols;


    // Allocate memory for stackHeight (2D array)
    game->height = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        game->height[i] = (int *)malloc(cols * sizeof(int));
        memset(game->height[i], 0, cols * sizeof(int)); // Initialize to 0
    }


    // Allocate memory for boardHistory (3D array) with fixed history size of 5
    int historySize = 5;
    game->currentBoard = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++)
    {
        game->currentBoard[i] = malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++)
        {
            game->currentBoard[i][j] = malloc(historySize * sizeof(char));
        }
    }


    // Second read: Populate the board and stack heights
    file = fopen(filename, "r");
    int row = 0;
    while (fgets(line, sizeof(line), file) && row < rows)
    {
        for (int col = 0; col < cols && line[col] != '\n' && line[col] != '\0'; col++)
        {
            if (line[col] != '.')
            {
                game->currentBoard[row][col][0] = line[col]; // Assume first layer for initial setup
                game->height[row][col] = 1;             // There's one letter at this spot
            }
        }
        row++;
    }


    fclose(file);
    return game;
}

GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    (void)game;
    (void)row;
    (void)col;
    (void)direction;
    (void)tiles;
    (void)num_tiles_placed;
    return game;
}

GameState* undo_place_tiles(GameState *game) {
    (void)game;
    return game;
}

void free_game_state(GameState *game) {
     if (game != NULL)
    {
        for (int i = 0; i < game->rows; i++)
        {
            for(int j = 0; j < game->cols; j++)
            {
                free(game->currentBoard[i][j]);
            }


            free(game->currentBoard[i]);
            free(game->height[i]);
        }
        free(game->currentBoard);
        free(game->height);
        free(game);
    }

}

void save_game_state(GameState *game, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Failed to open file %s for writing.\n", filename);
        return;
    }

    // Save the top visible layer (current state of the game)
    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            int stackHeight = game->height[i][j]; // Retrieve the stack height at position (i, j)
            if (stackHeight == 0) {
                // If there's no tile, use a dot as placeholder
                fprintf(file, ".");
            } else {
                // Access the top tile using height-1 and write it to the file
                char topTile = game->currentBoard[i][j][stackHeight - 1];
                fprintf(file, "%c", topTile);
            }
        }
        fprintf(file, "\n"); // Newline at the end of each row
    }

    // Save the heights of each stack
    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            fprintf(file, "%d", game->height[i][j]);
        }
        fprintf(file, "\n"); // Newline at the end of each row
    }

    fclose(file);
}
