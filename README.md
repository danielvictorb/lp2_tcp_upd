# Exercicio Pratico: Telemetria UDP e Cliente HTTP (TCP Puro)

## Aluno: Daniel Victor Carneiro Brandão da Costa
## Matrícula: 20230089678

Projeto que demonstra programacao de sockets em C usando apenas a API POSIX.
Nenhuma biblioteca HTTP externa foi utilizada. O exercicio tem tres partes:
um simulador de telemetria UDP (cliente/servidor), um cliente HTTP construido
sobre sockets TCP puros, e uma analise comparativa entre os protocolos.

## Estrutura do Projeto

| Arquivo                | Descricao                                             |
|------------------------|-------------------------------------------------------|
| `telemetria_server.c`  | Servidor UDP que recebe e exibe pacotes de telemetria  |
| `telemetria_client.c`  | Cliente UDP que gera e envia pacotes de telemetria     |
| `http_client.c`        | Cliente HTTP minimalista usando sockets TCP puros      |
| `Makefile`             | Automacao de compilacao e execucao com logs no terminal |
| `README.md`            | Documentacao e analise comparativa (este arquivo)      |

## Como Compilar e Executar

Compilar tudo:

```bash
make
```

Compilar individualmente:

```bash
make telemetria_server
make telemetria_client
make http_client
```

Limpar binarios:

```bash
make clean
```

### Telemetria UDP

Abra dois terminais. No primeiro, inicie o servidor:

```bash
make run_server
```

No segundo, inicie o cliente:

```bash
make run_client
```

### Cliente HTTP

```bash
make run_http
```

Para outro endpoint:

```bash
./http_client httpbin.org /get
```

## Parte 1: Saida do Simulador de Telemetria UDP

O cliente envia 10 pacotes no formato `PKT:<id>|LAT:<lat>|LON:<lon>|VEL:<vel>|TS:<ts>`,
um por segundo. O servidor recebe, faz o parse e exibe os dados formatados.

Saida do **cliente** (`telemetria_client`):

```
[INFO] Socket UDP criado com sucesso (fd=5)
[INFO] Destino: 127.0.0.1:9000
[INFO] Enviando 10 pacotes de telemetria...
============================================

[SEND] Pacote  1/10 -> PKT:1|LAT:-7.2100|LON:-39.3150|VEL:34.7|TS:1775487097
[SEND] Pacote  2/10 -> PKT:2|LAT:-7.2099|LON:-39.3149|VEL:34.6|TS:1775487098
[SEND] Pacote  3/10 -> PKT:3|LAT:-7.2098|LON:-39.3148|VEL:49.3|TS:1775487099
[SEND] Pacote  4/10 -> PKT:4|LAT:-7.2097|LON:-39.3147|VEL:20.7|TS:1775487100
[SEND] Pacote  5/10 -> PKT:5|LAT:-7.2096|LON:-39.3146|VEL:39.2|TS:1775487101
[SEND] Pacote  6/10 -> PKT:6|LAT:-7.2095|LON:-39.3145|VEL:13.8|TS:1775487102
[SEND] Pacote  7/10 -> PKT:7|LAT:-7.2094|LON:-39.3144|VEL:12.9|TS:1775487103
[SEND] Pacote  8/10 -> PKT:8|LAT:-7.2093|LON:-39.3143|VEL:26.3|TS:1775487104
[SEND] Pacote  9/10 -> PKT:9|LAT:-7.2092|LON:-39.3142|VEL:34.1|TS:1775487105
[SEND] Pacote 10/10 -> PKT:10|LAT:-7.2091|LON:-39.3141|VEL:26.0|TS:1775487106

============================================
[INFO] Todos os 10 pacotes enviados.
```

Saida do **servidor** (`telemetria_server`):

