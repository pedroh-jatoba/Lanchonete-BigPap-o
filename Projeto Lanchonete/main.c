#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "funcoes.c"

int fazerPedido(bool res_auto)
{
    //Adicionar verificador do Heap
    //Se o Heap estiver cheio, não fazer pedido
    printf("===============================================================\n");
    printf("BEM VINDO AO BIG PAPÃO, O SANDUICHE MAIS RAPIDO DO NORDESTE!!\n");
    printf("Queres fazer you pedido ?\n");
    printf(" 1 - Fazer Pedido\n 2 - Não fazer pedido... Vagabundo\n 3 - Fechar o código\n");
    printf("Resposta: ");
    return receberInput(res_auto, 2, 3);
}

ItemPedido receberItem(NomePedido nome)
{
    ItemPedido item;
    
    if(nome == NADA)
    {
        item.status = PRONTO;
        item.tempo_preparo_total = 0;
        item.tempo_restante_preparo = 0;
    }
    else
    {
        item.status = AGUARDANDO_PREPARADO;
        item.tempo_preparo_total = calcularPreparo(nome);
        item.tempo_restante_preparo = item.tempo_preparo_total;
    
    }
    item.nome = nome;
    return item;
}

Pedido receberPedido(bool res_auto)
{
    Pedido novo_pedido;
    int item_comer, item_beber;

    for(int max_itens = 2; max_itens > 0; max_itens--){
        printf("\nEscolha ate 4 itens (2 de comer e 2 de beber)\n");
        printf("\n\n==== Itens de Comer ====\n");
        printf(" 1 - Sanduiche Simples\n 2 - Sanduiche Medio\n 3 - Sanduiche Elaborado\n 4 - Batata Frita\n 5 - Nao quero nada\n");
        printf("Resposta: ");
        item_comer = receberInput(res_auto, 5, 5);
        item_comer--; //Tornar resposta compatível com a lista

        printf("\n\n==== Itens de Beber ====\n");
        printf("1 - Refrigerante\n 2 - Milk-Shake\n 3 - Suco\n 4 - Nao quero nada\n");
        printf("Resposta: ");
        item_beber = receberInput(res_auto, 4, 4);
        item_beber+=4;//Tornar resposta compatível com a lista     
        
        int i = 4/max_itens -1;
        novo_pedido.itens[i] = receberItem(item_comer);
        novo_pedido.itens[i-1] = receberItem(item_beber);
    }
    return novo_pedido;
}



void main_loop(int timer_global, int tam_ciclo){
    Heap fila_pedidos;
    ListaPedidos lista_nova = criarLista();
    bool res_auto;

    if(!criarHeap(&fila_pedidos, 10))
    {
        printf("Erro ao criar o heap.\n");
        return;
    }

    printf("Ligar modo automático (1 - Sim, 2 - Não)?");
    if(receberInput(false, 2, 2) == 1)
    {
        res_auto = true;
        srand(time(NULL));
    }

    do{
        switch(fazerPedido(res_auto))
        {
            case 1:
                lista_nova = adicionarLista(lista_nova, receberPedido(res_auto));
                break;
            case 3:
                sair();
                return;
        }
        printf("%d seg\n", timer_global);
        timer_global = timer_global - tam_ciclo;
    } while(timer_global >= 0);
}

int main(){
    int timer_global = 300;
    int tam_ciclo = 100;

    Equipamento equipamentos[4];
    for(int i = 0; i < 4; i++)
    {
        equipamentos[i].nome = i;
    }//Nomear cada equipamento

    Locais locais[4];
    for(int i = 0; i < 4; i++)
    {
        locais[i].nome = i;
    }//Nomear cada local

    main_loop(timer_global, tam_ciclo);
    
    return 0;
}
