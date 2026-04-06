CC      = gcc
CFLAGS  = -Wall -Wextra

# Cores ANSI para saida no terminal
GREEN   = \033[1;32m
CYAN    = \033[1;36m
YELLOW  = \033[1;33m
RED     = \033[1;31m
RESET   = \033[0m

TARGETS = telemetria_server telemetria_client http_client

# -------------------------------------------------------
#  Alvo principal — compila tudo
# -------------------------------------------------------
all: banner $(TARGETS) done

banner:
	@echo ""
	@echo "$(CYAN)============================================$(RESET)"
	@echo "$(CYAN)  Compilando projeto de Telemetria + HTTP   $(RESET)"
	@echo "$(CYAN)============================================$(RESET)"
	@echo ""

done:
	@echo ""
	@echo "$(GREEN)============================================$(RESET)"
	@echo "$(GREEN)  [OK] Todos os binarios compilados!       $(RESET)"
	@echo "$(GREEN)============================================$(RESET)"
	@echo ""

# -------------------------------------------------------
#  Alvos individuais de compilacao
# -------------------------------------------------------
telemetria_server: telemetria_server.c
	@echo "$(YELLOW)  [BUILD] Compilando telemetria_server.c$(RESET)"
	$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  [OK] telemetria_server compilado com sucesso!$(RESET)"
	@echo ""

telemetria_client: telemetria_client.c
	@echo "$(YELLOW)  [BUILD] Compilando telemetria_client.c$(RESET)"
	$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  [OK] telemetria_client compilado com sucesso!$(RESET)"
	@echo ""

http_client: http_client.c
	@echo "$(YELLOW)  [BUILD] Compilando http_client.c$(RESET)"
	$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  [OK] http_client compilado com sucesso!$(RESET)"
	@echo ""

# -------------------------------------------------------
#  Alvos de execucao (compilam se necessario)
# -------------------------------------------------------
run_server: telemetria_server
	@echo ""
	@echo "$(CYAN)============================================$(RESET)"
	@echo "$(CYAN)  [UDP] Iniciando servidor na porta 9000    $(RESET)"
	@echo "$(CYAN)============================================$(RESET)"
	@echo ""
	./telemetria_server

run_client: telemetria_client
	@echo ""
	@echo "$(CYAN)============================================$(RESET)"
	@echo "$(CYAN)  [UDP] Enviando telemetria para 127.0.0.1:9000$(RESET)"
	@echo "$(CYAN)============================================$(RESET)"
	@echo ""
	./telemetria_client

HTTP_HOST ?= httpbin.org
HTTP_PATH ?= /get

run_http: http_client
	@echo ""
	@echo "$(CYAN)============================================$(RESET)"
	@echo "$(CYAN)  [HTTP] Conectando a $(HTTP_HOST)$(HTTP_PATH)$(RESET)"
	@echo "$(CYAN)============================================$(RESET)"
	@echo ""
	./http_client $(HTTP_HOST) $(HTTP_PATH)

# -------------------------------------------------------
#  Limpeza
# -------------------------------------------------------
clean:
	@echo ""
	@echo "$(RED)  [CLEAN] Removendo binarios...$(RESET)"
	rm -f $(TARGETS)
	@echo "$(GREEN)  [OK] Limpeza concluida!$(RESET)"
	@echo ""

.PHONY: all banner done clean run_server run_client run_http
