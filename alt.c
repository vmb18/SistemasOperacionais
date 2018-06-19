#include "stdio.h"
#include "stdlib.h"
#include "time.h"

int main(){
	int k = NULL, *matriz = NULL;

	printf("Tamanho do vetor: ");
	scanf("%i", &k);

	matriz = (int *)malloc(k * sizeof(int));

	srand(time(NULL));
	FILE *arq = fopen("entrada", "w");
	for (int i = 0; i < k; i++)
	{
		matriz[i] = rand() % k + 1;

		fprintf(arq, "%i " ,  matriz[i]);
	}
	fclose(arq);

return 0;
}
