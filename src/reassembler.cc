#include "reassembler.hh"
#include "debug.hh"
#include <chrono>
#include <vector>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  uint64_t next_expected = output_.writer().bytes_pushed();
  uint64_t available_capacity = output_.writer().available_capacity();
  uint64_t last_available_index = uint64_t( max( int( next_expected + available_capacity - 1 ), 0 ) );
  uint64_t data_size = data.size();
  uint64_t last_data_index = uint64_t( max( int( first_index + data_size - 1 ), 0 ) );
  uint64_t write_index = max( next_expected, first_index );
  if ( is_last_substring && last_data_index <= last_available_index ) {
    finish = true;
  }

  last_data_index = min( last_available_index, last_data_index );

  if ( first_index > next_expected + available_capacity - 1 || data_size + first_index < next_expected ) {
    return;
  }

  string_view trimmed_data( data.data() + write_index - first_index,
                            min( last_data_index - write_index + 1, data.size() ) );

  if ( cache_.empty() && last_data_index >= write_index ) {
    cache_[write_index] = make_pair( last_data_index, string( trimmed_data ) );
  } else {
    for ( auto it = cache_.begin(); it != cache_.end(); ++it ) {

      uint64_t current_segment_start_index = it->first;
      if ( write_index < current_segment_start_index ) {
        if ( last_data_index < current_segment_start_index ) {
          cache_[write_index] = make_pair( last_data_index, string( trimmed_data ) );
          break;
        } else {
          uint64_t trim_size = current_segment_start_index - write_index;
          cache_[write_index]
            = make_pair( current_segment_start_index - 1, string( trimmed_data.substr( 0, trim_size ) ) );
          uint64_t new_segment_start = max( write_index + trim_size, it->second.first + 1 );

          if ( last_data_index < new_segment_start ) {
            break;
          } else {
            trimmed_data
              = trimmed_data.substr( new_segment_start - write_index, last_data_index - new_segment_start + 1 );
            write_index = new_segment_start;
          }
        }
      } else {
        uint64_t current_segment_end_index = it->second.first;
        if ( last_data_index <= current_segment_end_index ) {
          break;
        } else {
          uint64_t new_segment_start = max( current_segment_end_index + 1, write_index );
          trimmed_data = trimmed_data.substr( new_segment_start - write_index,
                                              last_data_index - current_segment_start_index + 1 );
          write_index = new_segment_start;
        }
      }
      if ( write_index > last_data_index ) {
        break;
      }
    }
  }

  if ( !cache_.empty() && last_data_index > cache_.rbegin()->second.first && !trimmed_data.empty() ) {
    uint64_t last_segment_start_index = max( write_index, ( cache_.rbegin()->second.first ) + 1 );
    trimmed_data = trimmed_data.substr( last_segment_start_index - write_index,
                                        last_data_index - last_segment_start_index + 1 );

    cache_[last_segment_start_index] = make_pair( last_data_index, string( trimmed_data ) );
  }

  string data_to_push = "";
  while ( !cache_.empty() && cache_.begin()->first == next_expected ) {
    uint64_t current_segment_size = cache_.begin()->second.first - cache_.begin()->first + 1;
    data_to_push += cache_.begin()->second.second;
    next_expected += current_segment_size;
    cache_.erase( cache_.begin()->first );
  }

  if ( !data_to_push.empty() ) {
    output_.writer().push( data_to_push );
  }

  if ( finish && cache_.empty() ) {
    output_.writer().close();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  uint64_t total_bytes_pending = 0;
  for ( auto x = cache_.begin(); x != cache_.end(); x++ ) {
    total_bytes_pending += x->second.second.size();
  }
  return total_bytes_pending;
}