#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "funcoes.h"

/*----------------- Utilitários -----------------*/
void sair() {
    printf("\nPrograma finalizado!\n");
    exit(0);
}

/*----------------- Input -----------------*/
int receberInput(bool res_auto, int opc_auto, int opc_manual) {
    int var_control;
    if (res_auto) {
        var_control = (rand() % opc_auto) + 1;
        printf("%d\n", var_control);
        return var_control;
    } else {
        if (scanf("%d", &var_control) != 1) {
            // limpar stdin e retornar um valor inválido para forçar nova leitura
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            return receberInput(res_auto, opc_auto, opc_manual);
        }
        if (var_control > 0 && var_control <= opc_manual)
            return var_control;
        printf("\nResposta inválida! Tente Novamente.\n");
        return receberInput(res_auto, opc_auto, opc_manual);
    }
}

/*----------------- HEAP (min-heap por tempo_preparo_total) -----------------*/
int criarHeap(Heap *heap, int capacidade) {
    heap->pedidos = (Pedido*) malloc(capacidade * sizeof(Pedido));
    if (heap->pedidos == NULL) return 0;
    heap->quantidade = 0;
    heap->capacidade = capacidade;
    return 1;
}

void liberarHeap(Heap* heap) {
    if (heap->pedidos) free(heap->pedidos);
    heap->pedidos = NULL;
    heap->quantidade = 0;
    heap->capacidade = 0;
}

static void swapPedido(Pedido *a, Pedido *b) {
    Pedido tmp = *a;
    *a = *b;
    *b = tmp;
}

static void subirNoHeap(Heap* heap, int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2;
        if (heap->pedidos[indice].tempo_restante_preparo < heap->pedidos[pai].tempo_restante_preparo) {
            swapPedido(&heap->pedidos[indice], &heap->pedidos[pai]);
            indice = pai;
        } else break;
    }
}

void inserirPedido(Heap* heap, Pedido novoPedido) {
    if (heap->quantidade >= heap->capacidade) {
        printf("Heap cheio: não foi possível inserir pedido.\n");
        return;
    }
    int idx = heap->quantidade++;
    heap->pedidos[idx] = novoPedido;
    subirNoHeap(heap, idx);
}

static void descerNoHeap(Heap* heap, int indice) {
    int n = heap->quantidade;
    while (true) {
        int l = 2*indice + 1;
        int r = 2*indice + 2;
        int menor = indice;
        if (l < n && heap->pedidos[l].tempo_restante_preparo < heap->pedidos[menor].tempo_restante_preparo) menor = l;
        if (r < n && heap->pedidos[r].tempo_restante_preparo < heap->pedidos[menor].tempo_restante_preparo) menor = r;
        if (menor != indice) {
            swapPedido(&heap->pedidos[indice], &heap->pedidos[menor]);
            indice = menor;
        } else break;
    }
}

int removerItemPrioritario(Heap* heap, Pedido* pedidoRemovido) {
    if (heap->quantidade == 0) return 0;
    *pedidoRemovido = heap->pedidos[0];
    heap->quantidade--;
    if (heap->quantidade > 0) {
        heap->pedidos[0] = heap->pedidos[heap->quantidade];
        descerNoHeap(heap, 0);
    }
    return 1;
}

/* Impressão simples do heap (vetor) */
void imprimirHeap(Heap* heap) {
    if (heap->quantidade == 0) {
        printf("Heap vazio\n");
        return;
    }
    
    printf("Heap[%d]:\n", heap->quantidade);
    for (int i = 0; i < heap->quantidade; ++i) {
        Pedido p = heap->pedidos[i];
        printf("[PRIORIDADE] PEDIDO ID:%d - TempoTotal:%d - TempoRestante:%d\n",
               p.id, p.tempo_preparo_total, p.tempo_restante_preparo);
    }
    printf("\n");
}


/*----------------- Lista de pedidos (duplamente ligada) -----------------*/
ListaPedidos criarLista() {
    ListaPedidos l;
    l.cabeca = l.cauda = NULL;
    l.quantidade = 0;
    return l;
}

