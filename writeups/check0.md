Checkpoint 0 Writeup
====================

My name: Devin Ma

My SUNet ID: devinma

I collaborated with: None

I would like to credit/thank these classmates for their help: None

This lab took me about [4] hours to do. I [did not] attend the lab session.

My secret code from section 2.1 was: 311002

I was surprised by or edified to learn that: I could ssh into "sunetid@cardinal.stanford.edu" to send emails

Describe ByteStream implementation. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

I have a send buffer (a string variable) that stores the buffered data. The available capacity is bascially the total capacity - the current size of the buffer. The reader reads from the buffer, this is equivalant to poping data from the buffer. Currently my implementation assumes no error happens in the stream (the "error_" state is not used at all). A better solution may include error handling at the reader end, and maybe some mechanism to notify the sender of any bytes of data received in error so that sender can re-send that part of the data.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I contributed a new test case that catches a plausible bug
  not otherwise caught: [provide Pull Request URL]