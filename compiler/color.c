#include <color.h>
#include <unistd.h>
#include <stdio.h>

void cm_init(ColorManager* cm) {
    cm->use_color = isatty(fileno(stderr));

    cm->colors[color_red_k] = "\x1b[31m";
    cm->colors[color_blue_k] = "\x1b[34m";
    cm->colors[color_reset_k] = "\x1b[0m";
    cm->colors[color_bold_k] = "\x1b[1m";
}


const char* get_color(ColorManager* cm, Color c) {
    if (!cm->use_color) return "";
    return cm->colors[c];
}
