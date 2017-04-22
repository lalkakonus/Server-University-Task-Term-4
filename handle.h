#ifndef __HANDLE_H__
#define __HANDLE_H__

#include <iostream>
#include <cstdlib>
#include <fstream>
#include "lib.h"
#include <fstream>

using namespace std;
using namespace SockWrap;

#define PORT_NUM 1234  // номер порта процесса-сервера 

class MyServerSocket : public InServerSocket {
public:
	MyServerSocket () : InServerSocket (PORT_NUM) {}
};

class ReqHandle {

public:
	string meth, path, vers, data, text;
	string uri, date, host, referer, user_agent, server;
	string content_length, c_type;
	string allow, last_modified, accept;

	ReqHandle():c_type("text/plain"), text("OK"), server("My server 1.0"), allow("GET, HEAD"), RespCode(200){};

	int RespCode;

	int ParseHeader(string str){

		string extension;
		int length = str.length(), pos1=0, pos2=0;
		
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

		if (access(path.c_str(), 0) == -1){	
			RespCode=404;
			text="Not found";
		};

		
		extension=path.substr(path.find('.',0)+1,path.length());
		cout << "File extension : " << extension << endl;
		if (extension == "html") c_type="text/html";
		if (extension == "jpeg" || extension == "jpg") c_type="image/jpeg";
			
		cout << "Path: " << path << endl;

		length = uri.length();
		
		while (pos2<length){
			if ((pos2=uri.find('&',pos1))==string::npos)
				pos2=length;
			cout << "Parametr: " << uri.substr(pos1, pos2-pos1) <<endl ;
			pos1=pos2+1;
		};

		return 0;
	};

	int ParseParam(string str){
		string par,val;
		int pos1;
		
		if ((pos1=str.find(':',0))==string::npos)
			return -1;
		
		cout << (par=str.substr(0, pos1)) << " = " << (val=str.substr(pos1+2,str.length()- pos1)) << endl;
		
		if (par=="Host") host=val;
		if (par=="User-Agent") user_agent=val;	
		if (par=="Data") data=val;	
		if (par=="Accept") accept=val;		
	
		return 0;
	};

	int ParseRequest(BaseSocket * pConn = NULL){
		string str;
		
		str=pConn->GetString();
		ParseHeader(str);
		
		while ((str=pConn->GetString())!="" && str!="\r")
			ParseParam(str);
		return 0;
	};

	int MakeResponse(BaseSocket * pConn = NULL){
		
		char buff[100]; 
		ifstream fin(path.c_str()); 
		int size,encount,other;

		memset(buff,0,100);
		if (fin.is_open()){
			fin.seekg(0,fin.end);
			size=fin.tellg();
			encount = size / 100;
			other = size % 100;	
			fin.seekg(0);
		};

		pConn->PutString(vers + " " + to_string(RespCode) + " "+text+"\r\n");	
		//pConn->PutString("Date"+date+"\n");	
		pConn->PutString("Server: "+server+"\r\n");	
		pConn->PutString("Allow: " + allow + "\r\n");
		//pConn->PutString("Last-Moified:"+last_modified+"\n");	
		if (RespCode==200){
			pConn->PutString("Content-Type: "+ c_type+"\r\n");	
			pConn->PutString("Content-Length: "+to_string(size)+"\r\n");	
			pConn->PutString("Accept-rages: bytes\n");
		};
		pConn->PutString("Connection: close\r\n");	
		pConn->PutChar('\n');	
		pConn->PutChar('\n');	
		
		if (RespCode==200){

		for (int i=0; i<encount; i++){
			fin.read(buff,100);
    			pConn->Write(buff,100);
		};

		fin.read(buff,other);
    		pConn->Write(buff,other); 
	
		cout << "\n";
		
		fin.close(); 

		};

		return 0;
	};

};

# endif
