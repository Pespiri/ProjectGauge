#include "stepper_x27_driver.h"

namespace stepper_x27_driver {
  typedef struct {
    uint32_t max_range;
    uint8_t multiplier;
  } stepper_x27_range_cfg_t;

  AccelStepper *stepper_handle;

  stepper_x27_cfg         stepper_cfg;
  stepper_x27_range_cfg_t stepper_range_cfg;

  bool stepper_x27_init(stepper_x27_cfg cfg) {
    if (!cfg.mode) {
      return false;
    } else if (stepper_handle) {
      return true;
    }

    memcpy(&stepper_cfg, &cfg, sizeof(stepper_x27_cfg));
    stepper_range_cfg = {
      .max_range = (!stepper_cfg.short_range ? stepper_cfg.full_range : stepper_cfg.short_range),
      .multiplier = (uint8_t)(1 + (stepper_cfg.mode == AccelStepper::HALF3WIRE || stepper_cfg.mode == AccelStepper::HALF4WIRE)),
    };

    stepper_handle = new AccelStepper(
      stepper_cfg.mode,
      stepper_cfg.pin1,
      stepper_cfg.pin2,
      stepper_cfg.pin3,
      stepper_cfg.pin4,
      true
    );

    stepper_handle->setSpeed(stepper_cfg.speed);
    stepper_handle->setAcceleration(stepper_cfg.acceleration);

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

    if (steps > (stepper_range_cfg.max_range * stepper_range_cfg.multiplier)) {
      steps = stepper_range_cfg.max_range * stepper_range_cfg.multiplier;
    }

    int16_t steps_to_go = abs(stepper_handle->distanceToGo()) - steps;
    while (stepper_handle->run() && abs(stepper_handle->distanceToGo()) >= steps_to_go);
  }

  void stepper_x27_set_position(uint16_t position) {
    if (!stepper_handle) {
      return;
    }

    uint32_t range = stepper_range_cfg.max_range * stepper_range_cfg.multiplier;
    if (position > range) {
      position = range;
    }

    stepper_handle->moveTo(position + (stepper_cfg.start_offset * stepper_range_cfg.multiplier));
  }

  uint16_t stepper_x27_calculate_position(uint8_t position, uint8_t dead_zone) {
    static uint16_t last_calc_pos = 0;
    uint16_t calc_pos = stepper_range_cfg.max_range * ((float)position / (0xFF));
    calc_pos *= stepper_range_cfg.multiplier;

    // filter out small movements
    if ((calc_pos - dead_zone) > last_calc_pos || (calc_pos + dead_zone) < last_calc_pos) {
      last_calc_pos = calc_pos;
    }

    return last_calc_pos;
  }

  void stepper_x27_go_home() {
    if (!stepper_handle) {
      return;
    }

    // set speed and acceleration to slow and jam stepper counter-clockwise to stop
    stepper_handle->setSpeed(175);
    stepper_handle->setAcceleration(75);
    stepper_handle->setCurrentPosition((stepper_cfg.full_range + 10) * stepper_range_cfg.multiplier);
    stepper_handle->runToNewPosition(0);

    // go to home position
    stepper_x27_set_position(0);
    stepper_handle->runToPosition();

    // reset speed and acceleration
    stepper_handle->setSpeed(stepper_cfg.speed);
    stepper_handle->setAcceleration(stepper_cfg.acceleration);
  }
}; // namespace stepper_x27_driver
