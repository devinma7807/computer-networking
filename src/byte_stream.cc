#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  (void)data; // Your code here.
  uint64_t data_size = static_cast<uint64_t>(data.size());
  uint64_t num_bytes_to_push = min(available_capacity(), data_size);
  buffer_.append(data.data(), num_bytes_to_push);
  num_bytes_pushed_ += num_bytes_to_push;

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
  return capacity_ - buffer_.size(); // Your code here.
}

uint64_t Writer::bytes_pushed() const
{
  return num_bytes_pushed_; // Your code here.
}

string_view Reader::peek() const
{
  return string_view(buffer_.data(), buffer_.size()); // Your code here.
}

void Reader::pop( uint64_t len )
{
  (void)len; // Your code here.
  uint64_t num_bytes_to_pop = min(len, bytes_buffered());
  buffer_.erase(0, num_bytes_to_pop);
  num_bytes_poped_ += num_bytes_to_pop;

}

bool Reader::is_finished() const
{
  return closed_ && bytes_buffered() == 0; // Your code here.
}

uint64_t Reader::bytes_buffered() const
{
  return static_cast<uint64_t>(buffer_.size()); // Your code here.
}

uint64_t Reader::bytes_popped() const
{
  return num_bytes_poped_; // Your code here.
}

