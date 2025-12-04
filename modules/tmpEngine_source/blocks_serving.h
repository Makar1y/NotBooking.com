#ifndef BLOCK_SERVING_H
#define BLOCK_SERVING_H

int handle_block_open(Block_manager *block_manager, FILE *output_file, unsigned int line, char *buffer, \
                       char *block_name_start, char *block_name, char *block_name_end, char *request_url, long start_pos);

int handle_block_repeat(FILE *output_file, unsigned int line, char *buffer, char *block_name_end);

int handle_block_end(Block_manager *block_manager, FILE *template_file, FILE *output_file, unsigned int line, \
                     char *block_name_end, char *request_url);

int replace_tag(FILE *output_file, unsigned int line, char *buffer, char *tag_name_start, char *tag_name_end, char *json_value);
#endif