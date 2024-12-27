The codes launch the SYN flooding attack to a simple echo server at port 2000 on a virtual machine 
(Lubuntu-64, with IP address 192.168.56.101).

SYN flood is a form of denial-of-service attack on data communications in which an attacker rapidly 
initiates a connection to a server without finalizing the connection. The server has to spend resources 
waiting for half-opened connections, which can consume enough resources to make the system unresponsive 
to legitimate traffic. The packet that the attacker sends is the SYN packet, a part of TCP's three-way 
handshake used to establish a connection.

SYN_flood.c launchs the attack in a single thread with raw socktet by generating a large number of 
random fake IP addresses.
SYN_flood_threads.c launches the attack through multiple threads. 
(The default is 20 threads, which can be changed by modifying the value of the macro NTHREADS.)

Since this code sends packets very frequently, it may cause the Wireshark or other packet capture 
software to crash. If you need to debug, it is recommended to uncomment the sleep(INTERVAL) line.
Then, by default, each thread will wait 1 second between sending two packets. This interval can be 
set by adjusting the value of the macro INTERVAL.