#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t *buffer;
  size_t capacity;
  size_t write_index;
  bool filled;
} avg_buffer_s;

void avg_buffer__initialize(avg_buffer_s *avg_buffer, uint8_t *buffer, size_t buffer_capacity);

uint8_t avg_buffer__get_value(avg_buffer_s *avg_buffer);

void avg_buffer__insert_value(avg_buffer_s *avg_buffer, uint8_t value);

uint8_t avg_buffer__get_insertions(avg_buffer_s *avg_buffer);
