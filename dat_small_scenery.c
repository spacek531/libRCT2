#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "dat.h"


static error_t animation_indices_decode(animation_indices_t* animation_indices,uint8_t* data,uint32_t data_length,uint32_t* length)
{
uint32_t pos=0;
    while(pos<data_length&&data[pos]!=0xFF)pos++;
    if(pos>=data_length)return ERROR_PREMATURE_END_OF_CHUNK;
animation_indices->num_indices=pos;
animation_indices->indices=malloc(animation_indices->num_indices);
memcpy(animation_indices->indices,data,animation_indices->num_indices);

*length=pos+1;
return ERROR_NONE;
}
static uint32_t animation_indices_get_encoded_length(animation_indices_t* animation_indices)
{
return animation_indices->num_indices+1;
}
static void animation_indices_encode(animation_indices_t* animation_indices,uint8_t* data)
{
memcpy(data,animation_indices->indices,animation_indices->num_indices);
data[animation_indices->num_indices+1]=0xFF;
}

error_t small_scenery_decode(small_scenery_t* scenery,chunk_t* chunk)
{
uint8_t* data;
uint32_t data_length;
error_t error=chunk_decode(chunk,&data,&data_length);
    if(error!=ERROR_NONE)return error;

FILE* file=fopen("DECOMPRESSED.DAT","w");
fwrite(data,1,data_length,file);
fclose(file);

//Load header data
    if(data_length<0x1C)
    {
    free(data);
    return ERROR_PREMATURE_END_OF_CHUNK;
    }
scenery->flags=*((uint32_t*)(data+6));
scenery->height=data[10];
scenery->cursor_sel=data[11];
scenery->build_fee=*((int16_t*)(data+12));
scenery->remove_fee=*((int16_t*)(data+14));
scenery->anim_delay=*((int16_t*)(data+20));
scenery->anim_parameters=*((int16_t*)(data+22));
scenery->anim_frames=*((int16_t*)(data+24));

uint32_t pos=0x1C;

//Load string table
uint32_t length;
error=string_table_decode(&(scenery->name),data+pos,data_length-pos,&length);
    if(error!=ERROR_NONE)return error;
pos+=length;

//Load group info
error=group_info_decode(&(scenery->group_info),data+pos,length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(scenery->name));
    return error;
    }
pos+=16;

//Load animation indices (if present)
    if(scenery->flags&SMALL_SCENERY_ANIMDATA)
    {
    error=animation_indices_decode(&(scenery->animation_indices),data+pos,length-pos,&length);
        if(error!=ERROR_NONE)
        {
        string_table_destroy(&(scenery->name));
        return error;
        }
    pos+=length;
    }
//Load images
error=image_list_decode(&(scenery->sprites),data+pos,data_length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(scenery->name));
    return error;
    }
free(data);
return ERROR_NONE;
}

error_t small_scenery_encode(small_scenery_t* scenery,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(scenery->name));
uint32_t animation_indices_length=scenery->flags&SMALL_SCENERY_ANIMDATA?animation_indices_get_encoded_length(&(scenery->animation_indices)):0;
uint32_t sprites_length=image_list_get_encoded_length(&(scenery->sprites));
printf("%d\n",sprites_length);
uint32_t length=0x2C+name_table_length+animation_indices_length+sprites_length;

//Allocate memory
uint8_t* data=malloc_or_die(length);
memset(data,0,length);

//Write header
*((uint32_t*)(data+6))=scenery->flags;
data[10]=scenery->height;
data[11]=scenery->cursor_sel;
*((int16_t*)(data+12))=scenery->build_fee;;
*((int16_t*)(data+14))=scenery->remove_fee;
*((int16_t*)(data+20))=scenery->anim_delay;
*((int16_t*)(data+22))=scenery->anim_parameters;
*((int16_t*)(data+24))=scenery->anim_frames;

uint32_t pos=0x1C;
string_table_encode(&(scenery->name),data+pos);
pos+=name_table_length;
group_info_encode(&(scenery->group_info),data+pos);
pos+=16;
animation_indices_encode(&(scenery->animation_indices),data+pos);
pos+=animation_indices_length;
image_list_encode(&(scenery->sprites),data+pos);
return ERROR_NONE;
}
