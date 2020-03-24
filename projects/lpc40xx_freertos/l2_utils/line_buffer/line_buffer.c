#include <string.h>

#include "line_buffer.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static bool line_buffer__private_is_initialized(const line_buffer_s *buffer) {
  bool buffer_is_initialized = false;
  if (NULL != buffer && NULL != buffer->memory && 0U != buffer->max_size) {
    buffer_is_initialized = true;
  }
  return buffer_is_initialized;
}

static bool line_buffer__private_is_full(const line_buffer_s *buffer) {
  return (buffer->character_count == buffer->max_size - 1U);
}

static bool line_buffer__private_is_empty(const line_buffer_s *buffer) { return (0U == buffer->character_count); }

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void line_buffer__init(line_buffer_s *buffer, void *memory, size_t size) {
  if (NULL != buffer && NULL != memory && 0U != size) {
    (void)memset(buffer, 0U, sizeof(line_buffer_s));
    (void)memset(memory, 0U, size);
    buffer->memory = (char *)memory;
    buffer->max_size = size;
    buffer->write_index = 0U;
    buffer->read_index = 0U;
    buffer->character_count = 0U;
    buffer->number_of_lines = 0U;
  }
}

bool line_buffer__add_byte(line_buffer_s *buffer, char byte) {
  bool add_byte_status = false;
  if (line_buffer__private_is_initialized(buffer) && !line_buffer__private_is_full(buffer)) {
    buffer->memory[buffer->write_index] = byte;
    buffer->write_index = (buffer->write_index + 1U) % buffer->max_size;
    add_byte_status = true;
    if ('\n' == byte) {
      buffer->number_of_lines++;
    }
    buffer->character_count++;
  }
  return add_byte_status;
}

bool line_buffer__remove_line(line_buffer_s *buffer, char *line, size_t line_max_size) {
  bool remove_line_status = false;

  if (!line_buffer__private_is_initialized(buffer) || line_buffer__private_is_empty(buffer)) {
    // buffer  is empty or uninitialized
  } else if (0U == line_max_size || NULL == line) {
    // parameter errors
  } else if (0U == buffer->number_of_lines) {
    // no lines to remove
  } else {
    const size_t minimum_buffer_length = buffer->max_size < line_max_size ? buffer->max_size : line_max_size;
    (void)memset(line, 0U, line_max_size);
    size_t line_index = 0U;
    while (line_index < minimum_buffer_length &&
           '\n' != buffer->memory[(buffer->read_index + line_index) % buffer->max_size] &&
           '\r' != buffer->memory[(buffer->read_index + line_index) % buffer->max_size]) {
      line[line_index] = buffer->memory[(buffer->read_index + line_index) % buffer->max_size];
      line_index++;
    }
    if ('\r' == buffer->memory[(buffer->read_index + line_index) % buffer->max_size]) {
      line_index++;
    }
    if ('\n' == buffer->memory[(buffer->read_index + line_index) % buffer->max_size]) {
      line_index++;
    }
    line[minimum_buffer_length - 1U] = '\0';
    buffer->character_count -= line_index;
    buffer->read_index = (buffer->read_index + line_index) % buffer->max_size;
    buffer->number_of_lines--;
    remove_line_status = true;
  }

  return remove_line_status;
}