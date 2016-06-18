#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
/*#include "renderer.h"*/
#include "dat.h"

void* malloc_or_die(size_t size)
{
void* data=malloc(size);
    if(data==NULL)
    {
    fputs("Memory allocation failed\n",stderr);
    exit(1);
    }
return data;
}

char* error_string(error_t error)
{
    switch(error)
    {
    case ERROR_NONE:
    return "None\n";
    break;
    case ERROR_FILE_OPERATION_FAILED:
    return "File operation failed\n";
    break;
    case ERROR_INVALID_CHUNK:
    return "Chunk is invalid\n";
    break;
    case ERROR_INVALID_OBJECT:
    return "Object is invalid\n";
    break;
    case ERROR_INVALID_ENCODING:
    return "Chunk has invalid encoding type\n";
    break;
    case ERROR_PREMATURE_EOF:
    return "File ended prematurely\n";
    break;
    case ERROR_CHECKSUM_MISMATCH:
    return "Checksum mismatch\n";
    break;
    case ERROR_INVALID_LANGUAGE:
    return "Invalid language code in string table";
    break;
    default:
    case ERROR_PREMATURE_END_OF_CHUNK:
    return "Decoded chunk data ended prematurely";
    break;
    case ERROR_SCANLINE_OUT_OF_BOUNDS:
    return "Sprite contained pixels outside the bounds of the image";
    break;
    return "Invalid error value\n";
    break;
    }
}

static uint32_t checksum_process_byte(uint32_t checksum,uint8_t byte)
{
uint32_t checksum_higher_bits=checksum&0xFFFFFF00u;
uint32_t checksum_lower_bits=checksum&0xFFu;
checksum_lower_bits^=byte;
checksum=checksum_higher_bits|checksum_lower_bits;
return (checksum<<11)|(checksum>>21);
}
static uint32_t calculate_checksum(uint8_t* header,uint8_t* data,uint32_t size)
{
uint32_t i;
/*Check checksum*/
uint32_t checksum=0xF369A75B;
checksum=checksum_process_byte(checksum,header[0]);/*Do first byte*/
    for(i=4;i<12;i++)checksum=checksum_process_byte(checksum,header[i]);/*Do filename*/
    for(i=0;i<size;i++)checksum=checksum_process_byte(checksum,data[i]);/*Checksum rest file*/;
return checksum;
}
static void calculate_salt(uint32_t current_checksum,uint32_t target,uint8_t* salt)
{
target^=(current_checksum<<25)|(current_checksum>>7);
salt[0]=(target&0x00000001)<<7;;
salt[1]=((target&0x00200000)>>14);
salt[2]=((target&0x000007F8)>>3);
salt[3]=((target&0xFF000000)>>24);
salt[4]=((target&0x00100000)>>13);
salt[5]=(target&0x00000004)>>2;
salt[6]=0;
salt[7]=((target&0x000FF000)>>12);
salt[8]=(target&0x00000002)>>1;
salt[9]=(target&0x00C00000)>>22;
salt[10]=(target&0x00000800)>>11;
}



