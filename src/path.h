#ifndef PATH_H_INCLUDED
#define PATH_H_INCLUDED
#include <stdint.h>


typedef struct //Path
{
//TODO fill in header
string_table_t name;
image_list_t sprites;
}path_t;


error_t path_decode(path_t* path,chunk_t* chunk);
error_t path_encode(path_t* path,uint8_t encoding,chunk_t* chunk);
void path_destroy(path_t* path);


#endif // PATH_H_INCLUDED
