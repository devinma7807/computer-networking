#include "reassembler.hh"
#include "debug.hh"
#include <iostream>
#include <vector>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{ 
  uint64_t next_expected = output_.writer().bytes_pushed();
  uint64_t available_capacity = output_.writer().available_capacity();
  uint64_t last_available_index = next_expected + available_capacity;
  uint64_t data_size = data.size();
  uint64_t last_data_index = first_index + data_size;
  uint64_t write_index = max(next_expected, first_index);
  
  if (first_index > next_expected + available_capacity - 1 || data_size + first_index < next_expected){return;}

  int start_index_data = 0;
  int end_index_data = 0;

  if (first_index < next_expected){
    start_index_data = next_expected - first_index;
    if (last_data_index > last_available_index){
      end_index_data = last_available_index - next_expected;
    }
    else{
      end_index_data = last_data_index - next_expected;
    }
  }
  else{
    if (last_data_index > last_available_index){
      end_index_data = last_available_index - first_index;
    }
    else{
      end_index_data = data_size;
    }
  }

  string trimmed_data = data.substr(start_index_data, end_index_data);
  if (is_last_substring && last_data_index <= last_available_index){
    finish = true;
    finish_index = last_data_index;
  }

  for (int x = 0; x < end_index_data; x++){
    // if (cache_.find(x + write_index) == cache_.end()){
    cache_[static_cast<uint64_t>(x + write_index)] = trimmed_data[x];
  }

  string data_to_push = "";
  vector<uint64_t> pushed_keys{};
  for (auto it = cache_.begin(); it != cache_.end(); it++) {
    if (it->first == next_expected){
      data_to_push += it->second;
      next_expected += 1;
      pushed_keys.push_back(it->first);
    }
    else{
      break;
    }
  }

  for (size_t x = 0; x < pushed_keys.size(); x++){
    cache_.erase(pushed_keys[x]);
  }


  // if (!cache_.empty() && next_expected == cache_.rbegin()->first){cache_.clear();
  if (!data_to_push.empty()){output_.writer().push(data_to_push);}

  if (finish && next_expected == finish_index){output_.writer().close();}

}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  // debug( "unimplemented count_bytes_pending() called" );
  return cache_.size();
}