NodePedido* criarNodePedido(Pedido pedido) {
    NodePedido *n = (NodePedido*) malloc(sizeof(NodePedido));
    if (!n) return NULL;
    n->pedido = pedido;
    n->ante = n->prox = NULL;
    return n;
}

void adicionarListaPedidos(ListaPedidos *lista, NodePedido *novo_node) {
    if (!lista || !novo_node) return;
    novo_node->prox = NULL;
    novo_node->ante = lista->cauda;
    if (lista->cauda) lista->cauda->prox = novo_node;
    lista->cauda = novo_node;
    if (!lista->cabeca) lista->cabeca = novo_node;
    lista->quantidade++;
}

void adicionarListaPedidosOrdenado(ListaPedidos *lista, NodePedido *novo_node) {
    if (!lista || !novo_node) return;

    NodePedido *atual = lista->cabeca;
    
    // percorre até achar posição correta (ordem decrescente pelo tempo total)
    while (atual && atual->pedido.tempo_preparo_total >= novo_node->pedido.tempo_preparo_total) {
        atual = atual->prox;
    }

    if (!atual) {
        // insere no final
        novo_node->ante = lista->cauda;
        novo_node->prox = NULL;
        if (lista->cauda) lista->cauda->prox = novo_node;
        lista->cauda = novo_node;
        if (!lista->cabeca) lista->cabeca = novo_node;
    } else if (!atual->ante) {
        // insere no começo
        novo_node->prox = atual;
        novo_node->ante = NULL;
        atual->ante = novo_node;
        lista->cabeca = novo_node;
    } else {
        // insere no meio
        novo_node->prox = atual;
        novo_node->ante = atual->ante;
        atual->ante->prox = novo_node;
        atual->ante = novo_node;
    }

    lista->quantidade++;
}

void liberarFuncionario(NodeFuncionario *node, ListaFuncionarios *origem, ListaFuncionarios *destino_reserva) {
    if (!node || !origem || !destino_reserva) return;
    
    printf("Funcionario %d finalizou o atendimento e voltou para a reserva.\n", node->funcionario.id);
    removerNodeFuncionario(origem, node);

    node->funcionario.status = LIVRE;
    node->funcionario.local_atual = RESERVA;
    node->funcionario.pedido_trabalhado = NULL;

    adicionarListaFuncionario(destino_reserva, node);
}

void processarRecepcao(Locais *local_recepcao, Locais *local_separador, ListaFuncionarios *reserva, int ciclo) {
    NodeFuncionario *func_atual = local_recepcao->funcionario.cabeca;
    
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox; // Guarda o próximo para o caso de remoção

        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_atendido = func_atual->funcionario.pedido_trabalhado;
            
            pedido_atendido->pedido.tempo_preparo_local -= ciclo;

            // Se o tempo acabou, a tarefa está concluída
            if (pedido_atendido->pedido.tempo_preparo_local <= 0) {
                printf("RECEPCAO: Atendimento do Pedido %d concluido. Movendo para a separacao.\n", pedido_atendido->pedido.id);
                
                // Adiciona o pedido à fila de espera do próximo local (Separador)
                adicionarListaPedidos(&local_separador->fila_espera, pedido_atendido);
                
                // Libera o funcionário, movendo-o de volta para a reserva
                liberarFuncionario(func_atual, &local_recepcao->funcionario, reserva);
            }
        }
        func_atual = proximo_func;
    }
}


NodePedido* removerListaPedidos_front(ListaPedidos *lista) {
    if (!lista || !lista->cabeca) return NULL;
    NodePedido *ret = lista->cabeca;
    lista->cabeca = ret->prox;
    if (lista->cabeca) lista->cabeca->ante = NULL;
    else lista->cauda = NULL;
    ret->prox = ret->ante = NULL;
    lista->quantidade--;
    return ret;
}

