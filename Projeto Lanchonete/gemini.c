#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para a função sleep(), que ajuda a visualizar a simulação

// --- Definições e Constantes ---

#define TEMPO_LIMITE_ENTREGA_SEGUNDOS (5 * 60)
#define MAX_NOME_ITEM 50
#define MAX_ITENS_PEDIDO 10
#define MAX_ITENS_EM_PREPARO 5 // Quantos itens podem ser feitos ao mesmo tempo (capacidade da cozinha)

// Cores para o terminal (ANSI escape codes)
#define COR_VERDE   "\033[92m"
#define COR_VERMELHO "\033[91m"
#define COR_AMARELO "\033[93m"
#define COR_RESET   "\033[0m"

// Enum para status, melhora a legibilidade
typedef enum { PENDENTE, EM_PREPARO, PRONTO, ENTREGUE, ATRASADO } StatusPedido;
typedef enum { AGUARDANDO, PRONTO_ITEM } StatusItem;

// --- Estruturas de Dados (Structs) ---

// Item do Cardápio
typedef struct {
    char nome[MAX_NOME_ITEM];
    int tempoPreparo;
} ItemCardapio;

// Apenas para simplificar, definimos o cardápio globalmente
const ItemCardapio CARDAPIO[] = {
    {"Hamburguer Classico", 180},
    {"X-Salada Especial",   240},
    {"Batata Frita",        150},
    {"Refrigerante",         15}
};
const int TAMANHO_CARDAPIO = sizeof(CARDAPIO) / sizeof(ItemCardapio);

// Para cada item dentro de um pedido
typedef struct {
    char nome[MAX_NOME_ITEM];
    StatusItem status;
} ItemDoPedido;

// Estrutura do Pedido (será um nó da lista encadeada)
typedef struct Pedido {
    int id;
    int tempoChegada;
    StatusPedido status;
    ItemDoPedido itens[MAX_ITENS_PEDIDO];
    int numItens;
    struct Pedido* proximo; // Ponteiro para o próximo pedido na fila
} Pedido;

// Item que está sendo preparado ativamente
typedef struct {
    Pedido* pedido; // Referência ao pedido original
    int indiceItem;   // Índice do item dentro do pedido
    int tempoRestante;
} ItemEmPreparo;

// Estrutura principal que gerencia toda a simulação
typedef struct {
    int tempoAtualSegundos;
    int cozinheirosDisponiveis;
    int proximoIdPedido;
    
    Pedido* filaPedidosPendentes; // Ponteiro para o início da fila (lista encadeada)
    ItemEmPreparo itensEmPreparo[MAX_ITENS_EM_PREPARO];
    int numItensEmPreparo;
} Lanchonete;


// --- Protótipos das Funções ---

void inicializarLanchonete(Lanchonete* l, int numCozinheiros);
Pedido* criarPedido(int id, int tempoChegada);
void adicionarItemAoPedido(Pedido* p, const char* nomeItem);
void adicionarPedidoLanchonete(Lanchonete* l, Pedido* p);
int getTempoPreparo(const char* nomeItem);
void simularCiclo(Lanchonete* l);
void limparMemoriaPedidos(Lanchonete* l);


// --- Implementação das Funções ---

// Inicializa a lanchonete com valores padrão
void inicializarLanchonete(Lanchonete* l, int numCozinheiros) {
    l->tempoAtualSegundos = 0;
    l->cozinheirosDisponiveis = numCozinheiros;
    l->proximoIdPedido = 1;
    l->filaPedidosPendentes = NULL;
    l->numItensEmPreparo = 0;
}

// Aloca memória e cria um novo pedido
Pedido* criarPedido(int id, int tempoChegada) {
    Pedido* novoPedido = (Pedido*)malloc(sizeof(Pedido));
    if (!novoPedido) {
        perror("Falha ao alocar memória para o pedido");
        exit(1);
    }
    novoPedido->id = id;
    novoPedido->tempoChegada = tempoChegada;
    novoPedido->status = PENDENTE;
    novoPedido->numItens = 0;
    novoPedido->proximo = NULL;
    return novoPedido;
}

