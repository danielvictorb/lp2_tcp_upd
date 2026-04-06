# Exercicio Pratico — Telemetria UDP e Cliente HTTP (TCP Puro)

Projeto que demonstra a programacao de sockets em C utilizando apenas a API POSIX, sem nenhuma biblioteca HTTP externa. O exercicio e composto por tres programas independentes e uma analise comparativa entre os protocolos UDP e TCP.

---

## Estrutura do Projeto

| Arquivo                | Descricao                                           |
|------------------------|-----------------------------------------------------|
| `telemetria_server.c`  | Servidor UDP que recebe e exibe pacotes de telemetria |
| `telemetria_client.c`  | Cliente UDP que gera e envia pacotes de telemetria    |
| `http_client.c`        | Cliente HTTP minimalista usando sockets TCP puros     |
| `Makefile`             | Automacao de compilacao e execucao com logs no terminal|
| `README.md`            | Documentacao e analise comparativa (este arquivo)     |

---

## Como Compilar

### Compilar tudo de uma vez

```bash
make
```

### Compilar individualmente

```bash
make telemetria_server
make telemetria_client
make http_client
```

### Limpar binarios

```bash
make clean
```

---

## Como Executar

### 1. Sistema de Telemetria UDP

Abra dois terminais.

**Terminal 1 — Servidor:**

```bash
make run_server
```

**Terminal 2 — Cliente:**

```bash
make run_client
```

O servidor exibira os pacotes recebidos em tempo real.

### 2. Cliente HTTP (TCP Puro)

```bash
make run_http
```

Por padrao, faz uma requisicao GET para `api.restful-api.dev/api/objects`. Para especificar outro destino:

```bash
make run_http HTTP_HOST=httpbin.org HTTP_PATH=/get
```

Ou diretamente:

```bash
./http_client api.restful-api.dev /api/objects
./http_client httpbin.org /get
```

---

## Formato do Pacote de Telemetria

```
PKT:<id>|LAT:<latitude>|LON:<longitude>|VEL:<velocidade>|TS:<timestamp>
```

Exemplo:

```
PKT:1|LAT:-7.2100|LON:-39.3150|VEL:25.3|TS:1700000001
```

---

## Analise Comparativa

### Questao 1 — Comportamento de inicializacao: UDP vs TCP

**Por que pacotes UDP sao perdidos se o servidor ainda nao iniciou?**

O UDP e um protocolo sem conexao (*connectionless*). O cliente simplesmente envia datagramas para um endereco IP e porta — nao existe nenhum mecanismo de handshake ou verificacao de que alguem esta escutando do outro lado. Se o servidor nao estiver com o socket aberto e associado (bind) a porta, o sistema operacional descarta silenciosamente os pacotes que chegam. O cliente nao recebe nenhuma notificacao de falha — o `sendto()` retorna sucesso porque o pacote foi entregue a camada de rede local.

**Por que o TCP falha imediatamente sem servidor?**

O TCP exige um processo de *three-way handshake* (SYN, SYN-ACK, ACK) antes de qualquer troca de dados. Quando o cliente chama `connect()`, ele envia um pacote SYN para o servidor. Se nao ha nenhum processo escutando na porta de destino, o sistema operacional do servidor responde com um pacote RST (reset), e a funcao `connect()` retorna erro imediatamente. Isso torna a falha explicita e detectavel pelo cliente.

---

### Questao 2 — Deteccao de falhas

**O UDP detecta se o servidor caiu?**

Nao. Como nao existe conexao entre cliente e servidor no UDP, o cliente continua chamando `sendto()` normalmente, mesmo que o servidor tenha sido encerrado. Os pacotes sao enviados para a rede e descartados sem nenhuma notificacao de volta. Do ponto de vista do cliente, nada mudou — ele nao tem como saber se os dados estao sendo recebidos ou nao, a menos que implemente um mecanismo de confirmacao na camada de aplicacao.

**O que acontece no TCP quando o servidor cai?**

Se a conexao TCP esta estabelecida e o servidor encerra (de forma limpa ou abrupta), o comportamento do `recv()` no cliente muda:

- **Encerramento limpo** (servidor chama `close()`): o `recv()` retorna `0`, indicando que a conexao foi fechada pelo outro lado.
- **Encerramento abrupto** (processo morto, crash): o `recv()` pode retornar `-1` com `errno` definido para `ECONNRESET`, ou o proximo `send()` pode gerar um `SIGPIPE` ou retornar erro com `EPIPE`.

Em ambos os casos, o TCP permite que o cliente detecte a falha e reaja.

---

