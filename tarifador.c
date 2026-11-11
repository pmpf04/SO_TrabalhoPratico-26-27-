#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char pedido[100];
    char resposta[100];

    char origem[16];
    char destino[16];
    float peso;

    int n;

    while ((n = read(0, pedido, sizeof(pedido) - 1)) > 0) {
        pedido[n] = '\0';
        if (sscanf(pedido, "%15s %15s %f",
                   origem, destino, &peso) == 3) {
            float distancia = 100.0;
            float valor = peso * 2.0;
            int tamanho = sprintf(resposta, "%.2f %.2f\n",
                                  distancia, valor);
            write(1, resposta, tamanho);
        }
    }

    return 0;
}