#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity )
{
  buffer_.reserve( capacity_ * 8 );
}

void Writer::push( string data )
{
  // (void)data; // Your code here.
  if ( start_index_ > buffer_.size() / 2 ) {
    buffer_.erase( buffer_.begin(), buffer_.begin() + start_index_ );
    end_index -= start_index_;
    start_index_ = 0;
  }
  uint64_t data_size = static_cast<uint64_t>( data.size() );
  uint64_t num_bytes_to_push = min( available_capacity(), data_size );
  buffer_.insert( buffer_.end(), data.begin(), data.begin() + num_bytes_to_push );
  num_bytes_pushed_ += num_bytes_to_push;
  end_index += num_bytes_to_push;
  buffer_size += num_bytes_to_push;
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

bool Writer::is_closed() const
{
  return closed_; // Your code here.
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - buffer_size; // Your code here.
}

uint64_t Writer::bytes_pushed() const
{
  return num_bytes_pushed_; // Your code here.
}

string_view Reader::peek() const
{
  if ( buffer_.empty() ) {
    return string_view {};
  }
  return string_view( buffer_.data() + start_index_, buffer_size );
}

void Reader::pop( uint64_t len )
{
  // (void)len; // Your code here.
  uint64_t num_bytes_to_pop = min( len, bytes_buffered() );
  start_index_ += num_bytes_to_pop;
  buffer_size -= num_bytes_to_pop;
  num_bytes_poped_ += num_bytes_to_pop;
}

bool Reader::is_finished() const
{
  return closed_ && bytes_buffered() == 0; // Your code here.
}

uint64_t Reader::bytes_buffered() const
{
  return static_cast<uint64_t>( buffer_size ); // Your code here.
}

uint64_t Reader::bytes_popped() const
{
  return num_bytes_poped_; // Your code here.
}
