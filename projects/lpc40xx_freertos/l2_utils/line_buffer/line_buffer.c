#include "line_buffer.h"

#include <stdio.h>

void line_buffer__init(line_buffer_s *buffer, void *memory, size_t size) {
  buffer->memory = memory;
  buffer->max_size = size;
  buffer->item_count = 0;
  buffer->write_index = 0;
  buffer->read_index = 0;
  memset(buffer->memory, 0, buffer->max_size);
}

bool line_buffer__add_byte(line_buffer_s *buffer, char byte) {
  bool add_status = false;

  if (line_buffer__get_item_count(buffer) != buffer->max_size) {
    *(char *)(buffer->memory + buffer->write_index) = byte;
    buffer->write_index = (buffer->write_index + 1) % buffer->max_size;
    buffer->item_count++;
    add_status = true;
  }

  return add_status;
}

size_t line_buffer__get_item_count(line_buffer_s *buffer) { return buffer->item_count; }

bool line_buffer__remove_line(line_buffer_s *buffer, char *line, size_t line_max_size) {
  size_t line_index = 0;

  if (line_buffer__get_item_count(buffer) > 0) {

    const char *line_buffer = (char *)buffer->memory;
    const size_t max_line_capacity = line_buffer__get_item_count(buffer) < (line_max_size - 1)
                                         ? line_buffer__get_item_count(buffer)
                                         : line_max_size - 1;
    bool found_new_line_character = false;

    while (line_index < max_line_capacity && !found_new_line_character) {
      found_new_line_character = line_buffer[buffer->read_index] == '\n';
      line[line_index++] = found_new_line_character ? '\0' : line_buffer[buffer->read_index];
      buffer->read_index = (buffer->read_index + 1) % buffer->max_size;
      buffer->item_count--;
    }

    if (!found_new_line_character) {
      line[line_index] = '\0';
    }
  }

  return line_index;
}
