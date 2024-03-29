#ifndef MCP2515_DRIVER_H
#define MCP2515_DRIVER_H

#include <Aruino_CAN_BUS_MCP2515/mcp_can.h>
#include <saab_can/cpp_utils/interfaces/can_handle_object.hpp>
#include <saab_can/data/saab_frame.h>
#include <stdint.h>

class Mcp2515Driver : public SCCanHandleObject {
private:
  typedef struct MCP_CAN_PIN_CFG_STRUCT {
    uint8_t cs_pin;
    uint8_t can_speed;
  } mcp_can_pin_cfg_t;

  typedef struct MCP_CAN_FILTER_STRUCT {
    unsigned long id_filter;
    unsigned long id_mask;
  } mcp_can_filter_t;

  MCP_CAN *mcp_handle;

  bool is_installed;

  mcp_can_pin_cfg_t mcp_can_pin_cfg;
  mcp_can_filter_t mcp_can_filter;

public:
  Mcp2515Driver(uint8_t cs_pin, uint8_t can_speed) {
    mcp_can_pin_cfg = {.cs_pin = cs_pin, .can_speed = can_speed};

    mcp_handle = nullptr;
    is_installed = false;
    mcp_can_filter = {};
  }

  ~Mcp2515Driver() { deinitialize(); }

  /** @brief  Initialize CAN bus driver */
  bool initialize();
  /** @brief  Deinitialize CAN bus driver */
  bool deinitialize();

  /** @brief  Update list of subscribed frame id's */
  bool update_frame_ids(sc_can_frame_id_list_t id_list) override;
  /** @brief  Read a CAN frame */
  bool read(saab_frame_t *frame) override;
  /** @brief  Write a CAN frame */
  bool write(saab_frame_t frame) override;
};

#endif
