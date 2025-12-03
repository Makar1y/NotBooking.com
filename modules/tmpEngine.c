#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// TODO Add documentation

// Max buffer length while scaning template file (must be greater than symbols in longest line)
#define LINE_BUFFER_SIZE 256
// [ ]
#define MAX_REQUEST_URL_LENGTH 256

#define END_NAME "end"
#define ERROR_PREFIX  "!!! ERROR: "
#define BLOCK_PREFIX "{%"
#define BLOCK_POSTFIX "%}"
#define BLOCK_NAME_SCAN_FORMAT "{%% %[^%{} ] %%}"
#define TAG_PREFIX "{{"
#define TAG_POSTFIX "}}"
#define TAG_NAME_SCAN_FORMAT "{{ %[^{} ] }}"



typedef struct {
    unsigned int line;
    unsigned int left_repeats;
    char* block_name;
} Block;

typedef struct {
    Block *blocks;
    unsigned int size;
} Block_manager;

Block_manager block_manager = {0};
char request_url[MAX_REQUEST_URL_LENGTH];



void serve_block(char *buffer, char *block_name_start, unsigned int line, FILE *template_file, FILE *output_file) {
    char *block_name_end = strstr(block_name_start, BLOCK_POSTFIX);

    if (block_name_end) {
        char *block_name = malloc(LINE_BUFFER_SIZE * sizeof(char));

        if (sscanf(block_name_start, BLOCK_NAME_SCAN_FORMAT, block_name)) {
            if ( strcmp(block_name, END_NAME) != 0 ) { // If not end of block
                if ( !block_manager.blocks || strcmp(block_manager.blocks[block_manager.size - 1].block_name, block_name) != 0) { // If first repeat of block
                    block_manager.size++;
                    Block *tmp_blocks = realloc(block_manager.blocks, block_manager.size * sizeof(Block));
                    
                    if ( !tmp_blocks ) { 
                        free(block_name);
                        block_manager.size--; // [ ] error
                        fprintf(stderr, "%sFailed to allocate memory for blocks in serve_block!\n", ERROR_PREFIX);
                        exit(EXIT_FAILURE);
                    }
                    block_manager.blocks = tmp_blocks;
                    block_manager.blocks[block_manager.size - 1].line = line;
                    block_manager.blocks[block_manager.size - 1].block_name = block_name;
                    block_manager.blocks[block_manager.size - 1].left_repeats = 3; // TODO Implement counts from json parser
                    
                    strcat(request_url, ".");
                    strcat(request_url, block_name);
                    
                    fwrite(buffer, sizeof(char), block_name_start - buffer, output_file);
                    fputs(block_name_end  + strlen(BLOCK_POSTFIX), output_file); // [ ] add ERROR
                    
                    fwrite(buffer, sizeof(char), block_name_start - buffer, output_file);
                    fputs(block_name_end  + strlen(BLOCK_POSTFIX), output_file); // [ ] add ERROR
                } else { // If repeating write html after block
                    fputs(block_name_end  + strlen(BLOCK_POSTFIX), output_file); // [ ] add ERROR
                }
            } else { // If end of block
                if (block_manager.blocks) {
                    if (block_manager.blocks[block_manager.size - 1].left_repeats) {
                        fseek(template_file, 0, SEEK_SET);
                        char unused_buffer[LINE_BUFFER_SIZE];
                        
                        for (unsigned int i = 0; i < block_manager.blocks[block_manager.size - 1].line - 1; ++i) {
                            fgets(unused_buffer, LINE_BUFFER_SIZE, template_file);
                        }
                        
                        block_manager.blocks[block_manager.size - 1].left_repeats--;
                        return; // Not print 
                    } else {
                        request_url[strlen(request_url) - strlen(block_manager.blocks[block_manager.size - 1].block_name) - 1] = '\0' ;
                        free(block_manager.blocks[block_manager.size - 1].block_name);
                        block_manager.size--;
                        fputs(block_name_end + strlen(BLOCK_POSTFIX), output_file); // [ ] add ERROR
                    }
                } else {
                    ; // [ ] Error
                }
            }
        } else { // Error
            fprintf(stderr, "%sNo block name provided after opened '%s' in template file on line %u !\n", ERROR_PREFIX, BLOCK_PREFIX, line);
        }
    } else { // Error
        fprintf(stderr, "%sBlock not closed with '%s' in template file on line %u!\n", ERROR_PREFIX, BLOCK_POSTFIX, line);
    }
}


void template_to_html(FILE *html_template, char *target_filename) {
    char *buffer = malloc(sizeof(char) * LINE_BUFFER_SIZE);
    FILE *output_file = fopen(target_filename, "w");
    unsigned int line = 0;

    while (fgets(buffer, LINE_BUFFER_SIZE, html_template)) {
        ++line;
        char *block_name_start = strstr(buffer, BLOCK_PREFIX); // If start of block in line
        char *tag_name_start = strstr(buffer, TAG_PREFIX); // if start of tag

        if (*request_url)
            printf("%u, %s \n", line, request_url);

        if (block_name_start) { // block serving
            serve_block(buffer, block_name_start, line, html_template, output_file);
        }

        #if 0
        if (tag_name_start) { // tag serving
            char *tag_name_end = strstr(buffer, TAG_POSTFIX);

            if (tag_name_end) {
                char tag_name[LINE_BUFFER_SIZE];

                if (sscanf(tag_name_start, TAG_NAME_SCAN_FORMAT, tag_name)) {
                    printf("%s %u, ", tag_name, line);
                    
                } else { // TODO Array elements
                    fprintf(stderr, "%sNo tag name provided after opened '%s' in template file on line %u !\n", ERROR_PREFIX, TAG_PREFIX, line);
                } // Empty tag
            } else { fprintf(stderr, "%sTag not closed with '%s' in template file on line %u !\n", ERROR_PREFIX, TAG_POSTFIX, line); } // Error
        }
        #endif

        if ( !block_name_start) { // && !tag_name_start // TODO
            fputs(buffer, output_file);
        }
    }

    // Cleaning
    free(buffer);
    if (block_manager.blocks) {
        free(block_manager.blocks);
    }
    block_manager.blocks = NULL;
    block_manager.size = 0;
    fclose(output_file);
}

int main() {
    FILE *test_template = fopen("../templates/template.html", "r");
    
    if (test_template) {
        template_to_html(test_template, "index.html");
        fclose(test_template);
    }
    
    return 0;
}