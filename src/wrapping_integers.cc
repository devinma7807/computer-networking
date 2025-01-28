#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  uint32_t wrapped = zero_point.raw_value_ + static_cast<uint32_t>( n );
  return Wrap32( wrapped );
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  constexpr uint64_t MAX_UINT32 = 1ULL << 32;
  uint64_t diff = static_cast<uint64_t>( raw_value_ - zero_point.raw_value_ );
  uint64_t mult = static_cast<uint64_t>( checkpoint / MAX_UINT32 );

  uint64_t closest = ( mult == 0 ) ? diff : ( mult - 1 ) * MAX_UINT32 + diff;
  uint64_t current_diff = ( checkpoint >= closest ) ? checkpoint - closest : closest - checkpoint;
  for ( uint64_t x = mult; x <= mult + 1; x++ ) {
    uint64_t new_closest = x * MAX_UINT32 + diff;
    uint64_t new_diff = ( checkpoint >= new_closest ) ? checkpoint - new_closest : new_closest - checkpoint;
    if ( new_diff < current_diff ) {
      closest = new_closest;
      current_diff = new_diff;
    }
  }
  return closest;
}
