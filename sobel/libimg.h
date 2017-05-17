#ifndef _IMGLIB_0
#define _IMGLIB_0
 
#define PPMREADBUFLEN 256

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <sys/queue.h>
 
typedef unsigned char color_component;
typedef color_component pixel[3];
typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int max_color;
    color_component high;
    pixel * buf;
} image_t;
typedef image_t * ppm_image;

float packColor(color_component r, color_component g, color_component b);
//void unpackColor(float f, color_component *p);
//void unpackColor(float f, pixel *p);
void unpackColor(float f, color_component *r, color_component *g, color_component *b);

void to_greyscale(ppm_image);
void sobel_filter(ppm_image img, ppm_image sobel_img);
void output_ppm(FILE *fd, ppm_image img);
ppm_image get_ppm(FILE *pf);
ppm_image alloc_img(unsigned int width, unsigned int height);
void free_img(ppm_image);
void fill_img(ppm_image img,
        color_component r,
        color_component g,
        color_component b );
void put_pixel_unsafe(
       	ppm_image img,
        unsigned int x,
        unsigned int y,
        color_component r,
        color_component g,
        color_component b );
void put_pixel_clip(
       	ppm_image img,
        unsigned int x,
        unsigned int y,
        color_component r,
        color_component g,
        color_component b );
#define GET_PIXEL(IMG, X, Y) (IMG->buf[ ((Y) * IMG->width + (X)) ])
#endif

