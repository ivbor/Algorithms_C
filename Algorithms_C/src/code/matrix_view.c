#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define ELEMENT_BUFFER_SIZE 32
#define FORMAT_WIDTH 5

#ifndef logger
#include "../../include/logger.h"
#endif

void printMatrix(int **matrix, int rows, int columns, int indexes) {
    // Allocate memory for the entire output buffer
    char *to_print = (char *)malloc(BUFFER_SIZE);
    if (!to_print) {
        log_message(LOG_FATAL, "Failed to allocate memory for to_print.");
        exit(EXIT_FAILURE);
    }
    to_print[0] = '\0';

    char one_element[ELEMENT_BUFFER_SIZE];

    // Printing column indexes if enabled
    if (indexes) {
        for (int i = 0; i <= columns; i++) {
            int required_length = snprintf(NULL, 0, "%*d ", FORMAT_WIDTH, i);
            if (required_length < ELEMENT_BUFFER_SIZE) {
                snprintf(
                    one_element, ELEMENT_BUFFER_SIZE, "%*d ", FORMAT_WIDTH, i
                );
                strncat(
                    to_print, one_element, BUFFER_SIZE - strlen(to_print) - 1
                );
            } else {
                log_message(
                    LOG_FATAL, "Allocated insufficient memory for one_element."
                );
                log_message(LOG_FATAL, "Difference (required - allocated) = ");

                char char_length[12];
                snprintf(
                    char_length, sizeof(char_length), "%d",
                    required_length - ELEMENT_BUFFER_SIZE
                );
                log_message(LOG_FATAL, char_length);

                free(to_print);
                exit(EXIT_FAILURE);
            }
        }
        strncat(to_print, "\n", BUFFER_SIZE - strlen(to_print) - 1);
    }

    // Printing matrix with optional row indexes
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j <= columns; j++) {
            if (indexes && j == 0) {
                snprintf(
                    one_element, ELEMENT_BUFFER_SIZE, "%*d ", FORMAT_WIDTH, i
                );
                strncat(
                    to_print, one_element, BUFFER_SIZE - strlen(to_print) - 1
                );
            }
            if (j < columns) {
                snprintf(
                    one_element, ELEMENT_BUFFER_SIZE, "%*d ", FORMAT_WIDTH,
                    matrix[i][j]
                );
                strncat(
                    to_print, one_element, BUFFER_SIZE - strlen(to_print) - 1
                );
            }
        }
        strncat(to_print, "\n\n", BUFFER_SIZE - strlen(to_print) - 1);
    }
    strncat(to_print, "\n", BUFFER_SIZE - strlen(to_print) - 1);

    // Log and free memory
    log_message(LOG_DEBUG, to_print);
    free(to_print);
}
