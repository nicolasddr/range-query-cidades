#ifndef __AVL__
#define  __AVL__

typedef struct {
    char nome[100];
    float latitude;
    float longitude;
    int codigo_uf;
    int ddd;
    int codigo_ibge;
} titem;


typedef struct _node{
    titem item;
    struct _node *esq;
    struct _node *dir;
    struct _node *pai; //Adição do ponteiro para o pai
    struct _node *proximo; //Ponteiro para lista encadeada para armzenar registros com chaves iguais
    int h;
}tnode;

void avl_insere(tnode ** parv,titem reg, int (*compara)(titem, titem));
void avl_remove(tnode ** parv,titem reg, int (*compara)(titem, titem));
void avl_destroi(tnode * parv);

void _rd(tnode ** pparv);
void _re(tnode ** pparv);
void _avl_rebalancear(tnode ** pparv);

int compara_nome(titem a, titem b);
int compara_latitude(titem a, titem b);
int compara_longitude(titem a, titem b);
int compara_codigo_uf(titem a, titem b);
int compara_ddd(titem a, titem b);

tnode ** range(tnode **parv, int tipo_comparacao, int item_comparado, titem item_comparacao, tnode **lista, int (*compara)(titem, titem));

#endif