#include "mcp2515_driver.h"

bool Mcp2515Driver::install() {
  if (mcp_handle || is_installed) {
    return true;
  }

  mcp_handle = new MCP_CAN(_cs_pin);
  if (mcp_handle && mcp_handle->begin(_can_speed) == CAN_OK) {
    is_installed = true;
    return true;
  }

  return false;
}

bool Mcp2515Driver::uninstall() {
  if (mcp_handle) {
    delete mcp_handle;
    mcp_handle = nullptr;
  }

  is_installed = false;
  return true;
}

bool Mcp2515Driver::update_frame_ids(sc_can_frame_id_list_t id_list) {
  if (!mcp_handle || !is_installed) {
    return false;
  }

  // build filter
  unsigned long mask = ~0, filter = ~0;
  for (size_t i = 0; i < id_list.size; i++) {
    mask = mask & (unsigned long)id_list.list[i];
    filter = filter & (unsigned long)id_list.list[i];
  }

  if (filter == id_filter && mask == id_mask) {
    // filter haven't changed since last time
    return true;
  }

  id_mask = mask, id_filter = filter;
  bool is_all_good = true;
  for (uint8_t i = 0; i < 6; i++) { // set all 6 filters
    if (mcp_handle->init_Filt(i, false, id_filter) != MCP2515_OK || mcp_handle->init_Mask(i, false, id_mask) != MCP2515_OK) {
      is_all_good = false;
    }
  }

  return is_all_good;
}

bool Mcp2515Driver::read(saab_frame_t *frame) {
  if (!mcp_handle || !is_installed) {
    return false;
  }

  bool result = false;
  if (mcp_handle->checkReceive()) {
    unsigned long id;

    frame->extd = mcp_handle->isExtendedFrame();
    frame->rtr = mcp_handle->isRemoteRequest();
    frame->ss = 0;
    frame->self = 0;
    frame->dlc_non_comp = 0;
    frame->reserved = 0;

    result = mcp_handle->readMsgBufID(&id, &frame->length, frame->data) == CAN_OK;
    frame->id = id;
  }

  return result;
}

bool Mcp2515Driver::write(saab_frame_t frame) {
  return (mcp_handle && is_installed)
    ? mcp_handle->sendMsgBuf(frame.id, frame.extd, frame.length, frame.data) == CAN_OK
    : false;
}
