#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "json_functions.h"
#include "modules/json_object/json_object.h"

char* JSON_DATA = "data/data.json";
#define HTML_TEMPLATE "./data/template.html"
#define TMP_FILENAME "./tmp_file"
#define ERROR_PREFIX "!!! Error ->"

#define MAX_LINE_LENGTH 5000
#define MAX_JSON_NAME_LENGTH 256

void my_perror(char* message) {
    char error_m[MAX_LINE_LENGTH] = ERROR_PREFIX;
    strcat(error_m, " ");
    strcat(error_m, message);
    perror(error_m);
}

int clear_hotel_data() {
    FILE* data_json = fopen(JSON_DATA, "w");
    if (NULL == data_json) {
        my_perror(JSON_DATA);
        return 1;
    }
    fprintf(data_json, "{}");
    fclose(data_json);

    return 0;
}

int read_hotel_from_file(const char* filename) {
    size_t len = strlen(filename);

    if (len < 5 || strcmp(filename + len - 5, ".json") != 0) {
        fprintf(stderr, "%s is not a .json file\n", filename);
        return 1;
    }

    FILE* data_json = fopen(filename, "r");
    if (NULL == data_json) {
        my_perror("Couldn't open this file");
        return 1;
    }
    fclose(data_json);
    JSON_DATA = malloc(strlen(filename) + 1);
    if (!JSON_DATA) {
        my_perror("Memory allocation failed");
        return 1;
    }
    strcpy(JSON_DATA, filename);
    return 0;
}

typedef int (*filter_callback)(FILE* file , void* user_data);

static int filter_hotel_objects(filter_callback callback, void* user_data, int remove_first) {
    FILE* data_json = fopen(JSON_DATA, "rb");
    if (NULL == data_json) {
        my_perror(JSON_DATA);
        return 1;
    }

    FILE* new_file = fopen(TMP_FILENAME, "wb");
    if (NULL == new_file) {
        my_perror(TMP_FILENAME);
        fclose(data_json);
        return 1;
    }

    int object_count = 0;
    int removed_count = 0;
    int found_first = 0;
    
    char buffer[MAX_LINE_LENGTH];
    int c;
    do {
        c = fgetc(data_json);
    }
    while (c != EOF && c != '[');

    if (c != '[') {
        fprintf(stderr, "%s Could not find array in %s\n", ERROR_PREFIX, JSON_DATA);
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

        int original_pos = ftell(data_json);
        int should_keep = callback(data_json, user_data);
        fseek(data_json, original_pos, SEEK_SET);

        if (remove_first && !found_first && !should_keep) {
            found_first = 1;
            removed_count++;
            int ret_code = skip_json_object(data_json);
            if (ret_code != JSON_PEEK_SUCCESS) {
                fprintf(stderr, "%s Error skipping object: Code %d\n", ERROR_PREFIX, ret_code);
            }
        }
        else if (should_keep || (remove_first && found_first)) {
            char hotel_obj[MAX_OBJECT_SIZE];
            int ret_code = read_json_object(data_json, hotel_obj, sizeof(hotel_obj));
            if (ret_code == JSON_PEEK_SUCCESS) {
                
                if (object_count > 0) {
                    fputs(",\n", new_file);
                }
                fputs("\t\t", new_file);
                fputs(hotel_obj, new_file);
                object_count++;
            } else {
                fprintf(stderr, "%s Error reading object: Code %d\n", ERROR_PREFIX, ret_code);
            }
        } else {
            int ret_code = skip_json_object(data_json);
            if (ret_code != JSON_PEEK_SUCCESS) {
                fprintf(stderr, "%s Error skipping object: Code %d\n", ERROR_PREFIX, ret_code);
            }
            removed_count++;
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
    
    if (remove_first) {
        printf("Removed first matching object. Total objects remaining: %d\n", object_count);
    } else {
        printf("Filtered objects. Kept: %d, Removed: %d\n", object_count, removed_count);
    }
    return 0;
}



typedef struct {
    const char* name;
} HotelNameData;

static int name_filter_callback(FILE* file, void* user_data) {
    HotelNameData* data = (HotelNameData*)user_data;
    char name_buffer[MAX_JSON_NAME_LENGTH];

    int ret_code = peek_json_object_property(file, "name", name_buffer, sizeof(name_buffer));
    if (ret_code == JSON_PEEK_SUCCESS) {
        if (strcmp(name_buffer, data->name) == 0) {
            return 0;
        }
    }
    else {
        fprintf(stderr, "Error peeking name property: Code %d\n", ret_code);
    }
    return 1;
}

int remove_hotel_by_name(const char const* name) {
    HotelNameData filter_data = {
        .name = name
    };
    return filter_hotel_objects(name_filter_callback, &filter_data, 1);
}



typedef struct {
    double min_price;
    double max_price;
} HotelPriceRangeData;

static int price_filter_callback(FILE* file, void* user_data) {
    HotelPriceRangeData* data = (HotelPriceRangeData*)user_data;
    char price_str[256];

    int ret_code = peek_json_object_property(file, "price", price_str, sizeof(price_str));
    if (ret_code == JSON_PEEK_SUCCESS) {
        double price = strtod(price_str, NULL);
        if (price < data->min_price || price > data->max_price) {
            return 0;
        }
    }
    else {
        fprintf(stderr, "Error peeking price property: Code %d\n", ret_code);
    }
    return 1;
}

int filter_hotels_by_price(double min_price, double max_price) {
    HotelPriceRangeData filter_data = {
        .min_price = min_price,
        .max_price = max_price
    };
    return filter_hotel_objects(price_filter_callback, &filter_data, 0);
}