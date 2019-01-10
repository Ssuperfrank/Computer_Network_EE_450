# EE450
socket programming

a. Full name: jiaqi fan

b. Student ID: 
 
c. What you have done in the assignment. 
	In the socket programming project, I made simulations of transmiiting messages between client, server and aws by TCP and UDP. 
	The client and monitor connect the aws by TCP on two different sockets, according to different port numbers. The aws exchanged data between server a b and c by UDP.
	I have tested all code on my local Ubuntu whether the link exists in the database or not. 


d. Implementations
	1.Client

	* create tcp socket to connect the server of aws
	* send link id, size, power to aws
	* receive the final result from aws
	* split the received data into some pieces by ","
	* print the final result. if not found, print no match,if found, print delay

	2.AWS
	* create udp and tcp sockets and bind address to it
	* listen to two sockets, and accept TCP connection from client and monitor
	* receive infomation from client by tcp
	* send infomation to server a and b by udp
	* receive data from server a and b by udp
	* judge if there is a match in the database
	* if no matches, send result to monitor and client directly
	* if there exists matches, send data to server c 
	* receive delays from server c by udp
	* send delay to client and monitor by tcp
	
	3.Server
	* create and bind address to the udp socket	
	* receive link id by udp from aws in server a and b
	* search for data from database	
	* send back data to aws by udp
	* receive all data from data from aws in server c
	* compute all caculations to get delays in server c
	* send delay to aws by udp

	4.Monitor
	* create tcp socket to connect aws
	* receive delays from aws by tcp and print all
