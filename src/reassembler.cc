#include "reassembler.hh"
#include "debug.hh"
#include <iostream>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{ 
  uint64_t num_bytes_pushed = output_.writer().bytes_pushed();
  uint64_t bytestream_available_capacity = output_.writer().available_capacity();
  uint64_t data_size = static_cast<uint64_t>(data.size());
  uint64_t max_push_index = num_bytes_pushed + bytestream_available_capacity;
  uint64_t segment_last_index = first_index + data_size;
  if (is_last_substring){last_index = segment_last_index;}
  
  if (first_index > max_push_index){return;}
  if (segment_last_index < num_bytes_pushed) {return;}
  
  uint64_t write_start_index = first_index - num_bytes_pushed;
  uint64_t write_end_index = min(bytestream_available_capacity, data_size);

  for (uint64_t ind = write_start_index; ind < write_end_index; ind ++){
    cache_[ind + write_start_index + num_bytes_pushed] = data[ind];
  }

  while (!cache_.empty() && cache_.begin()->first == num_bytes_pushed){
    output_.writer().push(cache_.begin()->second);
    // bytestream_available_capacity -= 1;
    num_bytes_pushed += 1;
    cache_.erase(cache_.begin());
  }

  if (is_last_substring && cache_.empty()){output_.writer().close();}


  
  // debug( "unimplemented insert({}, {}, {}) called", first_index, data, is_last_substring );
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  // debug( "unimplemented count_bytes_pending() called" );
  return cache_.size();
}
