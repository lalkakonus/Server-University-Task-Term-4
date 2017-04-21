#include <iostream>
#include "sock_wrap.h" 
#include <cstdlib>
#include <unistd.h>
#include <fstream>

using namespace std;
using namespace SockWrap;

#define PORT_NUM 1234	     // номер порта процесса-сервера 

int main(int argc, char* argv[])
{
	try {
      		char host[64];

    		if (gethostname(host, sizeof host) < 0) {
        		cout << "TROUBLE!" << endl;
			perror("Host name");
        		exit(-1);
      		}

    		InClientSocket sock(host, PORT_NUM);
    		sock.Connect();

		char buff[100]; 
		memset(buff,0,100);
		ifstream fin("/rawreq.txt"); 
 	
		fin.seekg(0,fin.end);
		int size=fin.tellg(),count = size / 100, moar = size % 100;	

		fin.seekg(0);

		for (int i=0; i<count; i++){
			fin.read(buff,100);
    			sock.Write(buff,100);
		};
	
		fin.read(buff,moar);
    		sock.Write(buff,100); 
 	
    		fin.close(); 

    	} catch (Exception& e) { e.Report();}
	
	return 0;
}

