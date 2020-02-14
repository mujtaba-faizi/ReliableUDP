# ReliableUDP

## Specifications: 
Code for sender and a receiver, that implements audio file transfer over UDP protocol using Linux sockets. Sender is required to open an audio file, read data chunks from file and write UDP segments. Finally, send these segments over UDP. Receiver must be able to receive, reorder and write data to a file at the receiving end.     
 
 
## Sender: 
The sender will read the file specified by the filename, and transfer it using UDP sockets. The sender should bind to the listenport in-order to receive acknowledgments and other signaling from the receiver. You should only use a single UDP socket for both sending and receiving data. Note that although UDP will allow you to send large packets using IP fragmentation, but make sure that you restrict your packets to 500 bytes (in payload), for efficiency. On completing the transfer, the sender should terminate and exit.
 
 
 
## Receiver: 
The receiver will need to bind to the UDP port specified on the command line, and receive a file from the sender that is sent over the port. Note that you should make sure that the filename used by the receiver is not the same as the one used by the sender, since otherwise the receiver will overwrite the file and interfere with the sender. The receiver should terminate and exit once the transfer is complete. 
 
## Implementing reliability in UDP 
Following is implemented to make UDP reliable:   
a) Sequence numbers   
b) Retransmission (selective repeat)   
c) Window size of 5-10 UDP segments   
d) Re ordering on receiver side   

## How to run

For running server(after compiling the Server.c file), type the command on bash terminal:

./Server [filename] [portnumber]

For running client(after compiling the Client.c file), type the command on bash terminal:

./Client [filename] [IPaddress_of_sever] [portnumber]

## Behaviour 
<p align="center">
1. Server waiting to receive the data bytes of the audio file “B.mp3”.   </p>
<p align="center">
  <img src="https://user-images.githubusercontent.com/30017031/34640028-1ec27e48-f30d-11e7-9509-48d6ec9a1bdb.PNG" />
</p>  
<p align="center">
  2. Client sending the audio file “Believer.mp3” to sever. </p>
<p align="center">
  <img src="https://user-images.githubusercontent.com/30017031/34640030-21ec6eda-f30d-11e7-8434-dea1fa0d8f40.PNG" />
</p>
 
 
## Code/Logic Design 
 
### 1. Client 
The sender will create a UDP socket and create a connection with the server using a port number and its IP address provided by the user. A packet contains the data bytes + sequence number of respective packet (0-4). Sender will read the audio file specified by the filename given by user and transfer it using the UDP socket restricted to 500 bytes per packet (in payload) and also transfer the sequence number along via “sendBuff” array. Window(of 5 packetsize) is a 2-dimensional array with size 5x500 holding each packet's data bytes(max 500). After sending a packet, the sender binds to the listenport to receive acknowledgments from the receiver. If ack is received, the “ack” array gets value true, otherwise false. After sending the last packet in window, sender checks for the unacked packets through the bool value of “ack”. If it is false, sender will retransmit after 2 seconds and wait for the ack again. Same happens for all unacked packets. After transfering all packets, the file and socket closes. 
 
 
### 2. Server 
The receiver will bind to the UDP port specified on the command line and receive packets from the sender sent over the port. It will buffer the bytes in the “window” array in its respective packet sequence number location. It will re-send the packet's sequence number to the client as ack number. Now, if the received sequence number is as expected(i.e. in order), it will write the bytes to the file after opening it. Otherwise, it wont write yet and assign the packet, bool value of false through the “order” array. When the last packet of window is received, it will write the bytes in file of those packets whose bool value was false, in order. Same happens for all other packets. 

 
 
