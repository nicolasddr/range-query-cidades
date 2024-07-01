#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "cJSON.c"
#include "cJSON.h"

tnode *avl_nome = NULL;
tnode *avl_latitude = NULL;
tnode *avl_longitude = NULL;
tnode *avl_codigo_uf = NULL;
tnode *avl_ddd = NULL;

int max(int a,int b){
    return a>b?a:b;
}

int altura(tnode *arv){
    int ret;
    if (arv==NULL){
        ret = -1;
    }else{
        ret = arv->h;
    }
    return ret;
}

void avl_insere(tnode ** parv,titem item, int (*compara)(titem, titem)){
    if (*parv == NULL){
        *parv = (tnode *) malloc(sizeof(tnode));
        (*parv)->item = item;
        (*parv)->esq = NULL;
        (*parv)->dir = NULL;
        (*parv)->pai = NULL;
        (*parv)->proximo = NULL;
        (*parv)->h = 0;

    }else if(compara((*parv)->item, item) > 0){
        avl_insere(&(*parv)->esq, item, compara);
        if((*parv)->esq != NULL){
            (*parv)->esq->pai = *parv; //Adiciona o pai do novo nó
        }
    }else if(compara((*parv)->item, item) < 0){
        avl_insere(&(*parv)->dir, item, compara);
        if((*parv)->dir != NULL){
            (*parv)->dir->pai = *parv; //Adiciona o pai do novo nó
        }
    } else { //Se as chaves forem iguais
        //Se a chave já existe, adiciona a lista encadeada
        tnode *novo = (tnode *) malloc(sizeof(tnode));
        novo->item = item;
        novo->esq = NULL;
        novo->dir = NULL;
        novo->pai = NULL; //Nó está na lista, por isso não tem pai
        novo->h = (*parv)->h;
        novo->proximo = (*parv)->proximo; //Proximo do novo nó aponta para o elemento que já estava na lista ou para NULL
        (*parv)->proximo = novo; //Novo entra no início da lista encadeada
    }
    (*parv)->h = max(altura((*parv)->esq),altura((*parv)->dir)) + 1;
    _avl_rebalancear(parv);
}
void _rd(tnode **parv){
    tnode * y = *parv; 
    tnode * x = y->esq;
    tnode * A = x->esq;
    tnode * B = x->dir;
    tnode * C = y->dir;

    y->esq = B; 
    x->dir = y;
    *parv  = x;

    //Atualização do ponteiro para o pai
    
    x->pai = y->pai;
    y->pai = x;

    if(B != NULL){
        B->pai = y;
    }

    y->h = max(altura(B),altura(C)) + 1;
    x->h = max(altura(A),altura(y)) + 1;
}
void _re(tnode **parv){
    tnode * x = *parv; 
    tnode * y = x->dir;
    tnode * A = x->esq;
    tnode * B = y->esq;
    tnode * C = y->dir;

    x->dir = B;
    y->esq = x; 
    *parv  = y;

    //Atualização do ponteiro para o pai
    
    y->pai = x->pai;
    x->pai = y;

    if(B != NULL){
        B->pai = x;
    }

    x->h = max(altura(A),altura(B)) + 1;
    y->h = max(altura(x),altura(C)) + 1;
}
void _avl_rebalancear(tnode **parv){
    int fb;
    int fbf;
    tnode * filho;
    fb = altura((*parv)->esq) - altura((*parv)->dir);

    if (fb  == -2){
        filho = (*parv)->dir;
        fbf = altura(filho->esq) - altura(filho->dir);
        if (fbf <= 0){ /* Caso 1  --> ->*/
            _re(parv);
        }else{   /* Caso 2  --> <-*/
            _rd(&(*parv)->dir);
            _re(parv);
        }
    }else if (fb == 2){  
        filho = (*parv)->esq;
        fbf = altura(filho->esq) - altura(filho->dir);
        if (fbf >=0){ /* Caso 3  <-- <-*/
            _rd(parv);
        }else{  /* Caso 4  <-- ->*/
            _re(&(*parv)->esq);
            _rd(parv);
        }
    }
}

tnode ** percorre_esq(tnode **arv){

    tnode * aux = *arv;

    //Se há um nó próximo na lista encadeada, retorne ele
    if(aux->proximo != NULL){
        return &(aux->proximo); 
    }

    //Se não há sub árvore a esquerda, retorne o próprio nó
    if (aux->esq  == NULL){
        return arv;
    }else{
        while (aux->esq->esq != NULL)
            aux = aux->esq;
        return &(aux->esq);
    }
}

