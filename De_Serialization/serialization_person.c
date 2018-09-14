
#include <stdlib.h>
#include "person_t.h"
#include "serialize.h"
#include <stdio.h>
#include <memory.h>
#include "sentinel.h"

/*Serialization function*/
void serialize_person_t(person_t *obj, ser_buff_t *b);
void serialize_company_t(company_t *obj, ser_buff_t *b);

/*DeSerialization function*/
company_t *de_serialize_company_t(ser_buff_t *b);
person_t *de_serialize_person_t(ser_buff_t *b);

/*print functions*/
void print_person(person_t *obj);
void print_company(company_t *obj);

/*Seserializing routine for company_t structure*/
company_t *de_serialize_company_t(ser_buff_t *b){

    /*In the beginning of ever derialization routine, always write
     * sentinel detection code*/

    /*sentinel detection code begin*/
    SENTINEL_DETECTION_CODE(b);
    /*sentinel detection code end*/

    /*Deserialization starts*/
    company_t *obj = calloc(1, sizeof(company_t));
    de_serialize_data((char *)obj->comp_name, b, 32);
    de_serialize_data((char *)&obj->emp_strength, b, sizeof(int));
    obj->CEO = de_serialize_person_t (b);
    return obj;
}

person_t *de_serialize_person_t(ser_buff_t *b){

    int loop_var = 0;
    unsigned int sentinel = 0;

    /*In the beginning of ever derialization routine, always write
     * sentinel detection code*/
    /*sentinel detection code  begin*/
    SENTINEL_DETECTION_CODE(b);
    /*sentinel detection code end*/

    person_t *obj =  calloc(1, sizeof(person_t));

    for(loop_var = 0 ; loop_var < 4; loop_var++){
        de_serialize_data((char *)&obj->vehicle_nos[loop_var], b, 
                                    sizeof(unsigned int));
    }

    de_serialize_data((char *)&obj->age, b, sizeof(int));
    de_serialize_data((char *)&sentinel, b, sizeof(unsigned int));
    if(sentinel == 0xFFFFFFFF){
        obj->height = NULL;
    }
    else{
        serialize_buffer_skip(b, -1 * sizeof(unsigned int));
        obj->height = calloc(1, sizeof(int));
        de_serialize_data((char *)obj->height, b, sizeof(int));
    }

   for(loop_var = 0 ; loop_var < 5; loop_var++){
       
       de_serialize_data((char *)&sentinel, b, sizeof(unsigned int));
       if(sentinel == 0xFFFFFFFF){
            obj->last_salary_amounts[loop_var] = NULL;
       }
       else{
            serialize_buffer_skip(b, -1 * sizeof(unsigned int));
            obj->last_salary_amounts[loop_var] = calloc(1, sizeof(unsigned int)); 
            de_serialize_data((char *)obj->last_salary_amounts[loop_var], b,
                sizeof(unsigned int));
       }
   }

   de_serialize_data((char *)obj->name, b, 32);

   company_t *company = de_serialize_company_t(b);
   obj->company = *company; /*shallow copy because obj->company is not a pointer*/
   free(company); /*shallow free*/

   for(loop_var = 0 ; loop_var < 3; loop_var++){
       company =  de_serialize_company_t(b);
       obj->dream_companies[loop_var] = *company;
       free(company);
   }

   obj->CEO = de_serialize_person_t(b); 

   for(loop_var = 0 ; loop_var < 5; loop_var++){
       obj->administrative_staff[loop_var] = de_serialize_person_t(b);
   }
   return obj;
}

void
serialize_company_t(company_t *obj, ser_buff_t *b){

    SENTINEL_INSERTION_CODE(obj,b);

    serialize_data(b, (char *)obj->comp_name, 32);
    serialize_data(b, (char *)&obj->emp_strength, sizeof(int));
    serialize_person_t(obj->CEO, b);
}









































