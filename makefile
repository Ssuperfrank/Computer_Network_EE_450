all: 
	
	
	g++ -o serverA serverA.cpp
	g++ -o serverB serverB.cpp
	g++ -o serverC serverC.cpp

	g++ -o aws aws.cpp
	
	g++ -o client client.cpp	

	g++ -o monitor monitor.cpp
	




ServerA:
	./serverA

ServerB:
	./serverB

ServerC:
	./serverC

AWS:	
	./aws

Client:
	./client

Monitor:
	./monitor

