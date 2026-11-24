#pragma once

#include <atomic>
#include <functional>

#include "freertos/FreeRTOS.h"

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"

#include "esphome/components/spi/spi.h"
#include "esphome/components/wmbus_common/wmbus.h"

#include "packet.h"
#include "transceiver.h"

namespace esphome {
namespace wmbus_radio {

class Radio : public Component {
public:
  void set_radio(RadioTransceiver *radio) { this->radio = radio; };

  void setup() override;
  void loop() override;
  void receive_frame();

  void add_frame_handler(std::function<void(Frame *)> &&callback);

  // Request a frequency change (safe to call from lambdas). The radio is
  // retuned by the receiver task, so SPI access is never concurrent.
  void set_frequency(uint32_t hz);
  uint32_t get_frequency();

protected:
  static void wakeup_receiver_task_from_isr(TaskHandle_t *arg);
  static void receiver_task(Radio *arg);
  bool apply_pending_frequency_();

  std::atomic<uint32_t> pending_frequency_{0};

  RadioTransceiver *radio{nullptr};
  TaskHandle_t receiver_task_handle_{nullptr};
  QueueHandle_t packet_queue_{nullptr};

  std::vector<std::function<void(Frame *)>> handlers_;
};
} // namespace wmbus_radio
} // namespace esphome
