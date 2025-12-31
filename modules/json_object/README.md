# JSON Object Module

A C library for working with JSON objects in file streams. Provides operations for peeking at properties, reading complete objects, and skipping objects.

## Overview

This module offers three core operations for JSON object manipulation in file streams:
- **Peek**: Examine object properties without moving the file cursor
- **Read**: Extract complete JSON objects and advance the cursor
- **Skip**: Skip over objects without parsing their contents

## ⚠️ Important: No JSON Validation

**This module assumes the input JSON is well-formed and correct.** It does NOT validate JSON syntax. If you provide malformed or invalid JSON, the behavior is undefined and may result in:
- Incorrect property values being returned
- Properties not being found when they exist
- Unexpected return codes

**This is by design** If your JSON might be malformed, validate it before using this module. Any issues caused by invalid JSON input are the caller's responsibility.

## API Reference

### 1. Peek at Object Property

```c
int peek_json_object_property(FILE* file, 
                               const char* property_name, 
                               char* result, 
                               size_t result_size);
```

Examines a JSON object property without advancing the file cursor.

**Parameters:**
- `file`: File pointer positioned before a JSON object
- `property_name`: Name of the property to search for (case-sensitive)
- `result`: Buffer to store the property value
- `result_size`: Size of the result buffer

**Returns:**
- `JSON_PEEK_SUCCESS` (0): Property found, simple value stored in result
- `JSON_PEEK_PROPERTY_NOT_FOUND` (1): Property not found
- `JSON_PEEK_BUFFER_TOO_SMALL` (2): Result buffer too small
- `JSON_PEEK_NULL_ARGUMENT` (3): NULL argument provided
- `JSON_PEEK_ORIGINAL_POS_ERROR` (4): Error getting/restoring file position
- `JSON_PEEK_ALLOCATION_FAILURE` (5): Memory allocation failure
- `JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE` (6): Not a JSON object

**Behavior:**
- Searches for an opening brace `{` to identify the start of the object
- File cursor remains at original position after call
- Searches only root-level properties
- Returns immediately when property is found

### 2. Read JSON Object

```c
int read_json_object(FILE* file, 
                     char* result, 
                     size_t result_size);
```

Reads the next complete JSON object from the file and advances the cursor past it.

**Parameters:**
- `file`: File pointer positioned before a JSON object
- `result`: Buffer to store the JSON object as string
- `result_size`: Size of the result buffer

**Returns:**
- `JSON_PEEK_SUCCESS` (0): Object successfully read into result
- `JSON_PEEK_BUFFER_TOO_SMALL` (2): Result buffer too small
- `JSON_PEEK_NULL_ARGUMENT` (3): NULL argument provided
- `JSON_PEEK_ORIGINAL_POS_ERROR` (4): Error getting/restoring file position
- `JSON_PEEK_ALLOCATION_FAILURE` (5): Memory allocation failure
- `JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE` (6): Not a JSON object
**Behavior:**
- Searches for an opening brace `{` to identify the start of the object
- File cursor advances past the object
- Complete object stored as string in result buffer

### 3. Skip JSON Object

```c
int skip_json_object(FILE* file);
```

Skips over the next JSON object without parsing or storing it.

**Parameters:**
- `file`: File pointer positioned before a JSON object

**Returns:**
- `JSON_PEEK_SUCCESS` (0): Object successfully skipped
- `JSON_PEEK_NULL_ARGUMENT` (3): NULL pointer provided
- `JSON_PEEK_ORIGINAL_POS_ERROR` (4): Error getting/restoring file position
- `JSON_PEEK_ALLOCATION_FAILURE` (5): Memory allocation failure
- `JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE` (6): Not a JSON object

**Behavior:**
- Searches for an opening brace `{` to identify the start of the object
- File cursor advances past the object

## Usage Examples

### Example 1: Peek at a Property

```c
#include "json_object.h"

int main() {
    FILE* file = fopen("data.json", "r");
    char name[256];
    
    int code = peek_json_object_property(file, "name", name, sizeof(name));
    
    if (code == JSON_PEEK_SUCCESS) {
        printf("Name: %s\n", name);
    } else if (code == JSON_PEEK_PROPERTY_NOT_FOUND) {
        printf("Property not found\n");
    }
    
    // File cursor is still at the beginning
    // Can now read the full object
    
    fclose(file);
    return 0;
}
```

### Example 2: Read Complete Object

```c
#include "json_object.h"

int main() {
    FILE* file = fopen("data.json", "r");
    char buffer[4096];
    
    int code = read_json_object(file, buffer, sizeof(buffer));
    
    if (code == JSON_PEEK_SUCCESS) {
        printf("Object: %s\n", buffer);
        // File cursor now positioned after the object
    }
    
    fclose(file);
    return 0;
}
```

### Example 3: Skip Objects in Array

```c
#include "json_object.h"

int main() {
    FILE* file = fopen("array.json", "r");
    
    // Skip opening bracket '['
    fgetc(file);
    
    // Skip first 5 objects in array
    for (int i = 0; i < 5; i++) {
        skip_json_object(file);
        fgetc(file); // Skip comma or whitespace
    }
    
    // Now positioned at 6th object
    char buffer[4096];
    read_json_object(file, buffer, sizeof(buffer));
    
    fclose(file);
    return 0;
}
```

## Return Code Reference

| Code | Constant |
|------|----------|
| 0 | `JSON_PEEK_SUCCESS` |
| 1 | `JSON_PEEK_PROPERTY_NOT_FOUND` |
| 2 | `JSON_PEEK_BUFFER_TOO_SMALL` |
| 3 | `JSON_PEEK_NULL_ARGUMENT` |
| 4 | `JSON_PEEK_ORIGINAL_POS_ERROR` |
| 5 | `JSON_PEEK_ALLOCATION_FAILURE` |
| 6 | `JSON_PEEK_OBJECT_NOT_STARTS_WITH_BRACE` |
## Compilation

```bash
# Compile the module
gcc -c json_object.c -o json_object.o

# Link with your program
gcc main.c json_object.o -o program
```

## Limitations

- **Maximum object size**: 64KB (defined by `MAX_OBJECT_SIZE`)
- **Maximum property name length**: 256 characters
- **Root-level search only**: `peek_json_object_property` searches only root-level properties
- **Property name matching**: Case-sensitive, exact match required
- **No JSON validation**: Assumes valid, well-formed JSON input
- **Character encoding**: Basic UTF-8 support (ASCII-safe)