#include "cachelab.h"
#include "cache.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

char* options = "hvs:E:b:t:";

char *help = 
"Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
"Options: \n"
"  -h         Print this help message. \n"
"  -v         Optional verbose flag. \n"
"  -s <num>   Number of set index bits. \n"
"  -E <num>   Number of lines per set. \n"
"  -b <num>   Number of block offset bits. \n"
"  -t <file>  Trace file. \n\n"
"Examples: \n"
"  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace \n"
"  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n"
;

char *invalid_input_prompt = "./csim-ref: invalid option -- ";

void parse_file(FILE *file, Cache *cache);
Operation* parse_line(char *line);

/* see cache.c */
extern int m, s, b, t, E, S, v;
extern uint64_t OFFSET_MASK;
extern uint64_t SIGNAL_MASK;
extern uint64_t INDEX_MASK;

int hit, miss, eviction;
// simulate system time
int time_stamp = 0;
// FILE* s1;

int main(int argc, char **argv)
{
    char c;
    FILE *file;
    // s1 = fopen("traces/s1.trace", "a+");
    while ((c=getopt(argc, argv, options)) != -1) {
        switch (c)
        {
        case 'v':
            v = true;
            break;
        case 'h':
            printf("%s", help);
            break;
        case 't':
            if (!(file=fopen(optarg, "r"))) {
                fprintf(stderr, "%s: No such file or directory\n", optarg);
                exit(0);
            }
            break;
        case 's':
            s = atoi(optarg);
            S = pow(2, s);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg); 
            break;
        default:
            fprintf(stderr, "%s'%c'\n%s", invalid_input_prompt, c, help);
            exit(0);
        }
    }
    t = m - s - b;
    OFFSET_MASK = (~((1 << (m - 1)) >> (m - b - 1)));
    SIGNAL_MASK = ((1 << (m - 1)) >> (t - 1));
    INDEX_MASK = ~(OFFSET_MASK + SIGNAL_MASK);
    /* initilize cache */
    Cache *cache = NULL;
    if (!init_cache(&cache)) {
        fprintf(stderr, "allocate memory for cache failed.\n");
        exit(0);
    }
    /* parse input file and operate cache */
    parse_file(file, cache);
    free_cache(cache);
    // fclose(s1);

    printSummary(hit, miss, eviction);
    return 0;
}

void parse_file(FILE *file, Cache *cache) {
    size_t len = 16;
    char buffer[len];
    while (fgets(buffer, len, file)) {
        time_stamp++;
        Operation* op = parse_line(buffer);
        if (op) {
            operate(cache, op);

        }
        free(op);
    }
}

Operation* parse_line(char* line) {
    int len = strlen(line);
    if (len == 0 || line[0] == LOAD_INSTRUCTION) 
        return NULL;
    /* get operation type */
    char type = line[1];
    /* get address */
    char buf[16]; int i = 2, j = 0;
    while (line[i] != ',') {
        buf[j++] = line[i++];
    }
    buf[j] = '\0';
    uint64_t addr = strtoull(buf, NULL, 16);
    /* get number of read bytes */
    i++; j = 0;
    while (i < len) {
        buf[j++] = line[i++];
    }
    buf[j] = '\0';
    int count = atoi(buf);
        if (v) printf("%c %" PRIx64",%d ", type, addr, count);
    Operation *op = init_operation(type, addr, count);
    // if (op->index == 3) {
    //     // printf("%" PRIu64"\n", addr);
    //     fputs(line, s1);
    // }
    return op;
}