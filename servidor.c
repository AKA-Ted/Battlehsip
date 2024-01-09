#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_PLAYERS 2
#define BOARD_SIZE 5

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    int hits;
} Player;

typedef struct {
    int sockfd;
    char name[20];
    Player player;
} GameClient;

void initializeBoard(Player *player) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            player->board[i][j] = ' ';
        }
    }
    player->hits = 0;
}

void printBoard(Player *player) {
    printf("Tabla de juego:\n");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            printf("%c ", player->board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int checkWinner(Player *player) {
    return player->hits >= (BOARD_SIZE * BOARD_SIZE) / 2;
}

void notifyPlayers(GameClient *clients[]) {
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        char message[50];
        sprintf(message, "Hola %s, ya te has conectado. ¡Que empiece la batalla naval!\n", clients[i]->name);
        write(clients[i]->sockfd, message, strlen(message));
    }
}

void playGame(GameClient *clients[]) {
    while (1) {
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            printBoard(&clients[i]->player);

            char message[50];
            sprintf(message, "Es tu turno, %s. Introduce las coordenadas (fila columna): ", clients[i]->name);
            write(clients[i]->sockfd, message, strlen(message));

            char buffer[256];
            read(clients[i]->sockfd, buffer, sizeof(buffer));

            int row, col;
            sscanf(buffer, "%d %d", &row, &col);

            if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || clients[i]->player.board[row][col] != ' ') {
                write(clients[i]->sockfd, "Movimiento inválido. Intenta de nuevo.\n", 38);
                --i;  // Repeat the turn for the same player
                continue;
            }

            if (clients[i]->player.board[row][col] == 'O') {
                write(clients[i]->sockfd, "Ya has disparado en esta posición. Intenta de nuevo.\n", 50);
                --i;  // Repeat the turn for the same player
                continue;
            }

            if (clients[i == 0 ? 1 : 0]->player.board[row][col] == 'X') {
                write(clients[i]->sockfd, "¡Has alcanzado un barco enemigo!\n", 33);
                clients[i]->player.hits++;
            } else {
                write(clients[i]->sockfd, "¡Has fallado! No hay barco en esta posición.\n", 44);
            }

            clients[i]->player.board[row][col] = 'O';

            if (checkWinner(&clients[i]->player)) {
                sprintf(message, "¡Felicidades %s! ¡Eres el ganador!\n", clients[i]->name);
                write(clients[i]->sockfd, message, strlen(message));

                sprintf(message, "¡Lo siento %s! Has perdido.\n", clients[i == 0 ? 1 : 0]->name);
                write(clients[i == 0 ? 1 : 0]->sockfd, message, strlen(message));

                close(clients[0]->sockfd);
                close(clients[1]->sockfd);
                exit(0);
            }
        }
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    GameClient *clients[MAX_PLAYERS];

    // Inicializar el servidor
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error en el enlace");
        exit(EXIT_FAILURE);
    }

    listen(serverSocket, MAX_PLAYERS);
    printf("Esperando a que los jugadores se conecten...\n");

    // Aceptar conexiones de clientes
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (clientSocket < 0) {
            perror("Error al aceptar la conexión del cliente");
            exit(EXIT_FAILURE);
        }

        clients[i] = (GameClient *)malloc(sizeof(GameClient));
        clients[i]->sockfd = clientSocket;

        printf("Cliente %d conectado. Introduce tu nombre: ", i + 1);
        read(clientSocket, clients[i]->name, sizeof(clients[i]->name));
        clients[i]->name[strlen(clients[i]->name) - 1] = '\0';  // Remove newline character

        initializeBoard(&clients[i]->player);
    }

    // Notificar a los jugadores que se han conectado
    notifyPlayers(clients);

    // Iniciar el juego
    playGame(clients);

    return 0;
}