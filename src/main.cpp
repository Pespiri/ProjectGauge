#include <Arduino.h>
#include <saab_can/saab_can.h>
#include <saab_can/services/can_reader_service.h>
#include <stdint.h>
#include <stdio.h>

#include "drivers/mcp2515_driver.h"
#include "drivers/stepper_x27_driver.h"
#include "project_defs.h"

#define MAIN_TAG "MAIN"
#define PACKET_TIMEOUT_MS 5000

unsigned long last_packet_received;

/** @brief  Setup */
void setup();
/** @brief  Main loop */
void loop();

/** @brief  Handle frame callback */
void handle_frame(saab_frame_t frame);

/** @brief  Reset Arduino analog and digital pins */
void reset_pins();

/** @brief  Test code for running the gauge */
void gauge_position_simulation_test();

void setup() {
  Serial.begin(LOGGER_BAUDRATE);
  reset_pins();

  SCCanHandleObject *can_handle = new Mcp2515Driver(CAN_PIN_CS, CAN_47K619BPS);
  if (!saabcan::sc_install_handle(can_handle)) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "ERROR   %s: %s failed to set CAN handle", MAIN_TAG, __func__);
    Serial.println(buffer);
    if (can_handle) {
      delete can_handle;
    }
  }

  sc_can_reader_subscription_t sub1 = { .id = "SUB_1", .frame_id = GAUGE_CAN_ID, .cb = handle_frame };
  saabcan::sc_can_reader_subscribe(sub1);

  stepper_x27_driver::stepper_x27_cfg config = {};
  config.mode = AccelStepper::HALF4WIRE;
  config.pin1 = GAUGE_PIN_1;
  config.pin2 = GAUGE_PIN_2;
  config.pin3 = GAUGE_PIN_3;
  config.pin4 = GAUGE_PIN_4;
  config.start_offset = 20;
  config.full_range = 315 * 2;
  config.short_range = 75 * 2;
  config.steps = 600;
  config.speed = 5000;
  config.acceleration = 715;
  stepper_x27_driver::stepper_x27_init(config);
}

void loop() {
  saabcan::sc_can_reader_advance();
  if (last_packet_received < (millis() - PACKET_TIMEOUT_MS)) {
    // packet timeout, return home
    stepper_x27_driver::stepper_x27_set_position(0);
  }

  // gauge_position_simulation_test();
  stepper_x27_driver::stepper_x27_run_steps(3);
}

void handle_frame(saab_frame_t frame) {
  // char buffer[80];
  // snprintf(
  //   buffer, sizeof(buffer), "DEBUG   %s: %s   id: 0x%03x   data: [%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x]",
  //   MAIN_TAG, __func__, (unsigned int)frame.id,
  //   frame.data[0], frame.data[1], frame.data[2], frame.data[3],
  //   frame.data[4], frame.data[5], frame.data[6], frame.data[7]
  // );
  // Serial.println(buffer);

  uint16_t position = stepper_x27_driver::stepper_x27_calculate_position(frame.data[0], 2);
  stepper_x27_driver::stepper_x27_set_position(position);
  last_packet_received = millis();
}

void reset_pins() {
  const char pins[] = {
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    A0, A1, A2, A3, A4, A5
  };

  for (uint8_t i = 0; i < sizeof(pins); i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
    delay(25);
  }
}

void gauge_position_simulation_test() {
  static bool flip_flop;
  static uint8_t tracker = 0;

  tracker = flip_flop ? tracker - 5 : tracker + 5;
  if (tracker == 0 || tracker == 255) {
    flip_flop = !flip_flop;
  }

  stepper_x27_driver::stepper_x27_set_position(flip_flop ? 0 : (uint16_t)~0);
}
