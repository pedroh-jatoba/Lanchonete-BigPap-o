#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "funcoes.h"
// #include "structs.h"

/*############# FUNCAO SAIR ##################*/
void sair(){
    printf("\nPrograma finalizado!");
    exit(0);
}


/*##################### FUNCAO MAIN LOOP ########################*/
int receberInput(bool res_auto, int opc_auto, int opc_manual)
{
    int var_control;
    if(res_auto)
    {
        var_control = rand() % opc_auto + 1;
        printf("%d\n", var_control);
        return var_control;
    }
    else
    {
        scanf("%d", &var_control);
        
        //Tratamento de erro
        if(var_control > 0 && var_control <= opc_manual)
            return var_control;
        printf("\nResposta inválida!\n");
        printf("Tente Novamente\n");
        return receberInput(res_auto, opc_auto, opc_manual);
    }
}




/* ####################### FUNÇÕES HEAP #######################################*/

int criarHeap(Heap *heap, int capacidade){
    heap->pedidos = (Pedido*) malloc(capacidade * sizeof(Pedido));

    if(heap->pedidos == NULL){
        return 0;
    }

    heap->quantidade = 0;
    heap->capacidade = capacidade;
    return 1;
}

void liberarHeap(Heap* heap){
    free(heap->pedidos);
    heap->pedidos = NULL;
    heap->quantidade = 0;
    heap->capacidade = 0;
}

void swap(Pedido *a, Pedido *b){
    Pedido temp = *a;
    *a = *b;
    *b = temp;
}

void subirNoHeap(Heap* heap, int indice){
    while(indice > 0 && heap->pedidos[indice].tempo_preparo_total < heap->pedidos[(indice - 1) / 2].tempo_preparo_total){
        swap(&heap->pedidos[indice], &heap->pedidos[(indice - 1) / 2]);
        indice = (indice - 1) / 2;
    }
}

void inserirPedido(Heap* heap, Pedido novoPedido){
    if(heap->quantidade == heap->capacidade){
        printf("A lista está cheia, não conseguimos fazer mais pedidos.\n");
        return;
    }

    heap->quantidade++;
    int indice = heap->quantidade - 1;
    heap->pedidos[indice] = novoPedido;
    subirNoHeap(heap, indice);
}


void descerNoHeap(Heap* heap, int indice){
    int menor = indice;
    int esquerda = 2 * indice + 1;
    int direita = 2 * indice + 2;

    if(esquerda < heap->quantidade && heap->pedidos[esquerda].tempo_preparo_total < heap->pedidos[indice].tempo_preparo_total){
        menor = esquerda;
    }


    if(direita < heap->quantidade && heap->pedidos[direita].tempo_preparo_total < heap->pedidos[menor].tempo_preparo_total){
        menor = direita;
    }

    if(menor != indice){
        swap(&heap->pedidos[indice], &heap->pedidos[menor]);
        descerNoHeap(heap, menor);
    }
}


int removerItemPrioritario(Heap* heap, Pedido* pedidoRemovido){
    if(heap->quantidade <= 0) return 0;

    *pedidoRemovido = heap->pedidos[0];

    if(heap->quantidade > 1){
        heap->pedidos[0] = heap->pedidos[heap->quantidade - 1];
        descerNoHeap(heap, 0);
    }
    heap->quantidade--;

    return 1;
}



/**
 * @brief Função auxiliar recursiva para imprimir o heap como uma árvore.
 * Não precisa ser declarada no .h, pois é usada apenas por imprimirHeap.
 * @param heap Ponteiro para o heap.
 * @param indice Índice do nó atual a ser impresso.
 * @param nivel Nível de profundidade na árvore (para indentação).
 */
void imprimirNoHeap(Heap* heap, int indice, int nivel) {
    if (indice >= heap->quantidade) {
        return;
    }

    // Visita o filho da direita primeiro
    imprimirNoHeap(heap, 2 * indice + 2, nivel + 1);

    // Imprime o nó atual com indentação
    for (int i = 0; i < nivel; i++) {
        printf("    ");
    }
    printf("(ID: %d T:%d)\n", heap->pedidos[indice].id, heap->pedidos[indice].tempo_preparo_total);

    // Visita o filho da esquerda
    imprimirNoHeap(heap, 2 * indice + 1, nivel + 1);
}

/**
 * @brief Imprime o estado atual do heap em dois formatos: vetor e árvore.
 * @param heap Ponteiro para o heap a ser impresso.
 */