void avl_remove(tnode **parv, titem reg, int (*compara)(titem, titem)){
    int cmp;
    tnode *aux;
    tnode **sucessor;
    if (*parv != NULL){
        cmp  = compara((*parv)->item, reg);
        if (cmp > 0){ /* ir esquerda*/
            avl_remove(&((*parv)->esq), reg, compara);
        }else if (cmp < 0){ /*ir direita*/
            avl_remove(&((*parv)->dir), reg, compara);
        }else{ /* ACHOU  */

            //Se o nó tem elemento proximo
            if((*parv)->proximo != NULL){
                //Remove o primeiro elemento da lista encadeada
                tnode *remover = (*parv)->proximo; //Copia o primeiro elemento para o nó remover
                (*parv)->proximo = remover->proximo; //Segundo elemento da lista(proximo do primeiro) passa a ser o primeiro
                free(remover); //Exclui o nó

            } else{ //Se não tem próximo, remove normalmente
                if ((*parv)->esq == NULL && (*parv)->dir == NULL){   /* no folha */
                    free(*parv);
                    *parv = NULL;
                }else if ((*parv)->esq == NULL || (*parv)->dir == NULL){ /* tem um filho*/
                    aux = *parv;
                    if ((*parv)->esq == NULL){
                        *parv = (*parv)->dir;
                    }else{
                        *parv = (*parv)->esq;
                    }
                    (*parv)->pai = aux->pai; //Atualiza o pai do filho para o pai do nó removido
                    free(aux);
                }else{ /* tem dois filhos */
                    sucessor = percorre_esq(&(*parv)->dir);
                    (*parv)->item = (*sucessor)->item;
                    avl_remove(&(*parv)->dir,(*sucessor)->item, compara);
                }
            }


        }

        if (*parv != NULL){
            (*parv)->h = max(altura((*parv)->esq),altura((*parv)->dir)) + 1;
            _avl_rebalancear(parv);
        }
    }
}
void avl_destroi(tnode *parv){
    if (parv!=NULL){
        avl_destroi(parv->esq);
        avl_destroi(parv->dir);
        avl_destroi(parv->pai); //Destruir pai
        free(parv);
    }
}


tnode * encontra_minimo(tnode *arv){
    tnode *aux = arv;
    while(aux->esq != NULL){
        aux = aux->esq;
    }

    return aux;
}

tnode * sucessor(tnode *no){
    tnode *aux = no;

    if(aux == NULL){
        return NULL;
    }

    //Se existe subarvore a direita
    if(aux->dir != NULL){
        aux = encontra_minimo(aux->dir); //Sucessor é o menor elemento dessa subarvore
        //printf("SUCESSOR: %d", aux->item);

        return aux;
    }

    //Se não existe subarvore a direita
    tnode *pai = aux->pai;
    while(pai != NULL && pai->dir == aux){
        aux = pai;
        pai = aux->pai;
    }

    //printf("SUCESSOR: %d", pai->item);
    return pai;

}

int compara_nome(titem a, titem b) {
    return strcmp(a.nome, b.nome);
}

int compara_latitude(titem a, titem b) {
    if (a.latitude < b.latitude) return -1;
    if (a.latitude > b.latitude) return 1;
    return 0;
}

int compara_longitude(titem a, titem b) {
    if (a.longitude < b.longitude) return -1;
    if (a.longitude > b.longitude) return 1;
    return 0;
}

int compara_codigo_uf(titem a, titem b) {
    return a.codigo_uf - b.codigo_uf;
}

int compara_ddd(titem a, titem b) {
    return a.ddd - b.ddd;
}


void imprimir_campo_no(tnode *parv, int campo){
    if (campo == 0) {//Nome
        printf("Nome: %s, Codigo IBGE: %d\n", parv->item.nome, parv->item.codigo_ibge);
    } else if (campo == 1) {//Latitude
       printf("Latitude: %lf, Codigo IBGE: %d\n", parv->item.latitude, parv->item.codigo_ibge);
    } else if (campo == 2) {//Longitude
        printf("Longitude: %lf, Codigo IBGE: %d\n", parv->item.longitude, parv->item.codigo_ibge);
    } else if (campo == 3) {//Código UF
        printf("Codigo UF: %d, Codigo IBGE: %d\n", parv->item.codigo_uf, parv->item.codigo_ibge);
    } else if (campo == 4) {//DD
       printf("DDD: %d, Codigo IBGE: %d\n", parv->item.ddd, parv->item.codigo_ibge);
    }
}

void imprimir_resultados(tnode *lista, int campo) {

    if (lista == NULL) {
        return;
    }

    tnode *atual = lista;
    while (atual != NULL) {
        imprimir_campo_no(atual, campo);
        atual = atual->proximo; // Supondo que 'esq' aponta para o próximo elemento na lista
    }
}

