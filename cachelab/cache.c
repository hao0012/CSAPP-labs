#include "cache.h"
#include<stdio.h>

extern int hit, miss, eviction;

extern int time_stamp;

/* name is align with CSAPP book */
/* length of address */
const int m = 64;
// bit length of index
int s;
// bit length of offset
int b;
// bit length of signal
int t;
// number of line each group
int E;
// number of group in cache
int S;

bool v = false;

uint64_t OFFSET_MASK;
uint64_t SIGNAL_MASK;
uint64_t INDEX_MASK;

Operation* init_operation(char type, uint64_t addr, int count) {
    Operation* op = (Operation *) malloc(sizeof(Operation));
    op->type = type;
    op->count = count;
    op->offset = addr & (OFFSET_MASK);
    op->index = (addr & INDEX_MASK) >> b;
    op->signal = (addr & SIGNAL_MASK) >> (b + s);
    return op;
}

Cache* init_cache(Cache** cache_p) {
    Cache *c = (*cache_p) = (Cache *) malloc(sizeof(Group) * S);
    if (!c) return NULL;
    for (int i = 0; i < S; i++) {
        Group *g = c[i] = (Group *) malloc(sizeof(Line*) * E);
        if (!g) return NULL;
        for (int j = 0; j < E; j++) {
            Line *l = g[j] = (Line *)malloc(sizeof(Line));
            l->valid = false;
            l->last_visit = -1;
        }
    }
    return *cache_p;
}

void free_cache(Cache *cache) {
    for (int i = 0; i < S; i++) {
        Group *g = cache[i];
        for (int j = 0; j < E; j++) {
            free(g[j]);
        }
        free(g);
    }
    free(cache);
}

bool visit(Cache *cache, Operation *op) {
    if (op->index >= S) {
        fprintf(stderr, "group out of range\n");
        exit(0);
    }
    Group *g = cache[op->index];
    for (int i = 0; i < E; i++) {
        Line *l = g[i];
        if (l->valid && l->signal == op->signal) {
            l->last_visit = time_stamp;
            return true;
        }
    }
    return false;
}

void load_into_cache(Cache *cache, Operation *op) {
    Group *g = cache[op->index];
    for (int i = 0; i < E; i++) {
        Line *l = g[i];
        if (!l->valid) {
            l->valid = true;
            l->signal = op->signal;
            return;
        }
    }
    substitute(g, op);
}

void substitute(Group *group, Operation* op) {
    LRU(group, op);
    eviction++;
    if (v) printf(" eviction");
}

void LRU(Group *group, Operation* op) {
    Line *l = group[0];
    int min = l->last_visit, j = 0;
    for (int i = 1; i < E; i++) {
        l = group[i];
        if (l->last_visit < min) {
            min = l->last_visit;
            j = i;
        }
    }
    l = group[j];
    l->valid = true;
    l->last_visit = -1;
    l->signal = op->signal;
}

void operate(Cache *cache, Operation* op) {
    /* visit cache (read or write) */
    if (!visit(cache, op)) {
        miss++;
        if (v) printf("miss");
        load_into_cache(cache, op);
        visit(cache, op);
    } 
    else {
        hit++;
        if (v) printf("hit");
    }
    if (op->type == MODIFY_DATA) {
        /* write */
        visit(cache, op);
        hit++;
        if (v) printf(" hit");
    }
    if (v) printf("\n");
}