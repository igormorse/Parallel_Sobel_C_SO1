#include "libimg.h"

#define TRUE (0==0)
#define FALSE (0==1)

#define DEBUG 0 

int main(int argc, char **argv)
{
    char temp[50];
    strcpy(temp,"");
    if(argc < 3)
    {
        printf("usage: ./sobel <image input file> <image output file>\n");
        return 1;
    }

    printf("input: %s\n",argv[1]);
    printf("output: %s\n",argv[2]);

    FILE *fp = fopen(argv[1],"rb");
    if(fp == NULL)
    {
        fprintf(stderr,"error, cannot open file %s\n",argv[1]);
        return 1;
    }

    ppm_image image = get_ppm(fp);
    if(image == NULL)
    {
        fprintf(stderr,"error, cannot load image %s\n",argv[1]);
        return 1;
    }
    fclose(fp);

#if DEBUG
    fp = fopen("test.ppm","wb");
    if(fp == NULL)
    {
        fprintf(stderr,"error, cannot open file test.ppm\n");
        return 1;
    }
    output_ppm(fp,image);
    fclose(fp);
#endif

    ppm_image image_sobel = alloc_img(image->width + 2, image->height + 2);
    fill_img(image_sobel,0,0,0);

    unsigned int i, j;
    for(i = 0 ; i < image_sobel->width ; i++)
    {
        for(j = 0 ; j < image_sobel->height ; j++)
        {
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

    to_greyscale(image_sobel);

    ppm_image res;
    res = alloc_img(image_sobel->width, image_sobel->height);
    if(res == NULL)
    {
        fprintf(stderr,"error, cannot allocate sobel image\n");
        return 1;
    }

    sobel_filter(image_sobel, res);

    fp = fopen(argv[2],"wb");
    if(fp == NULL)
    {
        fprintf(stderr,"error, cannot open file %s\n",argv[2]);
        return 1;
    }

    output_ppm(fp,res);
    fclose(fp);

    return 0;
}

