#include "libimg.h"

void setup_img(ppm_image img,unsigned int width, unsigned int height)
{
    img->buf = malloc(width * height * sizeof(pixel));
    img->width = width;
    img->height = height;
    img->high = 0;
    img->max_color = 255; //default
}

ppm_image alloc_img(unsigned int width, unsigned int height)
{
    ppm_image img;
    img = malloc(sizeof(image_t));
    img->buf = malloc(width * height * sizeof(pixel));
    img->width = width;
    img->height = height;
    img->high = 0;
    img->max_color = 255; //default
    return img;
}
 
void free_img(ppm_image img)
{
    free(img->buf);
    free(img);
}
 
void fill_img(
        ppm_image img,
        color_component r,
        color_component g,
        color_component b )
{
    unsigned int i, n;
    n = img->width * img->height;
    for (i=0; i < n; ++i)
    {
        img->buf[i][0] = r;
        img->buf[i][1] = g;
        img->buf[i][2] = b;
    }
}

void put_pixel_unsafe(
       	ppm_image img,
        unsigned int x,
        unsigned int y,
        color_component r,
        color_component g,
        color_component b )
{
    unsigned int ofs;
    ofs = (y * img->width) + x;
    img->buf[ofs][0] = r;
    img->buf[ofs][1] = g;
    img->buf[ofs][2] = b;
}
 
void put_pixel_clip(
       	ppm_image img,
        unsigned int x,
        unsigned int y,
        color_component r,
        color_component g,
        color_component b )
{
    if (x < img->width && y < img->height)
      put_pixel_unsafe(img, x, y, r, g, b);
}

ppm_image get_ppm(FILE *pf)
{
        char buf[PPMREADBUFLEN], *t;
        ppm_image img;
        unsigned int w, h, d;
        int r;
 
        if (pf == NULL) return NULL;
        t = fgets(buf, PPMREADBUFLEN, pf);
        /* the code fails if the white space following "P6" is not '\n' */
        if ( (t == NULL) || ( strncmp(buf, "P6\n", 3) != 0 ) ) return NULL;
        do
        { /* Px formats can have # comments after first line */
           t = fgets(buf, PPMREADBUFLEN, pf);
           if ( t == NULL ) return NULL;
        } while ( strncmp(buf, "#", 1) == 0 );
        r = sscanf(buf, "%u %u", &w, &h);
        if ( r < 2 ) return NULL;
 
        r = fscanf(pf, "%u", &d);
        if ( (r < 1) || ( d != 255 ) ) return NULL;
        fseek(pf, 1, SEEK_CUR); /* skip one byte, should be whitespace */
 
        img = alloc_img(w, h);
        if ( img != NULL )
        {
            img->high = 0;
            size_t rd = fread(img->buf, sizeof(pixel), w*h, pf);
            if ( rd < w*h )
            {
               free_img(img);
               return NULL;
            }
            return img;
        }
        return NULL;
}

void output_ppm(FILE *fd, ppm_image img)
{
  unsigned int n;
  (void) fprintf(fd, "P6\n%d %d\n%d\n", img->width, img->height, img->max_color);
  n = img->width * img->height;
  (void) fwrite(img->buf, sizeof(pixel), n, fd);
  (void) fflush(fd);
}

void to_greyscale(ppm_image img)
{
    float fr, fg, fb;

    unsigned int i,j;

    for(i = 0 ; i < img->width ; i++)
    {
        for(j = 0; j < img->height ; j++)
        {
            fr = (float) GET_PIXEL(img, i,j)[0]/255.0f;
            fg = (float) GET_PIXEL(img, i,j)[1]/255.0f;
            fb = (float) GET_PIXEL(img, i,j)[2]/255.0f;

            float fwb = sqrt((fr*fr + fg*fg + fb*fb)/3.0f);

            unsigned char c = (unsigned char) (fwb*255);
            if(c > img->high)
                img->high = c;

            put_pixel_unsafe(img, i,j, c, c, c);

        }
    }
}


