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

#define Aws_UDP_PORT 24947 
#define Aws_TCP_Client_PORT 25947
#define Aws_TCP_Monitor_PORT 26947

using namespace std;

int udp_sockfd, tcp_sockfd_client, tcp_sockfd_monitor;
struct sockaddr_in udp_server_addr, udp_client_addr;
struct sockaddr_in tcp_server_addr, tcp_monitor_addr, tcp_cliser_addr, tcp_climon_addr;
int accept_cli, accept_mon, rval;
string link_ID, size, power;
vector<string> input, result_A, result_B, result_C;


/*Create a UDP socket*/
void create_udp_socket(){
	udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	//test if create a socket successfully
	if(udp_sockfd == -1){
		perror("AWS UDP socket");
		exit(1);
    }
    
    //Bind socket to a ip/port
    //UDP
    memset(&udp_server_addr, 0, sizeof(udp_server_addr));    // make sure the struct is empty
    udp_server_addr.sin_family = AF_INET;
    udp_server_addr.sin_port   = htons(Aws_UDP_PORT);    //port
    udp_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //set IP
    
    //test if bind a socket successfully
    if( bind(udp_sockfd,
             (struct sockaddr*) &udp_server_addr,
             sizeof(udp_server_addr)) == -1){
        perror("AWS UDP bind");
        exit(1);
    }

}

//create tcp socket of client and monitor
void create_tcp_client(){
	tcp_sockfd_client = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sockfd_client == -1)
	{
		perror("AWS TCP client socket");
		exit(1);
	}
    
    tcp_sockfd_monitor = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sockfd_monitor == -1)
    {
        perror("AWS TCP monitor socket");
        exit(1);
    }
    
    
    memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));    // make sure the struct is empty
    tcp_server_addr.sin_family = AF_INET;
    tcp_server_addr.sin_port   = htons(Aws_TCP_Client_PORT);    //port
    tcp_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //set IP
    if(bind(tcp_sockfd_client, (struct sockaddr*) &tcp_server_addr, sizeof(tcp_server_addr)) == -1)
    {
        perror("AWS TCP client bind");
        exit(1);
    }
    
    
    memset(&tcp_monitor_addr, 0, sizeof(tcp_monitor_addr));    // make sure the struct is empty
    tcp_monitor_addr.sin_family = AF_INET;
    tcp_monitor_addr.sin_port   = htons(Aws_TCP_Monitor_PORT);    //port
    tcp_monitor_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //set IP
    if(bind(tcp_sockfd_monitor, (struct sockaddr*) &tcp_monitor_addr, sizeof(tcp_monitor_addr)) == -1)
    {
        perror("AWS TCP monitor bind");
        exit(1);
    }
    
    

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

/*send message to server a, b and c */
void send_to_server(int port, string server_name, string message)
{
	memset(&udp_server_addr,0, sizeof(udp_server_addr));
	udp_server_addr.sin_family = AF_INET;
    udp_server_addr.sin_port = htons(port);
    udp_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(udp_sockfd,message.c_str(),1024,0,(const struct sockaddr *) &udp_server_addr, sizeof(udp_server_addr));

    if(server_name == "C")
    {
	printf("The AWS sent link ID=<%s>, size=<%s>, power=<%s>, and link information to Backend-Server C using UDP over port <%d>\n",link_ID.c_str(),size.c_str(),power.c_str(),ntohs(udp_server_addr.sin_port));
    }else
    {
    printf("The AWS sent link ID=<%s> to Backend-Server <%s> using UDP over port <%d>\n",message.c_str(),server_name.c_str(),ntohs(udp_server_addr.sin_port));	
    }


}


/*receve infomation from server a and b. 
  return the infomation string */
string receive_from_server(string server_name)
{
		char recv[1024];
		socklen_t server_udp_len = sizeof(udp_client_addr);
		recvfrom(udp_sockfd,recv,1024,0,(struct sockaddr *) &udp_client_addr,&server_udp_len);

		string res = recv;
		string status;

		if(server_name =="A")
		{
		result_A = split(res, ",");
		status = result_A[0];
		}
		if(server_name =="B")
		{
		result_B = split(res, ",");
		status = result_B[0];
		}
		printf("The AWS received <%s> matches from Backend-Server <%s> using UDP over port <%d>\n", status.c_str() , server_name.c_str(), ntohs(udp_client_addr.sin_port));
		return res;
}

/**
	resort the input and result from server
	new string order << link id, size, power, bandwidth, length, v, noise power
*/
string resortInfo(string input, string result)
{
	string oldString = input + "," + result;
	string newString;
	vector<string> newStr;

	newStr = split(oldString,",");
	newString = newStr[0] + "," + newStr[1] + "," +newStr[2] + "," +newStr[5] + "," +newStr[6] + "," +newStr[7] + "," +newStr[8] ;
	return newString;
}


