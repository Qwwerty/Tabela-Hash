// Tabela_Hash3.cpp : Defines the entry point for the console application.
//

/*
	Comentários: Código todo feito em linguagem C

	IDE: Usei visual studio 2018 para o desenvolvimento do projeto

	Observações: Usei apenas C++ para usar o booleano, para ficar mais agradavel do que ultilizar 
				 0 e 1 parar respostas de true e false


*/

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "locale.h"

//Menu
int Menu() {

	printf("1 - CRIAR A TABELA HASH\n");
	printf("2 - INSERIR NA TABELA HASH\n");
	printf("3 - BUSCAR NA TABELA HASH\n");
	printf("4 - REMOÇÃO A TABELA HASH\n");
	printf("0 - SAIR\n\n");
	printf("DIGITE A OPERAÇÃO QUE DESEJA: ");
	int x;
	scanf_s("%d", &x);
	return x;

}


//Struct Aluno para demonstra armazenação
struct aluno
{
	int matricula;
	char nome[30];
};

//Inserindo o struct aluno
aluno inserirAluno() {
	aluno al;

	printf("DIGITE SUA MATRICULA: ");
	scanf_s("%d", &al.matricula);
	setbuf(stdin, NULL);
	printf("\nDIGITE SEU NOME: ");
	gets_s(al.nome);
	return al;
}

//Defini como o struct Hash vai ser chamado
typedef struct hash Hash;
struct hash
{
	int quantidade, TABLE_SIZE;
	struct aluno **itens;
};

//Criação da tabela Hash
Hash* criarHash(int TABLE_SIZE) {

	Hash* ha = (Hash*)malloc(sizeof(Hash));
	//Se for diferente de nulo é porque conseguiu criar
	if (ha != NULL) {
		int i;
		//Declara o tamanho do menor que vai ser passada pelo parametro
		ha->TABLE_SIZE = TABLE_SIZE;
		//Cria um vetor, Em cada posição do TABLE_SIZE vai ter um struct aluno
		ha->itens = (struct aluno**) malloc(TABLE_SIZE * sizeof(struct aluno*));

		//Verifica se ha->item foi alocada na memória
		if (ha->itens == NULL) {
			free(ha);
			return NULL;
		}
		//Posições que estão sendo ocupadas
		ha->quantidade = 0;
		//Coloca nulo em todas as posições do vetor
		for (i = 0; i < ha->TABLE_SIZE; i++) 
			ha->itens[i] = NULL;
	}
	return ha;

}

//Liberar a tabela hash
void liberarHash(Hash* ha) {

	//Se for diferente de nulo, é porque está alocada ainda na memória
	if (ha != NULL) {
		int i;

		//For para ir pegando cada posição do TABLE_SIZE e dando free para liberação da memória
		for (i = 0; i < ha->TABLE_SIZE; i++) {
			if(ha->itens != NULL)
				//Libera a memória alocada
				free(ha->itens[i]);
		}
		free(ha->itens);
		free(ha);
	}

}


#pragma region Gerador de chaves

//Método da divisão
int chaveDivisao(int chave, int TABLE_SIZE) {
	return (chave & 0x7FFFFFFF) % TABLE_SIZE;
}

//Método da Multiplicação
int chaveMultiplicacao(int chave, int TABLE_SIZE) {

	// "A" precisa ser: "0 < A < 1"
	float A = 0.6543212345;
	float val = A * chave;
	//Pega apenas a parte fracionária
	val = val - int(val);
	return (int)(val * TABLE_SIZE);

}

//Método da dobra
int chaveDobra(int chave, int TABLE_SIZE) {
	//Tanto de bits que vai pular..
	int numero_bits = 10;
	//Operador >> serve para pular os bits
	int parte1 = chave >> numero_bits;
	int parte2 = chave & (TABLE_SIZE - 1);
	return (parte1 ^ parte2);

}

