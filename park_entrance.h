#ifndef PARK_ENTRANCE_H_INCLUDED
#define PARK_ENTRANCE_H_INCLUDED
#include <stdint.h>


typedef struct{
//TODO fill in header
string_table_t name;
image_list_t sprites;
}park_entrance_t;


error_t park_entrance_decode(park_entrance_t* park_entrance,chunk_t* chunk);
error_t park_entrance_encode(park_entrance_t* park_entrance,uint8_t encoding,chunk_t* chunk);
void park_entrance_destroy(park_entrance_t* park_entrance);


#endif // PARK_ENTRANCE_H_INCLUDED
