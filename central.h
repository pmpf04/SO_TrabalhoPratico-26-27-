#ifndef CENTRAL_H
#define CENTRAL_H

#include "util.h"

/* Valores por omissao da central */
#define MAXESTAFETAS_DEFAULT 10
#define PRAZOACEITA_DEFAULT 30
#define BINDIR_DEFAULT "."

/* Limites definidos no enunciado */
#define MAX_ESTAFETAS 20
#define MAX_PEDIDOS_PENDENTES 30

/* Estados dos pedidos */
#define PEDIDO_PENDENTE  0
#define PEDIDO_EM_CURSO  1
#define PEDIDO_CONCLUIDO 2
#define PEDIDO_EXPIRADO  3
#define PEDIDO_PERDIDO   4

/* Configuracao da central */
typedef struct {
    int max_estafetas;
    int prazo_aceitacao;
    char bindir[256];
    char central_pipe[TAM_NOME_PIPE];
} ConfigCentral;

/* Informacao de um pedido */
typedef struct {
    int id;
    char origem[TAM_LOCAL];
    char destino[TAM_LOCAL];
    float peso;
    float distancia;
    float valor;
    int estado;
    int id_estafeta;
} Pedido;

/* Informacao de um estafeta gerido pela central */
typedef struct {
    int id;
    char username[TAM_USERNAME];
    int estado;
    int id_pedido_atual;
    int entregas_concluidas;
    float total_ganho;
} Estafeta;

/* Estatisticas gerais da central */
typedef struct {
    int pedidos_recebidos;
    int pedidos_atribuidos;
    int pedidos_concluidos;
    int pedidos_expirados;
    int pedidos_perdidos;
    float valor_total;
} Estatisticas;


#endif