//Tratando uma string como chave
int valorString(char *str) {
	int i, valor = 7;
	int tamanho = strlen(str);
	for (i = 0; i < tamanho; i++)
		valor = 31 * valor + (int)str[i];
	return valor;
}

#pragma endregion

//Tratamento Endereçamento Aberto
/*Funcionamento da sondagem Linear
	
	- Primeiro o elemento " i = 0 " é colocado na "posição" obtida pela "função de hashing": "pos"
	- Segundo elemento "colisão" é colocado na posição "pos + 1"
	- Terceiro elemento é colocado na "nova colisão" na posição "pos + 2"

*/
int sondaLinear(int pos, int i, int TABLE_SIZE) {
	return ((pos + i) & 0x7FFFFFFF) % TABLE_SIZE;
}

/*Funcionamento da sondagem Quadrática
	- Primmeiro elemento "i = 0" é colocado na "posição" obtida pela "função de hashing": "pos"
	- Segundo elemento vai ser a "colisão" é colocado na posição " pos + (c1 * 1) + (c2 * i^2)

*/
int sondagemQuadratica(int pos, int i, int TABLE_SIZE) {
	return (pos + i) + (2 * i) + (5 * i * i) % TABLE_SIZE;
}


/*Funcionamento da Duplo hash
	 - Primeiro elemento (i = 0) é colocado na "posição" obtida por "H1"
	 - Segundo elemento que é a colisão, é colocado na posição "H1 + i * H2", sempre vai incrementando i em casa colisão
*/
int duploHash(int H1, int chave, int i, int TABLE_SIZE) {
	//Usa "função de hashing" + 1, esse "+ 1" é para que nunca dê zero, se não não haverá deslocamento
	int H2 = chaveDivisao(chave, TABLE_SIZE) + 1;
	return ((H1 + i * H2) & 0x7FFFFFFF) % TABLE_SIZE;
}

//Inserção Endereço aberto
bool insereHash_EnderecoAberto(Hash* ha, struct aluno al) {
	//Verifica se ha foi criado e se o vetor não está todo completo, case isso retorna 1 que não foi possível
	if (ha == NULL || ha->quantidade == ha->TABLE_SIZE)
		return false;
	//chave vai ser a matricula do aluno
	int chave = al.matricula;
	int i, pos, newPos;
	//Gera um endereço
	pos = chaveDivisao(chave, ha->TABLE_SIZE);
	for (i = 0; i < ha->TABLE_SIZE; i++) {
		//gera outra na posição, mas na primeira vez o i é igual a 0, então não afeta na posição
		newPos = sondagemQuadratica(pos, i, ha->TABLE_SIZE);
		//Se aquela posição for igual a NULL, então pode armazenar lá
		if (ha->itens[newPos] == NULL) {
			struct aluno* novo;
			novo = (struct aluno*) malloc(sizeof(struct aluno));
			if (novo == NULL)
				return false;
			*novo = al;
			ha->itens[newPos] = novo;
			ha->quantidade++;
			return true;
		}
	}
	return false;
}

//Busca por Endereçamento Aberto
bool buscaHash_EnderecoAberto(Hash* ha, int matricula, struct aluno* al) {
	
	if (ha == NULL)
		return false;

	int pos, posNew, i;
	
	pos = chaveDivisao(matricula, ha->TABLE_SIZE);
	for (i = 0; i < ha->TABLE_SIZE; i++) {
		posNew = sondagemQuadratica(pos, i, ha->TABLE_SIZE);
		//Se for null, é porque o que está procurando não existe no vetor
		if (ha->itens[posNew] == NULL)
			return false;
		//Faz a comparação naquela posição se for retorna true, porque aquele aluno está no vetor
		if (ha->itens[posNew]->matricula == matricula) {
			//*al = *(ha->itens[posNew]);
			return true;
		}
	}

}

