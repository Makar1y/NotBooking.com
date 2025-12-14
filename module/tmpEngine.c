#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tmpEngine.h"

long getSizeOfFile(char* filename) {
  FILE *myfile = fopen(filename, "r");
  if(!myfile)
    return -1;

  fseek(myfile, 0, SEEK_END);
  long size = ftell(myfile);
  rewind(myfile);

  fclose(myfile);
  return size;
}

char* getTextOfFile(char* filename) {
  FILE *myfile = fopen(filename, "r");
  if(!myfile)
    return NULL;

  fseek(myfile, 0, SEEK_END);
  long size = ftell(myfile);
  rewind(myfile);

  char* text = malloc(size+1);
  fread(text, 1, size, myfile);
  text[size] = '\0';

  fclose(myfile);
  return text;
}

int checkErrors(char* template_html){
  long size = getSizeOfFile(template_html);
  char* text = getTextOfFile(template_html);

  int opening1 = 0, opening2 = 0, opening3 = 0;
  int line = 1, errorLine1 = 0, errorLine2 = 0, errorLine3 = 0;

  for (int i = 0; i < size; ++i) {
    if (text[i] == '{' && text[i + 1] == '{'){
      opening1++;
      if(opening1 > 1) {
        printf("Error: missing closing }} in line: %d", errorLine1);
        return 1;
      }
      errorLine1 = line;
    }
    if (text[i] == '}' && text[i + 1] == '}'){
      opening1--;
    }
    if(text[i] == '\n') {
      line++;
    }
    if (text[i] == '{' && text[i + 1] == '?'){
      opening2++;
      if(opening2 > 1) {
        printf("Error: missing closing ?} in line: %d", errorLine2);
        return 1;
      }
      errorLine2 = line;
    }
    if (text[i] == '?' && text[i + 1] == '}'){
      opening2--;
    }
    if(text[i] == '\n') {
      line++;
    }
    if (text[i] == '{' && text[i + 1] == '%'){
      opening3++;
      if(opening3 > 1) {
        printf("Error: missing closing %%} in line: %d", errorLine3);
        return 1;
      }
      errorLine3 = line;
    }
    if (text[i] == '%' && text[i + 1] == '}'){
      opening3--;
    }
    if(text[i] == '\n') {
      line++;
    }
  }
  if (opening1 == 1) {
    printf("Error: missing closing }} in line: %d", errorLine1);
    return 1;
  }
  if (opening2 == 1) {
    printf("Error: missing closing ?} in line: %d", errorLine2);
    return 1;
  }
  if (opening3 == 1) {
    printf("Error: missing closing %%} in line: %d", errorLine3);
    return 1;
  }
  return 0;
}

int countBlocks(char* filename_json){
  long size = getSizeOfFile(filename_json);
  char* text = getTextOfFile(filename_json);

  int count = 0;
  int inString = 0;

  for (int i = 0; i < size; ++i) {
    if (text[i] == '"'){
      int backslashes = 0;
      for (int j = i - 1; j >= 0 && text[j] == '\\'; j--) {
        backslashes++;
      }
      if (backslashes % 2 == 0) {
        inString = !inString;
      }
    }
    if(!inString && text[i] == '}' && text[i + 1] == ',') {
      count++;
    }
  }
  if (count > 0) count++;
  free(text);
  return count;
}

void checkForDuplicates(char* template_html, char* output_html){
  long size = getSizeOfFile(template_html);
  char* text = getTextOfFile(template_html);

  char* newHTML = calloc(size * 2, 1);

  int i;
  int plus = -1;
  char elements[JSON_ELEMENT_COUNT][MAX_JSON_KEY_LENGTH];
  int elementCount = 0;
  for (i = 1; i < size; ++i) {
    if(text[i] == '\n') //stupid windows
      size--;
    *(newHTML + i + plus) = text[i];
    if (text[i - 1] == '{' && text[i] == '{') {
      int j = 1;
      char* element = calloc(MAX_JSON_KEY_LENGTH, 1);
      while(text[i + j] != '}' && text[i + j + 1] != '}') {
        element[j - 1] = text[i + j];
        j++;
      }
      element[j - 1] = '\0';
      int found = 0;
      for(int k = 0; k < elementCount; k++) {
        if (!strcmp(element, elements[k])) {
          plus++;
          *(newHTML + i + plus) = '_';
          found = 1;
          break;
        }
      }
      if (!found) {
        strcpy(elements[elementCount], element);
        elementCount++;
      }
    }
  }
  newHTML[i + plus] = '\0';
  FILE* myfile = fopen(output_html, "w");
  fprintf(myfile, "%s\n", newHTML);
  fclose(myfile);
  free(newHTML);
};

