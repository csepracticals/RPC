#include <stdio.h>
#include "serialize.h"
#include <memory.h>
#include <stdlib.h>
#include <assert.h>

void 
init_serialized_buffer(ser_buff_t **b){
    (*b) = (ser_buff_t *)calloc(1, sizeof(ser_buff_t));
    (*b)->b = calloc(1, SERIALIZE_BUFFER_DEFAULT_SIZE);
    (*b)->size = SERIALIZE_BUFFER_DEFAULT_SIZE;
    (*b)->next = 0;
    return;
}

void
init_serialized_buffer_of_defined_size(ser_buff_t **b, int size){
    (*b) = (ser_buff_t *)calloc(1, sizeof(ser_buff_t));
    (*b)->b = calloc(1, size);
    (*b)->size = size;
    (*b)->next = 0;
}


char
is_serialized_buffer_empty(ser_buff_t *b){
	if(b->next == 0)	
		return 1;
	return 0;
}

void free_serialize_buffer(ser_buff_t *b){
	free(b->b);
	free(b);
}

int get_serialize_buffer_length(ser_buff_t *b){
	return b->size;
}

int get_serialize_buffer_current_ptr_offset(ser_buff_t *b){
	if(!b) return -1;
	return  b->next;
}

char* get_serialize_buffer_current_ptr(ser_buff_t *b){
	if(!b) return NULL;
	return  b->b + b->next;
}

int  get_serialize_buffer_data_size(ser_buff_t *b){
	return b->next;
}

void
serialize_buffer_skip(ser_buff_t *b, int size){
	int available_size = b->size - b->next;

	if(available_size >= size){
		b->next += size;
		return;
	}

	while(available_size < size){
		b->size = b->size * 2;
		available_size = b->size - b->next;
	}	

	b->b = realloc(b->b, b->size);	
	b->next += size;
}


void serialize_data(ser_buff_t *b, char *data, int nbytes){
    
    if (b == NULL) assert(0);

    ser_buff_t *buff = (ser_buff_t *)(b);
    int available_size = buff->size - buff->next;
    char isResize = 0;

    while(available_size < nbytes){
        buff->size = buff->size * 2;
        available_size = buff->size - buff->next;
        isResize = 1;
    }

    if(isResize == 0){
        memcpy((char *)buff->b + buff->next, data, nbytes);
        buff->next += nbytes;
        return;
    }

    // resize of the buffer
    buff->b = realloc(buff->b, buff->size); 
    memcpy((char *)buff->b + buff->next, data, nbytes);
    buff->next += nbytes;
    return;
}

void truncate_serialize_buffer(ser_buff_t **b){
	ser_buff_t *clone = NULL;

	if((*b)->next == (*b)->size){
		return;
	}
	init_serialized_buffer_of_defined_size(&clone, (*b)->next);
	memcpy(clone->b, (*b)->b, (*b)->next);
	clone->next = clone->size;
	free_serialize_buffer(*b);
	*b = clone;
}


void print_buffer_details(ser_buff_t *b, const char *fn, int lineno){

    printf("%s():%d : starting address = 0x%x\n", fn, lineno, (unsigned int)b);
    printf("size = %d\n", b->size);
    printf("next = %d\n", b->next);
}

/* de serialize fn */

void 
de_serialize_data(char *dest, ser_buff_t *b, int size){
	if(!b || !b->b) assert(0);
	if(!size) return;	
	if((b->size - b->next)< size) assert(0);	 

	memcpy(dest, b->b + b->next, size);
	b->next += size;
}

void copy_in_serialized_buffer_by_offset(ser_buff_t *b, 
                                         int size, 
                                         char *value, 
                                         int offset){
	if(offset > b->size){
		printf("%s(): Error : Attemt to write outside buffer boundaries\n", __FUNCTION__);
		return;
	}
	
	memcpy(b->b + offset, value, size);
}

void reset_serialize_buffer(ser_buff_t *b){
	b->next = 0;	
}