// Adiciona um item a um pedido existente
void adicionarItemAoPedido(Pedido* p, const char* nomeItem) {
    if (p->numItens < MAX_ITENS_PEDIDO) {
        strcpy(p->itens[p->numItens].nome, nomeItem);
        p->itens[p->numItens].status = AGUARDANDO;
        p->numItens++;
    } else {
        printf("Aviso: Limite de itens por pedido atingido.\n");
    }
}

// Adiciona um pedido ao final da fila de pendentes
void adicionarPedidoLanchonete(Lanchonete* l, Pedido* p) {
    printf("Tempo %ds: %sNovo pedido recebido! (ID: %d)%s\n", l->tempoAtualSegundos, COR_AMARELO, p->id, COR_RESET);
    if (l->filaPedidosPendentes == NULL) {
        l->filaPedidosPendentes = p;
    } else {
        Pedido* atual = l->filaPedidosPendentes;
        while (atual->proximo != NULL) {
            atual = atual->proximo;
        }
        atual->proximo = p;
    }
}

// Busca o tempo de preparo de um item no cardápio
int getTempoPreparo(const char* nomeItem) {
    for (int i = 0; i < TAMANHO_CARDAPIO; i++) {
        if (strcmp(CARDAPIO[i].nome, nomeItem) == 0) {
            return CARDAPIO[i].tempoPreparo;
        }
    }
    return -1; // Item não encontrado
}

// A função principal que move a simulação
void simularCiclo(Lanchonete* l) {
    l->tempoAtualSegundos++;

    // 1. Atualiza o tempo dos itens em preparo
    for (int i = 0; i < l->numItensEmPreparo; i++) {
        l->itensEmPreparo[i].tempoRestante--;

        // Se um item ficou pronto...
        if (l->itensEmPreparo[i].tempoRestante <= 0) {
            Pedido* p = l->itensEmPreparo[i].pedido;
            int itemIdx = l->itensEmPreparo[i].indiceItem;

            printf("Tempo %ds: Item '%s' do Pedido %d ficou pronto!\n", l->tempoAtualSegundos, p->itens[itemIdx].nome, p->id);
            p->itens[itemIdx].status = PRONTO_ITEM;
            l->cozinheirosDisponiveis++;

            // Remove o item da lista de preparo (deslocando os elementos)
            for (int j = i; j < l->numItensEmPreparo - 1; j++) {
                l->itensEmPreparo[j] = l->itensEmPreparo[j + 1];
            }
            l->numItensEmPreparo--;
            i--; // Reavalia o índice atual, pois o array mudou

            // Verifica se o pedido inteiro ficou pronto
            int pedidoCompleto = 1;
            for(int k = 0; k < p->numItens; k++) {
                if(p->itens[k].status != PRONTO_ITEM) {
                    pedidoCompleto = 0;
                    break;
                }
            }
            if(pedidoCompleto) {
                p->status = PRONTO;
                printf("Tempo %ds: %sPEDIDO COMPLETO! (ID: %d) está pronto para entrega.%s\n", l->tempoAtualSegundos, COR_VERDE, p->id, COR_RESET);
            }
        }
    }

    // 2. Inicia preparo de novos itens se houver cozinheiros livres e pedidos pendentes
    while (l->cozinheirosDisponiveis > 0 && l->filaPedidosPendentes != NULL) {
        Pedido* pedidoAtual = l->filaPedidosPendentes;
        int itemIniciado = 0;
        
        for (int i = 0; i < pedidoAtual->numItens; i++) {
            if (pedidoAtual->itens[i].status == AGUARDANDO) {
                if (l->numItensEmPreparo < MAX_ITENS_EM_PREPARO) {
                    pedidoAtual->status = EM_PREPARO;

                    l->itensEmPreparo[l->numItensEmPreparo].pedido = pedidoAtual;
                    l->itensEmPreparo[l->numItensEmPreparo].indiceItem = i;
                    l->itensEmPreparo[l->numItensEmPreparo].tempoRestante = getTempoPreparo(pedidoAtual->itens[i].nome);
                    
                    printf("Tempo %ds: Cozinheiro iniciou o preparo de '%s' do Pedido %d.\n", l->tempoAtualSegundos, pedidoAtual->itens[i].nome, pedidoAtual->id);
                    
                    l->numItensEmPreparo++;
                    l->cozinheirosDisponiveis--;
                    itemIniciado = 1;
                    break; // Sai do for de itens para dar chance a outro pedido
                }
            }
        }
        
        // Se todos os itens deste pedido já estão prontos ou em preparo, remove-o da fila de pendentes
        int todosItensIniciados = 1;
        for(int i = 0; i < pedidoAtual->numItens; i++){
            if(pedidoAtual->itens[i].status == AGUARDANDO) {
                todosItensIniciados = 0;
                break;
            }
        }

        if(todosItensIniciados){
             l->filaPedidosPendentes = l->filaPedidosPendentes->proximo;
        }

        if (!itemIniciado) {
            break; // Se não conseguiu iniciar nenhum item, para de tentar por agora
        }
    }
    
    // 3. Verifica prazos
    Pedido* p = l->filaPedidosPendentes;
    while(p) { // Itera sobre a lista para verificar prazos
        if(p->status != ATRASADO && p->status != PRONTO && p->status != ENTREGUE) {
             int tempoDecorrido = l->tempoAtualSegundos - p->tempoChegada;
             if(tempoDecorrido > TEMPO_LIMITE_ENTREGA_SEGUNDOS) {
                 p->status = ATRASADO;
                 printf("Tempo %ds: %sALERTA: Pedido %d estourou o tempo de entrega!%s\n", l->tempoAtualSegundos, COR_VERMELHO, p->id, COR_RESET);
             }
        }
        p = p->proximo;
    }
}

