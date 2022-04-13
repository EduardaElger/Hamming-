#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

unsigned short int converte_vetor_para_palavra_de_8_bits(unsigned short int vetor[8]) {
  unsigned short int palavra = 0, valor_do_bit = 1;

  int i;

  for(i = 0; i < 8; i++) {
    if (vetor[i] != 0) {
      palavra = palavra + valor_do_bit;
    }
    valor_do_bit = valor_do_bit + valor_do_bit;
  }

  /*

    i = 0;
    palavra      = 0000 0000 0000 0000
    valor_do_bit = 0000 0000 0000 0001
    
    vetor[0] != 0
    palavra      = 0000 0000 0000 0001
    valor_do_bit = 0000 0000 0000 0010
    
    vetor[1] != 0
    palavra      = 0000 0000 0000 0011
    valor_do_bit = 0000 0000 0000 0100
    
    vetor[2] != 0
    palavra      = 0000 0000 0000 0111
    valor_do_bit = 0000 0000 0000 1000

   */

  return palavra;
}


unsigned short int converte_vetor_para_palavra_de_16_bits(unsigned short int vetor[16]) {
  unsigned short int palavra = 0, valor_do_bit = 1;

  int i;

  for(i = 0; i < 16; i++) {
    if (vetor[i] != 0) {
      palavra = palavra + valor_do_bit;
    }
    valor_do_bit = valor_do_bit + valor_do_bit;
  }

  return palavra;
}


unsigned short int nucleo_da_escrita(unsigned char palavraM_8) {
  int i;

  unsigned short int palavraHamming, vetorHamming[16], vetorM[] = {0, 0, 0, 0, 0, 0, 0, 0};
  unsigned short int valor_do_bit = 1, palavraM = palavraM_8;

  // Converte palavraM para vetorM
  for(i = 0; i < 8; i++) {
    vetorM[i] = palavraM & valor_do_bit;
    if (vetorM[i] != 0) {
      vetorM[i] = 65535;
    }
    valor_do_bit = valor_do_bit + valor_do_bit;
  }

  // Atribuindo os M
  vetorHamming[3]  = vetorM[0]; // M1
  vetorHamming[5]  = vetorM[1]; // M2
  vetorHamming[6]  = vetorM[2]; // M3
  vetorHamming[7]  = vetorM[3]; // M4
  vetorHamming[9]  = vetorM[4]; // M5
  vetorHamming[10] = vetorM[5]; // M6
  vetorHamming[11] = vetorM[6]; // M7
  vetorHamming[12] = vetorM[7]; // M8

  // Calculando C
  vetorHamming[1] = vetorM[0] ^ vetorM[1] ^ vetorM[3] ^ vetorM[4] ^ vetorM[6]; // C1
  vetorHamming[2] = vetorM[0] ^ vetorM[2] ^ vetorM[3] ^ vetorM[5] ^ vetorM[6]; // C2
  vetorHamming[4] = vetorM[1] ^ vetorM[2] ^ vetorM[3] ^ vetorM[7];             // C4
  vetorHamming[8] = vetorM[4] ^ vetorM[5] ^ vetorM[6] ^ vetorM[7];             // C8
  //printf("C1=%u\nC2=%u\nC4=%u\nC8=%u\n", vetorHamming[1], vetorHamming[2], vetorHamming[4], vetorHamming[8]);

  // Calculando G
  vetorHamming[0] = vetorHamming[1];
  for(i = 2; i < 13; i++)
    vetorHamming[0] = vetorHamming[0] ^ vetorHamming[i];
  //printf("G=%u\n-----\n", vetorHamming[0]);

  // Atribuindo 0 nas posições não utilizadas
  vetorHamming[13] = 0;
  vetorHamming[14] = 0;
  vetorHamming[15] = 0;

  // Converter vetor para palavra
  palavraHamming = converte_vetor_para_palavra_de_16_bits(vetorHamming);

  // Retorna o hamming
  return palavraHamming;
}


