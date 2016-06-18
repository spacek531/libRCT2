
object_t* object_new_ride()
{
object_t* object=malloc(sizeof(object_t));
object->type=OBJECT_RIDE;
object->object_header=ride_header_new();
object->string_tables[0]=string_table_new();
object->string_tables[1]=string_table_new();
object->string_tables[2]=string_table_new();
object->optional=ride_structures_new();
object->images=image_list_new();
return object;
}


ride_header_t* ride_header_new()
{
int i;
ride_header_t* ride_header=malloc(sizeof(ride_header_t));
memset(ride_header,0,sizeof(ride_header_t));
    for(i=0;i<NUM_CARS;i++)
    {
    ride_header->cars[i].unknown[0]=1;
    ride_header->cars[i].unknown[1]=0;
    ride_header->cars[i].unknown[2]=0;
    }
return ride_header;
}
ride_header_t* ride_header_load(uint8_t* bytes,uint32_t* pos_ptr)
{
int i;

ride_header_t* ride_header=malloc(sizeof(ride_header_t));
/*Read flags*/
ride_header->flags=*((uint32_t*)(bytes+8));

/*Read track style & preview index*/
ride_header->preview_index=0;
    for(i=12;i<=14;i++)
    {
        if(bytes[i]!=0xFFu)
        {
        ride_header->track_style=bytes[i];
        break;
        }
    ride_header->preview_index++;
    }

/*Read zero cars*/
ride_header->zero_cars=bytes[18];
/*Read car icon index*/
ride_header->car_icon_index=bytes[19];
/*Read minimum and maximum car length*/
ride_header->minimum_cars=bytes[15];
ride_header->maximum_cars=bytes[16];
/*Read track sections*/
ride_header->track_sections=*((uint64_t*)(bytes+438));
/*Read excitement, intensity, nausea*/
ride_header->excitement=bytes[434];
ride_header->intensity=bytes[435];
ride_header->nausea=bytes[436];
/*Read max height*/
ride_header->max_height=bytes[437];
/*Read categories*/
ride_header->categories[0]=bytes[446];
ride_header->categories[1]=bytes[447];

/*Read car type information*/
ride_header->car_types[CAR_INDEX_DEFAULT]=bytes[20];
ride_header->car_types[CAR_INDEX_FRONT]=bytes[21];
ride_header->car_types[CAR_INDEX_SECOND]=bytes[22];
ride_header->car_types[CAR_INDEX_REAR]=bytes[23];
ride_header->car_types[CAR_INDEX_THIRD]=bytes[24];

/*Read car structures*/
uint8_t* car_data=bytes+26;
    for(i=0;i<NUM_CARS;i++)
    {
    car_t* car=ride_header->cars+i;
    /*Load highest rotation index*/
    car->highest_rotation_index=car_data[0];
    /*Load spacing*/
    car->spacing=*((uint32_t*)(car_data+4));
    /*Load car friction*/
    car->friction=*((uint16_t*)(car_data+8));
    /*Load riders*/
    car->rider_pairs=car_data[11]&0x80;
    car->riders=car_data[11]&0x7F;
    /*Load sprite flags*/
    car->sprites=*((uint16_t*)(car_data+12));
    /*Load rider sprites*/
    car->rider_sprites=car_data[84];
    /*Load flags*/
    car->flags=*((uint32_t*)(car_data+17));
    /*Load "extra frames*/
    car->extra_swing_frames=car_data[21];
    /*Load spin parameters*/
    car->spin_inertia=car_data[85];
    car->spin_friction=car_data[86];
    /*Write sound effects*/
    car->running_sound=car_data[87];
    car->secondary_sound=car_data[89];
    /*Load powered velocity*/
    car->powered_acceleration=car_data[91];
    car->powered_velocity=car_data[92];
    /*Load Z value*/
    car->z_value=car_data[95];
    /*Read unknown fields*/
    car->unknown[0]=(uint16_t)car_data[94];
    car->unknown[1]=*((uint16_t*)(car_data+96));
    car->unknown[2]=(uint16_t)car_data[88];
    car->unknown[3]=(uint16_t)car_data[90];
    car->unknown[4]=(uint16_t)car_data[10];
    car->unknown[5]=(uint16_t)car_data[14];
    car->unknown[6]=(uint16_t)car_data[15];
    car->unknown[7]=(uint16_t)car_data[16];
    car->unknown[8]=(uint16_t)car_data[93];
    /*Move to next car structure*/
    car_data+=101;
    }
*pos_ptr+=0x1C2;
return ride_header;
}
void ride_header_write(ride_header_t* header,buffer_t* buffer)
{
int i;
uint8_t header_bytes[0x1C2];
memset(header_bytes,0,0x1C2);

/*Write flags*/
*((uint32_t*)(header_bytes+8))=header->flags;
/*Write track style & preview index*/
memset(header_bytes+12,0xFFu,3);
header_bytes[12+header->preview_index]=header->track_style;
/*Write zero cars*/
header_bytes[18]=header->zero_cars;
/*Write car icon index*/
header_bytes[19]=header->car_icon_index;
/*Write minimum and maximum car length*/
header_bytes[15]=header->minimum_cars;
header_bytes[16]=header->maximum_cars;
/*Write track sections*/
*((uint64_t*)(header_bytes+438))=header->track_sections;

/*Write excitement, intensity, nausea*/
header_bytes[434]=header->excitement;
header_bytes[435]=header->intensity;
header_bytes[436]=header->nausea;
/*Write max height*/
header_bytes[437]=header->max_height;
/*Write categories*/
header_bytes[446]=header->categories[0];
header_bytes[447]=header->categories[1];
/*Write car type information*/
header_bytes[20]=header->car_types[CAR_INDEX_DEFAULT];
header_bytes[21]=header->car_types[CAR_INDEX_FRONT];
header_bytes[22]=header->car_types[CAR_INDEX_SECOND];
header_bytes[23]=header->car_types[CAR_INDEX_REAR];
header_bytes[24]=header->car_types[CAR_INDEX_THIRD];
/*Write values that are known, but not applicable*/
header_bytes[17]=0xFF;/*Flat ride "cars"*/
header_bytes[448]=0xFF;/*First shop item*/
header_bytes[449]=0xFF;/*Second shop item*/

/*Write car types*/
uint8_t* car_data=header_bytes+26;
    for(i=0;i<NUM_CARS;i++)
    {
    car_t* car=header->cars+i;
    /*Write highest rotation index*/
    car_data[0]=car->highest_rotation_index;
    /*Write spacing*/
    *((uint32_t*)(car_data+4))=car->spacing;
    /*Write friction*/
    *((uint16_t*)(car_data+8))=car->friction;
    /*Write riders*/
    car_data[11]=car->rider_pairs|car->riders;
    /*Write sprite flags*/
    *((uint16_t*)(car_data+12))=car->sprites;
    /*Write rider sprites*/
    car_data[84]=car->rider_sprites;
    /*Write flags*/
    *((uint32_t*)(car_data+17))=car->flags;
    /*Write "extra frames"*/
    car_data[21]=car->extra_swing_frames;
    /*Write spin parameters*/
    car_data[85]=car->spin_inertia;
    car_data[86]=car->spin_friction;
    /*Write sound effects*/
    car_data[87]=car->running_sound;
    car_data[89]=car->secondary_sound;


    /*Write powered velocity*/
    car_data[91]=car->powered_acceleration;
    car_data[92]=car->powered_velocity;
    /*Write Z value*/
    car_data[95]=car->z_value;
    /*Write unknown fields*/
    car_data[94]=(uint8_t)car->unknown[0];
    *((uint16_t*)(car_data+96))=car->unknown[1];
    car_data[88]=(uint8_t)car->unknown[2];
    car_data[90]=(uint8_t)car->unknown[3];
    car_data[10]=(uint8_t)car->unknown[4];
    car_data[14]=(uint8_t)car->unknown[5];
    car_data[15]=(uint8_t)car->unknown[6];
    car_data[16]=(uint8_t)car->unknown[7];
    car_data[93]=(uint8_t)car->unknown[8];
    /*Move to next car*/
    car_data+=101;
    }
buffer_write(buffer,header_bytes,0x1C2);
}
void ride_header_free(ride_header_t* header)
{
free(header);
}

