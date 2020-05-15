#include "avg_buffer.h"

#include <string.h>

void avg_buffer__initialize(avg_buffer_s *avg_buffer, uint8_t *buffer, size_t buffer_capacity) {
  avg_buffer->buffer = buffer;
  avg_buffer->write_index = 0;
  avg_buffer->filled = false;
  avg_buffer->capacity = buffer_capacity;
  memset(avg_buffer->buffer, 0, avg_buffer->capacity);
}

uint8_t avg_buffer__get_value(avg_buffer_s *avg_buffer) {
  size_t sum = 0;
  uint8_t avg = 0;

  size_t capacity = avg_buffer->filled ? avg_buffer->capacity : avg_buffer->write_index;

  if (avg_buffer->filled || avg_buffer->write_index > 0) {
    for (size_t i = 0; i < capacity; i++) {
      sum += avg_buffer->buffer[i];
    }

    avg = sum / capacity;
  }

  return avg;
}

void avg_buffer__insert_value(avg_buffer_s *avg_buffer, uint8_t value) {
  avg_buffer->buffer[avg_buffer->write_index++] = value;
  if (!avg_buffer->filled && avg_buffer->write_index >= avg_buffer->capacity) {
    avg_buffer->filled = true;
  }
  avg_buffer->write_index %= avg_buffer->capacity;
}

uint8_t avg_buffer__get_insertions(avg_buffer_s *avg_buffer) {
  return (avg_buffer->filled ? avg_buffer->capacity : avg_buffer->write_index);
}
