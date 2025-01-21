#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  if (message.RST || writer().has_error() || reader().has_error()){
    RST_ = true;
  }

  if (!RST_){
    Wrap32 payload_first_seqno = message.seqno;
    if (message.SYN){
      initial_seqno_ = payload_first_seqno;
      payload_first_seqno = payload_first_seqno + 1;
      syn_set_ = true;
    }
    if (message.FIN && syn_set_){fin_set_ = true;}
    
    uint64_t stream_first_seqno =  (payload_first_seqno + (-1 * syn_set_)).unwrap(initial_seqno_, stream_seqno_);
    reassembler_.insert(stream_first_seqno, message.payload, message.FIN);
    
    stream_seqno_ = writer().bytes_pushed();
    if (writer().is_closed()){
      ackno_ = ackno_->wrap(stream_seqno_ + syn_set_ + fin_set_, initial_seqno_);
    }
    else{
      ackno_ = ackno_->wrap(stream_seqno_ + syn_set_, initial_seqno_);
    }
  }
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  TCPReceiverMessage message {};
  message.window_size = min(65535 - reassembler_.count_bytes_pending(), writer().available_capacity());
  message.RST = RST_;
  if (syn_set_){message.ackno = ackno_;}
  return message;
}
