#ifndef __SOCK_WRAP_H__
#define __SOCK_WRAP_H__


#include <errno.h>
#include <string>
#include <iostream>
//#include <io.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>


namespace SockWrap {

#define MODE 2

// Exception - класс исключений, генерируемых библиотекой
class Exception {
protected:
	int m_ErrCode;
public:
	Exception (int errcode): m_ErrCode(errcode) {};
    	void Report(){
		fprintf(stderr,"Error, code # %d !", m_ErrCode);
	};
    	virtual std::string GetMessage() = 0;
};

// SocketExceptiOAon - класс исключений 
class SocketException : public Exception {
    static char * m_Message[];
public:
    enum SocketExceptionCode {
         ESE_SUCCESS, 		//0
         ESE_SOCKCREATE, 	//1
         ESE_SOCKCONN, 		//2
         ESE_SOCKILLEGAL,	//3
         ESE_SOCKHOSTNAME,	//4
         ESE_SOCKSEND,		//5
         ESE_SOCKRECV,		//6
         ESE_SOCKBIND,		//7
         ESE_SOCKLISTEN,	//8
         ESE_SOCKACCEPT,	//9
	 ESE_EXIT		//10
    };
    SocketException(SocketExceptionCode errcode) : Exception(errcode) {}
    	std::string GetMessage(){
		return (std::string("KEK"));
	}; ///For what??
};

//typedef SocketException::SocketExceptionCode sec; 

// SocketAddress - базовый абстрактный класс для представления //сетевых адресов
class SocketAddress {
protected:
    	struct sockaddr * m_pAddre; //m_pAddr
public:
	SocketAddress() : m_pAddre(NULL) {};
    	
	virtual ~SocketAddress () {};
    	virtual int GetLength() = 0;
    	virtual SocketAddress * Clone() = 0;
    	
	operator struct sockaddr * (){
		return m_pAddre;
	};
};

// UnSocketAddress - представление адреса семейства AF_UNIX
class UnSocketAddress : public SocketAddress {
public:
        UnSocketAddress (const char * SockName){
		if ((m_pAddre = (struct sockaddr *)(new (struct sockaddr_un))) < 0)
			throw ("Error: Memory Exception!");
		memset(m_pAddre, 0, sizeof(struct sockaddr_un));
		((struct sockaddr_un *)m_pAddre)->sun_family=AF_UNIX;
		strcpy(((struct sockaddr_un *)m_pAddre)->sun_path,SockName);
    	};
	
	int GetLength (){
		return sizeof(*((struct sockaddr_un *)m_pAddre));
	};

        SocketAddress * Clone(){
		UnSocketAddress * Copy = (new UnSocketAddress(((struct sockaddr_un *)m_pAddre)->sun_path));
		return Copy;
	};

        ~UnSocketAddress (){
		delete m_pAddre; 
	};
};

// InSocketAddress - представление адреса семейства AF_INET
class InSocketAddress : public SocketAddress {
public:
        InSocketAddress (const char * HostName, short PortNum){
		if ((m_pAddre =(struct sockaddr *)(new struct sockaddr_in)) < 0)
			throw ("Error: Memory Exception!");
		memset(m_pAddre,0,sizeof(*((struct sockaddr_in *)m_pAddre)));
		((struct sockaddr_in *)m_pAddre)->sin_family=AF_INET;
		((struct sockaddr_in *)m_pAddre)->sin_port=htons(PortNum);
		
		if (strcmp(HostName,"AAA"))
			((struct sockaddr_in *)m_pAddre)->sin_addr.s_addr=INADDR_ANY;
		else
			inet_aton(HostName, & (((struct sockaddr_in *)m_pAddre)->sin_addr));
	};

    	int GetLength (){
		return sizeof(*((struct sockaddr_in *)m_pAddre));
	};

        SocketAddress * Clone(){
		InSocketAddress * Copy = new InSocketAddress(
inet_ntoa(((struct sockaddr_in *)m_pAddre) -> sin_addr), 
ntohs( ((struct sockaddr_in *)m_pAddre)->sin_port));

		return Copy;
	};
        
	~InSocketAddress (){
		delete (struct sockaddr_in *)m_pAddre;
	};

};

// BaseSocket - базовый класс для сокетов
class BaseSocket {
public:
 	explicit BaseSocket(int sd = -1, SocketAddress * pAddr = NULL): m_Socket(sd), m_pAddr(pAddr) {};
	
	virtual ~BaseSocket(){
		if (shutdown(m_Socket,MODE)<0) //MODE = 2
			throw (SocketException(SocketException::ESE_EXIT));
		close(m_Socket);
		std::cout << "# Connection closed " << std::endl;
	};
	
	void Write(void * buf, int len){
		if (send(m_Socket, buf, len, 0)<0)
			throw (SocketException(SocketException::ESE_SOCKSEND));
		std::cout << (char *)buf;
		//std::cout << "# Write (buf *). Success " << (char *)buf << std::endl;
	};
    	
	void PutChar(int c){
		if (send(m_Socket, & c, 1, 0)<0)
			throw (SocketException(SocketException::ESE_SOCKSEND));
		//std::cout << "# Write (char). Success" << std::endl;
	};

    	void PutString(const char * str){
		if (send(m_Socket, str, strlen(str) + 1, 0)<0)
			throw (SocketException(SocketException::ESE_SOCKSEND));
		std::cout << str;
		//std::cout << "# Write (const char *): " << str << std::endl;
		//std::cout << "# * Size: " << strlen(str) + 1 << std::endl;
		//std::cout << "# * Write (const char *). Success" << std::endl;
	};
    	
