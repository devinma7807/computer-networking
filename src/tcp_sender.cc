#include "tcp_sender.hh"
#include "debug.hh"
#include "tcp_config.hh"

using namespace std;

// This function is for testing only; don't add extra state to support it.
uint64_t TCPSender::sequence_numbers_in_flight() const
{
  uint64_t num_pending_seqno = 0;
  for ( auto x = send_buffer_.begin(); x != send_buffer_.end(); x++ ) {
    num_pending_seqno += x->second.sequence_length();
  }
  return num_pending_seqno;
}

// This function is for testing only; don't add extra state to support it.
uint64_t TCPSender::consecutive_retransmissions() const
{
  return num_consecutive_retran_;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  TCPSenderMessage message = make_empty_message();
  string_view stream_bytes = reader().peek();
  uint16_t temp_receive_window = receive_window_;
  if ( receiver_full_ && receive_window_ > 0 ) {
    temp_receive_window = 1;
  }
  while ( ( temp_receive_window > 0 && !stream_bytes.empty() )
          || ( temp_receive_window > 0 && writer().is_closed() && !fin_set_ ) || !syn_set_ ) {
    if ( !syn_set_ ) {
      syn_set_ = true;
      message.SYN = true;
      current_RTO_ = initial_RTO_ms_;
      seqno_ = isn_;
    }
    uint64_t available_payload_size = min( { static_cast<uint64_t>( temp_receive_window - message.SYN ),
                                             stream_bytes.size(),
                                             TCPConfig::MAX_PAYLOAD_SIZE } );
    if ( available_payload_size > 0 ) {
      message.payload = string( stream_bytes.substr( 0, available_payload_size ) );
      reader().pop( available_payload_size );
    }
    if ( temp_receive_window > message.sequence_length() && writer().is_closed() && !fin_set_
         && reader().is_finished() ) {
      message.FIN = true;
      fin_set_ = true;
    }
    uint64_t message_length = message.sequence_length();
    if ( message_length > 0 ) {
      transmit( message );
      receive_window_ -= message_length;
      temp_receive_window -= message_length;
      send_buffer_.push_back( make_pair( abs_seqno_, message ) );
      seqno_ = seqno_ + message_length;
      abs_seqno_ += message_length;
      stream_bytes = reader().peek();
      message = make_empty_message();
    }
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  TCPSenderMessage empty_message {};
  empty_message.seqno = syn_set_ ? seqno_ : isn_;
  if ( writer().has_error() ) {
    empty_message.RST = true;
  }
  return empty_message;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  uint64_t abs_ackno = msg.ackno->unwrap( isn_, abs_seqno_ );

  if ( !send_buffer_.empty()
       && prev( send_buffer_.end() )->first + prev( send_buffer_.end() )->second.sequence_length() < abs_ackno )
    return;
  if ( !msg.ackno.has_value() && msg.window_size == 0 ) {
    writer().set_error();
    return;
  }

  uint64_t erase_count = 0;
  for ( auto x = send_buffer_.begin(); x != send_buffer_.end(); x++ ) {
    if ( x->first + x->second.sequence_length() <= abs_ackno ) {
      if ( x->second.SYN ) {
        receive_window_ = max( uint16_t( 1 ), msg.window_size );
        max_receive_window_ = receive_window_;
      } else {
        receive_window_ += x->second.sequence_length();
      }
      erase_count += 1;
    } else {
      break;
    }
  }
  send_buffer_.erase( send_buffer_.begin(), send_buffer_.begin() + erase_count );

  receiver_full_ = ( msg.window_size == 0 );
  if ( msg.window_size > max_receive_window_ ) {
    receive_window_ += ( msg.window_size - max_receive_window_ );
    max_receive_window_ = msg.window_size;
  }
  if ( pre_abs_ackno_ < abs_ackno ) {
    timer_ = 0;
    pre_abs_ackno_ = abs_ackno;
    num_consecutive_retran_ = 0;
    current_RTO_ = initial_RTO_ms_;
  }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  timer_ += ms_since_last_tick;
  if ( send_buffer_.empty() ) {
    timer_ = 0;
    return;
  }
  if ( timer_ >= current_RTO_ ) {
    transmit( send_buffer_.begin()->second );
    num_consecutive_retran_ += 1;
    if ( !receiver_full_ || send_buffer_.begin()->second.SYN ) {
      current_RTO_ *= 2;
    }
    timer_ = 0;
  }
}
