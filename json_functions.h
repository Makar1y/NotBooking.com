#ifndef JSON_FUNCTIONS_H
#define JSON_FUNCTIONS_H

extern char* JSON_DATA;

/// @brief Clears all contents of the file
/// @retval 0 - all ok
/// @retval 1 - error
int clear_hotel_data();

/// @brief Changes from which file data is read
/// @param filename Name of file from which to read data
/// @retval 0 - all ok
/// @retval 1 - error
int read_hotel_from_file(const char* filename);

/// @brief Remove hotel from json data file
/// @param name Name of hotel to remove (case sensetive)
/// @retval 0 - all ok
/// @retval 1 - error
int remove_hotel_by_name(const char const* name);

/// @brief Filter hotels by price range from json data file
/// @param min_price Minimum price (inclusive)
/// @param max_price Maximum price (inclusive)
/// @retval 0 - all ok
/// @retval 1 - error
int filter_hotels_by_price(double min_price, double max_price);

#endif