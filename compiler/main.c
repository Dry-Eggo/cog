#include <stdio.h>
#include <juve_utils.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    juve_buffer* jb = jb_create();

    jb_read_entire_file("makefile", jb);
    
    char* str = jb_str(jb);
    printf("result: %s\n", str);

    free(str);
    return 0;
}
