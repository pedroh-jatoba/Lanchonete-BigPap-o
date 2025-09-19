#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char name[50];
    int tempo_preparo;
} ItemPedido;

typedef struct{
    ItemPedido comer[2];
    ItemPedido bebida[2];
    int qtd_itens;
    int tempo_chegada; // Em que ciclo o pedido foi feito
    int prazo_entrega; // tempo_chegada + 300 ciclos (5 min = 300s)
} Pedido;

typedef struct Node {
    struct Node *prev;
    struct Node *next;
    Pedido pedido;
} Node;

typedef struct{
    Node *start;
    Node *end;
} FilaPedidos;

void menu(){

    int section1;
    Pedido p;
    do
    {
        printf("\n ===== Auto Atendimento Big Papão ===== \n");
        printf("1 - Item para Comer.\n");
        printf("2 - Item para Beber.\n");
        printf("3 - Finalizar Pedido.\n");
        printf("4 - Cancelar Pedido.\n");
        printf("Comando: ");
        scanf("%d", &section);

        switch(section){
            case 1:
                section2;
                printf("")
                break;
            case 2:
                printf("Beber.\n");
                break;
            case 3:
                printf("Preparar Pedido.\n");
                break;
            case 4:
                printf("Cancelar Pedido.\n");
                break;
        }
    } while (section != 3 && section != 4);
}



int main(){

    menu();
    return 0;
}