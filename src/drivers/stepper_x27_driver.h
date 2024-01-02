#ifndef STEPPER_X27_DRIVER_H
#define STEPPER_X27_DRIVER_H

#include <AccelStepper.h>
#include <stdint.h>

#define STEPPER_X27_DRIVER_TAG "STEPPER_X27"

namespace stepper_x27_driver {
  typedef struct STEPPER_X27_CFG_STRUCT {
    AccelStepper::MotorInterfaceType mode; // wire mode

    uint8_t pin1;
    uint8_t pin2;
    uint8_t pin3;
    uint8_t pin4;

    uint32_t start_offset; // start offset
    uint32_t full_range;   // full range (angle)
    uint32_t short_range;  // main range (angle) (0 for full range)
    uint32_t steps;        // total steps
    uint32_t speed;        // max speed
    uint32_t acceleration; // acceleration
  } stepper_x27_cfg_t;

  /**
   * @brief   Initialize stepper driver
   *
   * @param[in]   cfg   stepper configuration
   */
  bool stepper_x27_init(stepper_x27_cfg_t cfg);
  /** @brief  Deinitialize stepper driver */
  bool stepper_x27_deinit();

  /** @brief  Run for a set amount of steps */
  void stepper_x27_run_steps(uint16_t steps);
  /** @brief  Set new stepper position */
  void stepper_x27_set_position(uint16_t pos);
  /**
   * @brief   Calculate new position based on stepper limits
   *
   * @param[in]   pos         requested position (0 - 255)
   * @param[in]   dead_zone   steps to filter
   *
   * @return  new position based on range and offset
   */
  uint16_t stepper_x27_calculate_position(uint8_t pos, uint8_t dead_zone);
  /** @brief  Drive stepper to home position */
  void stepper_x27_go_home();
}; // namespace stepper_x27_driver

#endif
