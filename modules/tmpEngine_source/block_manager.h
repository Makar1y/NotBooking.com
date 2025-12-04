#ifndef BLOCK_MANAGER_H
#define BLOCK_MANAGER_H

typedef struct {
    long start_pos;
    unsigned int left_repeats;
    char* name;
} Block;

typedef struct {
    Block *blocks;
    unsigned int size;
} Block_manager;

void destroy_block_manager(Block_manager *bm);
int block_push(Block_manager *bm, const char *name, long start_pos, unsigned int repeats);
int block_pop(Block_manager *bm);
Block *block_top(Block_manager *bm);

#endif