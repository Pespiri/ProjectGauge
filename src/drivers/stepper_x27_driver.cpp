#include "stepper_x27_driver.h"

namespace stepper_x27_driver {
  AccelStepper *stepper_handle;
  stepper_x27_cfg stepper_config;

  bool stepper_x27_init(stepper_x27_cfg config) {
    if (!config.mode) {
      return false;
    } else if (stepper_handle) {
      return true;
    }

    memcpy(&stepper_config, &config, sizeof(stepper_x27_cfg));
    stepper_handle = new AccelStepper(
      stepper_config.mode,
      stepper_config.pin1,
      stepper_config.pin2,
      stepper_config.pin3,
      stepper_config.pin4,
      true
    );

    stepper_x27_go_home();
    return true;
  }

  bool stepper_x27_deinit() {
    if (!stepper_handle) {
      return true;
    }

    stepper_x27_go_home();
    stepper_handle->disableOutputs();
    delete stepper_handle;
    stepper_handle = nullptr;

    return true;
  }

  void stepper_x27_run_steps(uint16_t steps) {
    if (!stepper_handle) {
      return;
    }

    uint16_t steps_remaining = abs(stepper_handle->targetPosition() - stepper_handle->currentPosition());
    int16_t steps_taken = steps_remaining;
    int16_t steps_to_go = steps_remaining - steps;
    while (stepper_handle->run() && steps_taken > steps_to_go);
  }

  void stepper_x27_set_position(uint16_t position) {
    if (!stepper_handle) {
      return;
    }

    uint8_t multiplier = stepper_config.mode == AccelStepper::HALF3WIRE || stepper_config.mode == AccelStepper::HALF4WIRE ? 2 : 1;
    uint32_t max_range = (!stepper_config.short_range ? stepper_config.full_range : stepper_config.short_range);
    max_range *= multiplier;
    if (position > max_range) {
      position = max_range;
    }

    stepper_handle->moveTo(position + (stepper_config.start_offset * multiplier));
  }

  uint16_t stepper_x27_calculate_position(uint8_t position, uint8_t filter) {
    if (!stepper_handle) {
      return 0;
    }

    static uint16_t latest_calc_pos = 0;
    uint8_t multiplier = stepper_config.mode == AccelStepper::HALF3WIRE || stepper_config.mode == AccelStepper::HALF4WIRE ? 2 : 1;
    uint16_t calc_pos = (!stepper_config.short_range ? stepper_config.full_range : stepper_config.short_range) * ((float)position / (0xFF));
    calc_pos *= multiplier;

    // filter out small movements
    if (calc_pos - filter > latest_calc_pos || calc_pos + filter < latest_calc_pos) {
      latest_calc_pos = calc_pos;
    }

    return latest_calc_pos;
  }

  void stepper_x27_go_home() {
    if (!stepper_handle) {
      return;
    }

    // set speed and acceleration to slow and jam stepper counter-clockwise to stop
    uint8_t multiplier = stepper_config.mode == AccelStepper::HALF3WIRE || stepper_config.mode == AccelStepper::HALF4WIRE ? 2 : 1;
    stepper_handle->setSpeed(200);
    stepper_handle->setAcceleration(50);
    stepper_handle->setCurrentPosition((stepper_config.full_range + 10) * multiplier);
    stepper_handle->runToNewPosition(0);

    // go to home position
    stepper_x27_set_position(0);
    stepper_handle->runToPosition();

    // reset speed and acceleration
    stepper_handle->setSpeed(stepper_config.speed);
    stepper_handle->setAcceleration(stepper_config.acceleration);
  }
}; // namespace stepper_x27_driver
