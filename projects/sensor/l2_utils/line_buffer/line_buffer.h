#pragma once

#include <stdbool.h>
#include <string.h>

// Do not access this struct directly in your production code or in unit tests
// These are "internal" details of the code module
typedef struct {
  void *memory;
  size_t max_size;
  size_t max_line_capacity;
  size_t item_count;
  size_t write_index;
  size_t read_index;
} line_buffer_s;

/**
 * Initialize *line_buffer_s with the user provided buffer space and size
 * Use should initialize the buffer with whatever memory they need
 * @code
 *  char memory[256];
 *  line_buffer_s line_buffer = { };
 *  line_buffer__init(&line_buffer, memory, sizeof(memory));
 * @endcode
 */
void line_buffer__init(line_buffer_s *buffer, void *memory, size_t size);

// Adds a byte to the buffer, and returns true if the buffer had enough space to add the byte
bool line_buffer__add_byte(line_buffer_s *buffer, char byte);

// Returns the number of free character space available in a line buffer
size_t line_buffer__get_item_count(line_buffer_s *buffer);

/**
 * if the line buffer has a complete line, it will remove that contents and save it to "char * line"
 * @param line_max_size This is the max size of 'char * line' memory pointer
 */
bool line_buffer__remove_line(line_buffer_s *buffer, char *line, size_t line_max_size);