//Imprime todos os nós de uma subarvore
void imprimir_arv(tnode *lista, int campo) {

    if (lista == NULL) {
        return;
    }

    tnode *atual = lista;
    while (atual != NULL) {
        imprimir_campo_no(atual, campo);    
        atual = atual->proximo; // Supondo que 'esq' aponta para o próximo elemento na lista
    }
    imprimir_arv(lista->esq, campo);
    imprimir_arv(lista->dir, campo);

}

void carregar_dados(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(fsize + 1);
    fread(data, 1, fsize, file);
    fclose(file);

    data[fsize] = 0;

    cJSON *json = cJSON_Parse(data);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Erro antes do: %s\n", error_ptr);
        }
        free(data);
        exit(EXIT_FAILURE);
    }

    int count = cJSON_GetArraySize(json);
    for (int i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        
        titem reg;
        strcpy(reg.nome, cJSON_GetObjectItem(item, "nome")->valuestring);
        reg.latitude = (float)cJSON_GetObjectItem(item, "latitude")->valuedouble;
        reg.longitude = (float)cJSON_GetObjectItem(item, "longitude")->valuedouble;
        reg.codigo_uf = cJSON_GetObjectItem(item, "codigo_uf")->valueint;
        reg.ddd = cJSON_GetObjectItem(item, "ddd")->valueint;
        reg.codigo_ibge = cJSON_GetObjectItem(item, "codigo_ibge")->valueint;

        avl_insere(&avl_nome, reg, compara_nome);
        avl_insere(&avl_latitude, reg, compara_latitude);
        avl_insere(&avl_longitude, reg, compara_longitude);
        avl_insere(&avl_codigo_uf, reg, compara_codigo_uf);
        avl_insere(&avl_ddd, reg, compara_ddd);
    }

    cJSON_Delete(json);
    free(data);
}

//Função de busca comum
tnode *busca_avl(tnode *parv, titem item, int (*compara)(titem, titem)){
    if(parv == NULL){
        return NULL;
    }
    
    int cmp = compara(parv->item, item);

    //Item buscado menor que no
    if(cmp > 0){
        return busca_avl(parv->esq, item, compara); //Busca na subarvore da esquerda
    }else if(cmp < 0){ //Item maior que o no
        return busca_avl(parv->dir, item, compara); //Busca na subarvore da direita
    } else {
        return parv;
    }   

}

/*
tnode * busca_avl_range(int menor_maior, tnode *parv, titem item, int (*compara)(titem, titem), tnode *lista){
    if(parv == NULL){
        return NULL;
    }

    tnode *aux = parv;

    //Lista aponta para o primeiro nó como o começo da lista, os próximos vão ser adicionados na lista proximo do tnode dele
    if(lista == NULL){ //Se a lista estiver vazia
        lista = parv;
    } else {
        aux->proximo = lista->proximo; //Proximo do novo nó aponta para o elemento que já estava no próximo da lista
        lista->proximo = aux; //Novo entra no início da lista encadeada
    }
    
    //Percorrer toda a arvore
    if(menor_maior == 0){
        busca_avl_range(2, parv->esq, item, compara, lista); //Percorre toda a subarvore da esquerda recursivamente

    } else if(menor_maior == 1){
        busca_avl_range(2, parv->dir, item, compara, lista); //Percorre toda a subarvore da direita recursivamente

    } else {
        busca_avl_range(2, parv->esq, item, compara, lista); //Percorre toda a subarvore da esquerda recursivamente
        busca_avl_range(2, parv->dir, item, compara, lista); //Percorre toda a subarvore da direita recursivamente

    }
    
    return lista;
}
tnode * item_comp(tnode *parv, int tipo_comparacao, int item_comparado, titem item_comparacao){
    tnode *lista = NULL;


    //Faz busca comparando diferentes dados do titem dependendo da query
    switch (item_comparado)
    {
    case 0: // 0 => nome
        lista = range(parv, tipo_comparacao, item_comparado, item_comparacao, lista, compara_nome);
        break;
    case 1: // 1 => latitude
        lista = range(parv, tipo_comparacao, item_comparado, item_comparacao, lista, compara_latitude);
        break;
    case 2: // 2 => longitude
        lista = range(parv, tipo_comparacao, item_comparado, item_comparacao, lista, compara_longitude);
        break;
    case 3: // 3 => codigo_uf
        lista = range(parv, tipo_comparacao, item_comparado, item_comparacao, lista, compara_codigo_uf);
        break;
    case 4: // 4 => ddd
        lista = range(parv, tipo_comparacao, item_comparado, item_comparacao, lista, compara_ddd);
        break;
    default:
        break;
    }

    return lista;
}
tnode * range(tnode *parv, int tipo_comparacao, int item_comparado, titem item_comparacao, tnode *lista, int (*compara)(titem, titem)){

    //Encontra nó da query
    tnode *no_query = busca_avl(parv, item_comparacao, compara);

    if (no_query == NULL) {
        return lista; // Retorna a lista atual se o nó não for encontrado
    }

    //Busca todos os nós < ou > ou == da subarvore do no da query
    switch (tipo_comparacao)
    {
    case 0: // 0 -> expressão de <, busca na subarvore da esquerda
        lista = busca_avl_range(0, no_query->esq, item_comparacao, compara, lista);
        break;
    case 1: // 1 -> expressão de >, busca na subarvore da direita
        lista = busca_avl_range(1, no_query->dir, item_comparacao, compara, lista);
        break;
    case 2: // 2 -> expressão de =, retorna elemento
        lista = no_query;
        break;
    default:
        break;
    }

    //Retorna lista de elementos da query
    return lista;


}
tnode* busca_todos(tnode* parv, tnode* lista, int (*compara)(titem, titem)) {
    if (parv == NULL) {
        return lista;
    }

    // Busca na subárvore da direita (maiores)
    lista = busca_todos(parv->dir, lista, compara);

    // Adiciona o nó atual à lista
    tnode* novo_no = (tnode*) malloc(sizeof(tnode));
    *novo_no = *parv; // Copia os dados do nó atual para o novo nó
    novo_no->proximo = lista;
    lista = novo_no;

    // Busca na subárvore da esquerda (menores)
    lista = busca_todos(parv->esq, lista, compara);

    return lista;
}
*/


