#ifndef FUNCOES_H
#define FUNCOES_H

#include "structs.h" // Inclui as definições das structs

// --- Protótipos das Funções ---
void sair();
int criarHeap(Heap *heap, int capacidade);
void liberarHeap(Heap* heap);
void swap(Pedido *a, Pedido *b);
void subirNoHeap(Heap* heap, int indice);
void inserirPedido(Heap* heap, Pedido novoPedido);
void descerNoHeap(Heap* heap, int indice);
int removerItemPrioritario(Heap* heap, Pedido* pedidoRemovido);
void imprimirHeap(Heap* heap);
#endif