void imprimirLista(ListaPedidos lista) {
    NodePedido *atual = lista.cabeca;
    if (!atual) {
        printf("Lista de pedidos vazia.\n");
        return;
    }
    while (atual) {
        Pedido p = atual->pedido;
        printf("[NORMAL] PEDIDO ID:%d - Itens:%d - TempoTotal:%d - TempoRestante:%d - Status:%d\n", 
               p.id, p.num_itens, p.tempo_preparo_total, p.tempo_restante_preparo, p.status);

        for (int i = 0; i < p.num_itens; i++) {
            printf("  - ");
            imprimirNomeDoItem(p.itens[i].nome);
            imprimirStatusItem(p.itens[i].status);
            printf(" (t:%d)\n", p.itens[i].tempo_preparo_total);
        }
        printf("\n");  // separa pedidos
        atual = atual->prox;
    }
}

/*----------------- Impressão de pedidos e itens -----------------*/
void imprimirNomeDoItem(NomePedido nome) {
    switch (nome) {
        case SANDUICHE_SIMPLES: printf("Sanduiche Simples"); break;
        case SANDUICHE_MEDIO: printf("Sanduiche Medio"); break;
        case SANDUICHE_ELABORADO: printf("Sanduiche Elaborado"); break;
        case BATATA_FRITA: printf("Batata Frita"); break;
        case REFRIGERANTE: printf("Refrigerante"); break;
        case MILK_SHAKE: printf("Milk Shake"); break;
        case SUCO: printf("Suco"); break;
        case NADA: printf("Nada"); break;
        default: printf("Desconhecido"); break;
    }
}

void imprimirStatusItem(StatusItem status) {
    switch (status) {
        case AGUARDANDO_PREPARADO: printf(" (Aguardando)"); break;
        case EM_PREPARO: printf(" (Em preparo)"); break;
        case PRONTO: printf(" (Pronto)"); break;
        default: break;
    }
}

void imprimirPedido(Pedido pedido) {
    printf("PEDIDO ID:%d - Itens: %d - TempoTotal:%d - TempoRestante:%d - Status:%d\n", 
           pedido.id, pedido.num_itens, pedido.tempo_preparo_total, pedido.tempo_restante_preparo, pedido.status);
    for (int i = 0; i < pedido.num_itens; ++i) {
        printf("  - ");
        imprimirNomeDoItem(pedido.itens[i].nome);
        imprimirStatusItem(pedido.itens[i].status);
        printf(" (t:%d)\n", pedido.itens[i].tempo_preparo_total);
    }
}

/*----------------- Fila de Prioridade (pedidos com menos de 60 segundos) -----------------*/
void verificarEPriorizarPedidos(Locais *local, int tempo_atual) {
    NodePedido *atual = local->fila_espera.cabeca;
    NodePedido *anterior = NULL;
    
    while (atual != NULL) {
        NodePedido *proximo = atual->prox;

        // quanto tempo o pedido já está esperando
        int tempo_espera = tempo_atual - atual->pedido.tempo_chegada;

        if (tempo_espera >= 240) { // 4 minutos
            // remove da lista
            if (anterior == NULL) {
                local->fila_espera.cabeca = proximo;
                if (proximo) proximo->ante = NULL;
                else local->fila_espera.cauda = NULL;
            } else {
                anterior->prox = proximo;
                if (proximo) proximo->ante = anterior;
                else local->fila_espera.cauda = anterior;
            }
            local->fila_espera.quantidade--;

            // adiciona no heap de prioridade
            inserirPedido(&local->heap, atual->pedido);
            printf("Pedido ID:%d movido para PRIORIDADE (esperou 4min)\n", 
                   atual->pedido.id);

            free(atual); // libera o nó
        } else {
            anterior = atual;
        }

        atual = proximo;
    }
}


void processarFilaPrioridade(Locais *local) {
    printf("\n=== PROCESSANDO FILA DE PRIORIDADE ===\n");
    while (local->heap.quantidade > 0) {
        Pedido pedido_prioritario;
        if (removerItemPrioritario(&local->heap, &pedido_prioritario)) {
            printf("Processando pedido prioritário ID:%d (tempo restante: %d segundos)\n", 
                   pedido_prioritario.id, pedido_prioritario.tempo_restante_preparo);
            
            // Aqui você pode adicionar a lógica para processar o pedido prioritário
            // Por exemplo, mover para uma fila de processamento ou marcar como sendo preparado
        }
    }
    printf("=====================================\n\n");
}

