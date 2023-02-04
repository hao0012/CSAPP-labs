#ifndef CACHE_H_
#define CACHE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define LOAD_INSTRUCTION 'I'
#define LOAD_DATA 'L'
#define STORE_DATA 'S'
#define MODIFY_DATA 'M'

typedef struct operation {
    char type;
    uint64_t signal;
    uint64_t index;
    uint64_t offset;
    int count;
} Operation;

Operation* init_operation(char type, uint64_t op, int count);

typedef struct line {
    bool valid;
    uint64_t signal;
    // uint64_t block;
    int last_visit;
} Line;

/* a group in cache */
#define Group Line *
#define Cache Group *

Cache* init_cache(Cache **cache_p);

void free_cache(Cache *cache);

/* visit cache */
bool visit(Cache *cache, Operation* op);

/* load data from lower level to cache  */
void load_into_cache(Cache *cache, Operation *op);

/* substitute a line, using LRU or LFU */
void substitute(Group *group, Operation* op);

/* Least Recently Used */
void LRU(Group *group, Operation* op);

void operate(Cache *cache, Operation* op);

#endif