### Questao 3 — O que torna uma requisicao HTTP?

O HTTP nao e uma funcionalidade especial dos sockets — e simplesmente um protocolo de texto que roda sobre TCP. As mesmas funcoes de socket sao usadas (`socket`, `connect`, `send`, `recv`), sejam para HTTP ou qualquer outra comunicacao TCP.

O que torna uma comunicacao "HTTP" e exclusivamente a **formatacao do texto** enviado e recebido. Uma requisicao HTTP valida segue um formato rigido:

```
GET /caminho HTTP/1.1\r\n
Host: servidor.com\r\n
Connection: close\r\n
\r\n
```

Se o formato estiver errado — por exemplo, faltando o `\r\n` duplo no final, ou com o metodo escrito incorretamente — o servidor web pode ignorar a requisicao ou retornar um erro. O socket em si nao sabe e nao se importa com o conteudo; ele apenas transporta bytes.

---

### Questao 4 — Por que APIs REST usam TCP?

APIs REST dependem de TCP por tres razoes fundamentais:

1. **Confiabilidade**: O TCP garante que todos os bytes enviados serao recebidos na ordem correta, sem perda e sem duplicacao. Uma resposta JSON incompleta ou corrompida seria inutil para o cliente.

2. **Ordenacao**: O TCP mantem a ordem dos segmentos. No UDP, pacotes podem chegar fora de ordem, o que tornaria impossivel reconstruir uma resposta HTTP sem logica adicional na camada de aplicacao.

3. **Completude**: O TCP tem mecanismos de controle de fluxo e retransmissao. Se um segmento se perde na rede, ele e reenviado automaticamente. Isso garante que o corpo inteiro de uma resposta JSON — que pode ter milhares de bytes — chegue completo ao cliente.

Para o cenario tipico de APIs REST (requisicao-resposta, dados estruturados, integridade critica), a sobrecarga do TCP e um preco pequeno a pagar pela garantia de entrega.

---

### Questao 5 — Telemetria funcionaria com TCP?

Sim, funcionaria, mas com *tradeoffs* importantes:

**Vantagens do TCP para telemetria:**
- Garantia de entrega de todos os pacotes
- Deteccao de falhas na conexao
- Ordenacao dos dados

**Desvantagens do TCP para telemetria:**
- **Latencia**: O handshake inicial (SYN/SYN-ACK/ACK) adiciona atraso antes do primeiro dado ser enviado. Retransmissoes de pacotes perdidos tambem aumentam a latencia.
- **Head-of-line blocking**: Se um pacote e perdido, o TCP bloqueia a entrega dos pacotes subsequentes ate que o pacote perdido seja retransmitido. Em telemetria em tempo real, o dado mais recente e quase sempre mais importante que um dado antigo.
- **Overhead de conexao**: Manter uma conexao TCP persistente consome recursos do servidor (memoria, file descriptors). Com milhares de sensores, isso se torna significativo.

**Quando usar cada um para telemetria:**
- **UDP**: Quando a perda ocasional de um pacote e aceitavel e a latencia baixa e prioridade (GPS de veiculos, sensores IoT, jogos).
- **TCP**: Quando a perda de dados e inaceitavel (telemetria medica, registros financeiros, dados de auditoria).

---

### Questao 6 — Regra geral: quando usar TCP vs UDP

| Criterio                          | TCP                          | UDP                          |
|-----------------------------------|------------------------------|------------------------------|
| Confiabilidade necessaria?        | Sim — retransmissao automatica | Nao — pacotes podem se perder |
| Ordem dos dados importa?          | Sim — entrega ordenada       | Nao — pode chegar fora de ordem |
| Latencia e critica?               | Tolera atraso                | Precisa de minima latencia   |
| Tipo de comunicacao               | Requisicao-resposta, streams | Fire-and-forget, broadcast   |
| Exemplos                          | HTTP, APIs REST, email, SSH  | DNS, VoIP, streaming, jogos  |

**Framework de decisao:**

1. **Os dados precisam chegar completos e em ordem?** → Use TCP.
2. **Perder um pacote ocasionalmente e aceitavel?** → Use UDP.
3. **Latencia importa mais que confiabilidade?** → Use UDP.
4. **Precisa de broadcast/multicast?** → Use UDP (TCP e ponto-a-ponto).
5. **E um protocolo de requisicao-resposta com dados estruturados?** → Use TCP.

Na duvida, comece com TCP — ele e mais seguro e previsivel. Migre para UDP apenas quando houver uma necessidade clara de baixa latencia ou quando a perda de dados for aceitavel no contexto da aplicacao.
