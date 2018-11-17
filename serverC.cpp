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
#include <math.h>

#define ServerC_UDP_PORT 23947 

using namespace std;

int udp_sockfd;
struct sockaddr_in udp_server_addr, udp_client_addr;
char recv_buff[1024];
vector<string> rec_aws;
double tran_delay, prop_delay, end_delay;


//Create a UDP socket
void create_udp_socket(){
	udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	//test if create a socket successfully
	if(udp_sockfd == -1){
		perror("ServerC UDP socket");
		exit(1);
	}

	memset(&udp_server_addr, 0, sizeof(udp_server_addr));    // make sure the struct is empty
	udp_server_addr.sin_family = AF_INET;
	udp_server_addr.sin_port   = htons(ServerC_UDP_PORT);    //port
	udp_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //set IP

	//test if bind a socket successfully
	if( bind(udp_sockfd, (struct sockaddr*) &udp_server_addr, sizeof(udp_server_addr)) == -1){
		perror("ServerC UDP bind");
		exit(1);
	}
	
	socklen_t udp_length = sizeof(udp_client_addr);
	getsockname(udp_sockfd,(struct sockaddr*) &udp_client_addr , &udp_length);
	int portnum = ntohs(udp_client_addr.sin_port);
	printf("The Server C is up and running using UDP on port <%d>.\n",portnum);
}

/* split string by pattern */
vector<string> split(string str,string pattern)
{
    string::size_type pos;
    vector<string> result;
    
    str+=pattern;
    int size=str.size();
    
    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

/* convert double to string and round(2)*/
string to_string(double val)
{
	char buf[2000];
	sprintf(buf,"%.2f",val);
	return string (buf);
}

/* caculate the results*/
void compute()
{
	int id, size, power, bandwidth;
	double length, velocity, noise;

	id = atoi(rec_aws[0].c_str());
	size = atoi(rec_aws[1].c_str());
	power = atoi(rec_aws[2].c_str());
	bandwidth = atoi(rec_aws[3].c_str());
	length =  atof(rec_aws[4].c_str());
	velocity =  atof(rec_aws[5].c_str());
	noise =  atof(rec_aws[6].c_str());

	double s = pow(10,((power - 30)/10));
	double n = pow(10,((noise - 30)/10)); 
	double c = bandwidth * log2(1+s/n);

	tran_delay = (size /c) / 1000; 
	prop_delay = (length / velocity) /10;
	end_delay = tran_delay + prop_delay;
}


int main()
{
	create_udp_socket();

	while(1)
	{
		socklen_t serverC_udp_len = sizeof(udp_client_addr);
		if(-1 == recvfrom (udp_sockfd,recv_buff,1024,0,(struct sockaddr *) &udp_client_addr,&serverC_udp_len))
		{
			perror("serverC receive failed");
			exit(1);
		}
		rec_aws = split((string)recv_buff,",");
		printf("The Server C received link information of link <%s>, file size <%s>, and signal power <%s>\n", rec_aws[0].c_str(),rec_aws[1].c_str(),rec_aws[2].c_str());

		//caculate the result
		compute();
		printf("The Server C finished the calculation for link <%s>\n",rec_aws[0].c_str());


		string message = to_string(tran_delay) +  "," + to_string(prop_delay) + "," +to_string(end_delay);
		if(-1 ==sendto(udp_sockfd,message.c_str(),1024,0,(struct sockaddr *) &udp_client_addr,sizeof(udp_client_addr)))
		{
			perror("serverC response failed");
			exit(1);
		}
		printf("The Server C finished sending the output to AWS\n");
	}
	close(udp_sockfd);
	return 0;
}
