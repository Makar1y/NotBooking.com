#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "modules/json_object/json_object.h"

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

int filter_hotels_by_price(FILE* data_json, double min_price, double max_price) {
    if (NULL == data_json) {
        data_json = fopen(JSON_DATA, "rb");
        if (NULL == data_json) {
            my_perror(JSON_DATA);
            return 1;
        }
    }

    FILE* new_file = fopen(TMP_FILENAME, "w");
    if (NULL == new_file) {
        my_perror(TMP_FILENAME);
        fclose(data_json);
        return 1;
    }

    int hotel_count = 0;
    char buffer[MAX_LINE_LENGTH];
    char price_str[256];
    
    int found_array = 0;
    while (fgets(buffer, MAX_LINE_LENGTH, data_json)) {
        if (strstr(buffer, "[")) {
            found_array = 1;
            break;
        }
    }

    if (!found_array) {
        fprintf(stderr, "%s Could not find hotels array in %s\n", ERROR_PREFIX, JSON_DATA);
        fclose(new_file);
        fclose(data_json);
        return 1;
    }

    fputs("{\n", new_file);
    fputs("\t\"hotels\": [\n", new_file);

    while (!feof(data_json)) {
        int c;
        do {
            c = fgetc(data_json);
        }
        while (c != EOF && (isspace(c) || c == ','));
        
        if (c == ']' || c == EOF) {
            break;
        }
        
        fseek(data_json, -1, SEEK_CUR);
        
        int ret_code = peek_json_object_property(data_json, "price", price_str, sizeof(price_str));

        if (ret_code == JSON_PEEK_SUCCESS || ret_code == JSON_PEEK_PROPERTY_NOT_FOUND) {
            double price = 0;
            int include_hotel = 1;
            
            if (ret_code == JSON_PEEK_SUCCESS) {
                price = strtod(price_str, NULL);
                if (price < min_price || price > max_price) {
                    include_hotel = 0;
                }
            }
            
            if (include_hotel) {
                char hotel_obj[MAX_LINE_LENGTH * 10];
                ret_code = read_json_object(data_json, hotel_obj, sizeof(hotel_obj));
                
                if (ret_code == JSON_PEEK_SUCCESS) {
                    if (hotel_count > 0) {
                        fputs(",\n", new_file);
                    }
                    fputs("\t\t", new_file);
                    fputs(hotel_obj, new_file);
                    hotel_count++;
                } else {
                    fprintf(stderr, "%s Error reading hotel object: %d\n", ERROR_PREFIX, ret_code);
                }
            } else {
                ret_code = skip_json_object(data_json);
                if (ret_code != JSON_PEEK_SUCCESS) {
                    fprintf(stderr, "%s Error skipping hotel object: %d\n", ERROR_PREFIX, ret_code);
                }
            }
        } else {
            fprintf(stderr, "%s Error peeking price property: %d\n", ERROR_PREFIX, ret_code);
            skip_json_object(data_json);
        }
    }
    
    fputs("\n\t]\n", new_file);
    fputs("}\n", new_file);

    fclose(new_file);
    fclose(data_json);
    
    if (0 != remove(JSON_DATA)) {
        fprintf(stderr, "%s Can't delete %s file for replacing to updated version.\n", ERROR_PREFIX, JSON_DATA);
        return 1;
    }

    if (0 != rename(TMP_FILENAME, JSON_DATA)) {
        fprintf(stderr, "%s Can't rename tmp file(%s) to original path(%s).\n", ERROR_PREFIX, TMP_FILENAME, JSON_DATA);
        return 1;
    }
    
    printf("Filtered hotels by price range %.2f-%.2f. Found %d matching hotels.\n", min_price, max_price, hotel_count);
    return 0;
}
