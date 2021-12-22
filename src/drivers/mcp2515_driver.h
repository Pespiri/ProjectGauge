#ifndef MCP2515_DRIVER_H
#define MCP2515_DRIVER_H

#include <Aruino_CAN_BUS_MCP2515/mcp_can.h>
#include <stdint.h>
#include <saab_can/data/saab_frame.h>
#include <saab_can/interfaces/can_handle_base.h>
#include <saab_can/saab_can.h>

class Mcp2515Driver : public SCCanHandleBase {
  private:
    MCP_CAN *mcp_handle = nullptr;

    bool is_installed = false;

    uint8_t _cs_pin;
    uint8_t _can_speed;

    unsigned long id_filter = 0;
    unsigned long id_mask = 0;

  public:
    Mcp2515Driver(uint8_t cs_pin, uint8_t can_speed) {
      _cs_pin = cs_pin;
      _can_speed = can_speed;
    }

    /** @brief  Install CAN bus driver */
    bool install() override;
    /** @brief  Uninstall CAN bus driver */
    bool uninstall() override;
    /** @brief  Update list of subscribed frame id's */
    bool update_frame_ids(sc_can_frame_id_list_t id_list) override;
    /** @brief  Read a CAN frame */
    bool read(saab_frame_t *frame) override;
    /** @brief  Write a CAN frame */
    bool write(saab_frame_t frame) override;
};

#endif
