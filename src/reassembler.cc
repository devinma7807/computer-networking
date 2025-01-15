#include "reassembler.hh"
#include "debug.hh"
#include <iostream>
#include <vector>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{ 
  // cout << endl;
  uint64_t next_expected = output_.writer().bytes_pushed();
  uint64_t available_capacity = output_.writer().available_capacity();

  if (!cache_initialized){
    vector<char> new_cache(available_capacity);
    fill(new_cache.begin(), new_cache.end(), char());
    vector<bool> new_cache_occupied(available_capacity, false);
    cache_occupied_ = new_cache_occupied;
    cache_ = new_cache;
    cache_size = available_capacity;
    cache_initialized = true;
  }

  uint64_t cache_next_expected = next_expected % cache_size;
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
 
  // for (int x = 0; x < end_index_data; x++){
  // // if (cache_.find(x + write_index) == cache_.end()){
  //   cache_[static_cast<uint64_t>(x + write_index)] = trimmed_data[x];
  // }
  uint64_t cache_write_index = write_index % cache_size;
  for (int x = 0; x < end_index_data; x++){
    if (!cache_occupied_[cache_write_index]){
      cache_[cache_write_index] = trimmed_data[x];
      cache_occupied_[cache_write_index] = true;
      bytes_pending += 1;
      // cout << "store " << trimmed_data[x] << " to index " << cache_write_index << endl;
    }
    cache_write_index += 1;
    if (cache_write_index >= cache_size){
      cache_write_index = 0;
    }
  }
  

  string data_to_push = "";
  // vector<uint64_t> pushed_keys{};
  // for (auto it = cache_.begin(); it != cache_.end(); it++) {
  //   if (it->first == next_expected){
  //     data_to_push += it->second;
  //     next_expected += 1;
  //     pushed_keys.push_back(it->first);
  //   }
  //   else{
  //     break;
  //   }
  // }

  // for (size_t x = 0; x < pushed_keys.size(); x++){
  //   cache_.erase(pushed_keys[x]);
  // }

  while (cache_occupied_[cache_next_expected]){
    data_to_push += cache_[cache_next_expected];
    // cout << "push " << cache_[cache_next_expected] << " at index " << cache_next_expected << endl;
    cache_[cache_next_expected] = char();
    cache_occupied_[cache_next_expected] = false;
    bytes_pending -= 1;
    cache_next_expected += 1;
    if (cache_next_expected >= cache_size){
      cache_next_expected = 0;
    }
  }


  if (!data_to_push.empty()){output_.writer().push(data_to_push);}

  if (finish && output_.writer().bytes_pushed() == finish_index){output_.writer().close();}

}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  // debug( "unimplemented count_bytes_pending() called" );
  return bytes_pending;
}