// Libera a memória alocada para os pedidos na lista encadeada
void limparMemoriaPedidos(Lanchonete* l) {
    Pedido* atual = l->filaPedidosPendentes;
    while(atual != NULL) {
        Pedido* temp = atual;
        atual = atual->proximo;
        free(temp);
    }
    l->filaPedidosPendentes = NULL;
}


// --- Função Principal ---

int main() {
    Lanchonete minhaLanchonete;
    inicializarLanchonete(&minhaLanchonete, 2); // Simulação com 2 cozinheiros

    // Criando pedido 1 (chega no tempo 0)
    Pedido* p1 = criarPedido(minhaLanchonete.proximoIdPedido++, 0);
    adicionarItemAoPedido(p1, "Hamburguer Classico");
    adicionarItemAoPedido(p1, "Refrigerante");
    adicionarPedidoLanchonete(&minhaLanchonete, p1);

    // Criando pedido 2 (chega no tempo 60s)
    Pedido* p2 = criarPedido(minhaLanchonete.proximoIdPedido++, 60);
    adicionarItemAoPedido(p2, "X-Salada Especial");
    adicionarItemAoPedido(p2, "Batata Frita");

    // Loop principal da simulação
    printf("\n--- Iniciando Simulação ---\n");
    int pedido2_adicionado = 0;
    while (minhaLanchonete.filaPedidosPendentes != NULL || minhaLanchonete.numItensEmPreparo > 0) {
        
        // Evento: Adiciona o segundo pedido no tempo certo
        if (!pedido2_adicionado && minhaLanchonete.tempoAtualSegundos >= p2->tempoChegada) {
            adicionarPedidoLanchonete(&minhaLanchonete, p2);
            pedido2_adicionado = 1;
        }

        simularCiclo(&minhaLanchonete);
        
        // Descomente a linha abaixo para ver a simulação em "tempo real"
        // usleep(50000); // Pausa por 50ms para desacelerar a saída
    }

    printf("\n--- Simulação Concluída em %d segundos ---\n", minhaLanchonete.tempoAtualSegundos);
    
    // Importante: Liberar a memória dos pedidos que foram processados e retirados da fila
    // Uma implementação mais robusta moveria os pedidos prontos para outra lista antes de liberar
    // Aqui, como eles são removidos da única lista, temos que ter cuidado.
    // Neste exemplo simples, os nós não foram explicitamente liberados após o uso.
    // A função limparMemoria faria isso, mas os ponteiros p1 e p2 ficariam inválidos.
    // Vamos liberar p1 e p2 diretamente.
    free(p1);
    free(p2);
    
    return 0;
}