#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variables for board size, subgrid size, and number of hints
int size = 9;
int sub = 3;
int hints = 30;

// Function to allocate memory for the game board
int **allocate_board() {
    int **board = malloc(size * sizeof(int *));  // Allocate memory for rows
    for (int i = 0; i < size; i++) {
        board[i] = calloc(size, sizeof(int));  // Allocate memory for columns and initialize to 0
    }
    return board;
}

// Function to free the allocated memory for the board
void free_board(int **board) {
    for (int i = 0; i < size; i++) {
        free(board[i]);  // Free each row
    }
    free(board);  // Free the board itself
}

// Function to display the main menu to the user
void showMenu() {
    printf("\n===== SUDOKU MENU =====\n");
    printf("1. New Game\n");
    printf("2. Change board size (current: %dx%d)\n", size, size);
    printf("3. Change difficulty (current: %d hints)\n", hints);
    printf("4. Instructions\n");
    printf("5. Save game\n");
    printf("6. Load game\n");
    printf("7. Exit\n");
    printf("Choose an option: ");
}

// Function to change the board size based on user input
void changeBoardSize() {
    printf("Select board size:\n1. 4x4\n2. 9x9\n3. 16x16\n");
    int s;
    scanf("%d", &s);  // Get the user's choice for board size

    // Adjust the global variables based on the user's choice
    if (s == 1) {
        size = 4;
        sub = 2;
    }
    else if (s == 2) {
        size = 9;
        sub = 3;
    }
    else if (s == 3) {
        size = 16;
        sub = 4;
    }
    else {
        printf("Invalid input\n");  // Handle invalid input
    }
}

// Function to change the number of hints
void changeDifficulty() {
    printf("Enter number of hints: ");
    scanf("%d", &hints);  // Get the user's choice for the number of hints
}

// Function to display instructions for playing Sudoku
void showInstructions() {
    printf("\nFill the board so that each number appears only once in each row, column, and block.\n");
    printf("Enter a move as: row column value (e.g., 1 3 5).\n");
    printf("Enter value 0 to remove a number. To quit: -1 -1 -1.\n");
}

// Function to check if a number is present in the given row
int isInRow(int **board, int row, int num) {
    for (int col = 0; col < size; col++) {
        if (board[row][col] == num)
            return 1;  // Found the number in the row
    }
    return 0;  // Number not found in the row
}

// Function to check if a number is present in the given column
int isInCol(int **board, int col, int num) {
    for (int row = 0; row < size; row++) {
        if (board[row][col] == num)
            return 1;  // Found the number in the column
    }
    return 0;  // Number not found in the column
}

// Function to check if a number is present in the given subgrid
int isInBox(int **board, int startRow, int startCol, int num) {
    for (int i = 0; i < sub; i++) {
        for (int j = 0; j < sub; j++) {
            if (board[startRow + i][startCol + j] == num)
                return 1;  // Found the number in the box
        }
    }
    return 0;  // Number not found in the box
}

// Function to check if placing a number is safe (no conflicts in row, column, or subgrid)
int isSafe(int **board, int row, int col, int num) {
    return !isInRow(board, row, num) && !isInCol(board, col, num) && !isInBox(board, row - row % sub, col - col % sub, num);
}

// Function to shuffle an array randomly
void shuffle(int *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}

// Function to fill the board using a backtracking algorithm
int fillBoardBacktrack(int **board, int row, int col) {
    if (row == size)  // If all rows are filled, return true
        return 1;
    if (col == size)  // Move to the next row
        return fillBoardBacktrack(board, row + 1, 0);
    if (board[row][col] != 0)  // Skip already filled cells
        return fillBoardBacktrack(board, row, col + 1);

    int numbers[size];
    for (int i = 0; i < size; i++) numbers[i] = i + 1;  // Initialize array with numbers 1 to size
    shuffle(numbers, size);  // Shuffle the numbers for randomization

    for (int i = 0; i < size; i++) {
        int num = numbers[i];
        if (isSafe(board, row, col, num)) {  // Check if placing the number is safe
            board[row][col] = num;  // Place the number
            if (fillBoardBacktrack(board, row, col + 1))  // Try to fill the rest of the board
                return 1;
            board[row][col] = 0;  // Backtrack if not successful
        }
    }
    return 0;  // No solution found
}

// Function to remove K digits from the filled board to create a puzzle
void removeKDigits(int **board, int k, int **fixed) {
    int total = size * size;
    int *positions = malloc(total * sizeof(int));  // Create an array of positions
    for (int i = 0; i < total; i++) {
        positions[i] = i;  // Initialize position array with indices
    }

    shuffle(positions, total);  // Shuffle the positions

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            fixed[i][j] = 1;  // Mark all cells as fixed initially

    for (int i = 0; i < k; i++) {
        int index = positions[i];  // Get a random position
        int row = index / size;
        int col = index % size;
        board[row][col] = 0;  // Remove the number from the position
        fixed[row][col] = 0;  // Mark the cell as not fixed
    }

    free(positions);  // Free the allocated memory for positions
}

// Function to display the current state of the board
void displayBoard(int **board) {
    for (int i = 0; i < size; i++) {
        if (i % sub == 0 && i != 0) {
            for (int j = 0; j < size * 2 + sub - 1; j++) printf("-");  // Print row separator
            printf("\n");
        }
        for (int j = 0; j < size; j++) {
            if (j % sub == 0 && j != 0) printf("| ");  // Print column separator
            if (board[i][j] == 0)
                printf(". ");  // Empty cells are displayed as dots
            else
                printf("%d ", board[i][j]);  // Print filled cells
        }
        printf("\n");
    }
}

