#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "json_object.h"

#define MAX_OBJECT_SIZE 65536  // Maximum size of JSON object to peek
#define MAX_PROPERTY_NAME_LENGTH 256 // Maximum length of property name

static int parse_whitespace(const char** ptr, char* result, size_t result_size) {
    size_t i = 0;
    while (**ptr && isspace((unsigned char)**ptr)) {
        if (result) {
            if (i >= result_size - 1) {
                result[i] = '\0';
                return -1; // Buffer too small
            }
            result[i++] = **ptr;
        }
        (*ptr)++;
    }
    if (result && i < result_size) {
        result[i] = '\0';
    }
    return i;
}

static void skip_whitespace(const char** ptr) {
    parse_whitespace(ptr, NULL, 0);
}

static int parse_json_atomic_value(const char** ptr, char* result, size_t result_size) {
    int is_string = 0;
    if (**ptr == '"') {
        is_string = 1;
        (*ptr)++;
    }

    size_t i = 0;
    while (**ptr && ((is_string && **ptr != '"') || (!is_string && **ptr != ',' && **ptr != '}' && **ptr != '\n'))) {
        if (**ptr == '\\') {
            (*ptr)++;
            if (!**ptr) {
                break;
            }

            if (result) {
                if (i >= result_size - 1) {
                    result[i] = '\0';
                    return -1; // Buffer too small
                }
                switch (**ptr) {
                    case 'b':
                        result[i++] = '\b';
                        break;
                    case 'f':
                        result[i++] = '\f';
                        break;
                    case 'n':
                        result[i++] = '\n';
                        break;
                    case 't':
                        result[i++] = '\t';
                        break;
                    case 'r':
                        result[i++] = '\r';
                        break;
                    case 'u':
                        for (int j = 0; j < 4; j++) { // Skip uXXXX sequence
                            (*ptr)++;
                            if (!**ptr) {
                                break;
                            }
                        }
                        result[i++] = '?'; // Placeholder for unicode
                        break;
                    default:
                        result[i++] = **ptr; // ", \, /
                }
            }
        } else if (result) {
            if (i >= result_size - 1) {
                result[i] = '\0';
                return -1; // Buffer too small
            }
            result[i++] = **ptr;
        }

        (*ptr)++;
    }

    if (**ptr == '"') {
        (*ptr)++;
    }

    if (result) {
        result[i] = '\0';
    }

    return i;
}

static void skip_json_atomic_value(const char** ptr) {
    parse_json_atomic_value(ptr, NULL, 0);
}

static int parse_json_structured_value(const char** ptr, const char start_char, const char end_char, char* result, size_t result_size) {
    size_t i = 0;
    int depth = 1;

    if (result) {
        if (i >= result_size - 1) {
            result[i] = '\0';
            return -1; // Buffer too small
        }
        result[i++] = start_char;
    }
    (*ptr)++;

    while (**ptr && depth > 0) {
        int ws_len = parse_whitespace(ptr, result ? &result[i] : NULL, result_size - i);
        if (result) {
            if (ws_len == -1) {
                return -1; // Buffer too small
            }
            i += ws_len;
        }

        if (**ptr == '"') {
            if (result) {
                if (i >= result_size - 1) {
                    result[i] = '\0';
                    return -1; // Buffer too small
                }
                result[i++] = '"';
            }
            (*ptr)++;
            
            int val_len = parse_json_atomic_value(ptr, result ? &result[i] : NULL, result_size - i);

            if (result) {
                if (val_len == -1) {
                    return -1; // Buffer too small
                }
                i += val_len;
                if (i >= result_size - 1) {
                    result[i] = '\0';
                    return -1; // Buffer too small
                }
            }
            continue;
        }

        if (**ptr == start_char) {
            depth++;
        }
        else if (**ptr == end_char) {
            depth--;
        }

        if (**ptr) {
            if (result) {
                if (i >= result_size - 1) {
                    result[i] = '\0';
                    return -1; // Buffer too small
                }
                result[i++] = **ptr;
            }
            (*ptr)++;
        }
    }

    if (result) {
        result[i] = '\0';
    }

    return 0;
}

static void skip_json_structured_value(const char** ptr, const char start_char, const char end_char) {
    parse_json_structured_value(ptr, start_char, end_char, NULL, 0);
}

static void skip_json_value(const char** ptr) {
    skip_whitespace(ptr);

    if (**ptr == '{') {
        skip_json_structured_value(ptr, '{', '}');
    } else if (**ptr == '[') {
        skip_json_structured_value(ptr, '[', ']');
    } else  {
        skip_json_atomic_value(ptr);
    }
}

