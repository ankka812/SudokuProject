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

// Function to calculate the "energy" of the Sudoku state, which represents the number of conflicts (duplicate numbers in rows and columns)
int calculate_energy(SudokuState *state) {
    int energy = 0;
    // Check for conflicts in rows
    for (int i = 0; i < SIZE; i++) {
        int count[SIZE + 1] = {0};
        for (int j = 0; j < SIZE; j++) count[state->grid[i][j]]++;
        for (int k = 1; k <= SIZE; k++) if (count[k] > 1) energy += count[k] - 1;
    }
    // Check for conflicts in columns
    for (int j = 0; j < SIZE; j++) {
        int count[SIZE + 1] = {0};
        for (int i = 0; i < SIZE; i++) count[state->grid[i][j]]++;
        for (int k = 1; k <= SIZE; k++) if (count[k] > 1) energy += count[k] - 1;
    }

    return energy;
}

// Function to initialize the Sudoku state (fill missing numbers in blocks randomly while respecting Sudoku constraints)
void initialize_state(SudokuState *state) {
    // Loop through each block in the Sudoku grid (3x3)
    for (int bi = 0; bi < SIZE; bi += SUB) {
        for (int bj = 0; bj < SIZE; bj += SUB) {
            int present[SIZE + 1] = {0};
            int idx = 0;
            int missing[SIZE];

            // Identify which numbers are already used in the block
            for (int i = 0; i < SUB; i++) {
                for (int j = 0; j < SUB; j++) {
                    int val = state->grid[bi + i][bj + j];
                    if (val) present[val] = 1;
                }
            }

            // Determine missing numbers in the block
            for (int k = 1; k <= SIZE; k++) {
                if (!present[k]) missing[idx++] = k;
            }

            // Shuffle missing numbers randomly
            for (int i = idx - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = missing[i];
                missing[i] = missing[j];
                missing[j] = temp;
            }

            // Fill missing spots in the block with the shuffled numbers
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

    // Calculate the energy of the initialized state
    state->energy = calculate_energy(state);
}

// Function to generate a neighboring state by swapping two empty cells within a randomly chosen block
void generate_neighbor(SudokuState *current, SudokuState *neighbor) {

  // Copy the current state to the neighbor state
    memcpy(neighbor, current, sizeof(SudokuState));

    // Randomly select a block (3x3)
    int bi = (rand() % SUB) * SUB;
    int bj = (rand() % SUB) * SUB;

    // Identify the empty cells in the selected block
    int cells[SIZE][2];
    int count = 0;

    for (int i = 0; i < SUB; i++) {
        for (int j = 0; j < SUB; j++) {
            int r = bi + i;
            int c = bj + j;
            if (!neighbor->fixed[r][c]) {
                cells[count][0] = r;
                cells[count][1] = c;
                count++;
            }
        }
    }

    // If there are at least two empty cells, swap their values
    if (count >= 2) {
        int a = rand() % count;
        int b = rand() % count;
        while (b == a) b = rand() % count;

        int r1 = cells[a][0], c1 = cells[a][1];
        int r2 = cells[b][0], c2 = cells[b][1];

        int tmp = neighbor->grid[r1][c1];
        neighbor->grid[r1][c1] = neighbor->grid[r2][c2];
        neighbor->grid[r2][c2] = tmp;
    }

    // Calculate the energy of the new neighbor state
    neighbor->energy = calculate_energy(neighbor);
}

// Simulated Annealing algorithm to solve the Sudoku puzzle
void runSA(int **board) {
    SudokuState current;
    memset(&current, 0, sizeof(SudokuState));

    // Initialize the current state based on the input Sudoku board
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            current.grid[i][j] = board[i][j];
            current.fixed[i][j] = (board[i][j] != 0);
        }
    }

    // Initialize the state
    initialize_state(&current);

    // Simulated Annealing parameters
    double T = 5.0;    // Initial temperature
    double Tmin = 0.0001;    // Minimum temperature
    double alpha = 0.99;    // Cooling rate

    // Best solution found during the search
    SudokuState best;
    memcpy(&best, &current, sizeof(SudokuState));

    // Main loop of Simulated Annealing
    while (T > Tmin && best.energy > 0) {
        SudokuState neighbor;
        generate_neighbor(&current, &neighbor);
        neighbor.energy = calculate_energy(&neighbor);

        // Calculate energy difference between neighbor and current state
        int dE = neighbor.energy - current.energy;

        // Accept the new state if it has lower energy or based on probability function
        if (dE < 0 || (rand() / (double)RAND_MAX) < exp(-dE / T)) {
            memcpy(&current, &neighbor, sizeof(SudokuState));
            if (current.energy < best.energy) {
                memcpy(&best, &current, sizeof(SudokuState));
            }
        }

        // Cool down the temperature
        T *= alpha;
    }

    // Print the result (solved energy and the final grid)
    printf("Solved energy: %d\n", best.energy);
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = best.grid[i][j];
}
