#pragma once

#ifdef DEBUG_ENABLED 
#define TERMINAL_RESET       "\x1b[0m"
#define TERMINAL_BLACK       "\x1b[30m"
#define TERMINAL_RED         "\x1b[31m"
#define TERMINAL_GREEN       "\x1b[32m"
#define TERMINAL_YELLOW      "\x1b[33m"
#define TERMINAL_BLUE        "\x1b[34m"
#define TERMINAL_MAGENTA     "\x1b[35m"
#define TERMINAL_CYAN        "\x1b[36m"
#define TERMINAL_WHITE       "\x1b[37m"
#define TERMINAL_BOLD        "\x1b[1m"
#define TERMINAL_UNDERLINE   "\x1b[4m"
#   include <cstdio>
#   define STRFY(x) #x
#   define STR(x) STRFY(x)
#   define DEBUG_LOG(format_str, ...)   printf("%-32s: " TERMINAL_CYAN format_str TERMINAL_RESET , __FILE__ ":" STR(__LINE__) ,##__VA_ARGS__)
#   define DEBUG_ERR(format_str, ...)  fprintf(stderr, "%-32s: " TERMINAL_RED format_str TERMINAL_RESET, __FILE__ ":" STR(__LINE__) , ##__VA_ARGS__);
#else
#   define DEBUG_LOG(format_str, ...)
#   define DEBUG_ERR(format_str, ...)
#endif 