int main()
{
	create_udp_socket();
	create_tcp_client();
    
	printf("The AWS is up and running.\n");
	
	//Mark the socket for listening in
	listen(tcp_sockfd_client,5);
    listen(tcp_sockfd_monitor,5);

	socklen_t  tcp_monitor_addr_len = sizeof(tcp_climon_addr);
	accept_mon = accept(tcp_sockfd_monitor,(struct sockaddr*) &tcp_climon_addr,&tcp_monitor_addr_len);
	
	//Acept a call
	while(1){
		socklen_t  tcp_client_addr_len = sizeof(tcp_cliser_addr);
		accept_cli = accept(tcp_sockfd_client,(struct sockaddr*) &tcp_cliser_addr,&tcp_client_addr_len);
		if(accept_cli == -1 )
		{
			perror("accept failed");
			exit(1);
		}
     
        //receive from client
		char input_cli[1024];
		memset(input_cli,0,sizeof(input_cli)); // clear the input_cli
		if((rval = recv(accept_cli, input_cli, sizeof(input_cli), 0 )) < 0 )
		{
			perror("reading stream message error!");
			exit(1);
		}else if (rval == 0){
			printf("Ending connection\n");
		}

		string str = input_cli;
		input =  split(str, ",");

		link_ID = input[0];
		size    = input[1];
		power   = input[2];
	
		socklen_t tcp_1_length = sizeof(tcp_cliser_addr);
		getsockname(tcp_sockfd_client,(struct sockaddr*) &tcp_cliser_addr, &tcp_1_length);
		printf("The AWS received link ID=<%s>, size=<%s>, and power=<%s> from the client using TCP over port <%d>\n", link_ID.c_str(),size.c_str(),power.c_str(),ntohs(tcp_cliser_addr.sin_port));	


        //sent to monitor
        sendto(accept_mon,input_cli,sizeof(input_cli),0,(struct sockaddr *) &tcp_climon_addr, sizeof(tcp_climon_addr) );
		socklen_t tcp_2_length = sizeof(tcp_climon_addr);
		getsockname(tcp_sockfd_monitor,(struct sockaddr*) &tcp_climon_addr, &tcp_2_length);
        printf("The AWS sent link ID=<%s>, size=<%s>, and power=<%s> to the monitor using TCP over port <%d>\n",link_ID.c_str(),size.c_str(),power.c_str(),ntohs(tcp_climon_addr.sin_port));
    
        
        
        //sent to server a and b,then receive a and b
		send_to_server(21947,"A",link_ID);
		string resa = receive_from_server("A");	
		send_to_server(22947,"B",link_ID);
		string resb = receive_from_server("B");
		
		string status_A = result_A[0];  
		string status_B = result_B[0];

        // sent to server c
		string info;	
		if(status_A == "0" && status_B == "0"){
		
			
		char non[] = "none";
		sendto(accept_cli,non,sizeof(non),0,(struct sockaddr *) &tcp_cliser_addr, sizeof(tcp_cliser_addr) );
		socklen_t tcp_cli_length = sizeof(tcp_cliser_addr);
		getsockname(tcp_sockfd_client,(struct sockaddr*) &tcp_cliser_addr, &tcp_cli_length);

		sendto(accept_mon,non,sizeof(non),0,(struct sockaddr *) &tcp_climon_addr, sizeof(tcp_climon_addr) );
		socklen_t tcp_mon_length = sizeof(tcp_climon_addr);
		getsockname(tcp_sockfd_monitor,(struct sockaddr*) &tcp_climon_addr, &tcp_mon_length);
		
		printf("The AWS sent No Match to the client and the monitor using TCP over ports <%d> and <%d>, respectively\n", ntohs(tcp_cliser_addr.sin_port),ntohs(tcp_climon_addr.sin_port));
		continue;


		}else if(status_A != "0" && status_B == "0"){
			info = resortInfo(str,resa); 
		}else if(status_A == "0" && status_B != "0"){
			info = resortInfo(str,resb); 
		}
		send_to_server(23947,"C",info);						


        //receive from c
		char recv_from_C[1024];
		socklen_t serverC_udp_len = sizeof(udp_client_addr);
		recvfrom(udp_sockfd,recv_from_C,1024,0,(struct sockaddr *) &udp_client_addr,&serverC_udp_len);
		printf("The AWS received outputs from Backend-Server C using UDP over port <%d>\n", ntohs(udp_client_addr.sin_port));

        // sent to client
		result_C = split((string)recv_from_C,",");
		sendto(accept_cli,recv_from_C,sizeof(recv_from_C),0,(struct sockaddr *) &tcp_cliser_addr, sizeof(tcp_cliser_addr) );
		socklen_t tcp_cli_length = sizeof(tcp_cliser_addr);
		getsockname(tcp_sockfd_client,(struct sockaddr*) &tcp_cliser_addr, &tcp_cli_length);
		printf("The AWS sent delay=<%s>ms to the client using TCP over port <%d>\n", result_C[2].c_str() , ntohs(tcp_cliser_addr.sin_port));


        //sent to monitor
		sendto(accept_mon,recv_from_C,sizeof(recv_from_C),0,(struct sockaddr *) &tcp_climon_addr, sizeof(tcp_climon_addr) );
		socklen_t tcp_mon_length = sizeof(tcp_climon_addr);
		getsockname(tcp_sockfd_monitor,(struct sockaddr*) &tcp_climon_addr, &tcp_mon_length);
		printf("The AWS sent detailed results to the monitor using TCP over port <%d>\n",ntohs(tcp_climon_addr.sin_port));


	}


	return 0;

}
