#include <stdio.h>
#include "enums.h"

// --- Estruturas dos Pedidos ---

// Representa um item individual (ex: um sanduiche, uma batata)
typedef struct{
    NomePedido nome;
    int tempo_preparo_total;
    int tempo_restante_preparo;
    StatusItem status;
} ItemPedido;

// Representa um pedido completo de um cliente
typedef struct{
    ItemPedido itens[4]; // Um pedido tem no máximo 4 itens (2 de comer, 2 de beber)
    int num_itens;
    int tempo_chegada; // Guarda o "tempo_global" em que o pedido chegou
    int tempo_preparo_total; // Chave da prioridade para o Heap. Calculado na chegada.
    StatusPedido status;
} Pedido;

// --- Estruturas dos Recursos da Lanchonete ---

// Representa um funcionário e suas capacidades
typedef struct{
    int id;
    Habilidade habilidades[3]; // Um funcionario pode ter multiplas habilidades
    int num_habilidade;
    StatusFuncionario status;
    int id_pedido_atual; // Para saber qual pedido ele está trabalhando
} Funcionario; // :)^_____^O_O:-];O):O(>_<

// Representa um equipamento com capacidade limitada (chapa, fritadeira, etc)
typedef struct{
    char nome[50];
    int capacidade_maxima; // Sera definido pelas regras do nome Arthur
    int capacidade_usada;
} Equipamento; 

// --- Estrutura de Controle (Fila de Prioridades) ---

typedef struct
{
    ListaPedidos *pedidos; // Vetor dinâmico para armazenar os pedidos
    int quantidade; // Quantidade atual de pedidos na fila
    int capacidade; // Tamanho máximo do vetor
} Heap;

typedef struct{
    Pedido pedido;
    NodePedido *ante;
    NodePedido *prox;
} NodePedido;

typedef struct{
    NodePedido *cabeca;
    NodePedido *cauda;
    int quantidade;
} ListaPedidos;




