#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "settings.h"
#include "block_manager.h"
#include "html_parsing.h"
#include "blocks_serving.h"

// TODO errors tracebacks

int serve_block(Block_manager *block_manager, FILE *html_template, FILE *output_file, unsigned int line, char *buffer, \
                    char *block_name_start, char *request_url, long start_pos) {
    char block_name[LINE_BUFFER_SIZE];
    char *block_name_end = find_block_end(block_name_start);

    if ( !block_name_end ) {
        fprintf(stderr, "%s Block not closed with '%s'! (html line %u)\n", ERROR_PREFIX, BLOCK_POSTFIX, line);
        return 0;
    }

    if ( !parse_block_name(block_name_start, block_name) ) {
        fprintf(stderr, "%s Missing block name inside '%s %s' block! (html line %u)\n", ERROR_PREFIX, BLOCK_PREFIX, BLOCK_POSTFIX, line);
        return 0;
    }

    if (strcmp(block_name, END_NAME) == 0) {
        if ( !handle_block_end(block_manager, html_template, output_file, line, block_name_end, request_url) ) {
            return 0;
        }
    } else {
        Block *current_block = block_top(block_manager);
        if (current_block && strcmp(current_block->name, block_name) == 0) {
            if ( !handle_block_repeat(output_file, line, buffer, block_name_end) ) {
                return 0;
            }
        } else {
            if ( !handle_block_open(block_manager, output_file, line, buffer, block_name_start, block_name, block_name_end, request_url, start_pos) ) {
                return 0;
            }
        }
    }
    return 1;
}


int serve_tag(FILE *output_file, unsigned int line, char *buffer, char *tag_name_start, char *request_url) {
    char search_url[NAME_BUFFER_SIZE];
    char *tag_name_end = find_tag_end(tag_name_start);
    char tag_name[LINE_BUFFER_SIZE];
    char json_value[NAME_BUFFER_SIZE];

    if ( !tag_name_end ) {
        fprintf(stderr, "%s Tag not closed with '%s'! (html line %u)\n", ERROR_PREFIX, TAG_POSTFIX, line);
        return 0;
    }

    strcpy(search_url, request_url);
    if (parse_tag_name(tag_name_start, tag_name)) {
        strcat(search_url, ".");
        strcat(search_url, tag_name);
    }

    strcpy(json_value, "TEST"); // TODO Add funcfrom json parser

    if (replace_tag(output_file, line, buffer, tag_name_start, tag_name_end, json_value)) {
        return 1;
    }
    fprintf(stderr, "%s Replacing tag to value unsuccessful! (html line %u)\n", ERROR_PREFIX, line);
    return 0;
}


int process_line(Block_manager *block_manager, FILE *html_template, FILE *output_file, unsigned int line, char *buffer, char *request_url, long start_pos) {
    char *block_start = find_block_start(buffer);
    char *tag_start   = find_tag_start(buffer);

    if (block_start) {
        if (serve_block(block_manager, html_template, output_file, line, buffer, block_start, request_url, start_pos)) {
            return 3;
        } else {
            fprintf(stderr, "%s Processing line unsuccessful! (html line %u)\n", ERROR_PREFIX, line);
        }
    } else if (tag_start) {
        if (serve_tag(output_file, line, buffer, tag_start, request_url)) {
            return 2;
        } else {
            fprintf(stderr, "%s Processing line unsuccessful! (html line %u)\n", ERROR_PREFIX, line);
        }
    } else {
        fputs(buffer, output_file);
        return 1;
    }
    return 0;
}



int template_to_html(FILE *html_template, char *target_filename) {
    if ( !html_template ) {
        fprintf(stderr, "%s Wrong pinter to html template!\n", ERROR_PREFIX);
        return 0;
    }

    FILE *output_file = fopen(target_filename, "w");
    if ( !output_file ) {
        perror(target_filename);
        return 0;
    }

    char buffer[LINE_BUFFER_SIZE];
    char request_url[NAME_BUFFER_SIZE] = {0};
    Block_manager block_manager = {0};

    unsigned line = 0;
    long line_start_pos = ftell(html_template);
    while (fgets(buffer, LINE_BUFFER_SIZE, html_template)) {
        ++line;
        if ( !process_line(&block_manager, html_template, output_file, line, buffer, request_url, line_start_pos) ) {
            return 0;
        }
        line_start_pos = ftell(html_template);
    }

    destroy_block_manager(&block_manager);
    fclose(output_file);
    return 1;
}

int main() {
    FILE *test_template = fopen("../../templates/template.html", "r");
    
    if (test_template) {
        template_to_html(test_template, "index.html");
        fclose(test_template);
    }
    
    return 0;
}