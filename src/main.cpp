#include "drivers/mcp2515_driver.h"
#include "drivers/stepper_x27_driver.h"
#include "project_defs.h"

#include <Arduino.h>
#include <saab_can/cpp_utils/saab_can_cpp_utils.hpp>
#include <saab_can/saab_can.h>
#include <saab_can/utilities/can_reader_util.h>
#include <stdint.h>
#include <stdio.h>

#define MAIN_TAG          "MAIN"
#define PACKET_TIMEOUT_MS 5000

unsigned long last_packet_received;

/** @brief  Setup */
void setup();
/** @brief  Main loop */
void loop();

/** @brief  Handle frame callback */
void handle_frame(saab_frame_t frame);
/** @brief  Handle gauge value frame callback */
void handle_gauge_value_frame(saab_frame_t frame);

/** @brief  Test code for running the gauge */
void gauge_position_simulation_test();

/** @brief  Print saab_frame to console */
void print_frame(saab_frame_t frame, const char *func_name);

void setup() {
  Serial.begin(LOGGER_BAUDRATE);

  Mcp2515Driver *can_handle = new Mcp2515Driver(CAN_PIN_CS, CAN_47K619BPS);
  can_handle->initialize();
  sc_err_t err = sc_assign_can_handle_class_obj(can_handle);
  if (err != SC_OK) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "ERROR   %s: %s failed to assign CAN handle [code %i]", MAIN_TAG, __func__, err);
    Serial.println(buffer);
    delete can_handle;
  }

  sc_can_reader_subscription_t sub1 = {.id = "SUB_1", .frame_id = GAUGE_CAN_ID, .cb = handle_gauge_value_frame};
  sc_can_reader_subscribe(sub1);

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
  sc_can_reader_advance();
  if (last_packet_received < (millis() - PACKET_TIMEOUT_MS)) {
    // packet timeout, return home
    stepper_x27_driver::stepper_x27_set_position(0);
  }

  // gauge_position_simulation_test();
  stepper_x27_driver::stepper_x27_run_steps(3);
}

void handle_frame(saab_frame_t frame) {
  print_frame(frame, __func__);
}

void handle_gauge_value_frame(saab_frame_t frame) {
  // print_frame(frame, __func__);
  uint16_t position = stepper_x27_driver::stepper_x27_calculate_position(frame.data[0], 2);
  stepper_x27_driver::stepper_x27_set_position(position);
  last_packet_received = millis();
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

void print_frame(saab_frame_t frame, const char *name) {
  if (!name) {
    name = "_";
  }

  char time_str[10], buffer_str[96];
  double time = millis() / 1000.0F;
  dtostrf(time, sizeof(time_str) - 3, 2, time_str);

  snprintf(
    buffer_str, sizeof(buffer_str),
    "DEBUG   %s: %s %10ss   id 0x%03x   data [%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x]\n",
    MAIN_TAG, name, time_str, (uint16_t)frame.id,
    frame.data[0], frame.data[1], frame.data[2], frame.data[3],
    frame.data[4], frame.data[5], frame.data[6], frame.data[7]);

  Serial.print(buffer_str);
}
