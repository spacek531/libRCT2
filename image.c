#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "palette.h"
/*
image_t* image_new(uint8_t width,uint8_t height,uint8_t color)
{
image_t* image=malloc(sizeof(image_t));
image->x_offset=0;
image->y_offset=0;
image->flags=1;
image->width=width;
image->height=height;
image->data=malloc(height*sizeof(uint8_t*));
int i;
    for(i=0;i<height;i++)
    {
    image->data[i]=malloc(width*sizeof(uint8_t));
    memset(image->data[i],color,width);
    }
return image;
}

image_t* image_copy(image_t* source)
{
image_t* image=malloc(sizeof(image_t));
image->x_offset=source->x_offset;
image->y_offset=source->y_offset;
image->flags=source->flags;
image->width=source->width;
image->height=source->height;
image->data=malloc(source->height*sizeof(uint8_t*));

    for(int y=0;y<source->height;y++)
    {
    image->data[y]=malloc(source->width*sizeof(uint8_t));
        for(int x=0;x<source->height;x++)image->data[y][x]=source->data[y][x];
    }
return image;
}

void image_free(image_t* image)
{
int i;
    for(i=0;i<image->height;i++)free(image->data[i]);
free(image->data);
free(image);
}
*/



void image_save_bmp(image_t* image,char* filename)
{
int padding=(4-(image->width*3)%4)%4;

int data_size=image->height*(image->width*3+padding);

uint8_t bitmap_header[54];
memset(bitmap_header,0,54);

bitmap_header[0]='B';
bitmap_header[1]='M';
*((uint32_t*)(bitmap_header+2))=54+data_size;
*((uint32_t*)(bitmap_header+10))=54;
*((uint32_t*)(bitmap_header+14))=40;
*((uint32_t*)(bitmap_header+18))=image->width;
*((uint32_t*)(bitmap_header+22))=image->height;
*((uint16_t*)(bitmap_header+26))=1;
*((uint16_t*)(bitmap_header+28))=24;
*((uint32_t*)(bitmap_header+38))=2834;
*((uint32_t*)(bitmap_header+42))=2834;


FILE* file=fopen(filename,"w");

fwrite(bitmap_header,1,54,file);

    for(int y=image->height-1;y>=0;y--)
    {
        for(int x=0;x<image->width;x++)
        {
        color_t color=palette_color_from_index(image->data[x+image->width*y]);
        fputc(color.blue,file);
        fputc(color.green,file);
        fputc(color.red,file);
        }
        for(int k=0;k<padding;k++)fputc(0,file);
    }
fclose(file);
}
