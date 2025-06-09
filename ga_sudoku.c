#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// External global variables
extern int size, sub;
extern void displayBoard(int **board);

#define POP_SIZE 100    // Number of individuals in the population
#define MAX_GEN 1000   // Maximum number of generations
#define MUT_RATE 0.01   // Mutation probability

// Each individual represents one Sudoku board with a fitness score
typedef struct {
    int **grid;   // Array of Sudoku board values
    int fitness;   // Number of conflicts
} Individual;

// Copy a board
int **copy_board(int **src) {
    int **copy = malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        copy[i] = malloc(size * sizeof(int));
        memcpy(copy[i], src[i], size * sizeof(int));
    }
    return copy;
}

// Free memory used by an individual
void free_individual(Individual ind) {
    for (int i = 0; i < size; i++)
        free(ind.grid[i]);
    free(ind.grid);
}

// Count total conflicts
int evaluate(int **grid) {
    int conflicts = 0;

    // Row and column conflicts
    for (int i = 0; i < size; i++) {
        int row_count[100] = {0}, col_count[100] = {0};
        for (int j = 0; j < size; j++) {
            row_count[grid[i][j]]++;
            col_count[grid[j][i]]++;
        }
        for (int k = 1; k <= size; k++) {
            if (row_count[k] > 1) conflicts += row_count[k] - 1;
            if (col_count[k] > 1) conflicts += col_count[k] - 1;
        }
    }

    // Box conflicts
    for (int r = 0; r < size; r += sub) {
        for (int c = 0; c < size; c += sub) {
            int box_count[100] = {0};
            for (int i = 0; i < sub; i++)
                for (int j = 0; j < sub; j++)
                    box_count[grid[r+i][c+j]]++;
            for (int k = 1; k <= size; k++)
                if (box_count[k] > 1) conflicts += box_count[k] - 1;
        }
    }

    return conflicts;   // Total number of conflicts
}

// Mutate non-fixed cells
void mutate(int **grid, int **fixed) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (!fixed[i][j] && ((double) rand() / RAND_MAX) < MUT_RATE) {
                grid[i][j] = rand() % size + 1;   // Assign a new random value
            }
        }
    }
}

// Crossover: copy random rows from two parents
void crossover(int **parent1, int **parent2, int **child) {
    for (int i = 0; i < size; i++) {
        int *row_src = rand() % 2 ? parent1[i] : parent2[i];
        memcpy(child[i], row_src, size * sizeof(int));
    }
}

// Initialize the starting population
void initialize_population(Individual pop[], int **board, int **fixed) {
    for (int i = 0; i < POP_SIZE; i++) {
        pop[i].grid = copy_board(board);
        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                if (!fixed[r][c])
                    pop[i].grid[r][c] = rand() % size + 1;
            }
        }
        pop[i].fitness = evaluate(pop[i].grid);   // Count conflicts
    }
}

// Tournament selection: pick best of 3 random individuals
Individual tournament_selection(Individual pop[]) {
    int best = rand() % POP_SIZE;
    for (int i = 0; i < 3; i++) {
        int candidate = rand() % POP_SIZE;
        if (pop[candidate].fitness < pop[best].fitness)
            best = candidate;
    }
    return pop[best];   // Return best candidate (with lowest fitness)
}

// Main genetic algorithm loop
void runGA(int **board, int **fixed) {
    srand(time(NULL));
    Individual population[POP_SIZE];
    initialize_population(population, board, fixed);   // Create initial population

    Individual best = population[0];  // Start with first individual as best

    for (int gen = 0; gen < MAX_GEN; gen++) {
        Individual new_population[POP_SIZE];

        // Elitism: keep best individual unchanged
        new_population[0].grid = copy_board(best.grid);
        new_population[0].fitness = best.fitness;

        // Create the rest of the new population
        for (int i = 1; i < POP_SIZE; i++) {
            Individual p1 = tournament_selection(population);
            Individual p2 = tournament_selection(population);

            new_population[i].grid = copy_board(p1.grid);   // Start with one parent's grid
            crossover(p1.grid, p2.grid, new_population[i].grid);   // Combine rows
            mutate(new_population[i].grid, fixed);   // Mutate the result
            new_population[i].fitness = evaluate(new_population[i].grid);   // Evaluate it

            // Update best if found better individual
            if (new_population[i].fitness < best.fitness) {
                free_individual(best);
                best.grid = copy_board(new_population[i].grid);
                best.fitness = new_population[i].fitness;
            }
            // Free memory of intermediate individual
            free_individual(new_population[i]);
        }

        if (best.fitness == 0) {
            printf("\nSudoku solved in generation %d\n", gen);
            break;
        }
        if (gen == MAX_GEN - 1) {
            printf("\nMax generations reached without solution.\n");
        }

        if (gen % 100 == 0)
            printf("Generation %d, best fitness: %d\n", gen, best.fitness);
    }

    // Copy final best solution to original board
    for (int i = 0; i < size; i++)
        memcpy(board[i], best.grid[i], size * sizeof(int));

    printf("\nSolved Sudoku board:\n");
    //displayBoard(board);

    free_individual(best);
    for (int i = 0; i < POP_SIZE; i++)
        if (population[i].grid != best.grid)
            free_individual(population[i]);
}
