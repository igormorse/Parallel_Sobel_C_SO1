#include "libimg.h"
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <string.h>

#define TRUE (0==0)
#define FALSE (0==1)

#define DEBUG 0

ppm_image readImageFromFile(char *);
void childProcess(ppm_image,ppm_image,int);
void parentProcess(char *,ppm_image,pid_t);

int main(int argc, char **argv)
{
    
    // Process Id.
    pid_t pid;
    
    // Número de Processos "Trabalhadores".
    int numWorkers;
    
    // ID da Memória Compartilhada.
    int shmId;
    
    // Ponteiro para endereço da Memória Compartilhada.
    ppm_image image_sobel;

    if(argc < 3)
    {
        printf("usage: ./sobel <image input file> <image output file>\n");
        return 1;
    }

    // Se não for passado o número desejado, assume que são 4.
    if (argc < 4)
        numWorkers = 4;
    else
        numWorkers = atoi(argv[3]);
    
    // Realiza a Leitura das Variaveis de Argumento.
    char * inputFilePath = argv[1];
    char * outputFilePath = argv[2];    
    
    // Realiza a Leitura da Imagem do Arquivo.
    ppm_image image = readImageFromFile(inputFilePath);

    /*
    
        A partir daqui inicia o Processo de Criação de Memória Compartilhada.
        
        Definições Importantes:
        
        ppm_image = * image_t  ( libimg.h )
    
    */

    // Pede o Id da memóra Compartilhada com o tamanho da Imagem.
    shmId = shmget(IPC_PRIVATE, image->width * image->height * sizeof(pixel), IPC_CREAT | 0666);
    
    if (shmId < 0) {
      printf("*** shmget error ***\n");
      exit(1);
     }
    
    
    /*
    
        A Partir daqui Vários Processos Estarão em Execução.
    
    */
    
    // Realiza o Fork.
    pid = fork();
    
             
    // Associa o ponteiro para a memória compartilhada.
    image_sobel = (ppm_image) shmat(shmId, NULL, 0);
  
    // Aloca no Endereço da Memória Compartilhada a Imagem.
    image_sobel = alloc_img(image->width + 2, image->height + 2);
    
    
   
    

    if (pid < 0)
        printf("HOUVE UM ERRO AO REALIZAR O FORK!");
    else if (pid == 0){
        // Isso aqui não sei o que faz mas deixa aqui.
        fill_img(image_sobel,0,0,0);
        childProcess(image,image_sobel,1);
    }
    else{
        parentProcess(outputFilePath,image_sobel,pid);
    }
        
    exit(0);
}

ppm_image readImageFromFile(char * filePath){
    
    FILE *fp = fopen(filePath,"rb");
    
    if(fp == NULL)
    {
        fprintf(stderr,"error, cannot open file %s\n",filePath);
        exit(1);
    }

    ppm_image image = get_ppm(fp);
    
    if(image == NULL)
    {
        fprintf(stderr,"error, cannot load image %s\n",filePath);
       exit(1);
    }
    
    fclose(fp);
    
    printf("input: %s\n",filePath);
    
    fflush(stdout);
    
    return image;

}


void childProcess(ppm_image image, ppm_image image_sobel,int identifier){
    
      // Dividir entre processos o Loop a seguir.
    unsigned int i, j;
    for(i = 0 ; i < image_sobel->width ; i++)
    {
        for(j = 0 ; j < image_sobel->height ; j++){
            if((i == 0) || (i == (image_sobel->width-1)))
            {
                put_pixel_unsafe(image_sobel, i,j, 255,0,0);
            }
            else if((j == 0) || (j == (image_sobel->height-1)))
            {
                put_pixel_unsafe(image_sobel, i,j, 255,0,0);
            }
            else
            {
                put_pixel_unsafe(image_sobel, i,j, GET_PIXEL(image,i-1,j-1)[0], GET_PIXEL(image,i-1,j-1)[1], GET_PIXEL(image,i-1,j-1)[2]);
            }
        }
    }
}

void parentProcess(char * outputFilePath,ppm_image image_sobel,pid_t pIdentifier){
    
    wait(NULL);
    
    // Pai pinta a imagem em tons de cinza.
    to_greyscale(image_sobel);

    ppm_image res;
    
    res = alloc_img(image_sobel->width, image_sobel->height);
    
    if(res == NULL)
    {
        fprintf(stderr,"error, cannot allocate sobel image\n");
        exit(1);
    }

    sobel_filter(image_sobel, res);

    FILE *fp = fopen(outputFilePath,"wb");
    
    if(fp == NULL)
    {
        fprintf(stderr,"error, cannot open file %s\n",outputFilePath);
        exit(1);
    }

    output_ppm(fp,res);
    
    fclose(fp);
    
    printf("output: %s\n",outputFilePath);
    
}
