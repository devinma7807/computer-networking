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
  if ( receiver_full_ && receive_window_ > 0 ) {
    if ( !syn_set_ ) {
      syn_set_ = true;
      message.SYN = true;
      current_RTO_ = initial_RTO_ms_;
    } else if ( !stream_bytes.empty() ) {
      message.payload = char( stream_bytes[0] );
    }

    if ( message.sequence_length() == 0 && writer().is_closed() ) {
      message.FIN = true;
    }
    if ( message.sequence_length() > 0 ) {
      transmit( message );
      reader().pop( message.sequence_length() );
      send_buffer_[message.seqno] = message;
      isn_ = isn_ + 1;
      receive_window_ -= 1;
    }
  } else {
    while ( ( receive_window_ > 0 && !stream_bytes.empty() )
            || ( receive_window_ > 0 && writer().is_closed() && !fin_set_ ) ) {
      if ( !syn_set_ ) {
        syn_set_ = true;
        message.SYN = true;
        current_RTO_ = initial_RTO_ms_;
      }
      uint64_t available_payload_size = min( { static_cast<uint64_t>( receive_window_ - message.SYN ),
                                               stream_bytes.size(),
                                               TCPConfig::MAX_PAYLOAD_SIZE } );
      if ( available_payload_size > 0 ) {
        message.payload = string( stream_bytes.substr( 0, available_payload_size ) );
        reader().pop( available_payload_size );
      }
      if ( receive_window_ > message.sequence_length() && writer().is_closed() && !fin_set_
           && reader().is_finished() ) {
        message.FIN = true;
        fin_set_ = true;
      }
      uint64_t message_length = message.sequence_length();
      if ( message_length > 0 ) {
        transmit( message );
        receive_window_ -= message_length;
        send_buffer_[isn_] = message;
        isn_ = isn_ + message_length;
        stream_bytes = reader().peek();
        message = make_empty_message();
      }
    }
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  TCPSenderMessage empty_message {};
  empty_message.seqno = isn_;
  if ( reader().has_error() || writer().has_error() ) {
    empty_message.RST = true;
  }
  return empty_message;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  if ( !send_buffer_.empty()
       && prev( send_buffer_.end() )->first + prev( send_buffer_.end() )->second.sequence_length() < msg.ackno )
    return;
  if ( !msg.ackno.has_value() && msg.window_size == 0 ) {
    writer().set_error();
    return;
  }
  Wrap32 ackno = msg.ackno.has_value() ? *msg.ackno : isn_;

  for ( auto x = send_buffer_.begin(); x != send_buffer_.end(); x++ ) {
    if ( x->second.seqno + x->second.sequence_length() <= ackno ) {
      if ( x->second.SYN ) {
        receive_window_ = max( uint16_t( 1 ), msg.window_size );
        max_receive_window_ = receive_window_;
      } else {
        receive_window_ += x->second.sequence_length();
      }

      send_buffer_.erase( x->first );
    }
  }

  receiver_full_ = ( msg.window_size == 0 );
  if ( msg.window_size > max_receive_window_ ) {
    receive_window_ += ( msg.window_size - max_receive_window_ );
    max_receive_window_ = msg.window_size;
  }
  if ( pre_ackno_ < ackno ) {
    timer_ = 0;
    pre_ackno_ = ackno;
    num_consecutive_retran_ = 0;
    current_RTO_ = initial_RTO_ms_;
  }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  timer_ += ms_since_last_tick;
  if ( send_buffer_.empty() ) {
    timer_ = 0;
  }
  if ( timer_ >= current_RTO_ && !send_buffer_.empty() ) {
    transmit( send_buffer_.begin()->second );
    num_consecutive_retran_ += 1;
    if ( !receiver_full_ || send_buffer_.begin()->second.SYN ) {
      current_RTO_ *= 2;
    }
    timer_ = 0;
  }
}
