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

bool is_game_over(bool player_ships_1[BOARD_SIZE][BOARD_SIZE], bool player_ships_2[BOARD_SIZE][BOARD_SIZE]) {
    int ships_1, ships_2 = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (player_ships_1[i][j]) {
                ships_1++;
            }
            if (player_ships_2[i][j]) {
                ships_2++;
            }
        }
    }
    return (ships_1 == 0 || ships_2 == 0) ;
}

char* play_game(char player_board[BOARD_SIZE][BOARD_SIZE],
               bool enemy_ships[BOARD_SIZE][BOARD_SIZE],
               int x, int y) {
    print_board(player_board);
    char mensaje[1024];

    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (enemy_ships[y][x]) {
            player_board[y][x] = 'X'; // Marcamos el impacto en el tablero del jugador
            enemy_ships[y][x] = false; // "Hundimos" el barco enemigo
            x = 'A' + x;
            y++;
            mensaje[1024] = "¡Impacto! Has alcanzado un barco enemigo en la posición (%c, %d)!\n", x, y;
        } else {
            player_board[y][x] = 'O'; // Marcamos el disparo al agua en el tablero del jugador
            x = 'A' + x;
            y++;
            mensaje[1024] = "Disparo al agua en la posición (%c, %d).\n", x, y;
        }
    } else {
        mensaje[1024] = "Disparo fuera de los límites. Inténtalo de nuevo.\n";
    }
    
    return mensaje[1024];
}

int main() {
    //Sockets
    int server_socket, client_socket[MAX_CONNECTIONS];
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len;
    char chat[1024];

    //Battleships
    char player_board_1[BOARD_SIZE][BOARD_SIZE];
    char player_board_2[BOARD_SIZE][BOARD_SIZE];
    char game_board_1[BOARD_SIZE][BOARD_SIZE];
    char game_board_2[BOARD_SIZE][BOARD_SIZE];
    int y, x; 

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
        send(client_socket[i], "Bienvenido", sizeof(chat), 0);
        send(client_socket[i], "Espera a tu oponente", sizeof(chat), 0);
    }

    //Empieza el juego 
    init_board(player_board_1);
    init_board(player_board_2);
    init_board(game_board_1);
    init_board(game_board_2);

    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        send(client_socket[i], "Comienza el juego", sizeof(chat), 0);
        send(client_socket[i], "Coloca tus barcos", sizeof(chat), 0);
        if ( i == 0 ){
            send(client_socket[i], player_board_1, sizeof(player_board_1), 0);
            recv(client_socket[i], player_board_1, sizeof(player_board_1), 0);
            recv(client_socket[i], enemy_ships_player_1, sizeof(enemy_ships_player_1), 0);
        } else {
            send(client_socket[i], player_board_2, sizeof(player_board_2), 0);
            recv(client_socket[i], player_board_2, sizeof(player_board_2), 0);
            recv(client_socket[i], enemy_ships_player_2, sizeof(enemy_ships_player_2), 0);
        }
    }

    // Revisar que esté bien la información
    print_board(player_board_1);
    print_board(player_board_2);

    int turno = 1;
    while (!is_game_over(enemy_ships_player_1, enemy_ships_player_2)){
        if (turno == 1 ){
            send(client_socket[0], game_board_1, sizeof(game_board_1), 0);
            recv(client_socket[0], x, sizeof(x), 0);
            recv(client_socket[0], y, sizeof(y), 0);
            char* mensaje = play_game(game_board_1, enemy_ships_player_2, x, y);
            send(client_socket[0], mensaje, sizeof(mensaje), 0);
            send(client_socket[0], game_board_1, sizeof(game_board_1), 0);

        } else {
            send(client_socket[0], game_board_2, sizeof(game_board_2), 0);
            recv(client_socket[0], x, sizeof(x), 0);
            recv(client_socket[0], y, sizeof(y), 0);
            char* mensaje = play_game(game_board_2, enemy_ships_player_1, x, y);
            send(client_socket[0], mensaje, sizeof(mensaje), 0);
            send(client_socket[0], game_board_2, sizeof(game_board_2), 0);
        }
        turno = (turno == 1) ? 2 : 1;
    }

    // Cerrar conexiones
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        close(client_socket[i]);
    }

    close(server_socket);

    return 0;
}
