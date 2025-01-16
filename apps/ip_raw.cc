#include "socket.hh"

using namespace std;

class RawSocket : public DatagramSocket
{
public:
  RawSocket() : DatagramSocket( AF_INET, SOCK_RAW, IPPROTO_RAW ) {}
};

int main()
{
  // construct an Internet or user datagram here, and send using the RawSocket as in the Jan. 10 lecture
  string datagram;
  datagram += char( 0b0100'0101 );
  datagram += string( 7, 0 );
  datagram += char( 64 );
  datagram += char( 17 );
  datagram += string( 6, 0 );
  datagram += char( 10 );
  datagram += char( 144 );
  datagram += char( 0 );
  datagram += char( 42 ); // destination address

  datagram += char( 0 );
  datagram += char( 1 );
  datagram += char( 4 );
  datagram += char( 0 );

  string user_payload = "hi";
  datagram += char( 0 );
  datagram += ( user_payload.length() + 8 );
  datagram += string( 2, 0 );
  // datagram += "Lab1 exercise 2.2: Hi";
  datagram += user_payload;

  RawSocket {}.sendto( Address { "10.144.0.42" }, datagram );
  return 0;
}
