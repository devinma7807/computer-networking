Checkpoint 7 Writeup
====================

My name: [Devin Ma]

My SUNet ID: [devinma]

I collaborated with: [Thanawan Atchariyachanvanit (thanawan)]

I would like to thank/reward these classmates for their help: [Thanawan Atchariyachanvanit]

This checkpoint took me about [2] hours to do. I [did not] attend the lab session.

Solo portion:

- Did your implementation successfully start and end a conversation with another copy of itself? [y]

- Did it successfully transfer a one-megabyte file, with contents identical upon receipt? [y]

- Please describe what code changes, if any, were necessary to pass these steps:
At first try, no code changes was required to do the solo portion and the checksums matched, but when I was trying to double check on my implementations by running the tests, I found my implementation of the network interface failing the addtional tests that got added later because my implementation did not learn from the ARP requests not destined to my IP address and it did not send the pending datagrams right away after learning from ARP requests in the receive function. So I added that part and everything works.

Group portion:

- Who is your lab partner (and what is their SUNet ID, e.g. "winstein")? Thanawan Atchariyachanvanit (thanawan)

- Did your implementations successfully start and end a conversation with each other (with each implementation acting as ``client'' or as ``server'')? Yes, my implementation succeeded at being both the server and the client, the checksums for match for when I was sending and receiving the datagrams.

- Did you successfully transfer a one-megabyte file between your two
  implementations, with contents identical upon receipt? Yes, our checksums matched.

- Please describe what code changes, if any, were necessary to pass
  these steps, either by you or your lab partner. 
  No code changes was necessary from my end after the changes mentioned above.

Creative portion (if you did anything for our creative challenge,
                  please boast about it!)

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