```
[INFO] Socket UDP criado com sucesso (fd=5)
[INFO] Bind realizado na porta 9000
[INFO] Servidor de telemetria aguardando pacotes...
============================================

[RECV] Pacote de 127.0.0.1:59915 -> PKT:1|LAT:-7.2100|LON:-39.3150|VEL:34.7|TS:1775487097
  Pacote 1 | Lat: -7.2100 | Lon: -39.3150 | Velocidade: 34.7 | Timestamp: 1775487097

[RECV] Pacote de 127.0.0.1:59915 -> PKT:2|LAT:-7.2099|LON:-39.3149|VEL:34.6|TS:1775487098
  Pacote 2 | Lat: -7.2099 | Lon: -39.3149 | Velocidade: 34.6 | Timestamp: 1775487098

[RECV] Pacote de 127.0.0.1:59915 -> PKT:3|LAT:-7.2098|LON:-39.3148|VEL:49.3|TS:1775487099
  Pacote 3 | Lat: -7.2098 | Lon: -39.3148 | Velocidade: 49.3 | Timestamp: 1775487099

  ...

[RECV] Pacote de 127.0.0.1:59915 -> PKT:10|LAT:-7.2091|LON:-39.3141|VEL:26.0|TS:1775487106
  Pacote 10 | Lat: -7.2091 | Lon: -39.3141 | Velocidade: 26.0 | Timestamp: 1775487106
```

Todos os 10 pacotes foram enviados pelo cliente e recebidos pelo servidor.

## Parte 2: Saida do Cliente HTTP (TCP Puro)

O cliente resolve o DNS, conecta via TCP na porta 80, monta a requisicao HTTP
manualmente como texto e envia com `send()`. A resposta e lida em loop com `recv()`.

```bash
./http_client httpbin.org /get
```

Saida:

```
============================================
  Cliente HTTP (TCP puro)
============================================

[INFO] Resolvendo hostname: httpbin.org
[INFO] Hostname resolvido: httpbin.org -> 3.213.232.221
[INFO] Socket TCP criado (fd=7)
[INFO] Conectando a httpbin.org:80...
[INFO] Conectado a httpbin.org:80

[INFO] Enviando requisicao GET /get
--------------------------------------------
GET /get HTTP/1.1
Host: httpbin.org
Connection: close

--------------------------------------------

[INFO] Requisicao enviada (59 bytes)

[INFO] Aguardando resposta...
============================================
  RESPOSTA DO SERVIDOR
============================================

HTTP/1.1 200 OK
Date: Mon, 06 Apr 2026 14:51:56 GMT
Content-Type: application/json
Content-Length: 197
Connection: close
Server: gunicorn/19.9.0
Access-Control-Allow-Origin: *
Access-Control-Allow-Credentials: true

{
  "args": {},
  "headers": {
    "Host": "httpbin.org",
    "X-Amzn-Trace-Id": "Root=1-69d3c88c-3935e2da43f6357e16743390"
  },
  "origin": "181.77.96.66",
  "url": "http://httpbin.org/get"
}

============================================
[INFO] Conexao encerrada. Total: 422 bytes recebidos.
```

O corpo JSON retornado e identico ao que seria obtido com `curl http://httpbin.org/get`.

## Parte 3: Analise Comparativa

### Teste A: Servidor UDP iniciado apos o cliente

Ao executar o `telemetria_client` **antes** do `telemetria_server`, o cliente envia
todos os 10 pacotes normalmente (o `sendto()` retorna sucesso em cada um). Porem,
quando o servidor e iniciado depois, ele nao recebe nenhum desses pacotes. Eles foram
descartados pelo sistema operacional porque nao havia nenhum socket associado a porta 9000
no momento do envio.

Agora, ao tentar o mesmo com o cliente HTTP (TCP) apontando para `localhost` sem
nenhum servidor na porta 80:

```
[INFO] Resolvendo hostname: localhost
[INFO] Hostname resolvido: localhost -> 127.0.0.1
[INFO] Socket TCP criado (fd=5)
[INFO] Conectando a localhost:80...
[ERRO] Falha ao conectar: Connection refused
```

O `connect()` falha imediatamente com "Connection refused".

**Por que essa diferenca?**

O UDP nao tem conexao. O `sendto()` entrega o pacote para a camada de rede local e
retorna. Se ninguem esta escutando na porta, o pacote e descartado silenciosamente.
O cliente nao tem como saber.

O TCP exige um *three-way handshake* (SYN, SYN-ACK, ACK). Quando o cliente envia o
SYN e ninguem esta escutando, o SO responde com RST (reset), e o `connect()` retorna
erro na hora. A falha e explicita.

### Teste B: Queda durante a comunicacao

Se matarmos o `telemetria_server` com `kill` ou Ctrl+C enquanto o `telemetria_client`
ainda esta enviando pacotes, o cliente **nao percebe nada**. Ele continua chamando
`sendto()` e cada chamada retorna sucesso. Os pacotes seguintes sao simplesmente
descartados sem notificacao.

