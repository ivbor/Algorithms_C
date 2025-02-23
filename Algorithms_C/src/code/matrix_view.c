#include "../headers/matrix_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef Mystrcat
#include "../../include/mystrcat.c"
#endif

#ifndef Mystrdup
#include "../../include/mystrdup.c"
#endif

#ifndef logger
#include "../../include/logger.h"
#endif

#define BUFFER_SIZE 16384       // Buffer for full matrix string
#define ELEMENT_BUFFER_SIZE 32  // Buffer for formatting one element
#define FORMAT_WIDTH 5          // Width for formatted numbers

void printMatrix(int **matrix, int rows, int columns, int indexes) {
    // Allocate memory for the entire output buffer
    char *to_print = (char *)malloc(BUFFER_SIZE);
    if (!to_print) {
        log_message(
            LOG_FATAL, strdup("Failed to allocate memory for to_print.")
        );
        exit(EXIT_FAILURE);
    }

    char *to_print_copy = to_print;
    to_print[0] = '\0';  // Initialize empty string

    char one_element[ELEMENT_BUFFER_SIZE];

    // Printing column indexes if enabled
    if (indexes) {
        for (int i = 0; i <= columns; i++) {
            int required_length = snprintf(NULL, 0, "%*d ", FORMAT_WIDTH, i);
            if (required_length < ELEMENT_BUFFER_SIZE) {
                snprintf(
                    one_element, ELEMENT_BUFFER_SIZE, "%*d ", FORMAT_WIDTH, i
                );
                to_print = mystrcat(to_print, one_element);
            } else {
                log_message(
                    LOG_FATAL,
                    strdup("Allocated insufficient memory for one_element.")
                );
                log_message(
                    LOG_FATAL, strdup("Difference (required - allocated) = ")
                );

                char char_length[12];  // Enough to store any int value
                snprintf(
                    char_length, sizeof(char_length), "%d",
                    required_length - ELEMENT_BUFFER_SIZE
                );
                log_message(LOG_FATAL, char_length);

                free(to_print_copy);
                exit(EXIT_FAILURE);
            }
        }
        to_print = mystrdup("\n", to_print);
    }

    // Printing matrix with optional row indexes
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j <= columns; j++) {
            if (indexes && j == 0) {
                snprintf(
                    one_element, ELEMENT_BUFFER_SIZE, "%*d ", FORMAT_WIDTH, i
                );
                to_print = mystrcat(to_print, one_element);
            }
            if (j < columns) {
                snprintf(
                    one_element, ELEMENT_BUFFER_SIZE, "%*d ", FORMAT_WIDTH,
                    matrix[i][j]
                );
                to_print = mystrcat(to_print, one_element);
            }
        }
        to_print = mystrdup("\n\n", to_print);
        ;
    }
    mystrdup("\n", to_print);

    // Log and free memory
    log_message(LOG_DEBUG, to_print_copy);
    free(to_print_copy);
}