ride_structures_t* ride_structures_new()
{
int i;
ride_structures_t* structures=malloc(sizeof(ride_structures_t));
structures->num_default_colors=1;
structures->default_colors=malloc(sizeof(color_scheme_t));
structures->default_colors->colors[0]=0;
structures->default_colors->colors[1]=0;
structures->default_colors->colors[2]=0;
    for(i=0;i<4;i++)
    {
    structures->peep_positions[i].num=0;
    structures->peep_positions[i].positions=NULL;
    }
return structures;
}
void ride_structures_set_num_peep_positions(ride_structures_t* structures,int car,int num)
{
structures->peep_positions[car].num=num;
structures->peep_positions[car].positions=malloc(num);
memset(structures->peep_positions[car].positions,0,num);
}
void ride_structures_set_num_default_colors(ride_structures_t* structures,int num)
{
structures->default_colors=realloc(structures->default_colors,sizeof(color_scheme_t)*num);
    for(int i=structures->num_default_colors;i<num;i++)
    {
    structures->default_colors[i].colors[0]=0;
    structures->default_colors[i].colors[1]=0;
    structures->default_colors[i].colors[2]=0;
    }
structures->num_default_colors=num;
}
ride_structures_t* ride_structures_load(uint8_t* bytes,uint32_t* pos_ptr)
{
int i;
int pos=*pos_ptr;
ride_structures_t* structures=malloc(sizeof(ride_structures_t));
/*Sequence of 3 byte structures representing the default color schemes; I don't know why -1 means 32*/
structures->num_default_colors=bytes[pos]==0xFF?32:bytes[pos];
structures->default_colors=malloc(structures->num_default_colors*sizeof(color_scheme_t));
pos++;

	for(i=0;i<structures->num_default_colors;i++)
    {
    structures->default_colors[i].colors[0]=bytes[pos];
    structures->default_colors[i].colors[1]=bytes[pos+1];
    structures->default_colors[i].colors[2]=bytes[pos+2];
    pos+=3;
    }

    /*Four variable length structures*/
    for (i=0;i<4;i++)
    {
    uint16_t len=bytes[pos];
    pos++;
        /*If the length can't fit in a byte, the length is instead stored in the two bytes following*/
        if (len==0xFF)
        {
        len=*((uint16_t*)(bytes+pos));
        pos+=2;
        }
    structures->peep_positions[i].num=len;
    structures->peep_positions[i].positions=malloc(structures->peep_positions[i].num);
    int j;
        for(j=0;j<len;j++)
        {
        structures->peep_positions[i].positions[j]=bytes[pos];
        pos++;
        }
    }
*pos_ptr=pos;
return structures;
}
void ride_structures_write(ride_structures_t* structures,buffer_t* buffer)
{
int i;
/*Write number of 3 byte structures*/
buffer_write(buffer,&structures->num_default_colors,1);
/*Write 3 byte structures*/
    for(i=0;i<structures->num_default_colors;i++)
    {
    buffer_write(buffer,&structures->default_colors[i].colors[0],1);
    buffer_write(buffer,&structures->default_colors[i].colors[1],1);
    buffer_write(buffer,&structures->default_colors[i].colors[2],1);
    }
/*Write 4 byte peep positioning structures*/
    for(i=0;i<4;i++)
    {
    /*If the length will fit in a byte, write it*/
        if(structures->peep_positions[i].num<0xFFu)buffer_write(buffer,(uint8_t*)&structures->peep_positions[i].num,1);
    /*Otherwise write as a short*/
        else
        {
        uint8_t value=0xFFu;
        buffer_write(buffer,&value,1);
        buffer_write(buffer,(uint8_t*)&structures->peep_positions[i].num,2);
        }
    /*Write structure*/
    buffer_write(buffer,structures->peep_positions[i].positions,structures->peep_positions[i].num);
    }
}
void ride_structures_free(ride_structures_t* structures)
{
int i;
    for(i=0;i<4;i++)free(structures->peep_positions[i].positions);
free(structures);
}