//Metodo de remoção
bool removerHash_EndereçoAberto(Hash* ha, int matricula) {
	//Verifica se não é nulo
	if (ha == NULL)
		return false;

	int pos, posNew, i;
	//calcula a chave de acordo na matricula
	pos = chaveDivisao(matricula, ha->TABLE_SIZE);
	for (i = 0; i < ha->TABLE_SIZE; i++)
	{
		//Para tratar colisão sendo que não primeira vez o valor não interferi
		posNew = sondagemQuadratica(pos, i, ha->TABLE_SIZE);
		//se não exister return false
		if (ha->itens[posNew] == NULL)
			return false;
		//Se for a posição entra e coloca como nulo
		if (ha->itens[posNew]->matricula == matricula) {
			ha->itens[posNew] = NULL;
			return true;
		}

	}

}

#pragma region Inserção e busca sem colisão

//Se retorna 0 é porque tudo ocorreu certo
int insereHash_SemColisao(Hash* ha, struct aluno al)
{
	//Verifica se foi criado o ha ou se ele está preenchido totalmente
	if (ha == NULL || ha->quantidade == ha->TABLE_SIZE)
		return 1;

	int chave = al.matricula;
	int pos = chaveDivisao(chave, ha->TABLE_SIZE);
	struct aluno* novo;
	novo = (struct aluno*) malloc(sizeof(struct aluno));

	//Verificar se a memória do struct aluno foi criada
	if (novo == NULL)
		return 1;

	*novo = al;
	ha->itens[pos] = novo;
	ha->quantidade++;
	return 0;
}

int buscaHash_SemColisao(Hash* ha, int matricula, struct aluno* al)
{
	//Verifica se existe essa memória
	if (ha == NULL)
		return 1;

	//Gera a chave de busca
	int pos = chaveDivisao(matricula, ha->TABLE_SIZE);
	//Se for igual a null porque o elemento não existe
	if (ha->itens[pos] == NULL)
		return 1;

	//copia o que tem no ha->itens[pos] para *al
	*al = *(ha->itens[pos]);
	return 0;
}

#pragma endregion

Hash* ha;
struct aluno* al;

//No main fiz um switch para o usuario poder usar sem ficar toda hora reiniciando o programa
int main()
{
	//Para poder usar acentuação
	setlocale(LC_ALL, "Portuguese");
	bool saida = true;

	while (saida) {

		system("cls");
		switch (Menu())
		{

			case 1:
				system("cls");
				int x;

				printf("CRIAÇÃO DA TABELA\n\n");
				printf("TAMANHO DA TABELA: ");
				scanf_s("%d", &x);
				ha = criarHash(x);
				break;

			case 2:
				system("cls");
				printf("INSERÇÃO NA TABELA HASH\n\n");
				insereHash_EnderecoAberto(ha, inserirAluno());
				break;
			
			case 3:
				system("cls");
				printf("BUSCA NA TABELA HASH\n\n");
				
				int matricula;
				printf("DIGITE A MATRICULA PROCURADA: ");
				scanf_s("%d", &matricula);
				bool resposta;
				resposta = buscaHash_EnderecoAberto(ha, matricula, al);
				if(resposta)
					printf("\nALUNO FOI ENCONTRADO\n");
				else
					printf("\nALUNO NÃO FOI ENCONTRADO\n");
				system("pause");
				break;

			case 4: 
				system("cls");
				printf("REMOÇÃO DA TABELA HASH\n\n");
				printf("DIGITE A MATRICULA A REMOVER: ");
				scanf_s("%d", &matricula);
				bool resposta1;
				resposta1 = removerHash_EndereçoAberto(ha, matricula);
				if (resposta1)
					printf("\nALUNO FOI REMOVIDO\n");
				else
					printf("\nALUNO NÃO FOI REMOVIDO\n");
				system("pause");
				break;

			case 0:
				system("cls");
				printf("OBRIGADO POR USAR O PROGRAMA!!!\n");
				system("pause");
				saida = false;
				break;


		}
	}


    return 0;
}