error_t string_table_decode(string_table_t* table,uint8_t* data,uint32_t data_length,uint32_t* length)
{
    if(length==0)return ERROR_PREMATURE_END_OF_CHUNK;

//Initialize all strings to NULL
    for(int i=0;i<NUM_LANGUAGES;i++)table->strings[i]=NULL;

uint32_t pos=0;
    while(pos<data_length&&data[pos]!=0xFF)
    {
    //Load language
    uint8_t language=data[pos++];

    uint32_t string_length=0;
        while(pos+string_length<data_length&&data[pos+string_length]!=0)string_length++;

        //There must be at least one extra byte after the string to terminate the string table
        if(pos+string_length+1>=data_length||language>=NUM_LANGUAGES)
        {
        //Note string is not fully constructed at this point but is in a valid state. If the implementation changes it may no longer be appropriate to call this here.
        string_table_destroy(table);
        return language>=NUM_LANGUAGES?ERROR_INVALID_LANGUAGE:ERROR_PREMATURE_END_OF_CHUNK;
        }

    //Copy string to table
    table->strings[language]=malloc_or_die(string_length+1);
    memcpy(table->strings[language],data+pos,string_length+1);

    pos+=string_length+1;
    }
//Total length of the string table is the length of the last byte plus 1
*length=pos+1;
return ERROR_NONE;
}
uint32_t string_table_get_encoded_length(string_table_t* table)
{

uint32_t length=0;
    for(int i=0;i<NUM_LANGUAGES;i++)
    {
        if(table->strings[i]!=NULL)
        {
        length+=strlen((char*)table->strings[i])+2;
        }
    }
return length+1;
}
//Does not check bounds - string_table_get_encoded_length MUST be called first to determine how much data this will write.
void string_table_encode(string_table_t* table,uint8_t* data)
{
uint32_t pos=0;
    for(uint32_t i=0;i<NUM_LANGUAGES;i++)
    {
        if(table->strings[i]!=NULL)
        {
        data[pos++]=i;
        uint32_t length=strlen((char*)table->strings[i])+1;
        memcpy(data+pos,table->strings[i],length);
        pos+=length;

        }
    }
data[pos]=0xFF;
}
void string_table_destroy(string_table_t* table)
{
    for(int i=0;i<NUM_LANGUAGES;i++)
    {
        if(table->strings[i]!=NULL)free(table->strings[i]);
    }
}



error_t group_info_decode(group_info_t* group_info,uint8_t* data,uint32_t data_length)
{
    if(data_length<16)return ERROR_PREMATURE_END_OF_CHUNK;
group_info->flags=*((uint32_t*)data);
memcpy(group_info->name,data+4,8);
return ERROR_NONE;
}
void group_info_encode(group_info_t* group_info,uint8_t* data)
{
*((uint32_t*)data)=group_info->flags;
memcpy(data+4,group_info->name,8);
*((uint32_t*)(data+12))=0;
}


static error_t direct_bitmap_decode(image_t* image,uint8_t* data,uint32_t data_length)
{
    if(image->width*image->height>data_length)return ERROR_PREMATURE_END_OF_CHUNK;
memcpy(image->data,data,image->width*image->height);
return ERROR_NONE;
}
static uint32_t direct_bitmap_get_encoded_length(image_t* image)
{
return image->width*image->height;
}
static void direct_bitmap_encode(image_t* image,uint8_t* data,uint32_t* length)
{
memcpy(data,image->data,image->width*image->height);
*length=image->width*image->height;
}

static error_t compressed_bitmap_decode(image_t* image,uint8_t* data,uint32_t data_length)
{
memset(image->data,0,image->width*image->height);

//Check there are enough bytes for row offset table
    if(data_length<2*image->height)return ERROR_PREMATURE_END_OF_CHUNK;
            for(int row=0;row<image->height;row++)
			{
            //The data starts with a list of two byte offsets to the individual row data.
            uint32_t row_offset=((uint16_t*)data)[row];

            uint8_t last_one;
				do
				{
                //Check there are enough bytes for scanline header
                    if(row_offset+2>data_length)return ERROR_PREMATURE_END_OF_CHUNK;
                //Read scanline header
                uint8_t length=data[row_offset++];
                uint8_t x_displacement=data[row_offset++];

                //The most significant bit of the length is set only if this is the last element
                last_one=length&0x80;
                length=length&0x7F;
                //Check that this element isn't outside the bounds of the image
                    if(x_displacement+length>image->width)return ERROR_SCANLINE_OUT_OF_BOUNDS;
                //Check there are enough bytes for the scanline contents
                    if(row_offset+length>data_length)return ERROR_PREMATURE_END_OF_CHUNK;
                //Copy scanline contents to image
                memcpy(image->data+row*image->width+x_displacement,data+row_offset,length);
                row_offset+=length;
				}while (!last_one);
			}
return ERROR_NONE;
}
static uint32_t compressed_bitmap_get_encoded_length(image_t* image)
{
uint32_t length=image->height*2;
            for(int i=0;i<image->height;i++)
            {
            uint8_t* row=image->data+i*image->width;

            uint8_t x_displacement=0;
                do
                {
                //Find first non transparent pixel
                    while(x_displacement<image->width&&row[x_displacement]==0)x_displacement++;
                    if(x_displacement==image->width)break;
                //Find length. Sections can't be longer that 126 for some reason
                uint32_t scanline_length=0;
                    while(x_displacement+scanline_length<image->width&&row[x_displacement+scanline_length]!=0&&scanline_length<126)scanline_length++;
                x_displacement=x_displacement+scanline_length;
                length+=scanline_length+2;
                }while(x_displacement!=image->width);
            }
return length;
}
static void compressed_bitmap_encode(image_t* image,uint8_t* data,uint32_t* length)
{
uint32_t pos=image->height*2;
            for(int i=0;i<image->height;i++)
            {
             uint8_t* row=image->data+i*image->width;
            //Write row offset
            ((uint16_t*)data)[i]=pos;

            uint8_t x_pos=0;
                while(x_pos<image->width&&row[x_pos]==0)x_pos++;
                while(x_pos!=image->width)
                {
                //Find length of scanline
                uint8_t scanline_length=0;
                    while(x_pos+scanline_length<image->width&&row[x_pos+scanline_length]!=0&&scanline_length<126)scanline_length++;
                uint8_t x_displacement=x_pos;
                //Locate start of next scanline
                x_pos+=scanline_length;
                    while(x_pos<image->width&&row[x_pos]==0)x_pos++;
                data[pos++]=(x_pos==image->width)?(scanline_length|0x80):scanline_length;
                data[pos++]=x_displacement;
                memcpy(data+pos,row+x_displacement,scanline_length);
                pos+=scanline_length;
                }
            }
*length=pos;
}

