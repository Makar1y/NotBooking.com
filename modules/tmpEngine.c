#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// [ ] Add documentation
#define LINE_BUFFER_SIZE 256 // Max buffer length (while scaning template file)
#define MAX_REQUEST_URL_LENGTH 256

#define ERROR_PREFIX  "!!! ERROR: "


int template_to_html(FILE *html_template, char *target_filename) {
    char *buffer = malloc(sizeof(char) * LINE_BUFFER_SIZE);
    char *request_url = calloc(MAX_REQUEST_URL_LENGTH, sizeof(char));

    while (fgets(buffer, LINE_BUFFER_SIZE, html_template)) {
        char *block_name_start = strstr(buffer, "{%");

        if (block_name_start) {
            char *block_name_end = strstr(buffer, "%}");

            if (block_name_end) {
                char block_name[LINE_BUFFER_SIZE];
                if (sscanf(block_name_start, "{%% %[^%{}] %%}", block_name)) {
                    printf("Ok, %s\n", block_name);
                } else {
                    fprintf(stderr, "%sNo block name provided.\n", ERROR_PREFIX);
                }
            } // TODO Error
        } // TODO Error
    }
    printf("%s", buffer);
}

int main() {
    FILE *fptr = fopen("../templates/template.html", "r+");
    
    if (fptr) {
        template_to_html(fptr, "");
    }
    return 0;
}