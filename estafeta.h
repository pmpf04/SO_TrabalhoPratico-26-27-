
#ifndef ESTAFETA_H
#define ESTAFETA_H

#include "util.h"

/* Configuracao do estafeta */
typedef struct {
    char username[TAM_USERNAME];
} ConfigEstafeta;

/* Informacao apresentada pelo programa estafeta */
typedef struct {
    int estado;
    int id_pedido_atual;
    char origem[TAM_LOCAL];
    char destino[TAM_LOCAL];
    float peso;
    float valor;
    int entregas_concluidas;
    float total_ganho;
} DadosEstafeta;

#endif
