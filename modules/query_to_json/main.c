#include <stdio.h>
#include <stdlib.h>
#include "json_function.h"

int main(void)
{
    printf("Content-Type: text/plain\n\n");

    char *query = getenv("QUERY_STRING");
    print_function(query, stdout);

    return 0;
}
