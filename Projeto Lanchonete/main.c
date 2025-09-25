#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "structs.h"
#include "funcoes.c"



void main_loop(int timer_global, int tam_ciclo){
    Heap *fila_pedidos;
    if((fila_pedidos = malloc(sizeof(Heap))) == NULL) return;

    Funcionario *funcionario;
    if((funcionario = malloc(sizeof(Funcionario))) == NULL) return;

    Equipamento *equipamento;
    if((equipamento = malloc(sizeof(Equipamento))) == NULL) return;

    
    
    bool res_auto = false;
    int var_control;
    printf("Ligar modo automático (Número par: sim, Número ímpar: não)?");
    scanf("%d", &var_control);

    if(var_control % 2 == 0)
    {
        res_auto = true;
        srand(time(NULL));
    }
    
    do{
        
        printf("===============================================================\n");
        printf("BEM VINDO AO BIG PAPÃO, O SANDUICHE MAIS RAPIDO DO NORDESTE!!\n");
        printf("Queres fazer you pedido ?\n");
        printf(" 1 - Fazer Pedido\n 2 - Não fazer pedido... Vagabundo\n 3 - Fechar o código\n");
        printf("Resposta: ");

        if(res_auto){
            var_control = rand() % 2 + 1; // Só escolhe 2 opções para não fechar o código sem querer
            printf("%d\n", var_control);
        }
        else{
            scanf("%d", &var_control);
        }

        switch(var_control){
            case 1:
                int max_itens= 2;

                while(max_itens > 0){
                int item_comer;
                printf("\nEscolha ate 4 itens (2 de comer e 2 de beber)\n");
                printf("\n\n==== Itens de Comer ====\n");
                printf(" 1 - Sanduiche Simples\n 2 - Sanduiche Medio\n 3 - Sanduiche Elaborado\n 4 - Batata Frita\n 5 - Nao quero nada\n");
                printf("Resposta: ");

                if(res_auto){
                    item_comer = rand() % 5 + 1;
                    printf("%d\n", item_comer);
                }
                else{
                    scanf("%d", &item_comer);
                }
                

                int item_beber;
                printf("\n\n==== Itens de Beber ====\n");
                printf("1 - Refrigerante\n 2 - Milk-Shake\n 3 - Suco\n 4 - Nao quero nada\n");
                printf("Resposta: ");


                if(res_auto){
                    item_beber = rand() % 4 + 1;
                    printf("%d\n", item_beber);
                }
                else{
                    scanf("%d", &item_beber);
                }

                if((item_comer >= 1 && item_comer < 5) || (item_beber >= 1 && item_beber < 4)){
                    fazer_pedido(item_comer, item_beber);
                    printf("Fazer Pedido\n");
                }
                else{
                    printf("Nenhum item selecionado.\n");
                }

                max_itens--;
                }
                break;
            case 2:
                printf("Vagabundo\n");
                break;
            case 3:
                printf("Fechar o código\n");
                return;
            default:
                printf("Animal, digite algo concreto!\n");
                break;
        }



        printf("%d seg\n", timer_global);
        timer_global = timer_global - tam_ciclo;
    } while(timer_global >= 0);
    

    printf("Programa Finalizado\n");
}



int main(){
    int timer_global = 300;
    int tam_ciclo = 100;
    main_loop(timer_global, tam_ciclo);
    return 0;
}
