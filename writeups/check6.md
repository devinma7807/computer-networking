Checkpoint 6 Writeup
====================

My name: [Devin Ma]

My SUNet ID: [devinma]

I collaborated with: [None]

I would like to thank/reward these classmates for their help: [None]

This checkpoint took me about [4] hours to do. I [did not] attend the lab session.

Program Structure and Design of the Router [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: [The data structure I chose is map<uint8_t, std::unordered_map<uint32_t, route_to_next_>> where the keys of the map are prefix_lengths, keys of the unordered_map are the route_prefixes, and route_to_next is a structure containing the next_hop and interface_num. I chose this structure because of the requirement that we need to match the datagram dst IP address to the longest prefix length, since a map orders its elements by keys, finding the route with the longest prefix length match can be done easily by traversing the map in the reverse order and finding the first match in prefix_route. Furthermore, an unordered map is used for every prefix_length since the key look up for an unordered_map is relatively fast. Another data structure I considered was using a vector to store the structs containing all prefix_route, prefix_length, next_hop, and interface_num. However, with this data structure, we basically need to traverse the whole vector to find the match with the longest prefix length every time while we only need to tranverse a small subset of the map, so the current data structure is faster than using a vector of structs.]

Implementation Challenges:
[The part I spent most of the time debugging was the part that when the added route has a prefix length of 0. I did not realize there could be the case where the prefix_route being 0 with a prefix_length of 0 means that it matches every IP addresses. I spent some time looking at the test cases and found out about this with some debug outputs and fixed the problem.]

Remaining Bugs:
[None that I found.]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
