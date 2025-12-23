#include <stdio.h>
#include <assert.h>
#include <string.h>

#define JSON_DATA "data/data.json"
#define HTML_TEMPLATE "./data/template.html"
#define TMP_FILENAME "./tmp_file"
#define ERROR_PREFIX "!!! Error ->"

#define MAX_LINE_LENGTH 5000
#define MAX_JSON_NAME_LENGTH 256

#define JSON_NAME_SCAN_FORMAT "%[^\"]"

void my_perror(char* message) {
    char error_m[MAX_LINE_LENGTH] = ERROR_PREFIX;
    strcat(error_m, " ");
    strcat(error_m, message);
    perror(error_m);
}


int remove_hotel(FILE* data_json, const char const* name) {
    if (NULL == data_json) {
        data_json = fopen(JSON_DATA, "r+");
        if (NULL == data_json) {
            my_perror(JSON_DATA);
            return 1;
        }
    }

    FILE* new_file = fopen(TMP_FILENAME, "w");
    if (NULL == new_file) {
        my_perror(TMP_FILENAME);
        return 1;
    }

    int line = 0;
    int skip = 0;
    char buffer[MAX_LINE_LENGTH];
    char read_name[MAX_JSON_NAME_LENGTH];

    while (fgets(buffer, MAX_LINE_LENGTH, data_json)) {
        ++line;
        char* name_start = strstr(buffer, "\"name\"");

        if (name_start) {
            name_start = strstr(name_start + 7, "\""); // 7 is length of "name":
            int scanned = sscanf(name_start + 1, JSON_NAME_SCAN_FORMAT, read_name); 

            if (scanned) {
                if ( 0 == strcmp(read_name, name) ) {
                    skip = 1;
                    printf("Removing %s hotel..\n", read_name);
                } else {
                    skip = 0;
                }
            } else {
                fprintf(stderr, "%s invalid syntax on line %d in %s\n", ERROR_PREFIX, line, JSON_DATA);
            }
        }

        if ( 1 != skip) {
            fputs(buffer, new_file);
        }
    }

    fclose(new_file);
    fclose(data_json);
    
    if ( 0 != remove(JSON_DATA)) {
        fprintf(stderr, "%s Cant delete %s file for replacing to updated version.\n", ERROR_PREFIX, JSON_DATA);
        return 1;
    }

    if ( 0 != rename(TMP_FILENAME, JSON_DATA) ) {
        fprintf(stderr, "%s Cant rename tmp file(%s) to original path(%s).\n", ERROR_PREFIX, TMP_FILENAME, JSON_DATA);
        return 1;
    }

    printf("Found(if any) hotels removed.\n");
    return 0;
}
