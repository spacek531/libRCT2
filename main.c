#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "object.h"



int main()
{
FILE* file=fopen("ObjData/ARRT1.DAT","rb");


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
ride_t ride;
error=ride_decode(&ride,&(object.chunk));
    if(error!=ERROR_NONE)
    {
    object_destroy(&object);
    printf("Error: %s\n",error_string(error));
    return 1;
    }


printf("%x\n",ride.vehicles[ride.front_vehicle].flags);
//ride.flags=48409;
ride.vehicles[ride.default_vehicle].flags=0x800000;//890000
ride.vehicles[ride.default_vehicle].var_11=1;//890000
ride.vehicles[ride.default_vehicle].sprites=1;


ride.track_types[0]=4;
ride.categories[0]=2;

object_t object2;
object2.flags=0;//object.flags;
object2.checksum=object.checksum;
memcpy(object2.name,"TEST    ",8);
object_destroy(&object);

uint8_t* data;uint32_t length;
error=ride_encode(&ride,ENCODING_RLE,&(object2.chunk));
    if(error!=ERROR_NONE)
    {
    printf("Error: %s\n",error_string(error));
    }

file=fopen("ObjData/TEST.DAT","w");
    if(file==NULL)
    {
    printf("Could not open file for writing\n");
    return 1;
    }
error=object_write(&object2,file);
    if(error!=ERROR_NONE)
    {
    printf("Could not write file\n");
    object_destroy(&object2);
    return 1;
    }

object_destroy(&object2);
return 0;

/*
FILE* file=fopen(".DAT","rb");
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
    object_destroy(&object);
    printf("Error: %s\n",error_string(error));
    return 1;
    }
//	for(int i=0;i<NUM_LANGUAGES;i++)scenery.name.strings[i]=NULL;
//scenery.name.strings[LANGUAGE_ENGLISH_UK]="Calcite Block";

object_t object2;
object2.flags=object.flags;
object2.checksum=object.checksum;
memcpy(object2.name,"XXBQRK01",8);
object_destroy(&object);


uint8_t* data;uint32_t length;
error=small_scenery_encode(&scenery,ENCODING_RLE,&(object2.chunk));
    if(error!=ERROR_NONE)
    {
    printf("Error: %s\n",error_string(error));
    }

//small_scenery_destroy(&small_scenery);

file=fopen("CLCTB0.DAT","w");
    if(file==NULL)
    {
    printf("Could not open file for writing\n");
    return 1;
    }
error=object_write(&object2,file);
    if(error!=ERROR_NONE)
    {
    printf("Could not write file\n");
    object_destroy(&object2);
    return 1;
    }

object_destroy(&object2);
return 0;
*/
}