static size_t read_file_to_buffer(FILE* file, char* buffer) {
    size_t bytes_read = fread(buffer, 1, MAX_OBJECT_SIZE - 1, file);
    buffer[bytes_read] = '\0';
    return bytes_read;
}

int peek_json_object_property(FILE* file, const char* property_name, char* result, size_t result_size) {
    if (!file || !property_name || !result || result_size == 0) {
        return JSON_PEEK_NULL_ARGUMENT;
    }

    long original_pos = ftell(file);
    if (original_pos == -1) {
        return JSON_PEEK_ORIGINAL_POS_ERROR;
    }
    char* buffer = (char*)malloc(MAX_OBJECT_SIZE);
    if (!buffer) {
        return JSON_PEEK_ALLOCATION_FAILURE;
    }

    size_t bytes_read = read_file_to_buffer(file, buffer);
    fseek(file, original_pos, SEEK_SET);

    const char* ptr = buffer;
    skip_whitespace(&ptr);

    if (*ptr != '{') {
        free(buffer);
        return JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE;
    }
    ptr++;
    skip_whitespace(&ptr);

    while (*ptr && *ptr != '}') {
        skip_whitespace(&ptr);

        char prop_name[MAX_PROPERTY_NAME_LENGTH];
        int parse_result = parse_json_atomic_value(&ptr, prop_name, sizeof(prop_name));
        if (parse_result == -1) {
            free(buffer);
            return JSON_PEEK_BUFFER_TOO_SMALL;
        }

        skip_whitespace(&ptr);
        if (*ptr == ':') {
            ptr++;
        }
        skip_whitespace(&ptr);

        if (strcmp(prop_name, property_name) == 0) {
            if (*ptr == '{') {
                parse_result = parse_json_structured_value(&ptr, '{', '}', result, result_size);
                if (parse_result == -1) {
                    free(buffer);
                    return JSON_PEEK_BUFFER_TOO_SMALL;
                }
            } else if (*ptr == '[') {
                parse_result = parse_json_structured_value(&ptr, '[', ']', result, result_size);
                if (parse_result == -1) {
                    free(buffer);
                    return JSON_PEEK_BUFFER_TOO_SMALL;
                }
            } else {
                parse_result = parse_json_atomic_value(&ptr, result, result_size);
                if (parse_result == -1) {
                    free(buffer);
                    return JSON_PEEK_BUFFER_TOO_SMALL;
                }
            }

            free(buffer);
            return JSON_PEEK_SUCCESS;
        }

        // Not the property we're looking for, skip the value
        skip_json_value(&ptr);

        skip_whitespace(&ptr);
        if (*ptr == ',') {
            ptr++;
        }
    }

    free(buffer);
    return JSON_PEEK_PROPERTY_NOT_FOUND;
}

int read_json_object(FILE* file, char* result, size_t result_size) {
    if (!file || (result && result_size == 0)) {
        return JSON_PEEK_NULL_ARGUMENT;
    }

    long original_pos = ftell(file);
    if (original_pos == -1) {
        return JSON_PEEK_ORIGINAL_POS_ERROR;
    }

    char* buffer = (char*)malloc(MAX_OBJECT_SIZE);
    if (!buffer) {
        return JSON_PEEK_ALLOCATION_FAILURE;
    }

    size_t bytes_read = read_file_to_buffer(file, buffer);

    const char* ptr = buffer;
    skip_whitespace(&ptr);

    if (*ptr != '{') {
        free(buffer);
        return JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE;
    }

    const char* start_ptr = ptr;
    int ret_code = parse_json_structured_value(&ptr, '{', '}', result, result_size);
    if (ret_code == -1) {
        free(buffer);
        return JSON_PEEK_BUFFER_TOO_SMALL;
    }

    long new_pos = original_pos + (ptr - buffer);
    fseek(file, new_pos, SEEK_SET);

    free(buffer);
    return JSON_PEEK_SUCCESS;
}

int skip_json_object(FILE* file) {
    if (!file) {
        return JSON_PEEK_NULL_ARGUMENT;
    }

    long original_pos = ftell(file);
    if (original_pos == -1) {
        return JSON_PEEK_ORIGINAL_POS_ERROR;
    }

    char* buffer = (char*)malloc(MAX_OBJECT_SIZE);
    if (!buffer) {
        return JSON_PEEK_ALLOCATION_FAILURE;
    }

    size_t bytes_read = read_file_to_buffer(file, buffer);

    const char* ptr = buffer;
    skip_whitespace(&ptr);

    if (*ptr != '{') {
        free(buffer);
        return JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE;
    }

    skip_json_structured_value(&ptr, '{', '}');

    long new_pos = original_pos + (ptr - buffer);
    fseek(file, new_pos, SEEK_SET);

    free(buffer);
    return JSON_PEEK_SUCCESS;
}