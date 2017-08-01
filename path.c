#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "object.h"

error_t path_decode(path_t* path,chunk_t* chunk)
{
error_t error;
//Load header
    if(chunk->length<0xE)return ERROR_PREMATURE_END_OF_CHUNK;
//TODO load header
uint32_t length;
uint32_t pos=0xE;
//Load string table
error=string_table_decode(&(path->name),chunk->data+pos,chunk->length-pos,&length);
    if(error!=ERROR_NONE)return error;
pos+=length;

//Load images
error=image_list_decode(&(path->sprites),chunk->data+pos,chunk->length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(path->name));
    return error;
    }
return ERROR_NONE;
}
/*error_t path_encode(path_t* path,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(path->name));
uint32_t sprites_length=image_list_get_encoded_length(&(path->sprites));
uint32_t length=0x1E +name_table_length+sprites_length;

//Allocate memory
uint8_t* data=malloc_or_die(length);
memset(data,0,length);

//Write header
data[6]=path->cursor_sel;
data[7]=path->flags;
data[8]=path->clearance;
data[9]=path->effects;
*((int16_t*)(data+10))=path->build_fee;
data[13]=path->scrolling;

uint32_t pos=0xE;
string_table_encode(&(path->name),data+pos);
pos+=name_table_length;
group_info_encode(&(path->group_info),data+pos);
pos+=16;
image_list_encode(&(path->sprites),data+pos);
chunk_encode(chunk,encoding,data,length);
free(data);
return ERROR_NONE;
}*/

void path_destroy(path_t* path)
{
string_table_destroy(&(path->name));
image_list_destroy(&(path->sprites));
}
