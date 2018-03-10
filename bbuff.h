#ifndef BBUFF_H
#define BBUFF_H

#define BUFFER_SIZE 10


typedef struct {
   int factory_number;
   double time_stamp_in_ms;
} candy_t;

void bbuff_init(void);
void bbuff_blocking_insert(void* candy);
void* bbuff_blocking_extract(void);
_Bool bbuff_is_empty(void);


#endif
