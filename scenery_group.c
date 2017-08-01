#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "object.h"

error_t scenery_group_member_info_decode(scenery_group_member_info_t* member_info,uint8_t* data,uint32_t data_length,uint32_t* length)
{
member_info->num_members=0;

int pos=0;
	while(pos<data_length&&data[pos]!=0xFF)
	{
	member_info->num_members++;
	pos+=16;
	}
	if(pos>=data_length)return ERROR_PREMATURE_END_OF_CHUNK;

member_info->members=malloc_or_die(member_info->num_members*sizeof(scenery_group_member_t));
	for(int i=0;i<member_info->num_members;i++)
	{
	member_info->members[i].flags=*((uint32_t*)(data+16*i));
	member_info->members[i].checksum=*((int16_t*)(data+16*i+12));
	memcpy(member_info->members[i].name,data+16*i+4,8);
	}
*length=pos+1;
return ERROR_NONE;
}

error_t scenery_group_decode(scenery_group_t* scenery_group,chunk_t* chunk)
{
error_t error;
//Load header
	if(chunk->length<0x10E)return ERROR_PREMATURE_END_OF_CHUNK;
//TODO load header
uint32_t length;
uint32_t pos=0x10E;
//Load string table
error=string_table_decode(&(scenery_group->name),chunk->data+pos,chunk->length-pos,&length);
	if(error!=ERROR_NONE)return error;
pos+=length;

//Load member info
error=scenery_group_member_info_decode(&(scenery_group->member_info),chunk->data+pos,length-pos,&length);
	if(error!=ERROR_NONE)
	{
	string_table_destroy(&(scenery_group->name));
	return error;
	}
pos+=length;

//Load images
error=image_list_decode(&(scenery_group->sprites),chunk->data+pos,chunk->length-pos);
	if(error!=ERROR_NONE)
	{
	string_table_destroy(&(scenery_group->name));
	free(scenery_group->member_info.members);
	return error;
	}
return ERROR_NONE;
}
/*error_t scenery_group_encode(scenery_group_t* scenery_group,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(scenery_group->name));
uint32_t sprites_length=image_list_get_encoded_length(&(scenery_group->sprites));
uint32_t length=0x1E +name_table_length+sprites_length;

//Allocate memory
uint8_t* data=malloc_or_die(length);
memset(data,0,length);

//Write header
data[6]=scenery_group->cursor_sel;
data[7]=scenery_group->flags;
data[8]=scenery_group->clearance;
data[9]=scenery_group->effects;
*((int16_t*)(data+10))=scenery_group->build_fee;
data[13]=scenery_group->scrolling;

uint32_t pos=0xE;
string_table_encode(&(scenery_group->name),data+pos);
pos+=name_table_length;
group_info_encode(&(scenery_group->group_info),data+pos);
pos+=16;
image_list_encode(&(scenery_group->sprites),data+pos);
chunk_encode(chunk,encoding,data,length);
free(data);
return ERROR_NONE;
}*/

void scenery_group_destroy(scenery_group_t* scenery_group)
{
string_table_destroy(&(scenery_group->name));
image_list_destroy(&(scenery_group->sprites));
free(scenery_group->member_info.members);
}