void copyHTMLBlocks(char* template_html, char* output_html, char* filename_json){
  long size = getSizeOfFile(template_html);
  char* text = getTextOfFile(template_html);

  int blocksCount = countBlocks(filename_json);

  char* newHTML = calloc(size * blocksCount + size, 1);

  int i;
  for (i = 0; i < size - 1; ++i) {
    if (text[i] == '{' && text[i+1] == '?') {
      i+=2;
      break;
    }
    *(newHTML + i) = text[i];
  }
  newHTML[i-1] = '\0';
  char* copySegmend = malloc(size);
  int segmentSize = 0;
  for (;i < size - 1; ++i) {
    if (text[i] == '?' && text[i + 1] == '}') {
      break;
    }
    *(copySegmend + segmentSize) = text[i];
    segmentSize++;
  }
  copySegmend[segmentSize + 1] = '\0';
  segmentSize += 1;


  for (int j = 0; j < blocksCount; j++){
    strcat(newHTML, copySegmend);
  }
  i += 2;
  int add = strlen(newHTML)  - i;
  for (; i < size; ++i) {
    *(newHTML + (i + add)) = text[i];
  }
  FILE* myfile = fopen(output_html, "w");
  fprintf(myfile, "%s\n", newHTML);
  fclose(myfile);
  free(newHTML);
  free(copySegmend);
};


