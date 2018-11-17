#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <string>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>


#define ServerA_UDP_PORT 21947 

using namespace std;


int udp_sockfd;
struct sockaddr_in udp_server_addr, udp_client_addr;
char recv_buff[1024];
string resultData,sta_request;


//Create a UDP socket
void create_udp_socket()
{
	udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	//test if create a socket successfully
	if(udp_sockfd == -1){
		perror("ServerA UDP socket");
		exit(1);
	}
}


void search (string target)
{
	ifstream inFile("database_a.csv",ios::in);
	string lineStr;
	vector< vector<string> > strArray;
	// save file in a vector
	while (getline(inFile,lineStr))
	{
		stringstream ss(lineStr);
		string str;
		vector<string> lineArray;
		while(getline(ss,str,','))
		{
			lineArray.push_back(str);
		}
		strArray.push_back(lineArray);
	}
	//search 
	sta_request = "0";
	resultData  = "";
	for(int i = 0 ; i < strArray.size(); i++)
	{	
		if(target == strArray[i][0])
		{	
			sta_request = "1";
			resultData =  "," + strArray[i][0] + "," + strArray[i][1] + "," + strArray[i][2] + "," + strArray[i][3] + "," + strArray[i][4]; 
		}
	}
	//print the result
	printf("The server A has found <%s> match\n",sta_request.c_str());
}


int main()
{
	create_udp_socket();

	memset(&udp_server_addr, 0, sizeof(udp_server_addr));    // make sure the struct is empty
	udp_server_addr.sin_family = AF_INET;
	udp_server_addr.sin_port   = htons(ServerA_UDP_PORT);    //port
	udp_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //set IP

	//test if bind a socket successfully
	if( bind(udp_sockfd, (struct sockaddr*) &udp_server_addr, sizeof(udp_server_addr)) == -1){
		perror("ServerA UDP bind");
		exit(1);
	}

	printf("The Server A is up and running using UDP on port <%d>.\n",ServerA_UDP_PORT);

	
	while(1)
	{
		socklen_t serverA_udp_len = sizeof(udp_client_addr);
		if (recvfrom (udp_sockfd,recv_buff,1024,0,(struct sockaddr *) &udp_client_addr,&serverA_udp_len) == -1){
			perror("serverA receive failed");
			exit(1);
		}
		printf("The Server A received input <%s>\n", recv_buff);

		search(recv_buff);

		string message = sta_request  + resultData;

		if(-1 ==sendto(udp_sockfd,message.c_str(),1024,0,(struct sockaddr *) &udp_client_addr,sizeof(udp_client_addr)))
		{
			perror("serverA response failed");
			exit(1);
		}
		printf("The Server A finished sending the output to AWS\n");

	}

	return 0;

}
