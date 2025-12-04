#include <stdio.h>
#include <stdlib.h>

#define SOURCE_DIR "./tmpEngine_source/"
#define OUTPUT_DIR "./tmpEngineLibs/"
#define SIZE 256

int main() {

    char block_manager[SIZE];
    char blocks_serving[SIZE];
    char html_parsing[SIZE];
    char main[SIZE];
    char _static[SIZE];
    char _dynamic[SIZE];

    sprintf(block_manager, "gcc -c %sblock_manager.c -o %sblock_manager.o", SOURCE_DIR, OUTPUT_DIR);
    sprintf(blocks_serving, "gcc -c %sblocks_serving.c -o %sblocks_serving.o", SOURCE_DIR, OUTPUT_DIR);
    sprintf(html_parsing, "gcc -c %shtml_parsing.c -o %shtml_parsing.o", SOURCE_DIR, OUTPUT_DIR);
    sprintf(main, "gcc -c %smain.c -o %smain.o", SOURCE_DIR, OUTPUT_DIR);

    sprintf(_static, "ar rc %slibstatic.a %smain.o %shtml_parsing.o %sblocks_serving.o %sblock_manager.o", OUTPUT_DIR, OUTPUT_DIR, OUTPUT_DIR, OUTPUT_DIR, OUTPUT_DIR);
    sprintf(_dynamic, "gcc --shared -o %slibdynamic.dll %smain.o %shtml_parsing.o %sblocks_serving.o %sblock_manager.o", OUTPUT_DIR, OUTPUT_DIR, OUTPUT_DIR, OUTPUT_DIR, OUTPUT_DIR);

    system(block_manager);
    system(blocks_serving);
    system(html_parsing);
    system(main);

    system(_static);
    system(_dynamic);
    
    return 0;
}