/*----------------- Lista de Itens para Preparo -----------------*/

ListaItemPreparo criarListaItemPreparo() {
    ListaItemPreparo l;
    l.cabeca = l.cauda = NULL;
    l.quantidade = 0;
    return l;
}

NodeItemPreparo* criarNodeItemPreparo(ItemPreparo item) {
    NodeItemPreparo *n = (NodeItemPreparo*) malloc(sizeof(NodeItemPreparo));
    if (!n) return NULL;
    n->item = item;
    n->ante = n->prox = NULL;
    return n;
}

void adicionarListaItemPreparo(ListaItemPreparo *lista, NodeItemPreparo *novo_node) {
    if (!lista || !novo_node) return;
    novo_node->prox = NULL;
    novo_node->ante = lista->cauda;
    if (lista->cauda) lista->cauda->prox = novo_node;
    lista->cauda = novo_node;
    if (!lista->cabeca) lista->cabeca = novo_node;
    lista->quantidade++;
}

//================================ SEPARADOR ====================================

void distribuirItens(NodePedido *pedido_node, Equipamento equipamentos[]) {
    if (!pedido_node) return;

    printf("SEPARADOR: Distribuindo itens do Pedido %d para as filas dos equipamentos.\n", pedido_node->pedido.id);

    // Itera por todos os itens dentro do pedido
    for (int i = 0; i < pedido_node->pedido.num_itens; i++) {
        ItemPedido *item_original = &pedido_node->pedido.itens[i];

        if (item_original->nome == NADA) continue;

        // Itens de preparo rápido (como refrigerante) são marcados como prontos
        // e não entram em nenhuma fila de equipamento.
        if (item_original->nome == REFRIGERANTE) {
            item_original->status = PRONTO;
            printf("  -> Item 'Refrigerante' (Pedido %d) e instantaneo, sera pego na montagem.\n", pedido_node->pedido.id);
            continue; // Pula para o próximo item
        }

        // Cria um "ItemPreparo" para os demais itens
        ItemPreparo item_para_fila;
        item_para_fila.nome = item_original->nome;
        item_para_fila.tempo_preparo = item_original->tempo_preparo_total;
        item_para_fila.status = AGUARDANDO_PREPARADO;
        item_para_fila.pedido_pai = &pedido_node->pedido;

        NodeItemPreparo *node_item = criarNodeItemPreparo(item_para_fila);
        NomeEquipamento destino;

        // Decide para qual equipamento o item deve ir
        switch (item_original->nome) {
            case SANDUICHE_SIMPLES:
            case SANDUICHE_MEDIO:
            case SANDUICHE_ELABORADO:
                destino = CHAPA;
                break;
            case BATATA_FRITA:
                destino = PENEIRA;
                break;
            case MILK_SHAKE:
                destino = LIQUIDIFICADOR_MILK_SHAKE;
                break;
            case SUCO:
                destino = LIQUIDIFICADOR_SUCO;
                break;
            default: // Caso de segurança, não deve acontecer com a lógica acima
                free(node_item);
                continue;
        }

        // Adiciona o item à fila do equipamento correto
        adicionarListaItemPreparo(&equipamentos[destino].fila_espera, node_item);
        printf("  -> Item '");
        imprimirNomeDoItem(item_original->nome);
        printf("' (Pedido %d) enviado para a fila do equipamento %d.\n", pedido_node->pedido.id, destino);
    }
}