void imprimirHeap(Heap* heap) {
    printf("\n--- ESTADO ATUAL DO HEAP ---\n");
    if (heap->quantidade == 0) {
        printf("Heap Vazio.\n");
        printf("--------------------------\n");
        return;
    }

    // Visão 1: Como Vetor (Layout da Memória)
    printf("Visao Vetor [Quantidade: %d, Capacidade: %d]\n", heap->quantidade, heap->capacidade);
    printf("[ ");
    for (int i = 0; i < heap->quantidade; i++) {
        printf("(ID: %d T:%d) | ", heap->pedidos[i].id, heap->pedidos[i].tempo_preparo_total);
    }
    printf("]\n\n");

    // Visão 2: Como Árvore (Estrutura Lógica)
    printf("Visao Arvore (Raiz a esquerda):\n");
    imprimirNoHeap(heap, 0, 0); // Chama a função recursiva a partir da raiz
    printf("--------------------------\n");
}


/*################# FUNCOES LISTA DE PEDIDOS ##################################*/

ListaPedidos criarLista()
{
    ListaPedidos lista;
    lista.quantidade = 0;
    lista.cabeca = NULL;
    lista.cauda = NULL;
    return lista;
}

ListaPedidos removerLista(ListaPedidos lista)
{
    NodePedido *proxNode = lista.cabeca->prox;
    proxNode->ante = NULL;
    lista.cabeca->prox = NULL;
    lista.cabeca = proxNode;
    lista.quantidade--;
    return lista;
}

ListaPedidos adicionarLista(ListaPedidos lista, Pedido pedido)
{
    NodePedido *novo_node = (NodePedido*) malloc(sizeof(NodePedido)); 
    novo_node->pedido = pedido;
    novo_node->ante = lista.cauda;
    novo_node->prox = NULL;
    lista.cauda->prox = novo_node;
    lista.cauda = novo_node;
    lista.quantidade++;
    return lista;
}

/*################# FUNCOES LISTA DE PEDIDOS ##################################*/

ListaFuncionarios criarListaFuncionarios()
{
    ListaFuncionarios lista;
    lista.quantidade = 0;
    lista.cabeca = NULL;
    lista.cauda = NULL;
    return lista;
}

ListaFuncionarios removerListaFuncionario(ListaFuncionarios lista, NodeFuncionario *funcionario)
{
    if(lista.cabeca == funcionario)
    {
        lista.cabeca = funcionario->prox;
    }
    else if(lista.cauda == funcionario)
    {
        lista.cauda = funcionario->ante;
    }
    else
    {
        funcionario->ante->prox = funcionario->prox;
        funcionario->prox->ante = funcionario->ante;
    }

    funcionario->prox = NULL;
    funcionario->ante = NULL; 
    lista.quantidade--;
    return lista;
}

ListaFuncionarios adicionarListaFuncionario(ListaFuncionarios lista, NodeFuncionario *funcionario)
{
    if(lista.cabeca == NULL)
    {
        lista.cabeca = funcionario;
        lista.cauda = funcionario;
    }
    else
    {
        lista.cauda->prox = funcionario;
        funcionario->ante = lista.cauda;
        lista.cauda = funcionario;
    }
    lista.quantidade++;
    return lista;
}

/*################# FUNCOES IMPRIMIR #####################*/

void imprimirStatusPedido(StatusPedido status)
{
    switch(status) {
        case RECEBIDO:
            printf("Status: Recebido\n");
            break;
        case PREPARANDO_ITENS:
            printf("Status: Preparando itens\n");
            break;
        case AGUARDANDO_MONTAGEM:
            printf("Status: Aguardando montagem\n");
            break;
        case ENTREGUE:
            printf("Status: Entregue\n");
            break;
        case ATRASADO:
            printf("Status: Atrasado\n");
            break;
        case ARMAZENADO:
            printf("Status: Armazenado\n");
            break;
        default:
            printf("Status desconhecido\n");
            break;
    }
}

void imprimirStatusItem(StatusItem status)
{
    switch (status)
    {
    case AGUARDANDO_PREPARADO:
        printf("Aguardando preparo.\n");
        break;
    
    case EM_PREPARO:
        printf("Em preparo.\n");
        break;

    case PRONTO:
        printf("Pronto.\n");
        break;

    default:
        break;
    }
}

void imprimirNomeDoItem(NomePedido nome)
{
    switch(nome) {
        case SANDUICHE_SIMPLES:
            printf("Sanduíche Simples:\n");
            break;
        case SANDUICHE_MEDIO:
            printf("Sanduíche Médio:\n");
            break;
        case SANDUICHE_ELABORADO:
            printf("Sanduíche Elaborado:\n");
            break;
        case BATATA_FRITA:
            printf("Batata Frita:\n");
            break;
        case REFRIGERANTE:
            printf("Refrigerante:\n");
            break;
        case MILK_SHAKE:
            printf("Milk Shake:\n");
            break;
        case SUCO:
            printf("Suco:\n");
            break;
        default:
            printf("Pedido desconhecido:\n");
            break;
    }
}

void imprimirItensPedido(ItemPedido item)
{
    printf("\n");
    imprimirNomeDoItem(item.nome);
    imprimirStatusItem(item.status);
}