void
serialize_person_t(person_t *obj, ser_buff_t *b){


    int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    /* In the beginning of every Serialization routine, always encode 
     * 0XFFFFFFFF in the serialized buffer if the object being serialized
     * is NULL
     *
     * This is Sentinel insertion code*/
    SENTINEL_INSERTION_CODE(obj,b);

    for(loop_var = 0 ; loop_var < 4; loop_var++){
        serialize_data(b, (char *)&obj->vehicle_nos[loop_var], sizeof(int));
    }
    /*Alternatively*/
    /*
     * serialize_data(b, (char *)obj->vehicle_nos, sizeof(int) * 4);
     * */
    serialize_data(b, (char *)&obj->age, sizeof(unsigned int));
    if(obj->height)
        serialize_data(b, (char *)obj->height, sizeof(int));
    else
        serialize_data(b, (char *)&sentinel, sizeof(unsigned int));

    for(loop_var = 0 ; loop_var < 5; loop_var++){
        if(obj->last_salary_amounts[loop_var])
            serialize_data(b, (char *)obj->last_salary_amounts[loop_var],
                    sizeof(unsigned int));
        else{
            serialize_data(b, (char *)&sentinel, sizeof(unsigned int));
        }
    }

    serialize_data(b, (char *)obj->name, 32);
    serialize_company_t(&obj->company, b);

    for(loop_var = 0 ; loop_var < 3 ; loop_var++){
        serialize_company_t(&obj->dream_companies[loop_var], b);
    }

    serialize_person_t(obj->CEO, b);

    for(loop_var = 0 ; loop_var < 5; loop_var++){
        serialize_person_t(obj->administrative_staff[loop_var], b);
    }
}









































void
print_company(company_t *obj){

    if(!obj) return;
    printf("comp_name = %s\n", obj->comp_name);
    printf("emp_strength = %d\n", obj->emp_strength);
    print_person(obj->CEO);
}

void 
print_person(person_t *obj){

    if(!obj) return;

    int loop_var = 0;
    for(loop_var = 0; loop_var < 4; loop_var++){
        printf("vehicle no[%d] = %d\n", loop_var, obj->vehicle_nos[loop_var]);
    }

     printf("age = %d\n", obj->age);

     if(obj->height){
        printf("height = %d\n", *obj->height);
     }
     else{
        printf("height = NULL\n");
     }

    for(loop_var = 0; loop_var < 5; loop_var++){
        if(obj->last_salary_amounts[loop_var]){
            printf("last_salary_amounts[%d] = %d\n", 
                loop_var, *obj->last_salary_amounts[loop_var]);
        }
        else{
            printf("last_salary_amounts[%d] = Nil\n",
                loop_var);
        }
    }
    printf("name = %s\n", obj->name);
    print_company(&obj->company);

    /*Lets stop !*/
}


int 
main(int argc, char **argv){

    person_t p1;
    memset(&p1, 0, sizeof(person_t));    

    p1.vehicle_nos[0] = 1000;
    p1.vehicle_nos[1] = 1001;
    p1.vehicle_nos[2] = 1002;
    p1.vehicle_nos[3] = 1003;

    p1.age = 31;

    p1.height = calloc(1, sizeof(int));
    *p1.height = 167;

    p1.last_salary_amounts[0] = NULL;
    p1.last_salary_amounts[1] = NULL;
    p1.last_salary_amounts[2] = calloc(1, sizeof(unsigned int));
    *p1.last_salary_amounts[2] = 20000;
    p1.last_salary_amounts[3] = calloc(1, sizeof(unsigned int));
    *p1.last_salary_amounts[3] = 40000;
    p1.last_salary_amounts[4] = NULL;

    strncpy(p1.name, "Abhishek", strlen("Abhishek"));

    strncpy(p1.company.comp_name, "Juniper", strlen("Juniper"));
    p1.company.emp_strength = 10000;

    p1.company.CEO = NULL;

    /*Enough !! lests stop here, and try to serialize it*/

    printf("printing the object to be serialized on sending machine\n\n");
    print_person(&p1);


    ser_buff_t *b;
    init_serialized_buffer(&b);

    /*Serialize the person_t object. It will recirsively
     * serialize all internal sub-structures*/
    serialize_person_t (&p1, b);

    /*Now assume that we have sent the serialized buffer b
     * to recieving machine*/


    /* Recieving machine has recieved the serialized buffer, 
     * Lets derialize it and reconstruct the object from it*/

    /*reset the serialized buffer to read it from beginning again*/
    reset_serialize_buffer(b);

    person_t *p2 = de_serialize_person_t(b);

    /*We are done with serialized buffer, free it*/
    free_serialize_buffer(b);
    b = NULL;
    
    /*Verify whether recieving machine has successfuly reconstructed the
     * person_t object or not*/

    printf("\nprinting the deserialized object on recieving machine\n\n");
    print_person(p2);

    /*Match the output of reconstructed object p2 with the p1. They should be exactly same.
     * If not same, there is some error in serialization/deserialization code*/

    /*free the p2 structure, including all internal structures, feeling sleepy
     * pls do it !!*/

    return 0;
}