void processarSeparador(Locais *local_separador, Equipamento equipamentos[], ListaFuncionarios *reserva, ListaPedidos *pedidos_em_preparo, int ciclo) {
    NodeFuncionario *func_atual = local_separador->funcionario.cabeca;
    
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;

        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_sendo_separado = func_atual->funcionario.pedido_trabalhado;
            
            // Diminui o tempo restante da tarefa de separação
            pedido_sendo_separado->pedido.tempo_preparo_local -= ciclo; // Supondo ciclo de 10s

            // Se o tempo de separação acabou
            if (pedido_sendo_separado->pedido.tempo_preparo_local <= 0) {
                printf("SEPARADOR: Funcionario %d finalizou a separacao do Pedido %d.\n", func_atual->funcionario.id, pedido_sendo_separado->pedido.id);
                
                // 1. Manda os itens para as filas dos equipamentos
                distribuirItens(pedido_sendo_separado, equipamentos);

                // 2. Muda o status do pedido
                pedido_sendo_separado->pedido.status = PREPARANDO_ITENS;

                // 3. Move o pedido para a lista de acompanhamento
                adicionarListaPedidos(pedidos_em_preparo, pedido_sendo_separado);
                
                // 4. Libera o funcionário
                liberarFuncionario(func_atual, &local_separador->funcionario, reserva);
            }
        }
        func_atual = proximo_func;
    }
}

void separador(Locais *local_separador, ListaFuncionarios *reserva) {
    // Se não há pedidos na fila de espera, não há nada a fazer
    if (local_separador->fila_espera.cabeca == NULL) {
        return;
    }

    // Tenta alocar um funcionário com a habilidade de separação
    NodeFuncionario *separador_func = encontrarEAlocarFuncionario(reserva, HABILIDADE_SEPARACAO);

    // Se encontrou um funcionário e há um pedido esperando
    if (separador_func != NULL) {
        // Pega o primeiro pedido da fila de espera
        NodePedido *pedido_para_separar = removerListaPedidos_front(&local_separador->fila_espera);
        if (pedido_para_separar == NULL) {
             // Se pegou um funcionário mas o pedido sumiu, devolve o funcionário
            adicionarListaFuncionario(reserva, separador_func);
            separador_func->funcionario.status = LIVRE;
            return;
        }

        printf("SEPARADOR: Funcionario %d iniciou a separacao do Pedido %d (duracao: 30s).\n", 
               separador_func->funcionario.id, pedido_para_separar->pedido.id);

        // Define o timer de 30 segundos para a tarefa de separação
        pedido_para_separar->pedido.tempo_preparo_local = 30;

        // Associa o pedido ao funcionário e move o funcionário para a lista de ativos do local
        separador_func->funcionario.pedido_trabalhado = pedido_para_separar;
        separador_func->funcionario.local_atual = SEPARADOR;
        adicionarListaFuncionario(&local_separador->funcionario, separador_func);

        // Importante: O pedido agora está com o funcionário, não em uma lista de "sendo feitos" do local.
        // Ele só será movido para "pedidos_em_preparo" quando a separação terminar.
    }
}

void processarEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva, int ciclo) {
    for (int i = 0; i < 4; i++) { // Para cada equipamento
        for (int j = 0; j < equipamentos[i].capacidade_maxima; j++) { // Para cada slot de preparo
            
            // Verifica se há um item no slot
            if (equipamentos[i].itens_em_preparo[j].nome != NADA) {
                ItemPreparo *item = &equipamentos[i].itens_em_preparo[j];
                item->tempo_restante_preparo -= ciclo; // Decrementa o tempo [cite: 58]

                if (item->tempo_restante_preparo <= 0) {
                    // --- ITEM FICOU PRONTO! ---
                    item->status = PRONTO;
                    item->item_original->status = PRONTO; // Atualiza o status no pedido original! [cite: 13]

                    printf("EQUIPAMENTO %d: Item '", i);
                    imprimirNomeDoItem(item->nome);
                    printf("' (Pedido %d) ficou PRONTO.\n", item->pedido_pai->id);

                    // Libera o funcionário que estava no slot
                    liberarFuncionario(equipamentos[i].funcionarios_alocados[j], NULL, reserva);
                    equipamentos[i].funcionarios_alocados[j] = NULL;
                    
                    // Libera o slot do equipamento
                    item->nome = NADA; // Marca o slot como vazio
                    equipamentos[i].capacidade_usada--; // [cite: 56] (inverso)
                }
            }
        }
    }
}


void operarEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva) {
    for (int i = 0; i < 4; i++) { // Para cada equipamento
        
        // Enquanto houver capacidade E itens na fila
        while (equipamentos[i].capacidade_usada < equipamentos[i].capacidade_maxima &&
               equipamentos[i].fila_espera.cabeca != NULL) {
            
            Habilidade habilidade_necessaria;
            switch (equipamentos[i].nome) {
                case CHAPA: habilidade_necessaria = HABILIDADE_SANDUICHE; break;
                case PENEIRA: habilidade_necessaria = HABILIDADE_BATATA; break;
                case LIQUIDIFICADOR_SUCO: habilidade_necessaria = HABILIDADE_SUCO; break;
                case LIQUIDIFICADOR_MILK_SHAKE: habilidade_necessaria = HABILIDADE_BEBIDA; break;
            }

            // Tenta encontrar um funcionário livre com a habilidade certa [cite: 52]
            NodeFuncionario *operador = encontrarEAlocarFuncionario(reserva, habilidade_necessaria);

            if (operador == NULL) {
                break; // Se não há funcionário, não pode iniciar mais itens neste equipamento
            }

            // --- CONDIÇÕES ATENDIDAS: INICIAR PREPARO! --- [cite: 55]
            NodeItemPreparo *item_node = removerListaItemPreparo_front(&equipamentos[i].fila_espera);
            
            // Encontra um slot vazio no equipamento
            for (int j = 0; j < equipamentos[i].capacidade_maxima; j++) {
                if (equipamentos[i].itens_em_preparo[j].nome == NADA) {
                    // Aloca o item e o funcionário no slot
                    equipamentos[i].itens_em_preparo[j] = item_node->item;
                    equipamentos[i].funcionarios_alocados[j] = operador;
                    equipamentos[i].capacidade_usada++; // [cite: 56]
                    
                    // Atualiza o status do item no pedido original
                    item_node->item.item_original->status = EM_PREPARO; // [cite: 57]
                    
                    printf("EQUIPAMENTO %d: Funcionario %d iniciou preparo do item '", i, operador->funcionario.id);
                    imprimirNomeDoItem(item_node->item.nome);
                    printf("' (Pedido %d).\n", item_node->item.pedido_pai->id);

                    free(item_node); // Libera o nó da fila, pois o item já está no equipamento
                    break; 
                }
            }
        }
    }
}

NodeItemPreparo* removerListaItemPreparo_front(ListaItemPreparo *lista) {
    if (!lista || !lista->cabeca) return NULL;
    NodeItemPreparo *ret = lista->cabeca;
    lista->cabeca = ret->prox;
    if (lista->cabeca) lista->cabeca->ante = NULL;
    else lista->cauda = NULL;
    ret->prox = ret->ante = NULL;
    lista->quantidade--;
    return ret;
}

/*----------------- Receber pedido (interação) -----------------*/
static int next_pedido_id = 1;

int calcularPreparo(NomePedido pedido) {
    switch (pedido) {
        case BATATA_FRITA: return 190;
        case SANDUICHE_SIMPLES: return 58;
        case SANDUICHE_MEDIO: return 88;
        case SANDUICHE_ELABORADO: return 105;
        case REFRIGERANTE: return 5;
        case SUCO: return 38;
        case MILK_SHAKE: return 60;
        case NADA: return 0;
        default: return 0;
    }
}