error_t image_list_decode(image_list_t* image_list,uint8_t* data,uint32_t data_length)
{
//Get number of images
image_list->num_images=*((uint32_t*)data);
//Calculate offset of start of bitmap data
uint32_t bitmap_base=8+(image_list->num_images*16);
//Allocate images
image_list->images=malloc_or_die(image_list->num_images*sizeof(image_t));

uint32_t pos=8;
    for(uint32_t i=0;i<image_list->num_images;i++)
    {
    //Compute offset of start of graphic data for this image
    uint32_t offset=bitmap_base+*((uint32_t*)(data+pos));
    //Load image header
    image_t* image=image_list->images+i;
    image->width=*((uint16_t*)(data+pos+4));
    image->height=*((uint16_t*)(data+pos+6));
    image->x_offset=*((uint16_t*)(data+pos+8));
    image->y_offset=*((uint16_t*)(data+pos+10));
    image->flags=*((uint16_t*)(data+pos+12));
    image->data=malloc_or_die(image->width*image->height);
    pos+=16;

    //Read pixel data
    error_t error;
        if(offset>=data_length)error=ERROR_PREMATURE_END_OF_CHUNK;
		else if (image->flags&0x4)error=compressed_bitmap_decode(image,data+offset,data_length-offset);
        else error=direct_bitmap_decode(image,data+offset,data_length-offset);
    //If an error is encountered then free everything allocated
        if(error)
        {
            for(uint32_t j=0;j<=i;j++)free(image_list->images[i].data);
        free(image_list->images);
        return error;
        }

    }
return 0;
}
uint32_t image_list_get_encoded_length(image_list_t* list)
{
uint32_t length=8+list->num_images*16;
    for(int i=0;i<list->num_images;i++)
    {
        if(list->images[i].flags&0x4)length+=compressed_bitmap_get_encoded_length(list->images+i);
        else length+=direct_bitmap_get_encoded_length(list->images+i);
    }
return length;
}
void image_list_encode(image_list_t* list,uint8_t* data)
{
//Offset at which to write next bitmap
uint32_t bitmap_base=8+list->num_images*16;
uint32_t bitmap_pos=bitmap_base;
//Offset at which to write next header
uint32_t header_pos=8;
    for(int i=0;i<list->num_images;i++)
    {
    image_t* image=list->images+i;

    uint32_t bitmap_length;
        if(image->flags&0x4)compressed_bitmap_encode(image,data+bitmap_pos,&bitmap_length);
        else direct_bitmap_encode(image,data+bitmap_pos,&bitmap_length);

    //Load image header
    *((uint32_t*)(data+header_pos))=bitmap_pos-bitmap_base;
    *((uint16_t*)(data+header_pos+4))=image->width;
    *((uint16_t*)(data+header_pos+6))=image->height;
    *((uint16_t*)(data+header_pos+8))=image->x_offset;
    *((uint16_t*)(data+header_pos+10))=image->y_offset;
    *((uint16_t*)(data+header_pos+12))=image->flags;
    *((uint16_t*)(data+header_pos+14))=0;
    header_pos+=16;
    bitmap_pos+=bitmap_length;
    }
//Allocate images
//Write number of images
*((uint32_t*)data)=list->num_images;
//Write size of graphic data
*((uint32_t*)(data+4))=bitmap_pos-bitmap_base;
}
/*
void image_list_write(image_list_t* list,uint8_t* data)
{
uint32_t i,j;
//Write number of images
buffer_write(buffer,(uint8_t*)&list->num_images,4);
//Save the offset of the graphic data size field, so we can write it later
uint32_t graphic_data_size_offset=buffer->size;
buffer_expand(buffer,4);
//Save offset of image records to write later
int image_records_offset=buffer->size;
buffer_expand(buffer,16*list->num_images);

uint32_t bitmap_base=buffer->size;
//Write images
    for(i=0;i<list->num_images;i++)
    {
    image_t* image=list->images[i];
    //Write image record
    uint32_t start_address=buffer->size-bitmap_base;
    uint8_t* image_record_ptr=buffer->data+image_records_offset+i*16;
    *((uint32_t*)(image_record_ptr))=start_address;
    *((uint16_t*)(image_record_ptr+4))=image->width;
    *((uint16_t*)(image_record_ptr+6))=image->height;
    *((uint16_t*)(image_record_ptr+8))=image->x_offset;
    *((uint16_t*)(image_record_ptr+10))=image->y_offset;
    *((uint16_t*)(image_record_ptr+12))=image->flags;
    *((uint16_t*)(image_record_ptr+14))=0;
    //Write image data
        if(image->flags==1)
        {
            for(j=0;j<image->height;j++)buffer_write(buffer,image->data[j],image->width);
        }
        else //image->flags==5
        {
        //Store offset of the list of row offsets, and leave space for them in the buffer
        uint32_t row_offset_list_offset=buffer->size;
        buffer_expand(buffer,image->height*2);
            for(j=0;j<image->height;j++)
            {
            //Write offset of row into row offset list
            *((uint16_t*)(buffer->data+row_offset_list_offset+j*2))=buffer->size-row_offset_list_offset;

            uint8_t length=0;
            uint8_t x_displacement=0;
            //Calculate initial x_displacement
                while(x_displacement<image->width&&image->data[j][x_displacement]==0)x_displacement++;

                do
                {
                length=0;
                //Find length. Sections can't be longer that 126 for some reason
                    while(x_displacement+length<image->width&&image->data[j][x_displacement+length]!=0&&length<126)length++;
                //Compute new x_displacement
                uint8_t new_x_displacement=x_displacement+length;
                    while(new_x_displacement<image->width&&image->data[j][new_x_displacement]==0)new_x_displacement++;
                //If there are no more non transparent pixels, set last one flat
                    if(new_x_displacement==image->width)length|=0x80;
                //Write length
                buffer_write(buffer,&length,1);
                //Write x displacement
                buffer_write(buffer,&x_displacement,1);
                //Write data
                buffer_write(buffer,image->data[j]+x_displacement,length&0x7F);
                //Update the x displacement
                x_displacement=new_x_displacement;
                }while(!(length&0x80));
            }
        }
    }
//Write size of graphic data
uint32_t graphic_data_size=buffer->size-bitmap_base;
memcpy(buffer->data+graphic_data_size_offset,&graphic_data_size,4);
}
void image_list_set_image(image_list_t* list,int index,image_t* image)
{
    if(list->images[index]!=NULL)image_free(list->images[index]);
list->images[index]=image;
}
void image_list_set_num_images(image_list_t* list,uint32_t num_images)
{
uint32_t i;
    if(num_images<list->num_images)for(i=num_images;i<list->num_images;i++)image_free(list->images[i]);
list->images=realloc(list->images,num_images*sizeof(image_t*));
    if(num_images>list->num_images)for(i=list->num_images;i<num_images;i++)list->images[i]=NULL;
list->num_images=num_images;
}
void image_list_free(image_list_t* list)
{
uint32_t i;
    for(i=0;i<list->num_images;i++)image_free(list->images[i]);
free(list->images);
free(list);
}
*/


