#include "ultrasonic.h"

#include <math.h>

#include "sys_time.h"

/**
  Reference:
  https://www.mpja.com/download/hc-sr04_ultrasonic_module_user_guidejohn.pdf

  Ultrasonic ranging module HC - SR04 provides 2cm - 400cm non-contact
  measurement function, the ranging accuracy can reach to 3mm. The modules
  includes ultrasonic transmitters, receiver and control circuit. The basic principle
  of work:
  (1) Using IO trigger for at least 10us high level signal,
  (2) The Module automatically sends eight 40 kHz and detect whether there is a
  pulse signal back.
  (3) IF the signal back, through high level , time of high output IO duration is
  the time from sending ultrasonic to returning.
  Test distance = (Time from pulse until Echo rising-to-falling edge × velocity of sound (340M/S) / 2,

  Conversion to inches:
  time * Speed of Sound / 2 = time * (13503.9 in/s / 1000000 us) / 2 = time * 0.00675195 in/us = time / 148.105

  Working Voltage DC 5 V
  Working Current 15mA
  Working Frequency 40Hz
  Max Range 4m
  Min Range 2cm
  MeasuringAngle 15 degree
  Trigger Input Signal 10uS TTL pulse
  Echo Output Signal Input TTL lever signal and the range in proportion
 */

void ultrasonic__initialize(ultrasonic_s *ultrasonic, gpio__port_e trigger_port, uint8_t trigger_pin_number_0_to_31,
                            gpio__port_e echo_port, uint8_t echo_pin_number_0_to_31) {
  ultrasonic->trigger_out = gpio__construct_as_output(trigger_port, trigger_pin_number_0_to_31);
  ultrasonic->echo_in = gpio__construct_as_input(echo_port, echo_pin_number_0_to_31);
  ultrasonic->distance_us = 0U;
  ultrasonic->last_trigger_time_us = 0U;
  ultrasonic->start_new_reading = true;

  gpio__reset(ultrasonic->trigger_out);
}

// Blocking get range function which can stall at maximum (60 ms + 10 us) = 60.01 ms
void ultrasonic__get_range_blocking(ultrasonic_s *ultrasonic) {
  // Recommend to send trigger pulses at 50 us rate
  if ((sys_time__get_uptime_us() - ultrasonic->last_trigger_time_us) > 50U) {
    const uint64_t timer = sys_time__get_uptime_us();
    gpio__set(ultrasonic->trigger_out);
    while ((sys_time__get_uptime_us() - timer) < 10U) {
      ; // Hold trigger line high for 10 microseconds
    }
    gpio__reset(ultrasonic->trigger_out);
    ultrasonic->last_trigger_time_us = sys_time__get_uptime_us();

    bool echo_rising_edge_found = false;
    uint64_t echo_high_trigger_time_us = 0U;

    do {
      if (gpio__get(ultrasonic->echo_in)) { // sample echo until high
        echo_rising_edge_found = true;
        echo_high_trigger_time_us = sys_time__get_uptime_us() - ultrasonic->last_trigger_time_us;
      } else {
        if (echo_rising_edge_found) {                          // Echo was sampled high and deasserted
          ultrasonic->distance_us = echo_high_trigger_time_us; // Divide by 148U for inch conversion
          break;                                               // Exit when echo falling edge detected
        }
      }

    } while ((sys_time__get_uptime_us() - ultrasonic->last_trigger_time_us) < 60000U); // Timeout on 60 milliseconds
  }
}

// Blocks for 10 us for toggling Trigger line
void ultrasonic__initiate_range(ultrasonic_s *ultrasonic) {
  // Send trigger on the start of a new reading through interrupts or when a 60 ms timeout occurs
  if (ultrasonic->start_new_reading || (sys_time__get_uptime_us() - ultrasonic->last_trigger_time_us) > 60000U) {
    ultrasonic->start_new_reading = false;
    const uint64_t timer = sys_time__get_uptime_us();
    gpio__set(ultrasonic->trigger_out);
    while ((sys_time__get_uptime_us() - timer) < 10U) {
      ; // Hold trigger line high for 10 microseconds
    }
    gpio__reset(ultrasonic->trigger_out);
    ultrasonic->last_trigger_time_us = sys_time__get_uptime_us();
  }
}
