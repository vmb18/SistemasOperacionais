#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define TAM 20

void flush_in (){ //funçao para limpar o buffer
	int ch;
	while((ch = fgetc(stdin)) != EOF && ch != '\n'){}
}

typedef struct argumentos{ //struct para passagem dos argumentos na criaçao das threads
	int tam;
	int posi;
}thread;
struct argumentos *argthread;

int *varquivo; //ponteiro para o vetor dos valores lidos do arquivo de entrada


void merge (int *varquivo, int inicio, int meio, int fim){ //merge para ordenaçao entre pares de trechos ordenados pelas threads
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


void criaHeap (int *v, int i, int f){ //criaçao da estrutura do heap
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

void heapsort (int *v, int N){ //funçao de ordenaçao que recebe os trechos do vetor especificados por cada thread
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

 
void *tfunc (void *t_arg){ //funçao das threads
	struct argumentos *t; //declaraçao de uma struct do tipo "argumentos"
	t = (thread *) t_arg;

	int *v, tam, ini, i, p;

	//recebendo os argumentos passados por cada uma das threads
	tam = t->tam;
	ini = t->posi;

	v = (int *)malloc(tam * sizeof(int)); //alocaçao do vetor menor que sera ordenado

	//copiando um determinado trecho do vetor "varquivo" para o vetor menor "v"
	p = ini;
	for(i = 0; i < tam; i++){
		v[i] = varquivo[p];
		p++;
	}

	heapsort(v, tam); //chamada da funçao de ordenaçao para o vetor "v"

	//copiando os valores ja ordenados de volta para o vetor "varquivo"
	p = ini;
	for(i = 0; i < tam; i++){
		varquivo[p] = v[i];
		p++;
	}

	free(t_arg); //liberar o espaço de memoria
	pthread_exit(NULL); //terminar a thread
}


int main (){
	int N, i, p, T, fim, ini, *v;

	//entrada de N (quantidade de valores do arquivo)
	printf("Quantidade de valores a serem ordenados : ");
	scanf("%d", &N);

	//N deve ser maior do que 99.999, como especificado na proposta do projeto
	if(N <= 99999){
		printf("\nO arquivo deve conter mais de 99.999 valores.\n");
		printf("Insira novamente a quantidade: ");
		scanf("%d", &N);
	}

	//entrada de T (quantidade de threads que serao utilizadas para ordenaçao)
	printf("Quantidade de threads : ");
	scanf("%d", &T);

	flush_in(); //funçao para limpar o buffer

	//nome do arquivo que sera utilizado para a leitura dos valores
	printf("Nome do arquivo de entrada : ");
	char ne[TAM];
	gets(ne);

	//nome do arquivo em que serao gravados os valores ja ordenados no final do codigo
	printf("Nome do arquivo de saída : ");
	char ns[TAM];
	gets(ns);
	
	//alocaçao do vetor de tamanho N para leitura do arquivo de entrada
	varquivo = (int *)malloc(N * sizeof(int));

	//abrir arquivo de entrada com os valores a serem ordenados
	FILE *entrada = fopen(ne, "r");

	//caso tenha um problema em abrir o arquivo
	if(entrada == NULL)
		printf("Erro ao abrir o arquivo\n");

	//copiar os valores lidos no arquivo para o vetor "varquivo"
	for(p = 0; p < N; p++){
		fscanf(entrada, "%d", &i);
		varquivo[p] = i;
	}

	//fechar arquivo de entrada
	fclose(entrada);

	//declaraçao de T threads
	pthread_t meusthreads[T];

	int vpi[T], vpf[T], aux, tamv, cont;

	cont = 0; //variavel auxiliar para o calculo das posiçoes do vetor "varquivo" para cada thread. Determina em qual thread esta
	//calculos para saber os limites em que cada thread ira atuar no vetor "varquivo"
	for(p = 0; p < T; p++){
		cont++;
		fim = cont * (N/T) - 1;
		ini = fim - (N/T) + 1;
		vpi[p] = ini; //vpi guarda a posiçao inicial em que a thread ira atuar no vetor principal
		vpf[p] = fim; //vpf guarda a posiçao final em que a thread ira atuar no vetor principal
	}

	/*reorganiza as posiçoes de atuaçao das threads caso a divisao dos valores entre as threads possua resto. se o resto for 3,
	por exemplo, o primeiro, segundo e terceiro trechos irao possuir tamanho maiores para receber um valor a mais*/
	if( ((N - 1) - T * (N/T) - 1) != 0 ){ 
		cont = 0;
		for(p = 0; p < N%T; p++){
			vpf[cont] += 1; //aumenta em uma unidade a posiçao final do determinado trecho
			aux = cont + 1;
			while(aux <= T){
				vpi[aux] += 1; //posiçao inicial e final dos trechos seguintes devem ser deslocados uma unidade cada
				vpf[aux] += 1;
				aux++;
			}
		cont++;
		}
	}

	//for para a criaçao das threads
	for(p = 0; p < T; p++){
		argthread = (thread *)malloc(sizeof(thread)); //alocaçao de struct do tipo "argumentos"

		tamv = vpf[p] + 1 - vpi[p]; //tamanho do vetor "v" que a thread ira ordenar

		//argumentos das threads
		argthread->tam = tamv;
		argthread->posi = vpi[p];

		pthread_create(&meusthreads[p], NULL, tfunc, (void *)argthread); //criaçao de thread
	}

	//pthread_join possui o papel de semaforo. espera todas as threads terminarem para continuar a execuçao principal
	for(p = 0; p < T; p++){
		pthread_join(meusthreads[p], NULL);
	}

	/* chamadas do merge para a ordenaçao em pares dos trechos do vetores
	Cada vpi e vpf indicam os limites de ordenaçao executado pelas threads */
	
	/* a chamada do merge deve ter como parametros a posiçao inicial do primeiro trecho,
	a posiçao final do ultimo trecho, e o meio */

	/* como a funçao do merge que escolhemos usava como meio: meio = floor(posiçao inicial + posicao final)/2 ,
	entao devemos passar o valor da posiçao final do primeiro trecho do par, 
	ja que floor eh a funçao de arredondamento para baixo de um valor */

	//essa primeira chamada do merge sera feita para qualquer quantidade de threads (2, 4, 8 ou 16) 
	merge(varquivo, vpi[0], vpf[0], vpf[1]); //primeiro e segundo trecho

	if(T == 4){ //para 4 threads
		merge(varquivo, vpi[2], vpf[2], vpf[3]); //terceiro e quarto trechos
		merge(varquivo, vpi[0], vpf[1], vpf[3]); //junçao do primeiro e segundo merge
	}

	if(T == 8){ //para 8 threads
		merge(varquivo, vpi[2], vpf[2], vpf[3]); //terceiro e quarto trechos
		merge(varquivo, vpi[4], vpf[4], vpf[5]); //quinto e sexto trechos
		merge(varquivo, vpi[6], vpf[6], vpf[7]); //setimo e oitavo trechos
		merge(varquivo, vpi[0], vpf[1], vpf[3]); //junçao do primeiro e segundo merge
		merge(varquivo, vpi[4], vpf[5], vpf[7]); //junçao do terceiro e quarto merge
		merge(varquivo, vpi[0], vpf[3], vpf[7]); //junçao do quinto e sexto merge
	}

	if(T == 16){ //para 16 threads
		merge(varquivo, vpi[2], vpf[2], vpf[3]); //terceiro e quarto trechos
		merge(varquivo, vpi[4], vpf[4], vpf[5]); //quinto e sexto trechos
		merge(varquivo, vpi[6], vpf[6], vpf[7]); //setimo e oitavo trechos
		merge(varquivo, vpi[8], vpf[8], vpf[9]); //nono e decimo trechos
		merge(varquivo, vpi[10], vpf[10], vpf[11]); //decimo primeiro e decimo segundo trecho
		merge(varquivo, vpi[12], vpf[12], vpf[13]); //decimo terceiro e decimo quarto trechos
		merge(varquivo, vpi[14], vpf[14], vpf[15]); //decimo quinto e decimo sexto trechos
		merge(varquivo, vpi[0], vpf[1], vpf[3]); //junçao do primeiro e segundo merge
		merge(varquivo, vpi[4], vpf[5], vpf[7]); //junçao do terceiro e quarto merge
		merge(varquivo, vpi[8], vpf[9], vpf[11]); //junçao do quinto e sexto merge
		merge(varquivo, vpi[12], vpf[13], vpf[15]); //junçao do setimo e oitavo merge
		merge(varquivo, vpi[0], vpf[3], vpf[7]); //junçao do nono e decimo merge
		merge(varquivo, vpi[8], vpf[11], vpf[15]); //junçao do decimo primeiro e decimo segundo merge
		merge(varquivo, vpi[0], vpf[7], vpf[15]); //junçao do decimo terceiro e decimo quarto merge
	}

	//abrir arquivo de saida
	FILE *saida = fopen(ns, "w");

	//grava o vetor "varquivo" ja ordenado no arquivo de saida
	for(p = 0; p < N; p++){
		fprintf(saida, "%d ", varquivo[p]);
	}

	//fecha o arquivo de saida
	fclose(saida);

return 0;
}
