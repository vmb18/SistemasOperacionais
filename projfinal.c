#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#define TAM 20


void flush_in (){
	int ch;
	while((ch = fgetc(stdin)) != EOF && ch != '\n'){}
}

typedef struct argumentos{
	int *vetor;
	int tam;
	int posi;
}thread;

struct argumentos *argthread;

int *varquivo;


void merge (int *varquivo, int inicio, int meio, int fim){
	int *temp, p1, p2, tamanho, i, j, k;
	int fim1 = 0, fim2 = 0;
	tamanho = fim-inicio+1;
	p1 = inicio;
	p2 = meio+1;
	temp = (int *) malloc(tamanho*sizeof(int));
	if (temp != NULL){
		for(i = 0; i < tamanho; i++){
			if(!fim1 && !fim2){
				if(varquivo[p1] < varquivo[p2])
					temp[i] = varquivo[p1++];
				else
					temp[i] = varquivo[p2++];
				if(p1 > meio)
					fim1 = 1;
				if(p2 > fim)
					fim2 = 1;
			}
			else{
				if(!fim1)
					temp[i] = varquivo[p1++];
				else
					temp[i] = varquivo[p2++];
			}
		}
		for(j = 0, k = inicio; j < tamanho; j++, k++)
			varquivo[k] = temp[j];
	}
	free(temp);
}


void criaHeap (int *v, int i, int f){ //funçao para criar o heap (estrutura do heapsort)
	int aux = v[i];
	int j = i*2 + 1;
	while(j <= f){
		if(j < f){
			if(v[j] < v[j+1])
				j += 1;
		}
		if(aux < v[j]){
			v[i] = v[j];
			i = j;
			j = 2*i + 1;
		}
		else{
			j = f + 1;
		}
	}
	v[i] = aux;
}


void heapsort (int *v, int N){ //funçao para a ordenaçao dos valores
	int i, aux;
	for(i = (N-1)/2; i >= 0; i--){
		criaHeap(v, i, N-1);
	}
	for(i = N-1; i >= 1; i--){
		aux = v[0];
		v[0] = v[i];
		v[i] = aux;
		criaHeap(v, 0, i-1);
	}
}

 
void *tfunc (void *t_arg){
	struct argumentos *t;
	t = (thread *) t_arg;

	int *v, tam, ini, i, p;

	tam = t->tam;
	ini = t->posi;

	v = (int *)malloc(tam * sizeof(int));

	p = ini;

	for(i = 0; i < tam; i++){
		v[i] = varquivo[p];
		p++;
	}

	heapsort(v, tam);

	p = ini;

	for(i = 0; i < tam; i++){
		varquivo[p] = v[i];
		p++;
	}

	free(t_arg);
	pthread_exit(NULL);
}


int main (){
	int N, i, p, T, fim, ini, *v;

	printf("Quantidade de valores a serem ordenados : ");
	scanf("%d", &N);

	printf("Quantidade de threads : ");
	scanf("%d", &T);

	flush_in(); //limpar buffer

	printf("Nome do arquivo de entrada : ");
	char ne[TAM];
	gets(ne);

	printf("Nome do arquivo de saída : ");
	char ns[TAM];
	gets(ns);
	
	varquivo = (int *)malloc(N * sizeof(int));	

	FILE *entrada = fopen(ne, "r");
	if(entrada == NULL)
		printf("Erro ao abrir o arquivo\n");
	for(p = 0; p < N; p++){
		fscanf(entrada, "%d", &i);
		varquivo[p] = i;
	}
	fclose(entrada);

	pthread_t meusthreads[T];

	int vpi[T], vpf[T], aux, tamv, cont;

	cont = 0;
	for(p = 0; p < T; p++){
		cont++;
		fim = cont * (N/T) - 1;
		ini = fim - (N/T) + 1;
		vpi[p] = ini;
		vpf[p] = fim;
	}

	if( ((N - 1) - T * (N/T) - 1) != 0 ){ //se ainda houverem valores nao considerados p/posiçao final
		cont = 0;
		for(p = 0; p < N%T; p++){
			vpf[cont] += 1;
			aux = cont + 1;
			while(aux <= T){
				vpi[aux] += 1;
				vpf[aux] += 1;
				aux++;
			}
		cont++;
		}
	}

	for(p = 0; p < T; p++){
		argthread = (thread *)malloc(sizeof(thread));

		tamv = vpf[p] + 1 - vpi[p];

		argthread->tam = tamv;
		argthread->posi = vpi[p];

		pthread_create(&meusthreads[p], NULL, tfunc, (void *)argthread);
	}

	for(p = 0; p < T; p++){
		pthread_join(meusthreads[p], NULL);
	}

	if(T == 2){
 		merge(varquivo, vpi[0], vpf[0], vpf[1]);
	}
	if(T == 4){
		merge(varquivo, vpi[0], vpf[0], vpf[1]);
		merge(varquivo, vpi[2], vpf[2], vpf[3]);
		merge(varquivo, vpi[0], vpf[1], vpf[3]);
	}

	if(T == 8){
		merge(varquivo, vpi[0], vpf[0], vpf[1]);
		merge(varquivo, vpi[2], vpf[2], vpf[3]);
		merge(varquivo, vpi[4], vpf[4], vpf[5]);
		merge(varquivo, vpi[6], vpf[6], vpf[7]);
		merge(varquivo, vpi[0], vpf[1], vpf[3]);
		merge(varquivo, vpi[4], vpf[5], vpf[7]);
		merge(varquivo, vpi[0], vpf[3], vpf[7]);
	}

	if(T == 16){
		merge(varquivo, vpi[0], vpf[0], vpf[1]);
		merge(varquivo, vpi[2], vpf[2], vpf[3]);
		merge(varquivo, vpi[4], vpf[4], vpf[5]);
		merge(varquivo, vpi[6], vpf[6], vpf[7]);
		merge(varquivo, vpi[8], vpf[8], vpf[9]);
		merge(varquivo, vpi[10], vpf[10], vpf[11]);
		merge(varquivo, vpi[12], vpf[12], vpf[13]);
		merge(varquivo, vpi[14], vpf[14], vpf[15]);
		merge(varquivo, vpi[0], vpf[1], vpf[3]);
		merge(varquivo, vpi[4], vpf[5], vpf[7]);
		merge(varquivo, vpi[8], vpf[9], vpf[11]);
		merge(varquivo, vpi[12], vpf[13], vpf[15]);
		merge(varquivo, vpi[0], vpf[3], vpf[7]);
		merge(varquivo, vpi[8], vpf[11], vpf[15]);
		merge(varquivo, vpi[0], vpf[7], vpf[15]);
	}

	FILE *saida = fopen(ns, "w");
	for(p = 0; p < N; p++){
		fprintf(saida, "%d ", varquivo[p]);
	}
	fclose(saida);

return 0;
}
