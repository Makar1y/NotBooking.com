#ifndef MODULE_H
#define MODULE_H

#define MAX_JSON_KEY_LENGTH 256
#define JSON_ELEMENT_COUNT 6
#define MAX_JSON_VALUE_LENGTH 65526

long getSizeOfFile(char* filename);
char* getTextOfFile(char* filename);
int checkErrors(char* template_html);
void checkForDuplicates(char* template_html, char* output_html);
int countBlocks(char* filename_json);
void copyHTMLBlocks(char* template_html, char* output_html, char* filename_json);
void copyHTMLThatAreArrays(char* template_html, char* output_html, char* filename_json);

#endif