/*
  receberPedido: cria um Pedido com até 4 itens (dois de comer, dois de beber).
  Retorna NodePedido* alocado (quem chama deve adicioná-lo à lista/heap e liberar quando for o caso).
*/
NodePedido* receberPedido(bool res_auto, int *novo_id) {
    Pedido p;
    memset(&p, 0, sizeof(Pedido));
    p.id = next_pedido_id++;
    p.status = RECEBIDO;
    p.num_itens = 0;
    p.tempo_chegada = (int) time(NULL);

    printf("\nEscolha ate 4 itens (2 de comer e 2 de beber)\n");

    for (int bloco = 0; bloco < 2; ++bloco) {
        // itens de comer
        printf("\n==== Itens de Comer ====\n");
        printf(" 1 - Sanduiche Simples\n 2 - Sanduiche Medio\n 3 - Sanduiche Elaborado\n 4 - Batata Frita\n 5 - Nao quero nada\n");
        printf("Resposta: ");
        int item_comer = receberInput(res_auto, 5, 5);
        if (item_comer == 5) {
            // nao quer nada -> NADA
            p.itens[p.num_itens].nome = NADA;
            p.itens[p.num_itens].tempo_preparo_total = 0;
            p.itens[p.num_itens].status = PRONTO;
        } else {
            NomePedido np = (NomePedido)(item_comer - 1);
            p.itens[p.num_itens].nome = np;
            p.itens[p.num_itens].tempo_preparo_total = calcularPreparo(np);
            p.itens[p.num_itens].tempo_restante_preparo = p.itens[p.num_itens].tempo_preparo_total;
            p.itens[p.num_itens].status = AGUARDANDO_PREPARADO;
        }
        p.num_itens++;

        // itens de beber
        printf("\n==== Itens de Beber ====\n");
        printf("1 - Refrigerante\n2 - Milk-Shake\n3 - Suco\n4 - Nao quero nada\n");
        printf("Resposta: ");
        int item_beber = receberInput(res_auto, 4, 4);
        if (item_beber == 4) {
            p.itens[p.num_itens].nome = NADA;
            p.itens[p.num_itens].tempo_preparo_total = 0;
            p.itens[p.num_itens].status = PRONTO;
        } else {
            NomePedido np = (NomePedido)(item_beber + 4 - 1); // offset para enums de bebidas
            p.itens[p.num_itens].nome = np;
            p.itens[p.num_itens].tempo_preparo_total = calcularPreparo(np);
            p.itens[p.num_itens].tempo_restante_preparo = p.itens[p.num_itens].tempo_preparo_total;
            p.itens[p.num_itens].status = AGUARDANDO_PREPARADO;
        }
        p.num_itens++;
    }

    // calcula tempo_preparo_total simples como soma
    int soma = 0;
    for (int i = 0; i < p.num_itens; ++i) soma += p.itens[i].tempo_preparo_total;
    p.tempo_preparo_total = soma;
    p.tempo_restante_preparo = soma; // Inicializa com o tempo total
    p.tempo_preparo_local = soma;

    NodePedido *node = criarNodePedido(p);
    if (novo_id) *novo_id = p.id;
    return node;
}

/*----------------- Recepcao (fluxo simplificado) -----------------*/
ListaFuncionarios criarListaFuncionarios() {
    ListaFuncionarios l;
    l.cabeca = l.cauda = NULL;
    l.quantidade = 0;
    return l;
}

/*
  recepcao: recebe um pedido (interativo) e adiciona à fila de espera do local.
  Essa função é propositalmente simples: coloca o pedido na fila de espera do local (ListaPedidos).
*/
void recepcao(Locais *r, bool res_auto, ListaFuncionarios *reserva) {
    // Tenta encontrar um funcionário de caixa livre na reserva
    NodeFuncionario *caixa = encontrarEAlocarFuncionario(reserva, HABILIDADE_CAIXA);

    if (caixa == NULL) {
        printf("RECEPCAO: Nenhum funcionário de caixa disponível. Pedido não pode ser feito.\n");
        return; 
    }

    printf("RECEPCAO: Funcionario %d iniciou o atendimento.\n", caixa->funcionario.id);
    
    // Move o funcionário para o local da recepção
    caixa->funcionario.local_atual = RECEPCAO;
    adicionarListaFuncionario(&r->funcionario, caixa);

    // Cria um novo pedido
    int novo_id = 0;
    NodePedido *novo_pedido = receberPedido(res_auto, &novo_id);
    if (!novo_pedido) {
        printf("Falha ao criar pedido. Devolvendo funcionario para reserva.\n");
        liberarFuncionario(caixa, &r->funcionario, reserva); // Devolve o funcionário se o pedido falhar
        return;
    }
    novo_pedido->pedido.tempo_preparo_local = 30;
    caixa->funcionario.pedido_trabalhado = novo_pedido;
    adicionarListaPedidos(&r->pedido_sendo_feitos, novo_pedido);
    
    printf("Pedido %d sendo processado na recepcao (duracao: 30s).\n", novo_id);
}