void sobel_filter(ppm_image img, ppm_image sobel_img)
{
    const char sobel_x[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    const char sobel_y[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};

    unsigned int x,y;

    for(x = 1 ; x < img->width-2 ; x++)
    {
        for(y = 1; y < img->height-2 ; y++)
        {
            unsigned char fa = GET_PIXEL(img,x-1,y-1)[0]; 
            unsigned char fb = GET_PIXEL(img,x,y-1)[0];
            unsigned char fc = GET_PIXEL(img,x+1,y-1)[0];
            unsigned char fd = GET_PIXEL(img,x-1,y)[0];
            unsigned char fe = GET_PIXEL(img,x,y)[0];
            unsigned char ff = GET_PIXEL(img,x+1,y)[0];
            unsigned char fg = GET_PIXEL(img,x-1,y+1)[0];
            unsigned char fh = GET_PIXEL(img,x,y+1)[0];
            unsigned char fi = GET_PIXEL(img,x+1,y+1)[0];

            short pixel_x = (sobel_x[0][0] * fa ) + (sobel_x[0][1] * fb ) + (sobel_x[0][2] * fc) + (sobel_x[1][0] * fd) + (sobel_x[1][1] * fe) + (sobel_x[1][2] * ff) + (sobel_x[2][0] * fg) + (sobel_x[2][1] * fh) + (sobel_x[2][2] * fi);

            short pixel_y = (sobel_y[0][0] * fa ) + (sobel_y[0][1] * fb ) + (sobel_y[0][2] * fc) + (sobel_y[1][0] * fd) + (sobel_y[1][1] * fe) + (sobel_y[1][2] * ff) + (sobel_y[2][0] * fg) + (sobel_y[2][1] * fh) + (sobel_y[2][2] * fi);
           
            short val = abs(pixel_x) + abs(pixel_y);
            unsigned char char_val = (255-(unsigned char)(val));

            if(char_val > 200)
                char_val = 255;
            else if(char_val < 100)
                char_val = 0;

            //put_pixel_unsafe(img, x,y, (unsigned char) (val), (unsigned char) (val), (unsigned char) (val));
            //put_pixel_unsafe(img, x,y, ans, ans, ans);
            put_pixel_unsafe(sobel_img, x,y, char_val, char_val, char_val);
        }
    }

    //printf("max = %f\n",max);

}


//void sobel_filter(ppm_image img)
//{
//    const char sobel_x[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
//    const char sobel_y[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
//
//    unsigned int x,y;
//
//    for(x = 1 ; x < img->width-2 ; x++)
//    {
//        for(y = 1; y < img->height-2 ; y++)
//        {
//            unsigned char fa = GET_PIXEL(img,x-1,y-1)[0]; 
//            unsigned char fb = GET_PIXEL(img,x,y-1)[0];
//            unsigned char fc = GET_PIXEL(img,x+1,y-1)[0];
//            unsigned char fd = GET_PIXEL(img,x-1,y)[0];
//            unsigned char fe = GET_PIXEL(img,x,y)[0];
//            unsigned char ff = GET_PIXEL(img,x+1,y)[0];
//            unsigned char fg = GET_PIXEL(img,x-1,y+1)[0];
//            unsigned char fh = GET_PIXEL(img,x,y+1)[0];
//            unsigned char fi = GET_PIXEL(img,x+1,y+1)[0];
//
//            short pixel_x = (sobel_x[0][0] * fa ) + (sobel_x[0][1] * fb ) + (sobel_x[0][2] * fc) + (sobel_x[1][0] * fd) + (sobel_x[1][1] * fe) + (sobel_x[1][2] * ff) + (sobel_x[2][0] * fg) + (sobel_x[2][1] * fh) + (sobel_x[2][2] * fi);
//
//            short pixel_y = (sobel_y[0][0] * fa ) + (sobel_y[0][1] * fb ) + (sobel_y[0][2] * fc) + (sobel_y[1][0] * fd) + (sobel_y[1][1] * fe) + (sobel_y[1][2] * ff) + (sobel_y[2][0] * fg) + (sobel_y[2][1] * fh) + (sobel_y[2][2] * fi);
//           
//            short val = abs(pixel_x) + abs(pixel_y);
//            //unsigned char char_val = (255-(unsigned char)(val));
//            unsigned char char_val = (unsigned char)(val);
//
//            //printf("char_val = %d\n",char_val);
//            //unsigned char ans;
//
//            //ans = (255-(unsigned char)(val));
//            //
//            //if(ans > 200)
//            //    ans = 255;
//            //else if(ans < 100)
//            //    ans = 0;
//
//            //put_pixel_unsafe(img, x,y, (unsigned char) (val), (unsigned char) (val), (unsigned char) (val));
//            //put_pixel_unsafe(img, x,y, ans, ans, ans);
//            put_pixel_unsafe(img, x,y, char_val, char_val, char_val);
//        }
//    }
//    //printf("max = %f\n",max);
//
//}

float packColor(color_component r, color_component g, color_component b)
{
    return (float) (r + g * 255.0f + b * 255.0f * 255.0f);
}

void unpackColor(float f, color_component *r, color_component *g, color_component *b)
{
    *b = floor(f/255.0f/255.0f);
    *g = floor((f - (*b) * 255.0f * 255.0f) / 255.0f);
    *r = floor(f - (*b) * 255.0f * 255.0f - (*g) * 255.0f);
    
    //printf("rgb = %d,%d,%d\n",*r,*g,*b);
    
    //*b = *b / 255.0f;
    //*g = *g / 255.0f;
    //*r = *r / 255.0f;

    
}

//float packColor(vec3 color) {
//    return color.r + color.g * 256.0 + color.b * 256.0 * 256.0;
//}
//
//vec3 unpackColor(float f) {
//    vec3 color;
//    color.b = floor(f / 256.0 / 256.0);
//    color.g = floor((f - color.b * 256.0 * 256.0) / 256.0);
//    color.r = floor(f - color.b * 256.0 * 256.0 - color.g * 256.0);
//    // now we have a vec3 with the 3 components in range [0..255]. Let's normalize it!
//    return color / 255.0;
//}


