#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define SIZE 9
#define SUB 3
#define INF 1000000

typedef struct {
    int grid[SIZE][SIZE];
    int fixed[SIZE][SIZE];
    int energy;
} SudokuState;

// Calculates the energy (number of conflicts in rows and columns)
int calculate_energy(SudokuState *state) {
    int energy = 0;

    // Row conflicts
    for (int i = 0; i < SIZE; i++) {
        int count[SIZE + 1] = {0};
        for (int j = 0; j < SIZE; j++) count[state->grid[i][j]]++;
        for (int k = 1; k <= SIZE; k++) if (count[k] > 1) energy += count[k] - 1;
    }

    // Column conflicts
    for (int j = 0; j < SIZE; j++) {
        int count[SIZE + 1] = {0};
        for (int i = 0; i < SIZE; i++) count[state->grid[i][j]]++;
        for (int k = 1; k <= SIZE; k++) if (count[k] > 1) energy += count[k] - 1;
    }

    return energy;
}

// Initializes the Sudoku board by filling missing values randomly within each block
void initialize_state(SudokuState *state) {
    for (int bi = 0; bi < SIZE; bi += SUB) {
        for (int bj = 0; bj < SIZE; bj += SUB) {
            int present[SIZE + 1] = {0};
            int idx = 0;
            int missing[SIZE];

            // Track existing values
            for (int i = 0; i < SUB; i++) {
                for (int j = 0; j < SUB; j++) {
                    int val = state->grid[bi + i][bj + j];
                    if (val) present[val] = 1;
                }
            }

            // Collect missing values
            for (int k = 1; k <= SIZE; k++) {
                if (!present[k]) missing[idx++] = k;
            }

            // Shuffle missing values
            for (int i = idx - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = missing[i];
                missing[i] = missing[j];
                missing[j] = temp;
            }

            // Fill empty cells with shuffled values
            idx = 0;
            for (int i = 0; i < SUB; i++) {
                for (int j = 0; j < SUB; j++) {
                    if (state->grid[bi + i][bj + j] == 0) {
                        state->grid[bi + i][bj + j] = missing[idx++];
                    }
                }
            }
        }
    }

    state->energy = calculate_energy(state);
}

// Generates a neighbor state by swapping two unfixed cells in a random block
void generate_neighbor(SudokuState *current_state, SudokuState *neighbor_state) {
    memcpy(neighbor_state, current_state, sizeof(SudokuState));

    int bi = (rand() % SUB) * SUB;
    int bj = (rand() % SUB) * SUB;

    int cells[SIZE][2];
    int count = 0;

    for (int i = 0; i < SUB; i++) {
        for (int j = 0; j < SUB; j++) {
            int r = bi + i;
            int c = bj + j;
            if (!neighbor_state->fixed[r][c]) {
                cells[count][0] = r;
                cells[count][1] = c;
                count++;
            }
        }
    }

    if (count >= 2) {
        int a = rand() % count;
        int b = rand() % count;
        while (b == a) b = rand() % count;

        int r1 = cells[a][0], c1 = cells[a][1];
        int r2 = cells[b][0], c2 = cells[b][1];

        int tmp = neighbor_state->grid[r1][c1];
        neighbor_state->grid[r1][c1] = neighbor_state->grid[r2][c2];
        neighbor_state->grid[r2][c2] = tmp;
    }

    neighbor_state->energy = calculate_energy(neighbor_state);
}

// Simulated Annealing algorithm to solve Sudoku
void runSA(int **board) {
    SudokuState current_state;
    memset(&current_state, 0, sizeof(SudokuState));

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            current_state.grid[i][j] = board[i][j];
            current_state.fixed[i][j] = (board[i][j] != 0);
        }
    }

    initialize_state(&current_state);

    // SA parameters
    double T_start = 1000.0;
    double T_end = 0.01;
    double alpha = 0.99;

    SudokuState best_state;
    memcpy(&best_state, &current_state, sizeof(SudokuState));

    while (T_start > T_end && best_state.energy > 0) {
        SudokuState neighbor_state;
        generate_neighbor(&current_state, &neighbor_state);
        neighbor_state.energy = calculate_energy(&neighbor_state);

        int delta_energy = neighbor_state.energy - current_state.energy;

        if (delta_energy < 0 || (rand() / (double)RAND_MAX) < exp(-delta_energy / T_start)) {
            memcpy(&current_state, &neighbor_state, sizeof(SudokuState));
            if (current_state.energy < best_state.energy) {
                memcpy(&best_state, &current_state, sizeof(SudokuState));
            }
        }

        T_start *= alpha;
    }

    printf("Solved energy: %d\n", best_state.energy);
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = best_state.grid[i][j];
}
