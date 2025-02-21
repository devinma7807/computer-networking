#include <iostream>

#include "arp_message.hh"
#include "debug.hh"
#include "ethernet_frame.hh"
#include "exception.hh"
#include "helpers.hh"
#include "network_interface.hh"

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but
//! may also be another host if directly connected to the same network as the destination) Note: the Address type
//! can be converted to a uint32_t (raw 32-bit IP address) by using the Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
  uint32_t next_hop_ip = next_hop.ipv4_numeric();

  if ( arp_cache_.find( next_hop_ip ) != arp_cache_.end() ) {
    EthernetFrame frame;
    frame.header.type = EthernetHeader::TYPE_IPv4;
    frame.header.src = ethernet_address_;
    frame.header.dst = arp_cache_[next_hop_ip].first;
    frame.payload = serialize( dgram );
    transmit( frame );
    return;
  }

  if ( pending_arp_requests_.find( next_hop_ip ) == pending_arp_requests_.end()
       || pending_arp_requests_[next_hop_ip].second > 5000 ) {
    ARPMessage arp_request;
    arp_request.opcode = ARPMessage::OPCODE_REQUEST;
    arp_request.sender_ethernet_address = ethernet_address_;
    arp_request.sender_ip_address = ip_address_.ipv4_numeric();
    arp_request.target_ip_address = next_hop_ip;

    EthernetFrame arp_frame;
    arp_frame.header.type = EthernetHeader::TYPE_ARP;
    arp_frame.header.src = ethernet_address_;
    arp_frame.header.dst = ETHERNET_BROADCAST;
    arp_frame.payload = serialize( arp_request );
    transmit( arp_frame );
  }
  pending_arp_requests_[next_hop_ip].first.push_back( dgram );
}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
  if ( frame.header.dst != ethernet_address_ && frame.header.dst != ETHERNET_BROADCAST ) {
    return;
  }

  if ( frame.header.type == EthernetHeader::TYPE_IPv4 ) {
    InternetDatagram datagram;
    if ( parse( datagram, frame.payload ) ) {
      datagrams_received_.push( datagram );
    }
  }

  if ( frame.header.type == EthernetHeader::TYPE_ARP ) {
    ARPMessage arp;
    if ( parse( arp, frame.payload ) ) {
      arp_cache_[arp.sender_ip_address] = make_pair( arp.sender_ethernet_address, 30000 );
      if ( arp.opcode == ARPMessage::OPCODE_REQUEST && arp.target_ip_address == ip_address_.ipv4_numeric() ) {
        ARPMessage arp_response;
        arp_response.opcode = ARPMessage::OPCODE_REPLY;
        arp_response.sender_ethernet_address = ethernet_address_;
        arp_response.sender_ip_address = ip_address_.ipv4_numeric();
        arp_response.target_ip_address = arp.sender_ip_address;
        arp_response.target_ethernet_address = arp.sender_ethernet_address;

        EthernetFrame response_frame;
        response_frame.header.dst = arp.sender_ethernet_address;
        response_frame.header.src = ethernet_address_;
        response_frame.header.type = EthernetHeader::TYPE_ARP;
        response_frame.payload = serialize( arp_response );
        transmit( response_frame );
      } else if ( arp.opcode == ARPMessage::OPCODE_REPLY && arp.target_ip_address == ip_address_.ipv4_numeric() ) {
        if ( pending_arp_requests_.find( arp.sender_ip_address ) != pending_arp_requests_.end() ) {
          for ( auto it = pending_arp_requests_[arp.sender_ip_address].first.begin();
                it != pending_arp_requests_[arp.sender_ip_address].first.end();
                it++ ) {
            EthernetFrame eframe;
            eframe.header.dst = arp.sender_ethernet_address;
            eframe.header.src = ethernet_address_;
            eframe.header.type = EthernetHeader::TYPE_IPv4;
            eframe.payload = serialize( *it );
            transmit( eframe );
          }
          pending_arp_requests_.erase( arp.sender_ip_address );
        }
      }
    }
  }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  auto arp_it = arp_cache_.begin();
  while ( arp_it != arp_cache_.end() ) {
    if ( arp_it->second.second <= ms_since_last_tick ) {
      arp_it = arp_cache_.erase( arp_it );
    } else {
      arp_it->second.second -= ms_since_last_tick;
      arp_it++;
    }
  }

  auto pending_arp_it = pending_arp_requests_.begin();
  while ( pending_arp_it != pending_arp_requests_.end() ) {
    pending_arp_it->second.second += ms_since_last_tick;

    if ( pending_arp_it->second.second > 5000 ) {
      pending_arp_it = pending_arp_requests_.erase( pending_arp_it );
    } else {
      pending_arp_it++;
    }
  }
}
