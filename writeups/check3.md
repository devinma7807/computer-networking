Checkpoint 3 Writeup
====================

My name: Devin Ma

My SUNet ID: devinma

I collaborated with: Thanawan Atchariyachanvanit (lab partner)

I would like to thank/reward these classmates for their help: Thanawan Atchariyachanvanit

This checkpoint took me about [12] hours to do. I [did not] attend the lab session.

Program Structure and Design of the TCPSender [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: [

Data structure and design:
For tcp sender, I used the data structure vector<pair<uint64_t abs_seqno, TCPSenderMessage message>> for the send buffer. I keep track of abs_seqno in order to unwrap the ackno received to determine the messages to be erase from the send buffer.

Alternative design choices:
Initially I used a map instead of a vector for the send buffer. In terms of the difficulty of implementation, they are pretty much the same. Map has the ability to automatically order its elements based on keys so I thought this might come in handy when we need to retransmit the message with the lowest seqno after timeout. However, I realized that the elements would not be reordered since they are pushed to the send buffer in order so naturally the first element in the send buffer will be the element with the smallest seqno. Therefore, map's ability of ordering elements does not really provide extra value in this case, and there was no need for value lookup in my implementation, so map is actually not needed. Besides, sequential operations to vector could be faster than to map, so I changed to using vector instead of map.
]


Report from the hands-on component: [
I found a bug within my tcp_sender code while doing 4.1.2. I had in my code a loop that removes the fully acknowledged
messages from the send buffer. However, I was using an iterator to traverse through the data structure and erasing elements
at the same time. This ended up with a segment fault when I was trying to connect to Linux TCP since this loop did not end.

For 4.1.2, both programs ended cleanly after some data tramission. When tcp_ipv4 initiated the finish of connection, it will
wait for a while at the end to make sure tcp_native received tcp_ipv4's acknowledgement of tcp_native's FIN and this is consistent
with what was discussed in class.

For 4.1.3, tcp_ipv4 successfully transmitted and received all numbers of bytes mentioned as both a server and as a client.

For 4.2, I was able to work with Thanawan and manage to send messages to receive messages from each other with matching checksums. The biggest
file we tried to send was 75MB and the other side successfully received with matching checksum. The following are the command outputs:

        sender:
        cs144@cs144vm:~/minnow$ ls -l /tmp/test_bs_75000000.txt 
        -rw-rw-r-- 1 cs144 cs144 75000000 Jan 30 18:31 /tmp/test_bs_75000000.txt

        cs144@cs144vm:~/minnow$ sha256sum /tmp/test_bs_75000000.txt
        8904af4442301a09c4656d2554ead4a4aeef5fb8ab656a3742b4520956464a18  /tmp/test_bs_75000000.txt

        receiver:
        cs144@cs144vm:~/minnow$ ls -l /tmp/test_bs_75000000-received-labmate.txt
        -rw-rw-r-- 1 cs144 cs144 75000000 Jan 30 18:36 /tmp/test_bs_75000000-received-labmate.txt

        cs144@cs144vm:~/minnow$ sha256sum /tmp/test_bs_75000000-received-labmate.txt
        8904af4442301a09c4656d2554ead4a4aeef5fb8ab656a3742b4520956464a18  /tmp/test_bs_75000000-received-labmate.txt

For 4.3, webget was revised and passed all tests.]

Implementation Challenges:
[The most challenging part for me is when the receiver announced zero window size but window is not actually full from the sender's perspective. I struggled a bit on the definitions of "full window" and "zero-size window". At first I thought they could be combined into one representation, however, after some failed test cases and drawing, I realized that I needed another state to combine with this window to determine if the receive window is actually full and how to update the receive window from the sender's perspective.]

Remaining Bugs:
[There are no bugs as far as I know at the moment and tcp_ipv4 successfully communicated with tcp_native.]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [describe where to find]
