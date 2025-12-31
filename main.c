#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "json_functions.h"

int main() {

    filter_hotels_by_price(0, 20000, 40000);
    remove_hotel_by_name(0, "StarOtel");
    printf("Bye bye");
    return 0;
}