#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "object.h"

error_t wall_decode(wall_t* wall,chunk_t* chunk)
{
error_t error;
//Load header
    if(chunk->length<0xE)return ERROR_PREMATURE_END_OF_CHUNK;
wall->cursor_sel=chunk->data[6];
wall->flags=chunk->data[7];
wall->clearance=chunk->data[8];
wall->effects=chunk->data[9];
wall->build_fee=*((int16_t*)(chunk->data+10));
wall->scrolling=chunk->data[13];

uint32_t length;
uint32_t pos=0xE;
//Load string table
error=string_table_decode(&(wall->name),chunk->data+pos,chunk->length-pos,&length);
    if(error!=ERROR_NONE)return error;
pos+=length;
//Load group info
error=group_info_decode(&(wall->group_info),chunk->data+pos,length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(wall->name));
    return error;
    }
pos+=16;
//Load images
error=image_list_decode(&(wall->sprites),chunk->data+pos,chunk->length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(wall->name));
    return error;
    }
return ERROR_NONE;
}
error_t wall_encode(wall_t* wall,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(wall->name));
uint32_t sprites_length=image_list_get_encoded_length(&(wall->sprites));
uint32_t length=0x1E +name_table_length+sprites_length;

//Allocate memory
uint8_t* data=malloc_or_die(length);
memset(data,0,length);

//Write header
data[6]=wall->cursor_sel;
data[7]=wall->flags;
data[8]=wall->clearance;
data[9]=wall->effects;
*((int16_t*)(data+10))=wall->build_fee;
data[13]=wall->scrolling;

uint32_t pos=0xE;
string_table_encode(&(wall->name),data+pos);
pos+=name_table_length;
group_info_encode(&(wall->group_info),data+pos);
pos+=16;
image_list_encode(&(wall->sprites),data+pos);
chunk_encode(chunk,encoding,data,length);
free(data);
return ERROR_NONE;
}

void wall_destroy(wall_t* wall)
{
string_table_destroy(&(wall->name));
image_list_destroy(&(wall->sprites));
}
