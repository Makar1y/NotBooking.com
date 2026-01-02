#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "json_functions.h"

int main() {
    filter_hotels_by_price(20000, 40000);
    remove_hotel_by_name("StarOtel");
    read_hotel_from_file("data/data2.json");
    clear_hotel_data();
    printf("Bye bye");
    return 0;
}