error_t object_read(object_t* object,FILE* file)
{
//Read object header
uint8_t header[16];
    if(fread(header,16,1,file)!=1)
    {
    error_t error=feof(file)?ERROR_PREMATURE_EOF:ERROR_FILE_OPERATION_FAILED;
    fclose(file);
    return error;
    };
object->flags=*((uint32_t*)header);
memcpy(object->name,header+4,8);
object->checksum=*((uint32_t*)(header+12));

//Read chunk
error_t error=chunk_read(&(object->chunk),file);
    if(error!=ERROR_NONE)
    {
    fclose(file);
    return error;
    }
return ERROR_NONE;
}
error_t object_write(object_t* object,FILE* file)
{
    if(fwrite(&(object->flags),4,1,file)!=1)return ERROR_FILE_OPERATION_FAILED;
    if(fwrite(&(object->name),1,8,file)!=8)return ERROR_FILE_OPERATION_FAILED;
    if(fwrite(&(object->checksum),4,1,file)!=1)return ERROR_FILE_OPERATION_FAILED;
return chunk_write(&(object->chunk),file);
}



void object_destroy(object_t* object)
{
chunk_destroy(&(object->chunk));
}
//Validate checksum
//    if(calculate_checksum(header,chunk_bytes,chunk_length)!=*((uint32_t*)(header+12)))
    //{
    //chunk_free(&chunk);
    //return ERROR_CHECKSUM_MISMATCH;
   // }

