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

    }else if( compara((*parv)->item, item) > 0){
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

void imprime_no(tnode *no) {
    while (no != NULL) {
        printf("Item: %d\n", no->item);
        no = no->proximo;
    }
}

void imprime_avl(tnode *arv) {
    if (arv == NULL) {
        return;
    }

    // Percorre a subárvore esquerda
    imprime_avl(arv->esq);

    // Imprime o nó atual
    printf("Item: %d\n", arv->item);

    // Percorre a lista encadeada de chaves iguais
    tnode *temp = arv->proximo;
    while (temp != NULL) {
        printf("    -> Duplicado: %d\n", temp->item);
        temp = temp->proximo;
    }



    // Percorre a subárvore direita
    imprime_avl(arv->dir);
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

// Função para ler e inserir cidades de um arquivo JSON nas AVLs
void inserir_cidades_em_avls(const char *nome_arquivo) {
    // Abrir o arquivo JSON e ler seu conteúdo
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo JSON.\n");
        return;
    }

    // Obter o tamanho do arquivo para alocar o buffer
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);

    // Alocar um buffer para armazenar o conteúdo do arquivo JSON
    char *buffer = (char *)malloc(tamanho + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o buffer.\n");
        fclose(arquivo);
        return;
    }

    // Ler o conteúdo do arquivo para o buffer
    fread(buffer, 1, tamanho, arquivo);
    fclose(arquivo);
    buffer[tamanho] = '\0'; // Garantir terminação nula

    // Parse do conteúdo JSON
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        const char *erro = cJSON_GetErrorPtr();
        if (erro != NULL) {
            fprintf(stderr, "Erro no parse JSON: %s\n", erro);
        }
        free(buffer);
        return;
    }

    // Suponha que o JSON seja um array de objetos de cidades
    int num_cidades = cJSON_GetArraySize(root);
    for (int i = 0; i < num_cidades; ++i) {
        cJSON *cidade_json = cJSON_GetArrayItem(root, i);
        if (cidade_json == NULL) {
            continue; // Pular se não for um objeto válido
        }

        // Extrair os campos do objeto JSON
        titem cidade;
        cJSON *nome = cJSON_GetObjectItemCaseSensitive(cidade_json, "nome");
        cJSON *latitude = cJSON_GetObjectItemCaseSensitive(cidade_json, "latitude");
        cJSON *longitude = cJSON_GetObjectItemCaseSensitive(cidade_json, "longitude");
        cJSON *codigo_uf = cJSON_GetObjectItemCaseSensitive(cidade_json, "codigo_uf");
        cJSON *ddd = cJSON_GetObjectItemCaseSensitive(cidade_json, "ddd");
        cJSON *codigo_ibge = cJSON_GetObjectItemCaseSensitive(cidade_json, "codigo_ibge");

        if (cJSON_IsString(nome) && cJSON_IsNumber(latitude) && cJSON_IsNumber(longitude) &&
            cJSON_IsNumber(codigo_uf) && cJSON_IsNumber(ddd) && cJSON_IsNumber(codigo_ibge)) {
            strcpy(cidade.nome, nome->valuestring);
            cidade.latitude = latitude->valuedouble;
            cidade.longitude = longitude->valuedouble;
            cidade.codigo_uf = codigo_uf->valueint;
            cidade.ddd = ddd->valueint;
            cidade.codigo_ibge = codigo_ibge->valueint;

            // Inserir nas AVLs correspondentes
            avl_insere(&avl_nome, cidade, compara_nome);
            avl_insere(&avl_latitude, cidade, compara_latitude);
            avl_insere(&avl_longitude, cidade, compara_longitude);
            avl_insere(&avl_codigo_uf, cidade, compara_codigo_uf);
            avl_insere(&avl_ddd, cidade, compara_ddd);
        }
    }

    // Liberar recursos
    cJSON_Delete(root);
    free(buffer);
}


int main(){

    


    /*
    titem item;

    strcpy(item.nome, "Campo Grande");
    item.codigo_ibge = 12345;
    avl_insere(&avl_nome, item, compara_nome);

    strcpy(item.nome, "São Paulo");
    item.codigo_ibge = 123456;
    avl_insere(&avl_nome, item, compara_nome);


    tnode *no = sucessor(avl_nome);

    titem item2;

    strcpy(item2.nome, no->item.nome);

    printf("CODIGO IBGE SUCESSOR: %d\n", no->item.codigo_ibge);
    */
  

    inserir_cidades_em_avls("municipios.json");
    

    return 0;
}