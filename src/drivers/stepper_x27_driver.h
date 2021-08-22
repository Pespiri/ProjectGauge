#ifndef STEPPER_X27_DRIVER_H
#define STEPPER_X27_DRIVER_H

#include <AccelStepper.h>
#include <stdint.h>
#include <stdlib.h>

#define STEPPER_X27_DRIVER_TAG "STEPPER_X27"

namespace stepper_x27_driver {
  typedef struct {
    uint8_t mode;           // Stepper wire mode
    uint8_t pin1;           // Pin 1
    uint8_t pin2;           // Pin 2
    uint8_t pin3;           // Pin 3
    uint8_t pin4;           // Pin 4

    uint32_t start_offset;  // Stepper start offset
    uint32_t full_range;    // Stepper full range (angle)
    uint32_t short_range;   // Stepper main range (angle) (0 for full range)
    uint32_t steps;         // Stepper total steps
    uint32_t speed;         // Stepper max speed
    uint32_t acceleration;  // Stepper acceleration
  } stepper_x27_cfg;

  /**
   * @brief  Initialize stepper driver
   *
   * @param[in]   config  stepper configuration
   */
  bool stepper_x27_init(stepper_x27_cfg config);
  /** @brief  Deinitialize stepper driver */
  bool stepper_x27_deinit();

  /** @brief  Run for a set amount of steps */
  void stepper_x27_run_steps(uint16_t steps);
  /** @brief  Set new stepper position */
  void stepper_x27_set_position(uint16_t position);
  /** @brief  Calculate new position based on stepper limits */
  uint16_t stepper_x27_calculate_position(uint8_t position, uint8_t filter);
  /** @brief  Drive stepper to home position */
  void stepper_x27_go_home();
}; // namespace stepper_x27_driver

#endif