void copyHTMLThatAreArrays(char* template_html, char* output_html, char* filename_json){
  long size = getSizeOfFile(template_html);
  char* text = getTextOfFile(template_html);

  char* newHTML = calloc(size * 2, 1);

  int i;
  int newHTMLLen = 0;
  char arrays[JSON_ELEMENT_COUNT][MAX_JSON_KEY_LENGTH];
  char elements[JSON_ELEMENT_COUNT][MAX_JSON_KEY_LENGTH];
  int elementCount = 0;
  int elementOccuranceCount[JSON_ELEMENT_COUNT];
  int arraysCount = 0;
  int arraysOccuranceCount[JSON_ELEMENT_COUNT];
  for (i = 0; i < size;  ++i) {
    if (text[i] == '{' && text[i+1] == '%') {
      newHTML[newHTMLLen] = '\0';
      i++;
      while (text[i] != '{'){
        i++;
      };
      i++;
      char* array = malloc(MAX_JSON_VALUE_LENGTH);
      int arraySize = 0;
      while (text[i] != '}') {
        *(array + arraySize) = text[i];
        arraySize++;
        i++;
      }
      i++;
      *(array + arraySize) = '\0';
      int occurance = 1;
      int found = 0;
      for (int j = 0; j < arraysCount; j++) {
        if (!strcmp(array, arrays[j])){
          arraysOccuranceCount[j]++;
          occurance = arraysOccuranceCount[j];
          found = 1;
          break;
        }
      }
      
      if (!found) {
        strcpy(arrays[arraysCount], array);
        arraysOccuranceCount[arraysCount] = 1;
        arraysCount++;
      }
      char *copySegmend1 = malloc(size), *copySegmend2 = malloc(size);
      int segmentSize1 = 0, segmentSize2 = 0;

      while (text[i] != '{' || text[i+1] != '{'){
        *(copySegmend1 + segmentSize1) = text[i];
        segmentSize1++;
        i++;
      }
      i+=4;
      copySegmend1[segmentSize1] = '\0';
      while (text[i] != '%' || text[i+1] != '}') {
        *(copySegmend2 + segmentSize2) = text[i];
        segmentSize2++;
        i++;
      }
      copySegmend2[segmentSize2] = '\0';

      long size2 = getSizeOfFile(filename_json);
      char* text2 = getTextOfFile(filename_json);
      char* tmp = text2;

      int count = 0;
      while ((tmp = strstr(tmp, array)) != NULL) {
        count++;
        if (count == occurance) {
          break;
        }
        tmp += strlen(array);
      }
      int j = 0;
      while (*(tmp + j) != '[') {
        j++;
      };
      int doubleQuoteCount = 0;
      while (*(tmp + j) != ']') {
        if (*(tmp + j) == '"') {
          doubleQuoteCount++;
        }
        j++;
      }
      int copyCount = doubleQuoteCount/2;
      j = 0;
      while (*(tmp + j) != '[') {
        j++;
      }
      
      char arrayElements[JSON_ELEMENT_COUNT][MAX_JSON_VALUE_LENGTH];
      for (int k = 0; k < copyCount; k++) {
        int elementSize = 0;
        while (*(tmp + j) != '"') {
          j++;
          
        }
        j++;
        while (*(tmp + j) != '"'){
          arrayElements[k][elementSize] = *(tmp + j);
          elementSize++;
          j++;
        }
        j++;
        arrayElements[k][elementSize] = '\0';
      }
     

      for (int k = 0; k < copyCount; k++) {
        strcpy(newHTML + newHTMLLen, copySegmend1);
        newHTMLLen += strlen(copySegmend1);
        strcpy(newHTML + newHTMLLen, arrayElements[k]);
        newHTMLLen += strlen(arrayElements[k]);
        strcpy(newHTML + newHTMLLen, copySegmend2);
        newHTMLLen += strlen(copySegmend2);
      }
      i++;
      free(text2);
      free(copySegmend1);
      free(copySegmend2);
      free(array);
      continue;
    }

    if (text[i] == '{' && text[i+1] == '{') {
      newHTML[newHTMLLen] = '\0';
      i++;
      while (text[i] != '{'){
        i++;
      };
      i++;
      char* element = malloc(MAX_JSON_VALUE_LENGTH);
      element[0] = '"';
      int elementSize = 1;
      int underscore = 0;
      while (text[i] != '}') {
        if(text[i] == '_' && underscore == 0){
          underscore = 1;
          i++;
          continue;
        }
        *(element + elementSize) = text[i];
        elementSize++;
        i++;
      }
      i++;
      *(element + elementSize) = '"';
      *(element + elementSize + 1) = '\0';
      int occurance = 1;
      int found = 0;
      for (int j = 0; j < elementCount; j++) {
        if (!strcmp(element, elements[j])){
          if(!underscore)
            elementOccuranceCount[j]++;
          occurance = elementOccuranceCount[j];
          found = 1;
          break;
        }
      }
      
      if (!found) {
        strcpy(elements[elementCount], element);
        elementOccuranceCount[elementCount] = 1;
        elementCount++;
      }
      char* copySegmend = malloc(size);
      int segmentSize = 0;

      long size2 = getSizeOfFile(filename_json);
      char* text2 = getTextOfFile(filename_json);

      char* tmp = text2;

      int count = 0;
      while ((tmp = strstr(tmp, element)) != NULL) {
        count++;
        if (count == occurance) {
          break;
        }
        tmp += strlen(element);
      }
      
      int j = 0;
      while (*(tmp + j) != ' ') {
        j++;
      };
      j++;
      int quotecount = 0;
      if(*(tmp + j) == '"'){
        j++;
        while (tmp[j] != '\0') {
            if (tmp[j] == '"') {
                j++;
                break;
            }
            
              
            copySegmend[segmentSize] = tmp[j];
            segmentSize++;
            j++;
        }
      } else {
        while (tmp[j] != '\0') {
        char c = tmp[j];

        if ((c >= '0' && c <= '9') ||
            c == '-' || c == '+' ||
            c == '.' ||
            c == 'e' || c == 'E')
        {
          copySegmend[segmentSize] = c;
          segmentSize++;
          j++;
        } else {
            break;
        }
        }
      }

      copySegmend[segmentSize] = '\0';
      strcpy(newHTML + newHTMLLen, copySegmend);
      newHTMLLen += strlen(copySegmend);
      free(text2);
      free(copySegmend);
      free(element);
      continue;
    }
    if(text[i] == '\n') //stupid windows
      size--;
    newHTML[newHTMLLen] = text[i];
    newHTMLLen++;
  }
  FILE* myfile = fopen(output_html, "w");
  fprintf(myfile, "%s\n", newHTML);
  fclose(myfile);
  free(newHTML);
}



int processTemplate(char* template, char* output, char* data_json) {
  if(checkErrors(template)) {
    return 1;
  }
  checkForDuplicates(template, output);
  copyHTMLBlocks(output, output, data_json);
  copyHTMLThatAreArrays(output, output, data_json);
  return 0;
}

#ifdef TEST
int main() {
  processTemplate("templates/template.html", "templates/output.html", "templates/data.json");
  return 0;
}
#endif