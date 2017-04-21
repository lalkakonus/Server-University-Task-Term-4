#include <iostream>
#include <cstdlib>
#include <fstream>
#include "sock_wrap.h"
#include <fstream>

using namespace std;
using namespace SockWrap;

#define PORT_NUM 1234  // номер порта процесса-сервера 

class MyServerSocket : public InServerSocket {
public:
MyServerSocket () : InServerSocket (PORT_NUM) {}

    //void OnAccept (BaseSocket * pConn){};
};

class ReqHandle{

public:
	string meth, path, vers, data;
	string uri, date, host, referer, user_agent, server;
	string content_length, content_type;
	string allow, last_modified, accept;

	ReqHandle():server("KEK SERVER"), allow("GET, HEAD"), RespCode(200){};

	int RespCode;

	int ParseHeader(string str){


		int len = str.length();
		int cuptr = 0, pos1=0, pos2=0;
		
		pos2=str.find(' ',pos1);
		meth = str.substr(pos1, pos2 - pos1);
		pos1=pos2+1;
		cout << "Method: " << meth << endl;	
		if (meth!="GET" && meth!="HEAD")
			RespCode =  501;	
		

		pos2=str.find(' ',pos1);
		uri = str.substr(pos1, pos2 - pos1);
		pos1=pos2+1;
		cout << "URI: " << uri << endl;	
		
		pos2=str.length();
		vers = str.substr(pos1, pos2 - pos1-1);
		pos1=pos2+1;
		cout << "Version: " << vers << endl;	
		

		pos1=pos2=0;
		if ((pos2=uri.find('?',0))==string::npos)
			pos2=uri.length();
		path=uri.substr(1,pos2);
		pos1=pos2+1;
		cout << "Path: " << path << endl;

		len = uri.length();
		while (pos2<len){
			if ((pos2=uri.find('&',pos1))==string::npos)
				pos2=len;
			cout << uri.substr(pos1, pos2-pos1) <<endl ;
			pos1=pos2+1;
		};
		return 0;
	};

	int ParseParam(string str){
		string par,val;
		int pos1;
		pos1=str.find(':',0);
		cout << (par=str.substr(0, pos1)) << " = " << (val=str.substr(pos1+2,str.length()- pos1)) << endl;
		
		if (par=="Host") host=val;
		if (par=="User-agent") user_agent=val;	
		if (par=="Data") data=val;	
		if (par=="Accept") accept=val;		
	
		return 0;
	};

	int ParseRequest(BaseSocket * pConn = NULL){
		string str,header;
		header=pConn->GetString();
		ParseHeader(header);
		while ((str=pConn->GetString())!="" && str!="\r")
			ParseParam(str);
		return 0;
	};

	int MakeResponse(BaseSocket * pConn = NULL){
		
		cout << "Response: \n";


		char buff[100]; 
		memset(buff,0,100);
		ifstream fin(path.c_str()); 
		int size,count,moar;

		if (fin.is_open()){
			fin.seekg(0,fin.end);
			size=fin.tellg();
			count = size / 100;
			moar = size % 100;	
		}
		else{
			cout << "Couldn't open file!" << endl;
			throw "FILE";
		};
		//cout << "###" << vers << RespCode << endl;
		pConn->PutString(vers + " " + to_string(RespCode) + " OK\r\n");	
		//pConn->PutString("Date"+date+"\n");	
		//pConn->PutString("HTTP/1.1 200 OK\n");
		pConn->PutString("Server: "+server+"\r\n");	
		
		//pConn->PutString("Last-Moified:"+last_modified+"\n");	
		pConn->PutString("Content-Type: text/html\r\n");	
		pConn->PutString("Content-Length: "+to_string(size)+"\r\n");	
		pConn->PutString("Connection: close\r\n");	
		pConn->PutString("Accept-rages: bytes\r\n");
		pConn->PutString("\r\n\r\n");	

		
		fin.seekg(0);

		for (int i=0; i<count; i++){
			fin.read(buff,100);
    			pConn->Write(buff,100);
		};

		cout << "!!!!" << endl;
//
	
		fin.read(buff,moar);
    		pConn->Write(buff,moar); 
 		
		for (int j=0; j<15; j++)
			pConn->PutChar(' ');	
    		
		fin.close(); 

		return 0;
	};

};

int main(int argc, char* argv[])
{

	cout << endl;
	for (int j=0; j<80; j++) cout << "*" ;
	cout << endl;
    	
	try {
      		MyServerSocket sock;
		ReqHandle handler;
				
		cout << endl;
		
		BaseSocket * pConn = sock.Accept();
  	
		handler.ParseRequest(pConn);
		
		cout << "HEHEH" << endl;
		
		handler.MakeResponse(pConn);	
		delete pConn;
	
   	} catch (Exception& e) { e.Report();}	


	cout << endl;
	for (int j=0; j<80; j++) cout << "*" ;

    return 0;
}