// Function to save the current game state to a file
void saveGame(int **board, int **fixed) {
    if (board == NULL || fixed == NULL) {
        printf("No game to save.\n");
        return;
    }

    FILE *file = fopen("sudoku.txt", "w");  // Open file for writing
    if (file == NULL) {
        printf("Error opening file for saving.\n");
        return;
    }

    // Save game parameters and board state to the file
    fprintf(file, "%d %d %d\n", size, sub, hints);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fprintf(file, "%d ", board[i][j]);
        }
        fprintf(file, "\n");
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fprintf(file, "%d ", fixed[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);  // Close the file
    printf("Game saved successfully.\n");
}

// Function to load a previously saved game state from a file
void loadGame(int **board, int **fixed) {
    FILE *file = fopen("sudoku.txt", "r");  // Open file for reading
    if (file == NULL) {
        printf("Error opening file for loading.\n");
        return;
    }

    // Read the saved game parameters and board state from the file
    int newSize, newSub, newHints;
    fscanf(file, "%d %d %d", &newSize, &newSub, &newHints);

    if (newSize != 4 && newSize != 9 && newSize != 16) {
        printf("Invalid size in save file.\n");
        fclose(file);
        return;
    }

    size = newSize;
    sub = newSub;
    hints = newHints;

    // Free previous memory and allocate new memory for the board and fixed states
    if (board != NULL) free_board(board);
    if (fixed != NULL) free_board(fixed);
    board = allocate_board();
    fixed = allocate_board();

    // Read the board and fixed states from the file
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fscanf(file, "%d", &board[i][j]);
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fscanf(file, "%d", &fixed[i][j]);
        }
    }

    fclose(file);  // Close the file
    printf("Game loaded successfully.\n");
}

// Main function to run the game
int main(void) {
    srand(time(NULL));
    int **board = NULL;
    int **fixed = NULL;
    int choice;

    // Main game loop
    while (1) {
        showMenu();  // Display the main menu
        scanf("%d", &choice);  // Get the user's choice

        if (choice == 1) {
            board = allocate_board();
            fixed = allocate_board();

            fillBoardBacktrack(board, 0, 0);  // Fill the board with a valid Sudoku puzzle
            removeKDigits(board, size * size - hints, fixed);  // Remove hints to create a puzzle
            break;
        } else if (choice == 2) {
            changeBoardSize();  // Change the board size
        } else if (choice == 3) {
            changeDifficulty();  // Change the difficulty level
        } else if (choice == 4) {
            showInstructions();  // Show instructions
        } else if (choice == 5) {
            saveGame(board, fixed);  // Save the current game
        } else if (choice == 6) {
            loadGame(board, fixed);  // Load a saved game
        } else if (choice == 7) {
            printf("Exiting...\n");
            exit(0);  // Exit the program
        } else {
            printf("Invalid choice.\n");
        }
    }

    // Start the actual game loop where the user plays the game
    int row, col, val;
    time_t start = time(NULL), end;

    while (1) {
        displayBoard(board);  // Display the board
        printf("Enter row (1-%d), column (1-%d) and value (0=clear) or -1 -1 0 to show menu: ", size, size);
        scanf("%d %d %d", &row, &col, &val);

        if (row == -1 && col == -1 && val == 0) {
            printf("Game paused. Choose an option from the menu:\n");
            showMenu();
            scanf("%d", &choice);

            if (choice == 1) {
                if (board != NULL) {
                    free_board(board);
                    free_board(fixed);
                }
                board = allocate_board();
                fixed = allocate_board();

                fillBoardBacktrack(board, 0, 0);
                removeKDigits(board, size * size - hints, fixed);
                break;
            } else if (choice == 2) {
                changeBoardSize();
            } else if (choice == 3) {
                changeDifficulty();
            } else if (choice == 4) {
                showInstructions();
            } else if (choice == 5) {
                saveGame(board, fixed);
            } else if (choice == 6) {
                loadGame(board, fixed);
            } else if (choice == 7) {
                printf("Exiting...\n");
                exit(0);
            }
            continue;
        }

        row--;  // Adjust for 0-based index
        col--;  // Adjust for 0-based index

        if (row < 0 || row >= size || col < 0 || col >= size) {
            printf("Invalid coordinates. Try again.\n");
            continue;
        }

        if (val < 0 || val > size) {
            printf("Invalid value. Try again.\n");
            continue;
        }

        if (fixed[row][col]) {
            printf("Cannot change a fixed cell. Try a different cell.\n");
            continue;
        }

        if (val == 0) {
            board[row][col] = 0;  // Clear the cell
        } else if (isSafe(board, row, col, val)) {
            board[row][col] = val;  // Place the value in the cell if it's safe
        } else {
            printf("Invalid move. Try again.\n");
            continue;
        }

        // Check if the game is finished
        int finished = 1;
        for (int i = 0; i < size && finished; i++) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] == 0) {  // If any cell is still empty
                    finished = 0;
                    break;
                }
            }
        }

        if (finished) {  // If the board is full, end the game
            end = time(NULL);
            int totalTime = end - start;
            printf("Game time: %d minutes %d seconds\n", totalTime / 60, totalTime % 60);
            break;
        }
    }

    free_board(board);  // Free the memory for the board
    free_board(fixed);  // Free the memory for the fixed cells

    return 0;  // Exit the program
}
