#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "central.h"

int central_ativa = 1;


void tratador_sinais(int sig) {
    if (sig == SIGINT) {
        write(STDOUT_FILENO, "\nEncerramento do programa.\n", 27);
        central_ativa = 0;
    }
}

int carregar_config_central(int argc, char *argv[], ConfigCentral *config){
    char *valor;
    int i;

    /* Valores por omissao */
    config->max_estafetas = MAXESTAFETAS_DEFAULT;
    config->prazo_aceitacao = PRAZOACEITA_DEFAULT;
    strcpy(config->bindir, BINDIR_DEFAULT);

    /* Variaveis de ambiente */
    valor = getenv("MAXESTAFETAS");
    if (valor != NULL)
        config->max_estafetas = atoi(valor);

    valor = getenv("PRAZOACEITA");
    if (valor != NULL)
        config->prazo_aceitacao = atoi(valor);

    valor = getenv("BINDIR");
    if (valor != NULL)
        strcpy(config->bindir, valor);

    valor = getenv("CENTRAL_PIPE");
    if (valor == NULL || valor[0] == '\0') {
        printf("Erro: CENTRAL_PIPE nao definida!\n");
        return -1;
    }

    strcpy(config->central_pipe, valor);

    /* Argumentos opcionais */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            config->max_estafetas = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            config->prazo_aceitacao = atoi(argv[i + 1]);
            i++;
        }
        else {
            printf("Argumento invalido: %s\n", argv[i]);
            return -1;
        }
    }

    /* Validacao dos valores */
    if (config->max_estafetas <= 0 ||
        config->max_estafetas > MAX_ESTAFETAS ||
        config->prazo_aceitacao <= 0) {
        printf("Erro: parametros invalidos!\n");
        return -1;
    }
    return 0;
}

void mostrar_stats(Estatisticas *stats) {
    printf("\n--- Estatisticas ---\n");
    printf("Pedidos recebidos: %d\n", stats->pedidos_recebidos);
    printf("Pedidos atribuidos: %d\n", stats->pedidos_atribuidos);
    printf("Pedidos concluidos: %d\n", stats->pedidos_concluidos);
    printf("Pedidos expirados: %d\n", stats->pedidos_expirados);
    printf("Pedidos perdidos: %d\n", stats->pedidos_perdidos);
    printf("Valor total: %.2f euros\n", stats->valor_total);
}

void consultar_tarifador(Pedido *pedido, int canal_ct[2], int canal_tc[2]) {
    char str[100];
    int tamanho = sprintf(str, "%s %s %.2f\n",
                          pedido->origem,
                          pedido->destino,
                          pedido->peso);

    int nbytes = write(canal_ct[1], str, tamanho);
    if (nbytes == -1) {
        perror("Erro na escrita");
        return;
    }
    int n = read(canal_tc[0], str, sizeof(str) - 1);
    if (n <= 0) {
        perror("Erro na leitura do tarifador");
        return;
    }
    str[n] = '\0';
    sscanf(str, "%f %f", &pedido->distancia, &pedido->valor);
    printf("Distancia: %.2f km\n", pedido->distancia);
    printf("Valor: %.2f euros\n", pedido->valor);
}

void ler_comandos(Estatisticas *stats, int canal_ct[2], int canal_tc[2]) {
    char linha[128];
    char comando[20];
    char origem[16];
    char destino[16];
    float peso;

    while (central_ativa) {
        printf("\ncentral> ");
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            break;
        }
        comando[0] = '\0';
        int args_lidos = sscanf(linha, "%19s %15s %15s %f",
                                comando, origem, destino, &peso);

        if (args_lidos <= 0) {
            continue;
        }
        
        if (strcmp(comando, "pedido") == 0) {
            if (args_lidos == 4) {
                Pedido pedido;
                strcpy(pedido.origem, origem);
                strcpy(pedido.destino, destino);
                pedido.peso = peso;
                consultar_tarifador(&pedido, canal_ct, canal_tc);

            } else {
                printf("Erro: Argumentos invalidos.\n");
                printf("Uso: pedido <origem> <destino> <peso>\n");
            }
        }
        else if (strcmp(comando, "stats") == 0) {
            if (args_lidos == 1) {
                mostrar_stats(stats);
            } else {
                printf("Erro: O comando 'stats' nao recebe parametros.\n");
            }
        }
        else if (strcmp(comando, "encerra") == 0) {
            if (args_lidos == 1) {
                printf("A iniciar encerramento da central...\n");
                central_ativa = 0;
            } else {
                printf("Erro: O comando 'encerra' nao recebe parametros.\n");
            }
        }
        else if (strcmp(comando, "estafetas") == 0) {
            if (args_lidos == 1) {
                printf("[DEBUG] Validação OK: A listar estafetas...\n");
            } else {
                printf("Erro: O comando 'estafetas' nao recebe parametros.\n");
            }
        }
        else if (strcmp(comando, "prazo") == 0) {
            if (args_lidos == 2) {
                int novo_prazo = atoi(origem); 
                if (novo_prazo > 0) {
                    printf("[DEBUG] Validação OK: Prazo alterado para %d segundos.\n", novo_prazo);
                    
                } else {
                    printf("Erro: O prazo tem de ser um numero inteiro positivo.\n");
                }
            } else {
                printf("Erro: Argumentos invalidos.\nUso: prazo <n>\n");
            }
        }
        else {
            printf("Erro: Comando desconhecido ('%s').\n", comando);
        }
    }
}

int main(int argc, char *argv[]) {

    struct sigaction sa;
    sa.sa_handler = tratador_sinais;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Erro ao configurar sinais");
        return 1;
    }

    ConfigCentral config;
    Estatisticas stats = {0};

    if (carregar_config_central(argc, argv, &config) == -1)
        return 1;

    printf("Maximo estafetas: %d\n", config.max_estafetas);
    printf("Prazo aceitacao: %d\n", config.prazo_aceitacao);
    printf("BINDIR: %s\n", config.bindir);
    printf("CENTRAL_PIPE: %s\n", config.central_pipe);

    int canal_ct[2];
    if (pipe(canal_ct) == -1) {
        perror("Erro na criacao do pipe");
        return 1;
    }

    int canal_tc[2];
    if (pipe(canal_tc) == -1) {
        perror("Erro na criacao do pipe");

        close(canal_ct[0]);
        close(canal_ct[1]);

        return 1;
    }
    
    pid_t pid = fork();

    if (pid == -1) {
        perror("Erro no fork");
        return 1;
    }

    if (pid == 0) {
        close(canal_ct[1]);
        close(canal_tc[0]);

        dup2(canal_ct[0], STDIN_FILENO);
        dup2(canal_tc[1], STDOUT_FILENO);

        close(canal_ct[0]);
        close(canal_tc[1]);

        execl("./tarifador", "tarifador", NULL);

        perror("Erro no exec");
        exit(1);
    }

    close(canal_ct[0]);
    close(canal_tc[1]);

    ler_comandos(&stats, canal_ct, canal_tc);

    close(canal_ct[1]);
    close(canal_tc[0]);

    int retProc;
    waitpid(pid, &retProc, 0);

    printf("Central encerrada.\n");

    return 0;
}   //export CENTRAL_PIPE="/tmp/central_pipe" para testar

