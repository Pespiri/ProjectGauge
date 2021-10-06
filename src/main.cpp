#include <Arduino.h>
#include <saab_can/saab_can.h>
#include <stdint.h>
#include <stdio.h>

#include "drivers/mcp2515_driver.h"
#include "drivers/stepper_x27_driver.h"
#include "project_defs.h"

#define MAIN_TAG "MAIN"
#define PACKET_TIMEOUT_MS 5000

unsigned long last_packet_received;

/** @brief  Setup ATMEGA328P */
void setup();
/** @brief  Main task loop */
void loop();

/** @brief  Reset Arduino analog and digital pins */
void reset_pins();

/** @brief  Test code for running the gauge */
void test_simulation();

void setup() {
  reset_pins();
  Serial.begin(LOGGER_BAUDRATE);

  SCCanHandleBase *can_handle = new Mcp2515Driver(CAN_PIN_CS, CAN_47KBPS);
  if (!saabcan::sc_install_handle(can_handle)) {
    char buffer[64];
    snprintf(buffer, 64, "ERROR   %s: %s Failed to set CAN handle", MAIN_TAG, __func__);
    Serial.println(buffer);
    if (can_handle) {
      delete can_handle;
    }
  } else {
    uint32_t ids[1];
    ids[0] = (unsigned long)GAUGE_CAN_ID;
    sc_can_frame_id_list_t id_list = { .size = 1, .list = ids };
    saabcan::sc_update_frame_ids(id_list);
  }

  stepper_x27_driver::stepper_x27_cfg config;
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
  saab_frame_t frame;
  if (saabcan::sc_read_frame(&frame) && frame.id == (uint32_t)GAUGE_CAN_ID) {
    uint16_t position = stepper_x27_driver::stepper_x27_calculate_position(frame.data[0], 2);
    stepper_x27_driver::stepper_x27_set_position(position);
    last_packet_received = millis();

    // char buffer[96];
    // snprintf(
    //   buffer, 96, "DEBUG   %s: %s   id: 0x%04x   data: [0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x]",
    //   MAIN_TAG, __func__, (unsigned int)frame.id,
    //   frame.data[0], frame.data[1], frame.data[2], frame.data[3],
    //   frame.data[4], frame.data[5], frame.data[6], frame.data[7]
    // );
    // Serial.println(buffer);
  }

  if (last_packet_received < millis() - PACKET_TIMEOUT_MS) {
    stepper_x27_driver::stepper_x27_set_position(0);
  }

  // test_simulation();
  stepper_x27_driver::stepper_x27_run_steps(25);
}

void reset_pins() {
  const char digital_pins[14] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
  const char analog_pins[6] = { A0, A1, A2, A3, A4, A5 };

  for (uint8_t i = 0; i < 14; i++) {
    pinMode(digital_pins[i], OUTPUT);
    delay(50);
    digitalWrite(digital_pins[i], 0);
    delay(50);
  }

  for (uint8_t i = 0; i < 6; i++) {
    pinMode(analog_pins[i], OUTPUT);
    delay(50);
    analogWrite(analog_pins[i], 0);
    delay(50);
  }
}

void test_simulation() {
  static bool flip_flop;

  uint16_t pos = flip_flop ? 0xFF : 0;
  flip_flop = !flip_flop;
  stepper_x27_driver::stepper_x27_set_position(pos);
  stepper_x27_driver::stepper_x27_run_steps(0xFF);
}