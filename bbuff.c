#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "bbuff.h"

// Global variables
void* candyBuff[BUFFER_SIZE];
int full;
int empty;
int current;



void bbuff_init(void) {
    
    //initial all variables
    full = 0;
    current = 0;
    empty = BUFFER_SIZE;
 

}

void bbuff_blocking_insert(void* p) {
 
    int a = current + full;
    int b = empty + full;
    candyBuff[a % b] = p;
    full = full+ 1;
    empty = empty -1;


}

void* bbuff_blocking_extract(void) {
    
    int temp = full+empty;

    void* result = candyBuff[current];
    current = (current + 1) % temp;
    full =full -1;
    empty =empty + 1;
 
    return result;
}

_Bool bbuff_is_empty(void) {
    return (full == 0);
}


