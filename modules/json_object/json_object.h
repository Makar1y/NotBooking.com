#include <stdio.h>

#define MAX_OBJECT_SIZE 65536  // Maximum size of JSON object to peek
#define MAX_PROPERTY_NAME_LENGTH 256 // Maximum length of property name

// Return codes
#define JSON_PEEK_SUCCESS 0                         // Property found, value is a string/number/boolean/null
#define JSON_PEEK_PROPERTY_NOT_FOUND 1              // Property not found in object
#define JSON_PEEK_BUFFER_TOO_SMALL 2                // Result buffer is too small
#define JSON_PEEK_NULL_ARGUMENT 3                   // One of the provided arguments was NULL
#define JSON_PEEK_ORIGINAL_POS_ERROR 4              // ftell/fseek failed to get/restore position
#define JSON_PEEK_ALLOCATION_FAILURE 5              // malloc failed to allocate memory
#define JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE 6    // First non-whitespace character is not '{'

/// @brief Peek at the next JSON object property without moving file cursor
/// @param file File pointer to read from
/// @param property_name Name of the property to search for (case-sensitive)
/// @param result Buffer to store the property value
/// @param result_size Size of the result buffer
/// @return Return code indicating success or type of error
/// @retval JSON_PEEK_SUCCESS - Property found, value stored in result
/// @retval JSON_PEEK_PROPERTY_NOT_FOUND - Property not found
/// @retval JSON_PEEK_BUFFER_TOO_SMALL - Result buffer too small
/// @retval JSON_PEEK_NULL_ARGUMENT - One of the arguments is NULL
/// @retval JSON_PEEK_ORIGINAL_POS_ERROR - Error getting/restoring file position
/// @retval JSON_PEEK_ALLOCATION_FAILURE - Memory allocation failure
/// @retval JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE - First token is not '{'
int peek_json_object_property(FILE* file, const char* property_name, char* result, size_t result_size);

/// @brief Read the next JSON object from the file
/// @param file File pointer positioned before a JSON object
/// @param result Buffer to store the JSON object as string
/// @param result_size Size of the result buffer
/// @return Return code indicating success or type of error
/// @retval JSON_PEEK_SUCCESS - Object successfully read into result
/// @retval JSON_PEEK_BUFFER_TOO_SMALL - Result buffer too small
/// @retval JSON_PEEK_NULL_ARGUMENT - File pointer is NULL or result buffer is NULL with size 0
/// @retval JSON_PEEK_ORIGINAL_POS_ERROR - Error getting/restoring file position
/// @retval JSON_PEEK_ALLOCATION_FAILURE - Memory allocation failure
/// @retval JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE - First token is not '{'
int read_json_object(FILE* file, char* result, size_t result_size);

/// @brief Skip over the next JSON object in the file
/// @param file File pointer to read from
/// @return Return code indicating success or type of error
/// @retval JSON_PEEK_SUCCESS - Object successfully skipped
/// @retval JSON_PEEK_NULL_ARGUMENT - File pointer is NULL
/// @retval JSON_PEEK_ORIGINAL_POS_ERROR - Error getting/restoring file position
/// @retval JSON_PEEK_ALLOCATION_FAILURE - Memory allocation failure
/// @retval JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE - First token is not '{'
int skip_json_object(FILE* file);