//file=fopen("decoded.DAT","w");
//fwrite(chunk_bytes,chunk_length,1,file);
//fclose(file);
/*
    switch(header[0]&0xF)
    {
    case OBJECT_RIDE:
        //object_ride_load(object,bytes->data,&pos);
    break;
    case OBJECT_SMALL_SCENERY:
        //if(object_small_scenery_read(object,chunk_bytes,chunk_length))goto error;
    break;

    default:
    assert(0);
    break;
    }
return ERROR_NONE;
}

int object_save_dat(object_t* object,uint8_t* name,uint32_t checksum)
{
    if(strlen((char*)name)>8)return 1;


    switch(object->type)
    {
    case OBJECT_RIDE:
    object_ride_write(object,decoded_file);
    break;
    case OBJECT_SMALL_SCENERY:
    object_small_scenery_write(object,decoded_file);
    break;
    }

//Create header
uint8_t header[HEADER_SIZE];
memset(header,0,HEADER_SIZE);

//Write object type
header[0]=object->type;
//Write filename in header
uint8_t* header_filename=header+4;
memset(header_filename,' ',8);
    for(i=start_index;i<=end_index;i++)
    {
    header_filename[i-start_index]=toupper(filename[i]);
    }
//Specify that data is compressed
header[0x10]=1;

//Write checksum forcing salt
uint32_t cur_checksum=calculate_checksum(header,decoded_file->data,decoded_file->size);
uint8_t* salt=decoded_file->data+decoded_file->size;
buffer_expand(decoded_file,11);
calculate_salt(cur_checksum,checksum,salt);

//Calculate checksum - commented out because checksum is now passed as a parameter
//uint32_t checksum=calculate_checksum(header,decoded_file->data,decoded_file->size);
//Write checksum
*((uint32_t*)(header+12))=checksum;


//Encode file with RLE
buffer_t* encoded_bytes=compress_data(decoded_file->data,decoded_file->size);
//Write file size
*((uint32_t*)(header+17))=encoded_bytes->size;

//Free decode data
buffer_free(decoded_file);


FILE* file=fopen(filename,"wb");
fwrite(header,1,HEADER_SIZE,file);
fwrite(encoded_bytes->data,1,encoded_bytes->size,file);
fclose(file);

buffer_free(encoded_bytes);
printf("File saved with filename %s\n",filename);
}
*/