void imprimirPedido(Pedido pedido)
{
    printf("\nPEDIDO %d\n", pedido.id);
    for(int i = 0; i<pedido.num_itens; i++)
    {
        imprimirItensPedido(pedido.itens[i]);
    }
    printf("Tempo de chegada: %ds.\n", pedido.tempo_chegada);
    printf("Tempo de preparo total: %ds.\n", pedido.tempo_preparo_total);
}

void imprimirLista(ListaPedidos lista)
{
    if(lista.cabeca == NULL)
    {
        printf("\nA fila está vazia!\n");
        return;
    }
    NodePedido *pivo = lista.cabeca;
    while(pivo != NULL)
    {
        imprimirPedido(pivo->pedido);
    }
    printf("Quantidade de pedidos: %d\n", lista.quantidade);
}




/*#################### SEM DEFINICAO ########################*/

int calcularPreparo(NomePedido pedido) //Retorna o tempo de preparo em segundos
{
    switch(pedido)
    {
        case BATATA_FRITA:
            return 190; // 3 min e 10 seg

        case SANDUICHE_SIMPLES:
            return 58;

        case SANDUICHE_MEDIO:
            return 88; // 1 min e 28 seg

        case SANDUICHE_ELABORADO:
            return 105; // 1 min e 45 seg

        case REFRIGERANTE:
            return 5;

        case SUCO:
            return 38;

        case MILK_SHAKE:
            return 60; // 1 min
    }
}


// int verifica_chapa()
// {
//     return 0;
// }

// int verifica_batata()
// {
//     return 0;
// }

// int verifica_batata()
// {
//     return 0;
// }

int tempo_restante(Pedido *pedido, Funcionario funcionario[], Equipamento equipamentos[])
{
    int tempo_preparo = pedido->itens[0].tempo_preparo_total;
    int local_vago;
    int tempo_funcionario;

/*
    - tempo_preparo = tempo que ele demora para ser preparado
    - local_vago = tempo onde o local onde ele será preparado estará vazio 
    - tempo_funcionario = menor tempo em que teremos um funcionário apto a preparar;

    if (tempo_funcionario > local_vago)
    {
        tempo_restante = tempo_preparo + local_vago; 
    }
    else
    {
        tempo_restante = tempo_preparo + tempo_funcionario;
    }

    - Faça isso com todos os outros itens da lista:
    - Puxa um item do pedido;
    - tempo_preparo = tempo que ele demora para ser preparado
    - local_vago = tempo onde o local onde ele será preparado estará vazio 
    - tempo_funcionario = menor tempo em que teremos um funcionário apto a

    if (tempo_funcionario > local_vago)
    {
        if(tempo_restante < tempo_preparo + local_vago) tempo_restante = tempo_preparo + local_vago; 
        
    }
    else
    {
        if(tempo_restante < tempo_preparo + tempo_funcionario) tempo_restante = tempo_preparo + tempo_funcionario;
    }
*/
    return 0;
    }




// Funções relacionados a bandeja

bool todosItensProntos(Pedido pedido) {
    for(int i = 0; i < pedido.num_itens; i++) {
        if(pedido.itens[i].status != PRONTO) {
            return false;
        }
    }
    return true;
}


void atualizarPedidosParaMontagem(ListaPedidos *lista, ListaPedidos *fila_montagem) {
    NodePedido *pivo = lista->cabeca;
    while(pivo != NULL) {
        if(todosItensProntos(pivo->pedido) && pivo->pedido.status != AGUARDANDO_MONTAGEM) {
            pivo->pedido.status = AGUARDANDO_MONTAGEM;
            *fila_montagem = adicionarLista(*fila_montagem, pivo->pedido);
            printf("pedido %d pronto para montagem!\n", pivo->pedido.id);
        }
        pivo = pivo->prox;
    }
}

void montarBandeja(ListaPedidos *fila_montagem, Funcionario funcionarios[], int num_funcionarios) {
    if(fila_montagem->cabeca == NULL) return;

    NodePedido *pedidoNode = fila_montagem->cabeca;
    Pedido *pedido = &pedidoNode->pedido;

    for(int i = 0; i < num_funcionarios; i++) {
        Funcionario *f = &funcionarios[i];
        if(f->status == LIVRE) {
            for(int h = 0; h < f->num_habilidade; h++) {
                //checa o funcionario disponivel
                if(f->habilidades[h] == HABILIDADE_MONTAGEM) {
                    f->status = OCUPADO; // status funcionario e inicio da montagem
                    f->id_pedido_atual = pedido->id;

                    printf("Funcionario %d iniciou montagem do pedido %d.\n", f->id, pedido->id);

                    pedido->tempo_preparo_local = 30; // "tempo de preparo"

                    pedido->status = ENTREGUE; //pedido montado
                    printf("pedido %d foi ENTREGUE!\n", pedido->id);

                    *fila_montagem = removerLista(*fila_montagem);

                    f->status = LIVRE; // livrando funcionario
                    f->id_pedido_atual = -1;// livra o funcionario do id do pedido

                    return;
                }
            }
        }
    }
}