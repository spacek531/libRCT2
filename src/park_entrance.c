#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "object.h"

error_t park_entrance_decode(park_entrance_t* park_entrance,chunk_t* chunk)
{
error_t error;
//Load header
    if(chunk->length<0x8)return ERROR_PREMATURE_END_OF_CHUNK;
//TODO load header
uint32_t length;
uint32_t pos=0x8;
//Load string table
error=string_table_decode(&(park_entrance->name),chunk->data+pos,chunk->length-pos,&length);
    if(error!=ERROR_NONE)return error;
pos+=length;

//Load images
error=image_list_decode(&(park_entrance->sprites),chunk->data+pos,chunk->length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(park_entrance->name));
    return error;
    }
return ERROR_NONE;
}
/*error_t park_entrance_encode(park_entrance_t* park_entrance,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(park_entrance->name));
uint32_t sprites_length=image_list_get_encoded_length(&(park_entrance->sprites));
uint32_t length=0x1E +name_table_length+sprites_length;

//Allocate memory
uint8_t* data=malloc_or_die(length);
memset(data,0,length);

//Write header
data[6]=park_entrance->cursor_sel;
data[7]=park_entrance->flags;
data[8]=park_entrance->clearance;
data[9]=park_entrance->effects;
*((int16_t*)(data+10))=park_entrance->build_fee;
data[13]=park_entrance->scrolling;

uint32_t pos=0xE;
string_table_encode(&(park_entrance->name),data+pos);
pos+=name_table_length;
group_info_encode(&(park_entrance->group_info),data+pos);
pos+=16;
image_list_encode(&(park_entrance->sprites),data+pos);
chunk_encode(chunk,encoding,data,length);
free(data);
return ERROR_NONE;
}*/

void park_entrance_destroy(park_entrance_t* park_entrance)
{
string_table_destroy(&(park_entrance->name));
image_list_destroy(&(park_entrance->sprites));
}
