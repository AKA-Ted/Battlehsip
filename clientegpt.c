#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define SERVER_ADDRESS "127.0.0.1"
#define BOARD_SIZE 5
#define NUM_SHIPS 3

// Función para colocar los barcos en el tablero
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

void place_ships(char board[BOARD_SIZE][BOARD_SIZE],
                 bool enemy_ships[BOARD_SIZE][BOARD_SIZE]) {
    int num_ships_placed = 0;

    while (num_ships_placed < NUM_SHIPS) {
        char x_char;
        int y;
        printf("\nColoca el barco %d\n", num_ships_placed + 1);
        printf("Ingresa la coordenada (por ejemplo, A5): ");
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

void play_game(char player_board[BOARD_SIZE][BOARD_SIZE],
               bool enemy_ships[BOARD_SIZE][BOARD_SIZE]) {
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

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char message[1024];
    int turno = 1; 
    char x_char;
    int y;
    int x;

    //Battleships
    char player_board[BOARD_SIZE][BOARD_SIZE];

    // Matriz para registrar la ubicación de los barcos enemigos
    bool enemy_ships_player[BOARD_SIZE][BOARD_SIZE] = {false};

    // Crear el socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error al conectar con el servidor");
        exit(EXIT_FAILURE);
    }

    // Recibir mensaje de bienvenida del servidor
    recv(client_socket, message, sizeof(message), 0);
    printf("%s\n", message);

    // Recibir mensaje de espera
    recv(client_socket, message, sizeof(message), 0);
    printf("%s\n", message);

    // Recibir mensaje de que comianza el juego
    recv(client_socket, message, sizeof(message), 0);
    printf("%s\n", message);

    // Recibir mensaje de que coloque el barco
    recv(client_socket, message, sizeof(message), 0);
    printf("%s\n", message);

    // Recibe el tablero
    recv(client_socket, player_board, sizeof(player_board), 0);
    print_board(player_board);

    place_ships(player_board, enemy_ships_player);

    send(client_socket, player_board, sizeof(player_board), 0);
    send(client_socket, enemy_ships_player, sizeof(enemy_ships_player), 0);

    print_board(player_board);

    while (1) {
        printf("Tu turno");

        recv(client_socket, player_board, sizeof(player_board), 0);
        recv(client_socket, enemy_ships, sizeof(enemy_ships), 0);
        print_board(player_board);
        
        play_game(player_board, enemy_ships);

        send(client_socket, player_board, sizeof(player_board), 0);
        send(client_socket, enemy_ships_player, sizeof(enemy_ships_player), 0);
    }

    // Cerrar conexión
    close(client_socket);

    return 0;
}
