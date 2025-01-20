Checkpoint 1 Writeup
====================

My name: Devin Ma

My SUNet ID: devinma

I collaborated with: Nikhil Aditya Krishnan

I would like to thank/reward these classmates for their help: Nikhil Aditya Krishnan

This lab took me about [8] hours to do. I [did not] attend the lab session.

I was surprised by or edified to learn that: [I needed to put in the exact destination address in the sendto parameter in the hands on exercise for my partner to receive the datagram.]

Report from the hands-on component of the lab checkpoint: [include
information from 2.1(4), and report on your experience in 2.2]

2.1
1223/1230 packets, 0% loss, min/avg/ewma/max = 80.639/92.988/90.849/184.567 ms
    a. The average round trip time was 92.988 ms.
    b. The delivery rate was 99.43%, so the loss rate 0.57%.
    c. I did not see any duplicated datagrams.
    e. My partner and I could find all the fields of the Internet datagram discussed in class.
    f. The checksome was different for the same datagram that was captured on my end and the one captured at my partner's end.

2.2
For this exercise, my partner and I had to put in the IP address in the sendto command (RawSocket {}.sendto( Address { "10.144.0.42" }, datagram );) in order 
for the other person to receive the datagram, this was different from what was shown in class. My partner and I were both able to receive the user datagram sent using the Rawsocket without using sudo and the IP datagram with sudo. We also experimented with sending the datagram with missing fields, and these datagrams could not be received on the other end.


Describe Reassembler structure and design. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

(a)
The data structure in my implementation is a map<uint64_t, pair<uint64_t, string>>. When the insert function is called, the overlapping part of the data will be trimmed into non-overlapping segments with the data stored, then the non-overlapping segments will be stored. The key of the map is the first sequence number
of the non-overlapping segment. The first element of the pair is the ending sequence number of the non-overlapping segment, and the second element of a pair is just the non-overlapping segment itself. I think the ending index of the segment was not technically necessary because we can always get the index by adding the size of the segment to the starting sequence number. However, storing this ending sequence number was used in determining overlapping data in the incoming data, so storing this ending sequence number saves the complexity of recalculation of it. The benefit of using a map for this problem is that it will order its elements whenever new elements come in based on the key, which saves the effort of ordering data segments during implementation.

(b)
I have experimented with other designs, one of them was using a single vector with a length of capacity_ to store every character waiting to be pushed. This approach was easier to write for the portion of checking overlapping data. The appraoch for checking overlapping data I took with this data structure was that I would first trim the incoming data according to the available capacity, then I would loop through the trimmed data and see if a byte was already stored by checking its corresponding slot in the vector was already occupied. This was not a very good design since data segments can be very long, and looping through a large number of very long segments was very inefficient. If the length of a segment is M and the total number of segments we get is n, then this design has a time complexity O(mn). It would be way more efficient if we only compare the starting and ending sequence numbers of the stored segments with that of the incoming segment, which has a time complexity of O(n).

(c)
The part of code that I found hardest to implement was trimming the overlapping data since there are many cases of how data could be overlapping with the stored data. There were many bugs in this part of code at the beginning, but after drawing out all the cases we could have overlap, the details on determining the overlapping parts became more clear. I debugged the code by setting break points and checking the starting and ending indexes of the overlapping segments during testing.

(d)
For now, I am not seeing any bugs, but I will come up with additional test cases to test for bugs.


- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]



