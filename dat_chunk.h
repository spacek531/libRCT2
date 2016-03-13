#ifndef DAT_CHUNK_H_INCLUDED
#define DAT_CHUNK_H_INCLUDED
#include <stdint.h>

enum chunk_encoding
{
ENCODING_NONE,
ENCODING_RLE,
ENCODING_STRING,
ENCODING_ROTATE
};

typedef struct
{
uint8_t* data;
uint32_t length;
uint8_t encoding;
}chunk_t;

error_t chunk_decode(chunk_t* chunk,uint8_t** data,uint32_t* length);
error_t chunk_encode(chunk_t* chunk,uint8_t *data,uint32_t length);

error_t chunk_read(chunk_t* chunk,FILE* file);
error_t chunk_write(chunk_t* chunk,FILE* file);

void chunk_destroy(chunk_t* chunk);

#endif // DAT_CHUNK_H_INCLUDED

/*
typedef struct
{
uint8_t* data;
uint32_t size;
uint32_t allocated;
}buffer_t;
void buffer_init(buffer_t* buffer);
void buffer_expand(buffer_t* buffer,uint32_t num_bytes);
void buffer_write(buffer_t* buffer,uint8_t* bytes,uint32_t num_bytes);
void buffer_free(buffer_t* buffer);
*/
