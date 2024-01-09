#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BOARD_SIZE 5

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    int ships;
} Player;

void initializeBoard(Player *player) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            player->board[i][j] = ' ';
        }
    }
    player->ships = 0;
}

void printBoard(Player *player, int showShips) {
    printf("Your Board:\n");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (showShips || player->board[i][j] == 'O' || player->board[i][j] == 'X') {
                printf("%c ", player->board[i][j]);
            } else {
                printf("~ ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[256];

    // Inicializar el cliente
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Error al crear el socket del cliente");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Reemplazar con la dirección IP del servidor
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error al conectar con el servidor");
        exit(EXIT_FAILURE);
    }

    printf("Conectado al servidor. Introduce tu nombre: ");
    fgets(buffer, sizeof(buffer), stdin);
    write(clientSocket, buffer, strlen(buffer));

    Player player;
    initializeBoard(&player);

    printf("Introduce la cantidad de barcos que deseas colocar: ");
    scanf("%d", &player.ships);

    for (int i = 0; i < player.ships; ++i) {
        int row, col;
        printf("Introduce la posición del barco %d (fila columna): ", i + 1);
        scanf("%d %d", &row, &col);

        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || player.board[row][col] == 'O') {
            printf("Posición inválida. Introduce una posición válida.\n");
            --i;  // Repetir la colocación del barco
        } else {
            player.board[row][col] = 'O';
        }
    }

    printf("Barcos colocados correctamente. Comienza el juego!\n");

    while (1) {
        printBoard(&player, 0);

        printf("Introduce las coordenadas para atacar (fila columna): ");
        scanf("%s", buffer);
        write(clientSocket, buffer, strlen(buffer));

        read(clientSocket, buffer, sizeof(buffer));
        printf("Resultado del ataque: %s\n", buffer);

        printf("Esperando el tiro del oponente...\n");
        read(clientSocket, buffer, sizeof(buffer));
        printf("Tiro del oponente:\n%s", buffer);
    }

    close(clientSocket);
    return 0;
}