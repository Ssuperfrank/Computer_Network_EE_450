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
vector<string> info, output;


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


int main()
{
	tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_sockfd == -1)
	{
		perror("Monitor TCP socket");
		exit(1);
	}
	memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));  
	tcp_server_addr.sin_family = AF_INET;
	tcp_server_addr.sin_addr.s_addr =  inet_addr("127.0.0.1");
	tcp_server_addr.sin_port = htons(26947);
	printf("The monitor is up and running\n");


	if(-1 == connect(tcp_sockfd,(struct sockaddr*)&tcp_server_addr, sizeof(tcp_server_addr)))
	{
		perror("Monitor TCP connect");
		exit(1);
	}
	

	while(1)
	{
		char recv_info[1024];
		if(recv(tcp_sockfd,recv_info, sizeof(recv_info),0) == -1)
		{
			perror("receive1 failed");
			close(tcp_sockfd);
			exit(1);
		}
		info = split((string)recv_info,",");
		printf("The monitor received link ID=<%s>, size=<%s>, and power=<%s> from the AWS\n", info[0].c_str(),info[1].c_str(),info[2].c_str());	


		char recv_result[1024];
		if(recv(tcp_sockfd,recv_result, sizeof(recv_result),0) == -1)
		{
			perror("receive2 failed");
			close(tcp_sockfd);
			exit(1);
		}
		output = split((string)recv_result,",");
		if(output.size()>1)
		{
			printf("The result for link <%s>:\n Tt = <%s>ms\n Tp = <%s>ms\n Delay = <%s>ms\n",info[0].c_str(),output[0].c_str(),output[1].c_str(),output[2].c_str());
		}
		else
		{
			printf("Found no matches for link <%s>\n",info[0].c_str());
		}
	}
	
	close(tcp_sockfd);
}