void object_ride_load(object_t* object,uint8_t* bytes,uint32_t* pos_ptr)
{
object->type=OBJECT_RIDE;
/*Load object header*/
object->object_header=ride_header_load(bytes,pos_ptr);
/*Load string tables*/
        for(int i=0;i<NUM_STRING_TABLES;i++)
        {
        object->string_tables[i]=string_table_load(bytes,pos_ptr);
        }
/*Load unknown strutures*/
object->optional=ride_structures_load(bytes,pos_ptr);
/*Load images*/
object->images=image_list_load(bytes,pos_ptr);
}

void object_ride_write(object_t* object,buffer_t* buffer)
{
/*Write header*/
ride_header_write(object->ride_header,decoded_file);
/*Write string tables*/
    for(i=0;i<NUM_STRING_TABLES;i++)
    {
    string_table_write(object->string_tables[i],decoded_file);
    }
/*Write unknown structures*/
ride_structures_write(object->optional,decoded_file);
/*Write images*/
image_list_write(object->images,decoded_file);
}
void object_free_ride(object_t* object)
{
ride_header_free(object->object_header);
string_table_free(object->string_tables[0]);
string_table_free(object->string_tables[1]);
string_table_free(object->string_tables[2]);
ride_structures_free(object->optional);
image_list_free(object->images);

}
