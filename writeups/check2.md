Checkpoint 2 Writeup
====================

My name: Devin Ma

My SUNet ID: devinma

I collaborated with: None

I would like to thank/reward these classmates for their help: None

This lab took me about [5] hours to do. I [did not] attend the lab session.

Describe Wrap32 and TCPReceiver structure and design. [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

My implementation of Wrap32 leverages simple arithmetic. The wrap() function is simply add two 32 bit unsigned integers and output their sum into a Wrap32 type variable. The unwrap() function involes comparing between three potential choices of the unwrapped numbers using a loop to get the closest number to checkpoint. 
Disclosure: I got "constexpr uint64_t MAX_UINT32 = 1ULL << 32;" from "https://www.geeksforgeeks.org/left-shift-right-shift-operators-c-cpp/". Compared to what I wanted to use, which was the pow(), 1ULL << 32 involves only bit shifting so it could run faster than using pow() to get the 64-bit representation of the max of 32-bit unsigned int.

My implementation of the TCPReceiver did not utilize any data structures (besides the one used in reassembler) since it was not really necessary here. My approach is basically keeping track of two numbers, the byte stream sequence number used as a parameter of insert() function of reassembler and the ackno sequence number used to send acknowledgement to sender(). The byte stream sequence number is also used to unwrap the message sequence number from sender.

I think a different design could be using a pair<> to store both the wrapped value and the number of times that it has wrapped around so that we can just use these values to get the unwrapped one. However, I did not think it was necessary to do this since the time complexity would be similar for both approaches and arithmetic approach is fast enough.

Implementation Challenges:
I think overall this implementation was not very hard, one small challenge I encountered that my implementation kept failing the "Stream error -> RST flag" test under recv_special. At first I thought it was the problem of the receive() function but then I found that receive was not called during this test, so then I identified the problem being in the condition for setting the RST flag in the send() function.

Remaining Bugs:
No bugs found for now.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [describe where to find]
