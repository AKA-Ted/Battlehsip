#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define BOARD_SIZE 5
#define NUM_SHIPS 3

// Función para inicializar el tablero con agua ('~')
void init_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = '~';
        }
    }
}

// Función para imprimir el tablero
void print_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    printf("   ");
        for (char c = 'A'; c < 'A' + BOARD_SIZE; c++) {
            printf("%c ", c);
        }
    printf("\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// Función para colocar los barcos en el tablero
void place_ships(char board[BOARD_SIZE][BOARD_SIZE],
                 bool enemy_ships[BOARD_SIZE][BOARD_SIZE]) {
    int num_ships_placed = 0;

    while (num_ships_placed < NUM_SHIPS) {
        char x_char;
        int y;
        printf("\nColoca el barco %d\n", num_ships_placed + 1);
        printf("Ingresa la coordenada (por ejemplo, A1): ");
        scanf(" %c%d", &x_char, &y);

        int x = toupper(x_char) - 'A'; // Convertir letra a índice
        y--;                           // Ajustar el índice a partir de 0

        if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE &&
            board[y][x] == '~') {
            board[y][x] = 'S';
            num_ships_placed++;
            enemy_ships[y][x] = true;
            printf("Barco colocado en la posición (%c, %d)\n", 'A' + x, y + 1);
        } else {
            printf("Posición inválida. Inténtalo de nuevo.\n");
        }
    }
}

bool is_game_over(bool ships[BOARD_SIZE][BOARD_SIZE]) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (ships[i][j]) {
                count++;
            }
        }
    }
  return count == NUM_SHIPS;
}

void play_game(char player_board[BOARD_SIZE][BOARD_SIZE],
               bool enemy_ships[BOARD_SIZE][BOARD_SIZE]) {
    while (!is_game_over(enemy_ships)) {
        printf("\nDispara al barco enemigo!\n");
        print_board(player_board);

        char x_char;
        int y;
        printf("Ingresa la coordenada a disparar (por ejemplo, A5): ");
        scanf(" %c%d", &x_char, &y);

        int x = toupper(x_char) - 'A'; // Convertir letra a índice
        y--;                           // Ajustar el índice a partir de 0

        if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
            if (enemy_ships[y][x]) {
                printf("¡Impacto! Has alcanzado un barco enemigo en la posición (%c, "
                    "%d)!\n",
                    'A' + x, y + 1);
                player_board[y][x] =
                    'X'; // Marcamos el impacto en el tablero del jugador
                enemy_ships[y][x] = false; // "Hundimos" el barco enemigo
            } else {
                printf("Disparo al agua en la posición (%c, %d).\n", 'A' + x, y + 1);
                player_board[y][x] =
                    'O'; // Marcamos el disparo al agua en el tablero del jugador
            }
        } else {
            printf("Disparo fuera de los límites. Inténtalo de nuevo.\n");
        }
    }
    printf("\n¡Felicidades! ¡Has hundido todos los barcos enemigos!\n");
}

int main() {
    char player_board_1[BOARD_SIZE][BOARD_SIZE];
    char player_board_2[BOARD_SIZE][BOARD_SIZE];

    // Matriz para registrar la ubicación de los barcos enemigos
    bool enemy_ships_1[BOARD_SIZE][BOARD_SIZE] = {false};

    init_board(player_board_1);
    init_board(player_board_2);
    printf("¡Bienvenido a Battleship!\n");
    printf("Tablero jugador 1\n");
    print_board(player_board_1);
    printf("Tablero jugador 2\n");
    print_board(player_board_2);
    printf("Coloca tus barcos:\n");
    place_ships(player_board_1, enemy_ships_1);

    printf("\nComienza el juego:\n");
    printf("TABLERO 1 \n");
    print_board(player_board_1);
    play_game(player_board_2, enemy_ships_1);

    return 0;
}