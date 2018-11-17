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

using namespace std;

int tcp_sockfd;
struct sockaddr_in tcp_server_addr, tcp_client_addr;
int link_ID, size, power;
vector<string> output ;


void creat_tcp_client()
{
	tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_sockfd == -1)
	{
		perror("Client TCP socket");
		exit(1);
	}
	memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));    // make sure the struct is empty
	tcp_server_addr.sin_family = AF_INET;
	tcp_server_addr.sin_port = htons(25947);
	tcp_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //set IP

	printf("The client is up and running\n");

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




int main(int argc, char const *argv[])
{
	string link_ID = argv[1];
	string size    = argv[2];
	string power   = argv[3];

	char buff[1024];
	string str = link_ID +","+ size +","+ power;
	strcpy(buff,str.c_str());


	creat_tcp_client();

	if(connect(tcp_sockfd,(struct sockaddr*)&tcp_server_addr,sizeof(tcp_server_addr)) < 0)
	{
		perror("connect failed");
		close(tcp_sockfd);
		exit(1);
	}

	if(send(tcp_sockfd,buff,sizeof(buff),0) < 0)
	{
		perror("send failed");
		close(tcp_sockfd);
		exit(1);
	}
	printf("The client sent ID=<%s>, size=<%s>, and power=<%s> to AWS\n",argv[1],argv[2],argv[3]);


	char recv_client[1024];
	if(recv(tcp_sockfd,recv_client, sizeof(recv_client),0) == -1)
	{
		perror("receive failed");
		close(tcp_sockfd);
		exit(1);
	}

	output = split((string)recv_client,",");
	if(output.size()>1)
	{
		printf("The delay for link <%s> is <%s>ms\n",argv[1],output[2].c_str());
	}
	else
	{
		printf("Found no matches for link <%s>\n", argv[1]);
	}
	
	close(tcp_sockfd);
}
