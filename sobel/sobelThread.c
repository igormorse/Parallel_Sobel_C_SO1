#include    "libimg.h"
#include    <stdlib.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/shm.h>
#include    <string.h>
#include    <stdarg.h>
#include    <pthread.h>
#include    <sys/time.h>
#include    <unistd.h>
#include    <stdint.h>
#define     TRUE (0==0)
#define     FALSE (0==1)

#define DEBUG 0

ppm_image image;

ppm_image image_res;
ppm_image image_sobel;

int width_thread;

void proccess_image_pixels(ppm_image image, ppm_image image_sobel);
void printFlushed(char *, ...);

ppm_image readImageFromFile(char *);

void childProcess(ppm_image,ppm_image,int);
void parentProcess(char *,ppm_image,pid_t);

void threadProcess(ppm_image,ppm_image,int ,int , int , int , int );
void image_save(char *,ppm_image);

void * paralelizar(void *);

struct timeval tval_before, tval_after, tval_result;

int main(int argc, char **argv)
{
    // Número de Processos "Trabalhadores".
    int numWorkers;

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
    image = readImageFromFile(inputFilePath);
    
    // Aloca o espaço da Imagem.
    image_sobel = alloc_img(image->width + 2, image->height + 2);
    
    // Isso aqui não sei o que faz mas deixa aqui.
    fill_img(image_sobel,0,0,0);
    
    // Pixelagem a imagem
    proccess_image_pixels(image,image_sobel);
    
    to_greyscale(image_sobel);
    
    //  aloca a imagem de saída
    image_res = alloc_img(image_sobel->width, image_sobel->height);
    
    // separa a largura que vai ser processada por cada workers
    width_thread = image_sobel->width/numWorkers;

    /*
    
        A Partir daqui as Threads são criadas para processar a imagem.
        
        O número de Threads depende de numWorkers.
    
    */
    
    // Inicio do Timer.
    gettimeofday(&tval_before, NULL);

    // Threads.
    pthread_t threads[numWorkers];
    
    int t;
    for(t = 0; t < numWorkers; t++){
        
        if (DEBUG == 1)
            printFlushed("\n\nIniciando Thread: %d\n", t);
        
        // Inicializa as Threads.
        int errorCreate = pthread_create(&threads[t], NULL, paralelizar, (void *) t);
        
        if (errorCreate != 0){
            perror("\n\n Nao foi possivel criar mais Threads!\n\n");
            exit(1);
        }
    }
    
    printFlushed("\n\nESPERANDO THREADS FILHAS EXECUTAREM!\n\n");
    
    // Realiza a Espera de Todas As Threads Terminarem.
    int i = 0;
    for (i = 0; i < numWorkers; i++){
       
       int errorJoin = pthread_join(threads[i], NULL);
       
        if (errorJoin != 0){
            perror("\n\n Nao foi possivel gerenciar Threads!\n\n");
            exit(1);
        }
    }
       
    printFlushed("\n\nTHREADS FILHAS TERMINADAS, SALVANDO ARQUIVO...\n\n");
    
    // Salva a imagem depois do sobel
    image_save(outputFilePath,image_res);
    
    /*
    
        Terminado todo o processamento das Threads, Calcula o tempo total de Execução.
    
    */
    
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    printFlushed("\nTime elapsed: %ld.%06ld segundos\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
    
    pthread_exit(0);
}

void * paralelizar(void * threadId){
    
    int tid = (int) threadId;
    
    if (DEBUG == 1)    
        printFlushed("Inicio : %d , FIM : %d ,Thread = %d\n", ( 1 + ( (tid) * width_thread) ), ( width_thread + 1 +  ((tid) * width_thread) ) ,(tid));
    
    // Chamada do processamento do filtro
    threadProcess(image_sobel,image_res,tid,1 + (tid * width_thread),1,width_thread + 1 +  (tid * width_thread),image_sobel->height);
     
    pthread_exit(0);
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

void threadProcess(ppm_image image_sobel,ppm_image image_res,int tid,int startWidth, int startHeight, int endWidth, int endHeight)
{
    if(image_res == NULL)
    {
        fprintf(stderr,"error, cannot allocate sobel image\n");
        exit(1);
    }
    sobel_filter_at(image_sobel, image_res, startWidth, startHeight, endWidth, endHeight);
}

void image_save(char * outputFilePath,ppm_image res)
{
    
    FILE *fp = fopen(outputFilePath,"wb");
    
    if(fp == NULL)
    {
        fprintf(stderr,"error, cannot open file %s\n",outputFilePath);
        exit(1);
    }

    output_ppm(fp,res);
    
    fclose(fp);
    
    printFlushed("output: %s\n",outputFilePath);
}
// Printf com Flush no Stdout.
void printFlushed(char * format, ...){
    
    va_list args;
    
    va_start (args, format);
    
    vprintf(format,args);
    
    va_end(args);
    
    fflush(stdout);
}
