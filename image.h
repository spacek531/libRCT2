#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED
#include<stdint.h>

typedef struct
{
uint16_t width;
uint16_t height;
int16_t x_offset;
int16_t y_offset;
uint16_t flags;
uint8_t* data;
}rct2_image_t;

void image_new(rct2_image_t* image,uint16_t width,uint16_t height,int16_t x_offset,int16_t y_offset,uint16_t flags);
void image_copy(rct2_image_t* src,rct2_image_t* dst);
void image_blit(rct2_image_t* dst,rct2_image_t* src,int16_t x_offset,int16_t y_offset);
void image_write_bmp(rct2_image_t* image,FILE* file);
void rct2_image_destroy(rct2_image_t* image);
#endif // IMAGE_H_INCLUDED
