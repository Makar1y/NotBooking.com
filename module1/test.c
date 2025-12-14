#include <stdio.h>
#include <stdlib.h>
#include "module1.h"

int main() {
  if(checkErrors("../templates/template.html")) {
    return 0;
  }
  checkForDuplicates("../templates/template.html", "test.html");
  copyHTMLBlocks("test.html", "test.html", "../templates/data.json");
  copyHTMLThatAreArrays("test.html", "test.html", "../templates/data.json");
  return 0;
}

/* to compile: 
cd module1
gcc module1.c test.c -o test.exe
.\test.exe
*/