Isso acontece porque no UDP nao existe conceito de "conexao estabelecida". O cliente
apenas dispara datagramas para um endereco. Nao ha confirmacao de recebimento (ACK).

No TCP a situacao e diferente. Se o servidor cai durante uma transferencia:

- Se o servidor encerrou de forma limpa (chamou `close()`), o `recv()` no cliente
  retorna `0`, sinalizando fim da conexao.
- Se o servidor caiu abruptamente (crash, kill -9), o `recv()` retorna `-1` com
  `errno = ECONNRESET`, ou o proximo `send()` recebe `EPIPE`/`SIGPIPE`.

Em ambos os casos o TCP notifica o cliente. O UDP nao.

### Questao 3: O que faz a comunicacao ser "HTTP"?

No `http_client.c` usamos exatamente as mesmas funcoes de socket que usariamos para
qualquer comunicacao TCP: `socket()`, `connect()`, `send()`, `recv()`. Nao ha nada
de especial nas chamadas de sistema.

O que torna a comunicacao "HTTP" e exclusivamente a **formatacao do texto** que
enviamos pelo socket. A requisicao precisa seguir um formato especifico:

```
GET /caminho HTTP/1.1\r\n
Host: servidor.com\r\n
Connection: close\r\n
\r\n
```

Se mandarmos qualquer outra string pelo mesmo socket (por exemplo, "ola mundo"), a
conexao TCP funciona normalmente, mas o servidor web nao vai entender e vai ignorar
ou retornar erro. HTTP e so texto formatado sobre TCP.

### Questao 4: Por que APIs REST usam TCP e nao UDP?

Uma API REST precisa que a resposta JSON chegue **completa, na ordem certa e sem
partes faltando**. O TCP garante isso com seus mecanismos de:

- Retransmissao automatica de segmentos perdidos
- Entrega ordenada (os dados chegam na mesma sequencia em que foram enviados)
- Controle de fluxo (o receptor nao e sobrecarregado)

Se a resposta JSON viesse por UDP, pacotes poderiam se perder ou chegar fora de
ordem. O cliente receberia um JSON incompleto ou embaralhado, que nao faria sentido
nenhum ao tentar fazer o parse. Seria necessario implementar toda essa logica de
confiabilidade na camada de aplicacao, o que equivale a reinventar o TCP.

### Questao 5: A telemetria do drone seria prejudicada com TCP?

Funcionaria, mas com tradeoffs importantes.

Com TCP, teriamos garantia de entrega e deteccao de falhas, o que e bom. Porem, para
telemetria em tempo real de um drone, o dado mais recente e quase sempre mais
relevante que um dado de 5 segundos atras. Se um pacote TCP se perde na rede, o
protocolo trava a entrega dos pacotes seguintes ate retransmitir o perdido
(*head-of-line blocking*). Enquanto isso, o drone ja se moveu e a posicao antiga
perdeu relevancia.

Alem disso, o handshake inicial adiciona latencia, e manter conexoes TCP persistentes
para milhares de sensores consome mais recursos do servidor.

Para a telemetria deste exercicio (GPS de drone, dados que perdem relevancia rapido),
o UDP faz mais sentido: se um pacote se perdeu, o proximo ja traz dados mais recentes.

Se a telemetria fosse de dados criticos (ex: registros medicos, dados financeiros),
ai sim o TCP seria a escolha certa, porque perder um dado seria inaceitavel.

### Questao 6: Regra geral para decidir entre TCP e UDP

| Criterio                     | TCP                             | UDP                             |
|------------------------------|----------------------------------|---------------------------------|
| Confiabilidade necessaria?   | Sim, retransmissao automatica   | Nao, pacotes podem se perder    |
| Ordem dos dados importa?     | Sim, entrega ordenada           | Nao, pode chegar fora de ordem  |
| Latencia e critica?          | Tolera atraso                   | Precisa de minima latencia      |
| Tipo de comunicacao          | Requisicao-resposta, streams    | Fire-and-forget, broadcast      |
| Exemplos                     | HTTP, APIs REST, email, SSH     | DNS, VoIP, streaming, jogos     |

Na pratica: se os dados precisam chegar completos e na ordem, use TCP. Se perder um
pacote de vez em quando e aceitavel e latencia importa, use UDP. Na duvida, comece
com TCP (mais seguro) e migre para UDP se a latencia virar um problema.
