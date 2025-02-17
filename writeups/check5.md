Checkpoint 5 Writeup
====================

My name: [Devin Ma]

My SUNet ID: [devinma]

I collaborated with: [None]

I would like to thank/reward these classmates for their help: [None]

This checkpoint took me about [5] hours to do. I [did not] attend the lab session.

Program Structure and Design of the NetworkInterface [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]:
[I used std::unordered_map<uint32_t, std::pair<EthernetAddress, size_t>> arp_cache_ for keeping track of address mappings, where the uint32_t is the IP address, and the size_t is the timer. Also, I used
std::unordered_map<uint32_t, std::pair<std::vector<InternetDatagram>, size_t>> pending_arp_requests_ for storing Internet datagrams belonging to the IP address that does not have a mapping to Ethernet address yet. The reason I used these two data structures was that the information (including pending datagrams, address mapping) corresponding to an IP address can be looked up easily if the IP address itself is the lookup key. I considered using vectors of pairs instead of unordered_map, however, vectors are not efficient in this case. With vectors, we could either store pair<EthernetAddress, size_t> or pair<std::vector<InternetDatagram>, size_t> at the vector index corresponding to the integer representation of the IP address, which wastes a lot of space because of the sparsity of the IP addresses, or add the IP address into the pairs, which requires traversing through the vector every time we want to access information related to an IP address so it would be slower than unordered_map on average. Furthermore, map was also considered to replace unordered_map, but the main advantage of map over unordered_map is that a map automatically stores its elements in order based on keys, however, in this case we do not need the elements to be in order, so there was no point in wasting the time complexity of ordering elements.
]

Implementation Challenges:
[This checkpoint was relatively easy for me. The biggest challenge I encountered was trying to figure out how to parse and serialize payloads. At first glance, I thought I needed to call the parse and serialize functions defined in the classes, like InternetDatagram. However, as vscode kept prompting errors for my code, I read through the provided code base thoroughly by tracing the functions in the code base. As I traced them down and also saw the templates for parse() and serialize() in helpers, I understood how they were used, which turned out to be very straight forward.]

Remaining Bugs:
[None that I found.]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
