#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_CONNECTIONS 2
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

bool is_game_over(bool ships[BOARD_SIZE][BOARD_SIZE]) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (ships[i][j]) {
                printf("HAY BARCO EN %d %d", i, j);
                count++;
            }
        }
    }
    return count == 0;
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
    //Sockets
    int server_socket, client_socket[MAX_CONNECTIONS];
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len;
    char welcome[] = "Bienvenido a battleship ";
    char start[] = "Comienza el juego ";
    char init_game[] = "Coloca tus barcos ";
    char wait[] = "Espera a tu oponente ...";
    char chat[1024];

    //Battleships
    char player_board_1[BOARD_SIZE][BOARD_SIZE];
    char player_board_2[BOARD_SIZE][BOARD_SIZE];
    char game_board_1[BOARD_SIZE][BOARD_SIZE];
    char game_board_2[BOARD_SIZE][BOARD_SIZE];

    // Matriz para registrar la ubicación de los barcos enemigos
    bool enemy_ships_player_1[BOARD_SIZE][BOARD_SIZE] = {false};
    bool enemy_ships_player_2[BOARD_SIZE][BOARD_SIZE] = {false};  

    // Crear el socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Vincular el socket a la dirección y al puerto
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error al vincular el socket");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        perror("Error al escuchar conexiones");
        exit(EXIT_FAILURE);
    }

    printf("Esperando a los jugadores...\n");

    // Aceptar conexiones de dos clientes
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        client_address_len = sizeof(client_address);
        client_socket[i] = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket[i] == -1) {
            perror("Error al aceptar la conexión");
            exit(EXIT_FAILURE);
        }
        printf("Jugador %d conectado\n", i + 1);

        // Enviar mensaje de bienvenida al cliente
        send(client_socket[i], welcome, strlen(welcome), 0);
    }

    //Empieza el juego 
    init_board(player_board_1);
    init_board(player_board_2);
    init_board(game_board_1);
    init_board(game_board_2);

    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        send(client_socket[i], start, strlen(start), 0);
        send(client_socket[i], init_game, strlen(init_game), 0);
        send(client_socket[i], init_game, strlen(init_game), 0);
        send(client_socket[i], player_board_1, strlen(player_board_1), 0);
        recv(client_socket[i], player_board_1, strlen(player_board_1), 0);
        recv(client_socket[i], enemy_ships_player_1, strlen(enemy_ships_player_1), 0);
    }


    print_board(player_board_1);

    while (1) {
        for (int i = 0; i < MAX_CONNECTIONS; ++i) {

        }
 //       turno = (turno == 1) ? 2 : 1;
    }

    // Cerrar conexiones
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        close(client_socket[i]);
    }

    close(server_socket);

    return 0;
}
