/*
 * telemetria_server.c
 *
 * Servidor UDP de telemetria.
 * Recebe datagramas no formato:
 *   PKT:<id>|LAT:<lat>|LON:<lon>|VEL:<vel>|TS:<timestamp>
 * e exibe os dados formatados no terminal.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTA 9000
#define TAM_BUFFER 512

int main(void) {
    setbuf(stdout, NULL);

    int sockfd;
    struct sockaddr_in endereco_server, endereco_client;
    socklen_t tam_client = sizeof(endereco_client);
    char buffer[TAM_BUFFER];

    /* Cria socket UDP */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("[ERRO] Falha ao criar socket UDP");
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Socket UDP criado com sucesso (fd=%d)\n", sockfd);

    /* Permite reutilizar a porta imediatamente apos encerrar */
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ERRO] setsockopt SO_REUSEADDR");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* Configura endereco do servidor */
    memset(&endereco_server, 0, sizeof(endereco_server));
    endereco_server.sin_family      = AF_INET;
    endereco_server.sin_addr.s_addr = INADDR_ANY;
    endereco_server.sin_port        = htons(PORTA);

    /* Associa o socket a porta */
    if (bind(sockfd, (struct sockaddr *)&endereco_server, sizeof(endereco_server)) < 0) {
        perror("[ERRO] Falha no bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Bind realizado na porta %d\n", PORTA);
    printf("[INFO] Servidor de telemetria aguardando pacotes...\n");
    printf("============================================\n\n");

    /* Loop infinito de recepcao */
    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, TAM_BUFFER - 1, 0,
                             (struct sockaddr *)&endereco_client, &tam_client);
        if (n < 0) {
            perror("[ERRO] recvfrom");
            continue;
        }
        buffer[n] = '\0';

        /* Identifica o remetente */
        char *ip_client = inet_ntoa(endereco_client.sin_addr);
        int porta_client = ntohs(endereco_client.sin_port);

        printf("[RECV] Pacote de %s:%d -> %s\n", ip_client, porta_client, buffer);

        /* Faz o parse do pacote */
        int id;
        double lat, lon, vel;
        long ts;

        int campos = sscanf(buffer, "PKT:%d|LAT:%lf|LON:%lf|VEL:%lf|TS:%ld",
                            &id, &lat, &lon, &vel, &ts);

        if (campos == 5) {
            printf("  Pacote %d | Lat: %.4f | Lon: %.4f | Velocidade: %.1f | Timestamp: %ld\n\n",
                   id, lat, lon, vel, ts);
        } else {
            printf("  [AVISO] Formato invalido (%d campos lidos)\n\n", campos);
        }
    }

    close(sockfd);
    return 0;
}