	void PutString(const std::string& s){
		if (send(m_Socket, s.c_str(), s.length()+ 1, 0)<0)
			throw (SocketException(SocketException::ESE_SOCKSEND));
		std::cout << s.c_str();
		//std::cout << "# Write (std::string &): " << s.c_str() << std::endl;
		//std::cout << "# * Size: " << s.length() << std::endl;
		//std::cout << "# * Write success" << std::endl;
	};

    	int Read (void * buf, int len){
		int rsize = 0;
		if ((rsize = recv(m_Socket, buf, len, 0))<0)
			throw (SocketException(SocketException::ESE_SOCKRECV));
		std::cout << "# Read (buf *). Success " << (char *)buf << std::endl;
		return rsize;
	};

    	int GetChar(){
		int chr = 0;
		if ((chr = recv(m_Socket, & chr, 1, 0))<0)
			throw (SocketException(SocketException::ESE_SOCKRECV));
		std::cout << "# Read (char). Success" << std::endl;
		return chr;
		
	};

#define BUFFLEN 300

    	std::string GetString(){
		char * buf = new char[BUFFLEN];
		int len=0;
		if ((len = recv(m_Socket, buf, BUFFLEN , MSG_PEEK)) < 0)
			throw (SocketException(SocketException::ESE_SOCKRECV));

		int i=0;
		while (buf[i]!='\n') i++;

		if ((len = recv(m_Socket, buf, i + 1, 0)) < 0)
			throw (SocketException(SocketException::ESE_SOCKRECV));


	//	std::cout << "# Read (std::string): " << std::endl;
	//	std::cout << "# * Size: " << len << std::endl ;
		std::string str(buf,i);
		delete [] buf;
	//	std::cout << "# * Read successfully" << std::endl;
		if (len==0) str="";
//		std::cout << str << std::endl;
		return str;
	};
    	
	int GetSockDescriptor(){
		return m_Socket;
	};

protected:
    	int m_Socket;
    	SocketAddress * m_pAddr;
    
	void CheckSocket();//What is it for&???
};

// ClientSocket - базовый класс для клиентских сокетов
class ClientSocket: public BaseSocket {
public:
	void Connect(){
		if ((connect(m_Socket, (struct sockaddr *)(* m_pAddr), m_pAddr -> GetLength()))<0)
			throw (SocketException(SocketException::ESE_SOCKCONN));
		printf("# Connect succesfull \n");
	};
};

// ServerSocket - базовый класс для серверных сокетов
class ServerSocket: public BaseSocket {
public:
	BaseSocket * Accept(){
		int socketfd = 0;
		unsigned int a = m_pAddr->GetLength(), *b = &a;		
//SHITO???? 3 arg
		printf("# Wait for client \n");
		if ((socketfd = accept(m_Socket, (struct sockaddr *)(* m_pAddr),/*(socklen_t *)*/ b))<0)
			throw (SocketException(SocketException::ESE_SOCKSEND));
		printf("# Accept succesfull \n");
		return (new BaseSocket(socketfd, m_pAddr->Clone()));
	};
protected:

	void Bind(){
		if ((bind(m_Socket, (struct sockaddr *)(*m_pAddr), m_pAddr -> GetLength()))<0){
			int err = errno;
			std::cout << "Err# " << err << std::endl;
			throw (SocketException(SocketException::ESE_SOCKBIND));
		};
		printf("# Bind succesfull \n");
	};

    	void Listen(int BackLog){
		if ((listen(m_Socket, BackLog) <0))
			throw (SocketException(SocketException::ESE_SOCKLISTEN));
		printf("# Listen started \n");
	};

	virtual void OnAccept (BaseSocket * pConn) {}; ///??????
};

// UnClientSocket - представление клиентского сокета семейства //AF_UNIX
class UnClientSocket: public ClientSocket {
public:
	UnClientSocket (const char * Address){
	
		if ((m_Socket = socket(AF_UNIX, SOCK_DGRAM, IPPROTO_UDP))<0)
			throw (SocketException(SocketException::ESE_SOCKCREATE));
		if ((m_pAddr = new UnSocketAddress(Address)) < 0)
			throw (SocketException(SocketException::ESE_SOCKCREATE));
	};
};

// InClientSocket - представление клиентского сокета семейства //AF_INET
class InClientSocket: public ClientSocket {
public:
	InClientSocket(const char * HostName, short PortNum){
	
		if ((m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
			throw (SocketException(SocketException::ESE_SOCKCREATE));
		printf("# Client socket created \n");

		if ((m_pAddr = new InSocketAddress(HostName, PortNum)) < 0)
			throw (SocketException(SocketException::ESE_SOCKCREATE));
		printf("# Internet Address structure ceated \n");
	};
};

// UnServerSocket - представление серверного сокета семейства //AF_UNIX
class UnServerSocket: public ServerSocket {
public:
    	UnServerSocket(const char * Address){

		if ((m_Socket = socket(AF_UNIX, SOCK_DGRAM, IPPROTO_UDP))<0)
			throw (SocketException(SocketException::ESE_SOCKCREATE));
		if ((m_pAddr = new UnSocketAddress(Address)) < 0)
			throw (SocketException(SocketException::ESE_SOCKCREATE));
		Bind();
		Listen(2);
	};
};

// InServerSocket - представление серверного сокета семейства //AF_INET
class InServerSocket: public ServerSocket {
public:
	InServerSocket(short PortNum) throw (SocketException){

		if ((m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
			throw (SocketException(SocketException::ESE_SOCKCREATE));
		printf("# Internet Server socket created \n");			

		if ((m_pAddr = new InSocketAddress("AAA", PortNum)) < 0)
			throw (SocketException(SocketException::ESE_SOCKCREATE));
		printf("# Internet Address structure created \n");
		
		Bind();
		Listen(2);
	};
};

}; // конец namespace SockWrap

#endif