void seleciona_busca(tnode *parv, int comparacao, titem criterio, int (*compara)(titem, titem), int campo){
    tnode *resultados = NULL;
    tnode *no = NULL;
    
    if(comparacao == 0){
        //Imprime todos os nós menores
        no = busca_avl(parv, criterio, compara);
        imprimir_arv(no->esq, campo);
    } else if(comparacao == 1){
        //Imprime todos os nós maiores
        no = busca_avl(parv, criterio, compara);
        imprimir_arv(no->dir, campo);
    } else if(comparacao == 2){
        //Imprime todos os nós iguais ao dado comparado
        resultados = busca_avl(parv, criterio, compara);
        imprimir_resultados(resultados, campo);
    }
}



void buscar_por_criterios() {
    int campo;
    int comparacao;
    titem criterio;
    char nome[50];
    
    tnode *parv = NULL;
    

    printf("Escolha o campo para a busca:\n");
    printf("0 - Nome\n");
    printf("1 - Latitude\n");
    printf("2 - Longitude\n");
    printf("3 - Codigo UF\n");
    printf("4 - DDD\n");
    scanf("%d", &campo);
    
    printf("Escolha o tipo de comparacao:\n");
    printf("0 - Menor que (<)\n");
    printf("1 - Maior que (>)\n");
    printf("2 - Igual a (=)\n");
    scanf("%d", &comparacao);

    tnode *resultados = NULL;

    if (campo == 0) {
        printf("Digite o nome: ");
        getchar();
        fgets(nome, sizeof(nome), stdin);
        nome[strcspn(nome, "\n")] = 0; // Remover o caractere de nova linha do final da string
        strcpy(criterio.nome, nome);

        seleciona_busca(avl_nome, comparacao, criterio, compara_nome, campo);
    } else if (campo == 1) {
        printf("Digite a latitude: ");
        scanf("%f", &criterio.latitude);
        seleciona_busca(avl_latitude, comparacao, criterio, compara_latitude, campo);
    } else if (campo == 2) {
        printf("Digite a longitude: ");
        scanf("%f", &criterio.longitude);
        seleciona_busca(avl_longitude, comparacao, criterio, compara_longitude, campo);
    } else if (campo == 3) {
        printf("Digite o codigo UF: ");
        scanf("%d", &criterio.codigo_uf);
        seleciona_busca(avl_codigo_uf, comparacao, criterio, compara_codigo_uf, campo);
    } else if (campo == 4) {
        printf("Digite o DDD: ");
        scanf("%d", &criterio.ddd);
        seleciona_busca(avl_ddd, comparacao, criterio, compara_ddd, campo);
    }

}

int main(){

    //inserir_cidades_em_avls("municipios.json");
    carregar_dados("municipios.json");

    buscar_por_criterios();


    return 0;
}