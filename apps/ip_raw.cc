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
  datagram += char( 5 );
  datagram += string( 6, 0 );
  datagram += char(); //destination address
  datagram += "Lab1 hands-on exercises: HI";

  RawSocket {}.sendto( Address {"1"}, datagram);


  return 0;
}
