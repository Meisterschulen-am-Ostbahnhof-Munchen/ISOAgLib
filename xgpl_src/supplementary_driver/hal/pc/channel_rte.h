#ifndef CHANNEL_RTE_H
#define CHANNEL_RTE_H

namespace __HAL {

// true, if indexing of digital output and input channels is
// continuous; false, if indexing is disrupted:
bool const is_digital_io_continuous = false; //true;

// The channel index and directios (input/output) refer to the control
// rather than the control's periphery! When implementing a control's
// periphery, that configuration shall also be used, but with inverse
// direction (periphery's input == control's output; periphery's
// output == control's input; index keeps being the same).
struct channel_dimensions_s {
  enum index_e {
    // Increasing this value results in increasing every channel index
    // by the same amount:
    base = 48, //0, // e.g. set to 48 for virtual/loopback mode
    // begin of output channel range (including):
    output_begin = base,
    // end of output channel range (excluding):
    output_end = base + 24,
    // begin of input channel range (including):
    input_begin = is_digital_io_continuous ? output_end : base,
    // end of input channel range (excluding):
    input_end = input_begin + 24
  };
  enum count_e {
    // number of output channels:
    n_output = output_end - output_begin,
    // number of input channels:
    n_input = input_end - input_begin
  };
};

#ifdef RTE_CLIENT
template <typename Range>
bool get_rte_channel(
  rtd_channel_t       &r_channel_number,
  uint8_t             logical_number)
{
  bool is_ok = Range::begin + logical_number < Range::end;
  if (is_ok)
    r_channel_number = Range::begin + logical_number;
//  if (1 == Range::range_id) //DEBUG
//    cout << "get_rte_channel" << " " << (is_ok ? unsigned(r_channel_number) : 0xFFFFu) << " from " << Range::begin << ", " << unsigned(logical_number) << endl;
  return is_ok;
}

template <typename Range>
bool get_logical_channel(
    uint8_t             &r_channel_logical_number,
    rtd_channel_t       rte_number)
{
  bool is_ok = Range::begin <= rte_number && rte_number < Range::end;
  if (is_ok)
    r_channel_logical_number = rte_number - Range::begin;
//  if (1 == Range::range_id) //DEBUG
//    cout << "get_logical_channel" << " " << (is_ok ? unsigned(r_channel_logical_number) : 0xFFFFu) << " from " << Range::begin << ", " << rte_number << endl;
  return is_ok;
}

template <typename Range>
void report_bit_state(
  uint8_t logical_channel_number,
  bool bit_state);

template <typename Range>
int dinout_state(
  rtd_handler_para_t *para,
  uint16_t state,
  uint16_t mask)
{
  rtd_channel_t rte_channel = para->rtd_msg->channel;

  for (int n_bits = 16; n_bits--; ) {
    if (mask & 1) {
      // a bit state is being reported
      uint8_t channel_logical_number;
      bool got_channel = get_logical_channel<Range>(channel_logical_number, rte_channel);
      if (got_channel) {
        report_bit_state<Range>(channel_logical_number, 0 != (state & 1));
      }
    }
    mask >>= 1;
    state >>= 1;
    rte_channel++;
  }
  return 0;
}
#endif //def RTE_CLIENT

} //namespace __HAL

#endif //ndef CHANNEL_RTE_H
