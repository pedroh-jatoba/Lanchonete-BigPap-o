#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdbool.h>
#include "structs.h"

// Utilitários
void sair();

// Input
int receberInput(bool res_auto, int opc_auto, int opc_manual);

// Heap
int criarHeap(Heap *heap, int capacidade);
void liberarHeap(Heap* heap);
void inserirPedido(Heap* heap, Pedido novoPedido);
int removerItemPrioritario(Heap* heap, Pedido* pedidoRemovido);
void imprimirHeap(Heap* heap);

// Fila de Prioridade
void verificarEPriorizarPedidos(Locais *local, int tempo_atual);
void processarFilaPrioridade(Locais *local);

// Lista de pedidos (simples)
ListaPedidos criarLista();
NodePedido* criarNodePedido(Pedido pedido);
void adicionarListaPedidos(ListaPedidos *lista, NodePedido *novo_node); // adiciona no final
void adicionarListaPedidosOrdenado(ListaPedidos *lista, NodePedido *novo_node);
NodePedido* removerListaPedidos_front(ListaPedidos *lista); // remove e retorna a cabeça (ou NULL)
void imprimirLista(ListaPedidos lista);

// Funções de impressão helpers
void imprimirPedido(Pedido pedido);
void imprimirNomeDoItem(NomePedido nome);
void imprimirStatusItem(StatusItem status);

// Recepção / criação de pedidos
NodePedido* receberPedido(bool res_auto, int *novo_id);
void recepcao(Locais *r, int ciclo, bool res_auto, ListaFuncionarios *reserva);

// Cálculo de preparo
int calcularPreparo(NomePedido pedido);

// Inicializadores simples
ListaFuncionarios criarListaFuncionarios();

//Separador
// Adicione/Atualize estes protótipos em funcoes.h
void distribuirItens(NodePedido *pedido_node, Equipamento equipamentos[]);
void processarSeparador(Locais *local_separador, Equipamento equipamentos[], ListaFuncionarios *reserva, ListaPedidos *pedidos_em_preparo);
void separador(Locais *local_separador, ListaFuncionarios *reserva);

#endif // FUNCOES_H
