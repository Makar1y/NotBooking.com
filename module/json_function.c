#include "json_function.h"
#include <string.h>
#include <stdlib.h>


typedef struct
{

    char *key;
    char *value;

} pair;


int print_function(const char *query, FILE *json)
{

    if(!query || !json)
        return 0;

    char buffer[1024];
    strncpy(buffer, query, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    pair params[100];
    int params_count = 0;

    char *split_string = strtok(buffer, "&=");
    while (split_string && params_count < 100)
    {
        params[params_count].key = split_string;
        split_string = strtok(NULL,"&=");

        if(!split_string)
            break;

        params[params_count].value = split_string;
        decoding_url(params[params_count].value);

        params_count++;
        split_string = strtok(NULL,"&=");

    }
    fprintf(json, "{\n");

    int cond = 1;
    for (int i=0; i<params_count; i++)
    {
        if(strcmp(params[i].key,"amenities") == 0)
            continue;

        if(!cond)
            fprintf(json, ",\n");

        fprintf(json, " \"%s\": \"%s\"",params[i].key, params[i].value);
        cond = 0;
    }
    int cond_amenities = 0, cond_value = 1;
    for (int i=0; i<params_count; i++)
    {
        if(strcmp(params[i].key,"amenities") != 0)
            continue;
        if (!cond_amenities)
        {
            if (!cond)
                fprintf(json, ",\n");
            fprintf(json, "  \"amenities\": [");
            cond_amenities = 1;
        }

        if(!cond_value)
            fprintf(json, ", ");

        fprintf(json, " \"%s\"", params[i].value);
        cond = 0;
        cond_value = 0;
    }

    if(cond_amenities)
        fprintf(json, "]");

    fprintf(json, "\n}\n");
    return 0;
}


static void url_decode(char *s)
{
    char *source = s;
    char *destination = s;

    while (*source)
    {
        if (*source == '+')
        {
            *destination++ = ' ';
            source++;
        }
        else if (*source == '%' && source[1] && source[2])
        {

            char hex[3] = { source[1], source[2], '\0' };
            *dst++ = (char) strtol(hex, NULL, 16);
            src += 3;
        }
        else
        {
            *destination++ = *source++;
        }
    }
    *destination = '\0';
}

