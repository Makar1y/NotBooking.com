#ifndef JSON_FUNCTIONS_H
#define JSON_FUNCTIONS_H

/// @brief Remove hotel from json data file
/// @param data_json opened data file or 0 (will be opened inside function)
/// @param name Name of hotel to remove (case sensetive)
/// @retval 0 - all ok
/// @retval 1 - error
int remove_hotel(FILE* data_json, const char const* name);

/// @brief Filter hotels by price range from json data file
/// @param data_json opened data file or 0 (will be opened inside function)
/// @param min_price Minimum price (inclusive)
/// @param max_price Maximum price (inclusive)
/// @retval 0 - all ok
/// @retval 1 - error
int filter_hotels_by_price(FILE* data_json, double min_price, double max_price);
#endif