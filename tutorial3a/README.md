In terminal:
Type "make" to generate all three executable files: client, server, and test_continuous_send.
Type “make clean” to remove all the generated files.
Type “make client” to only generate “client” and “make clean-client” to remove it.
Type “make server” to only generate “ server” and “make clean-server” to remove it.
Type “make test” to only generate “test_continuous_send” and “make clean-test” to remove it.

In client:
Type "./fancyclient IP_ADDRESS PORT_NUMBER"  then type multiple lines of input message to send it.
Then, it will output the received messages.

In server:
Type "./server PORT_NUMBER" then the server will receive the message, print it (with the thread number), and send back the same message.

In test_continuous_send:
This file is to test the multithreaded server. The function is similar to a single-thread client, but it will continuously send the same input message.
You can open multiple terminals and run the program with different inputs to test the multithreaded server.

In test_continuous_resppond:
This file is to test the multithreaded client. The function is similar to the multithreaded server, but it will continuously respond the same message.

The folder "ex06-mutex" contains a small exercise before tutorial 3a.