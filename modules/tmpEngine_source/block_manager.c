#include "block_manager.h"
#include <stdlib.h>
#include <string.h>


void destroy_block_manager(Block_manager *bm) {
    for (unsigned i = 0; i < bm->size; i++)
        free(bm->blocks[i].name);
    free(bm->blocks);
}

Block *block_top(Block_manager *bm) {
    if (bm->size == 0)
        return NULL;
        
    return &bm->blocks[bm->size - 1];
}

int block_push(Block_manager *bm, const char *name, long start_pos, unsigned int repeats) {
    Block *tmp = realloc(bm->blocks, sizeof(Block) * (bm->size + 1));
    if (!tmp)
        return 0;

    bm->blocks = tmp;
    Block *new = &bm->blocks[bm->size];
    bm->size++;

    char *copied_name = malloc((strlen(name) + 1) * sizeof(char));
    if (!copied_name)
        return 0;
    strcpy(copied_name, name);

    new->name = copied_name;
    new->start_pos = start_pos;
    new->left_repeats = repeats;

    return 1;
}

int block_pop(Block_manager *bm) {
    if (bm->size == 0)
        return 0;

    free(bm->blocks[bm->size - 1].name);
    bm->size--;
    return 1;
}
