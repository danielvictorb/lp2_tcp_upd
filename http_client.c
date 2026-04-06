/*
 * http_client.c
 *
 * Cliente HTTP minimalista usando sockets TCP puros.
 * Nenhuma biblioteca HTTP e utilizada — a requisicao e construida
 * manualmente como texto formatado sobre TCP.
 *
 * Uso:
 *   ./http_client <host> <caminho>
 *
 * Exemplos:
 *   ./http_client api.restful-api.dev /api/objects
 *   ./http_client httpbin.org /get
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORTA_HTTP   "80"
#define TAM_BUFFER   4096
#define TAM_REQUEST  1024

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <host> <caminho>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s api.restful-api.dev /api/objects\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *host    = argv[1];
    const char *caminho = argv[2];

    printf("============================================\n");
    printf("  Cliente HTTP (TCP puro)\n");
    printf("============================================\n\n");

    /* --- Resolucao de DNS --- */
    printf("[INFO] Resolvendo hostname: %s\n", host);

    struct addrinfo hints, *resultado;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host, PORTA_HTTP, &hints, &resultado);
    if (status != 0) {
        fprintf(stderr, "[ERRO] getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    /* Mostra o IP resolvido */
    char ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in *addr = (struct sockaddr_in *)resultado->ai_addr;
    inet_ntop(AF_INET, &addr->sin_addr, ip_str, sizeof(ip_str));
    printf("[INFO] Hostname resolvido: %s -> %s\n", host, ip_str);

    /* --- Cria socket TCP --- */
    int sockfd = socket(resultado->ai_family, resultado->ai_socktype, resultado->ai_protocol);
    if (sockfd < 0) {
        perror("[ERRO] Falha ao criar socket TCP");
        freeaddrinfo(resultado);
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Socket TCP criado (fd=%d)\n", sockfd);

    /* --- Conecta ao servidor --- */
    printf("[INFO] Conectando a %s:%s...\n", host, PORTA_HTTP);
    if (connect(sockfd, resultado->ai_addr, resultado->ai_addrlen) < 0) {
        perror("[ERRO] Falha ao conectar");
        close(sockfd);
        freeaddrinfo(resultado);
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Conectado a %s:%s\n\n", host, PORTA_HTTP);
    freeaddrinfo(resultado);

    /* --- Monta a requisicao HTTP manualmente --- */
    char request[TAM_REQUEST];
    int req_len = snprintf(request, TAM_REQUEST,
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        caminho, host);

    printf("[INFO] Enviando requisicao GET %s\n", caminho);
    printf("--------------------------------------------\n");
    printf("%s", request);
    printf("--------------------------------------------\n\n");

    /* --- Envia a requisicao --- */
    ssize_t enviado = send(sockfd, request, req_len, 0);
    if (enviado < 0) {
        perror("[ERRO] Falha ao enviar requisicao");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Requisicao enviada (%zd bytes)\n\n", enviado);

    /* --- Recebe a resposta completa --- */
    printf("[INFO] Aguardando resposta...\n");
    printf("============================================\n");
    printf("  RESPOSTA DO SERVIDOR\n");
    printf("============================================\n\n");

    char buffer[TAM_BUFFER];
    ssize_t recebido;
    long total_bytes = 0;

    while ((recebido = recv(sockfd, buffer, TAM_BUFFER - 1, 0)) > 0) {
        buffer[recebido] = '\0';
        printf("%s", buffer);
        total_bytes += recebido;
    }

    if (recebido < 0) {
        perror("\n[ERRO] recv");
    }

    printf("\n\n============================================\n");
    printf("[INFO] Conexao encerrada. Total: %ld bytes recebidos.\n", total_bytes);

    close(sockfd);
    return 0;
}
