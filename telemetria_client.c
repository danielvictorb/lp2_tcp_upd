/*
 * telemetria_client.c
 *
 * Cliente UDP de telemetria.
 * Envia 10 pacotes de telemetria para o servidor na porta 9000.
 *
 * Formato do pacote:
 *   PKT:<id>|LAT:<lat>|LON:<lon>|VEL:<vel>|TS:<timestamp>
 *
 * Regras dos dados:
 *   - Latitude inicia em -7.2100, incremento +0.0001 por pacote
 *   - Longitude inicia em -39.3150, incremento +0.0001 por pacote
 *   - Velocidade: float aleatorio entre 10.0 e 50.0
 *   - Timestamp: time(NULL)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTA_SERVIDOR 9000
#define IP_SERVIDOR    "127.0.0.1"
#define TOTAL_PACOTES  10
#define TAM_BUFFER     512

/* Gera velocidade aleatoria entre 10.0 e 50.0 */
static float velocidade_aleatoria(void) {
    return 10.0f + ((float)rand() / (float)RAND_MAX) * 40.0f;
}

int main(void) {
    setbuf(stdout, NULL);

    int sockfd;
    struct sockaddr_in endereco_server;
    char buffer[TAM_BUFFER];

    srand((unsigned int)time(NULL));

    /* Cria socket UDP */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("[ERRO] Falha ao criar socket UDP");
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Socket UDP criado com sucesso (fd=%d)\n", sockfd);

    /* Configura endereco do servidor destino */
    memset(&endereco_server, 0, sizeof(endereco_server));
    endereco_server.sin_family      = AF_INET;
    endereco_server.sin_port        = htons(PORTA_SERVIDOR);
    endereco_server.sin_addr.s_addr = inet_addr(IP_SERVIDOR);

    printf("[INFO] Destino: %s:%d\n", IP_SERVIDOR, PORTA_SERVIDOR);
    printf("[INFO] Enviando %d pacotes de telemetria...\n", TOTAL_PACOTES);
    printf("============================================\n\n");

    double lat = -7.2100;
    double lon = -39.3150;

    for (int i = 1; i <= TOTAL_PACOTES; i++) {
        float vel = velocidade_aleatoria();
        long ts = (long)time(NULL);

        int len = snprintf(buffer, TAM_BUFFER,
                           "PKT:%d|LAT:%.4f|LON:%.4f|VEL:%.1f|TS:%ld",
                           i, lat, lon, vel, ts);

        ssize_t enviado = sendto(sockfd, buffer, len, 0,
                                 (struct sockaddr *)&endereco_server,
                                 sizeof(endereco_server));
        if (enviado < 0) {
            perror("[ERRO] sendto");
        } else {
            printf("[SEND] Pacote %2d/%d -> %s\n", i, TOTAL_PACOTES, buffer);
        }

        lat += 0.0001;
        lon += 0.0001;

        if (i < TOTAL_PACOTES) {
            sleep(1);
        }
    }

    printf("\n============================================\n");
    printf("[INFO] Todos os %d pacotes enviados.\n", TOTAL_PACOTES);

    close(sockfd);
    return 0;
}
