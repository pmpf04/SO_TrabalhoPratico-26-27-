
#ifndef UTIL_H
#define UTIL_H


#define TAM_TIPO 12
#define TAM_CORPO_PEDIDO 50
#define TAM_LOCAL 16
#define TAM_USERNAME 32
#define TAM_NOME_PIPE 50



#define ESTAFETA_LIVRE 0
#define ESTAFETA_OCUPADO 1


#define MSG_PEDIDO          "pedido"
#define MSG_REGISTO_EST     "RegistoEst"
#define MSG_ACEITA          "aceita"
#define MSG_ENTREGUE        "entregue"
#define MSG_ESTADO          "estado"
#define MSG_GANHOS          "ganhos"
#define MSG_SAI             "sai"
#define MSG_ANUNCIO         "anuncio"
#define MSG_RES_ACEITA      "resAceita"
#define MSG_RES_ENTREGA     "resEntrega"
#define MSG_RES_ESTADO      "resEstado"
#define MSG_RES_GANHOS      "resGanhos"
#define MSG_ENCERRAR        "encerra"


/* Mensagem obrigatoria do gerador */
typedef struct {
    char tipo[TAM_TIPO];
    char corpo[TAM_CORPO_PEDIDO];
} MensagemPedido;


/* Registo inicial do estafeta */
typedef struct {
    char tipo[TAM_TIPO];
    char username[TAM_USERNAME];
    char pipe_central_para_estafeta[TAM_NOME_PIPE];
    char pipe_estafeta_para_central[TAM_NOME_PIPE];
} MensagemRegistoEst;


/* Mensagem para aceitar um pedido */
typedef struct {
    char tipo[TAM_TIPO];
    int id_pedido;
} MensagemAceita;


/* Mensagens sem dados adicionais */
typedef struct {
    char tipo[TAM_TIPO];
} MensagemSimples;


/* Anuncio de um pedido ao estafeta */
typedef struct {
    char tipo[TAM_TIPO];
    int id_pedido;
    char origem[TAM_LOCAL];
    char destino[TAM_LOCAL];
    float peso;
    float valor;
    int tempo_restante;
} MensagemAnuncio;


/* Resposta a uma operacao */
typedef struct {
    char tipo[TAM_TIPO];
    int resultado;
} MensagemResposta;


/* Resposta ao comando estado */
typedef struct {
    char tipo[TAM_TIPO];
    int estado;
    int id_pedido;
    char origem[TAM_LOCAL];
    char destino[TAM_LOCAL];
    float peso;
    float valor;
} MensagemRespostaEstado;


/* Resposta ao comando ganhos */
typedef struct {
    char tipo[TAM_TIPO];
    int entregas_concluidas;
    float total_ganho;
} MensagemRespostaGanhos;

#endif