unsigned char nucleo_da_leitura(unsigned short int palavraHamming) {
  int i;

  unsigned short int vetorHamming[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned short int palavraM, novoC[4], xorC[4], vetorM[8], auxiliar, bit_errado = 0, valor_do_bit = 1;

  // Converte palavraHamming para vetorHamming
  for(i = 0; i < 16; i++) {
    vetorHamming[i] = palavraHamming & valor_do_bit;
    if (vetorHamming[i] != 0) {
      vetorHamming[i] = 65535;
    }
    valor_do_bit = valor_do_bit + valor_do_bit;
  }

  // Atribuindo 0 nas posições não utilizadas
  vetorHamming[13] = 0;
  vetorHamming[14] = 0;
  vetorHamming[15] = 0;

  // Salva G antigo
  int g_antigo = vetorHamming[0];

  // Calculando novo C
  novoC[0] = vetorHamming[3] ^ vetorHamming[5]  ^ vetorHamming[7]  ^ vetorHamming[9]  ^ vetorHamming[11]; // C1
  novoC[1] = vetorHamming[3] ^ vetorHamming[6]  ^ vetorHamming[7]  ^ vetorHamming[10] ^ vetorHamming[11]; // C2
  novoC[2] = vetorHamming[5] ^ vetorHamming[6]  ^ vetorHamming[7]  ^ vetorHamming[12];                    // C4
  novoC[3] = vetorHamming[9] ^ vetorHamming[10] ^ vetorHamming[11] ^ vetorHamming[12];                    // C8
  //printf("NC1=%u\nNC2=%u\nNC4=%u\nNC8=%u\n", novoC[0], novoC[1], novoC[2], novoC[3]);

  // XOR do novo C com o velho C
  xorC[0] = vetorHamming[1] ^ novoC[0];
  xorC[1] = vetorHamming[2] ^ novoC[1];
  xorC[2] = vetorHamming[4] ^ novoC[2];
  xorC[3] = vetorHamming[8] ^ novoC[3];

  // Pega o bit errado de acordo com o XOR do C
  if (xorC[0] != 0) {
    bit_errado = bit_errado + 1;
  }
  if (xorC[1] != 0) {
    bit_errado = bit_errado + 2;
  }
  if (xorC[2] != 0) {
    bit_errado = bit_errado + 4;
  }
  if (xorC[3] != 0) {
    bit_errado = bit_errado + 8;
  }

  // Verificando se o arquivo está intacto
  if (bit_errado == 0) {
    printf("Palavra intacta.\n");

    vetorM[0] = vetorHamming[3];
    vetorM[1] = vetorHamming[5];
    vetorM[2] = vetorHamming[6];
    vetorM[3] = vetorHamming[7];
    vetorM[4] = vetorHamming[9];
    vetorM[5] = vetorHamming[10];
    vetorM[6] = vetorHamming[11];
    vetorM[7] = vetorHamming[12];

    palavraM = converte_vetor_para_palavra_de_8_bits(vetorM);

    // Retorna o M sem precisar executar os próximos passos
    return palavraM;
  }

  printf("Bit errado na posição: %u\n", bit_errado + 1);

  // Inverte o bit errado
  vetorHamming[bit_errado] = ~vetorHamming[bit_errado];

  // Refazer atribuição dos vetores
  vetorM[0] = vetorHamming[3];
  vetorM[1] = vetorHamming[5];
  vetorM[2] = vetorHamming[6];
  vetorM[3] = vetorHamming[7];
  vetorM[4] = vetorHamming[9];
  vetorM[5] = vetorHamming[10];
  vetorM[6] = vetorHamming[11];
  vetorM[7] = vetorHamming[12];

  // Calculando C novamente
  vetorHamming[1] = vetorM[0] ^ vetorM[1] ^ vetorM[3] ^ vetorM[4] ^ vetorM[6]; // C1
  vetorHamming[2] = vetorM[0] ^ vetorM[2] ^ vetorM[3] ^ vetorM[5] ^ vetorM[6]; // C2
  vetorHamming[4] = vetorM[1] ^ vetorM[2] ^ vetorM[3] ^ vetorM[7];             // C4
  vetorHamming[8] = vetorM[4] ^ vetorM[5] ^ vetorM[6] ^ vetorM[7];             // C8

  // Calculando G novamante
  vetorHamming[0] = vetorHamming[1];
  for(i = 2; i < 13; i++)
    vetorHamming[0] = vetorHamming[0] ^ vetorHamming[i];

  // Verificando G novo com G antigo
  if ((vetorHamming[0] ^ g_antigo) == 0) {
    printf("Palavra recuperada.\n");

    vetorM[0] = vetorHamming[3];
    vetorM[1] = vetorHamming[5];
    vetorM[2] = vetorHamming[6];
    vetorM[3] = vetorHamming[7];
    vetorM[4] = vetorHamming[9];
    vetorM[5] = vetorHamming[10];
    vetorM[6] = vetorHamming[11];
    vetorM[7] = vetorHamming[12];

    palavraM = converte_vetor_para_palavra_de_8_bits(vetorM);
  } else {
    printf("Palavra irrecuperável.\n");
    exit(-1);
  }

  return palavraM;
}


void escrita(char nome[]) {
  int tamanho = 0, i = 0;

  char novo_nome[200];

  unsigned short int palavraHamming;

  unsigned char leitura;

  strcpy(novo_nome, nome);
  strcat(novo_nome, ".wham");

  FILE *arquivo = fopen(nome, "rb"); // arquivo lido normal
  FILE *hamming = fopen(novo_nome, "wb"); // arquivo .wham

  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    exit(-1);
  }

  fseek(arquivo, 0, SEEK_END);
  tamanho = ftell(arquivo);
  fseek(arquivo, 0, SEEK_SET);

  for (i = 0; i < tamanho; i++) {
    fread(&leitura, 1, 1, arquivo);
    palavraHamming = nucleo_da_escrita(leitura);
    fwrite(&palavraHamming, 2, 1, hamming);
  }

  fclose(arquivo);
  fclose(hamming);
}


void leitura(char nome[]) {
  int i;

  char novo_nome[200];

  unsigned short int leitura;
  unsigned char palavraM;

  strncpy(novo_nome, nome, strlen(nome) - 5);
  novo_nome[strlen(nome) - 5] = '\0';

  FILE *arquivo = fopen(novo_nome, "wb"); // arquivo normal
  FILE *hamming = fopen(nome, "rb"); // arquivo .wham lido

  if (hamming == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    exit(-1);
  }

  fseek(hamming, 0, SEEK_END);
  int tamanho = ftell(hamming);
  fseek(hamming, 0, SEEK_SET);

  for (i = 0; i < tamanho; i++) {
    leitura = 0;
    fread(&leitura, 2, 1, hamming);
    palavraM = nucleo_da_leitura(leitura);
    fwrite(&palavraM, 1, 1, arquivo);
    i++;
  }

  fclose(arquivo);
  fclose(hamming);
}


int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Argumentos inválidos!\n");
    exit(-1);
  }

  if (strcmp(argv[2], "-r") == 0) {
    printf("Leitura.\n");
    leitura(argv[1]);
  }

  else if (strcmp(argv[2], "-w") == 0) {
    printf("Escrita.\n");
    escrita(argv[1]);
  }

  else {
    printf("Argumento inválido!\n");
    exit(-1);
  }

  return 0;
}