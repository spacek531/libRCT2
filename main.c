#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "dat.h"

//TODO Insert bounds checks in image read



int main()
{
const char* filename="ObjData/BUTTFLY.DAT";

FILE* file=fopen(filename,"rb");
    if(file==NULL)
    {
    printf("Could not open file\n");
    return 1;
    }

object_t object;
error_t error=object_read(&object,file);
    if(error!=ERROR_NONE)
    {
    printf("Error: %s\n",error_string(error));
    return 1;
    }
fclose(file);


small_scenery_t scenery;
error=small_scenery_decode(&scenery,&(object.chunk));
    if(error!=ERROR_NONE)
    {
    printf("Error: %s\n",error_string(error));
    }


object_t object2=object;
object2.flags=object.flags;
object2.checksum=object.checksum;
memcpy(object2.name,object.name,8);

//uint8_t* data;uint32_t length;
//chunk_decode(&(object.chunk),&data,&length);
//chunk_encode(&(object2.chunk),data,length);



error=small_scenery_encode(&scenery,&(object2.chunk));
    if(error!=ERROR_NONE)
    {
    printf("Error: %s\n",error_string(error));
    }

file=fopen("BUTTFLY2.DAT","w");
error=object_write(&object2,file);
    if(error!=ERROR_NONE)
    {
    printf("Could not open file\n");
    return 1;
    }
object_destroy(&object);
return 0;
}
