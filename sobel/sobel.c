#include    "libimg.h"
#include    <stdlib.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/shm.h>
#include    <string.h>
#include    <stdarg.h>

#define TRUE (0==0)
#define FALSE (0==1)

#define DEBUG 0

ppm_image image; 

void printFlushed(char *, ...);
ppm_image readImageFromFile(char *);
void childProcess(ppm_image,ppm_image,int);
void parentProcess(char *,ppm_image,pid_t);
void proccess_image_pixels(ppm_image image, ppm_image image_sobel);

 // Número de Processos "Trabalhadores".
int numWorkers;

int main(int argc, char **argv)
{
    // Process Id.
    pid_t pid;
    
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
        
    if(numWorkers%2!=0){
        printFlushed("Numero de trabalhadores só pode ser par");
        exit(0);
    }
        
    
    // Realiza a Leitura das Variaveis de Argumento.
    char * inputFilePath = argv[1];
    char * outputFilePath = argv[2];
    
    // Realiza a Leitura da Imagem do Arquivo.
    image = readImageFromFile(inputFilePath);

    // Pede o Id da memóra Compartilhada com o tamanho da Imagem.
    shmId = shmget(IPC_PRIVATE, sizeof(image_t), IPC_CREAT | 0666);
    
    if (shmId < 0) {
      perror("*** shmget error ***\n");
      exit(1);
     }
    
    
    /*
    
        A Partir daqui Vários Processos Estarão em Execução.
    
    */
    
    // Associa o ponteiro para a memória compartilhada.
    image_sobel = (ppm_image) shmat(shmId, 0, 0);
    
    // Aloca o Buffer de Pixel Compartilhado da Imagem que foi compartilhada acima. O Buffer contém os Pixels individuais da Imagem com as propriedades ( R G B )
    setup_shared_img(image_sobel,image->width + 2, image->height + 2);
    
    // Isso aqui não sei o que faz mas deixa aqui.
    fill_img(image_sobel,0,0,0);
    
    // Realiza a marcação dos Pixels.
    proccess_image_pixels(image,image_sobel);
    
    // Pai pinta a imagem em tons de cinza.
    to_greyscale(image_sobel);
    
    int i = 0;
    
    for (i = 0 ; i < numWorkers ; i++){
    
    
        // Realiza o Fork.
        pid = fork();
        
        // Realiza diferentes métodos para processo filho ou Pai.
        if (pid < 0){
            perror("\n\nHOUVE UM ERRO AO REALIZAR O FORK!\n\n");
            exit(1);
        }
        else if (pid == 0){
            childProcess(image,image_sobel,i);
            break;
        }
        
        // Imprime o PID do Processo.
        printFlushed("\n Parent PID: %d\n", pid);
    }
    
    // Imprime os Filhos.
    if (pid == 0)
        printFlushed("\n\nNumber of Childs: %d / %d\n",i+1,numWorkers);
  
    if (pid > 0 ){
        
        parentProcess(outputFilePath,image_sobel,pid);
        
        /* detach from the segment: */
        if (shmdt(image_sobel) == -1) {
            perror("*** shmdt error ***\n");
            exit(1);
        }
        
        /* detach from the segment: */
        if (shmdt(image_sobel->buf) == -1) {
            perror("*** shmdt error ***\n");
            exit(1);
        }
    }
        
    exit(0);
}

// Realiza a Leitura de um Arquivo de Imagem para dados.
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
    
    printFlushed("input: %s\n",filePath);
    
    return image;

}

void proccess_image_pixels(ppm_image image, ppm_image image_sobel){
    
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

// Processamento do Processo Filho.
void childProcess(ppm_image image, ppm_image image_sobel, int identifier){
    
    /* dividir por coluna */
    int widthChunk = image->width/(numWorkers/2) ;
    int heightChunk = image->height/2 ;
    
    int startHeight;
    int startWidth;
    int endHeight;
    int endWidth;
    
    if(identifier%2==0){
        if(identifier==0){
           identifier=1; 
           startWidth =1;
        } 
        else{
            startWidth = widthChunk*(identifier/2)+1;
        }
        startHeight = 1;
        endHeight = heightChunk+1;
        endWidth = (widthChunk*((identifier/2)+1))+1;
        
      if(identifier==1) identifier=0;
    }
    else{
        startHeight = heightChunk;
        startWidth = widthChunk*(identifier/2)+1;
        endHeight = heightChunk*2+1;
        endWidth = widthChunk*((identifier/2)+1)+1;
    }
    
    printFlushed("\nFilho :%d\nTamanho da imagem: Width:%d x Height:%d\n\nVou trabalhar de: width:%d x height:%d",identifier,image->width, image->height, startWidth, startHeight);
    printFlushed("\nVou trabalhar ate: width:%d x height:%d \n",endWidth, endHeight);
    
    sobel_filter_at(image,image_sobel,startWidth,startHeight,endWidth,endHeight);
}

// Processamento do Processo Pai.
void parentProcess(char * outputFilePath,ppm_image image_sobel,pid_t pIdentifier){
    
    printFlushed("\n\nEsperando pelos Filhos Terminarem...\t PID: %d\n\n", pIdentifier);
    
    while(wait(NULL) > 0);
    
    printFlushed("\n\nResumindo Execução do Pai...\t PID: %d\n\n", pIdentifier);

    FILE *fp = fopen(outputFilePath,"wb");
    
    if(fp == NULL)
    {
        fprintf(stderr,"error, cannot open file %s\n",outputFilePath);
        exit(1);
    }

    output_ppm(fp,image_sobel);
    
    fclose(fp);
    
    printf("output: %s\n",outputFilePath);
    
}

// Printf com Flush no Stdout.
void printFlushed(char * format, ...){
    
    va_list args;
    
    va_start (args, format);
    
    vprintf(format,args);
    
    va_end(args);
    
    fflush(stdout);
}
