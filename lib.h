#ifndef __SOCK_WRAP_H__
#define __SOCK_WRAP_H__

#include <string>
#include <iostream>
//#include <io.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace SockWrap {

// Exception - класс исключений, генерируемых библиотекой
class Exception {
protected:
    int     m_ErrCode;
public:
                Exception(int errcode) : m_ErrCode(errcode) {}
    void        Report();
    virtual std::string GetMessage() = 0;
};

// SocketExceptiOAon - класс исключений 
class SocketException : public Exception {
    static char * m_Message[];
public:
    enum SocketExceptionCode {
         ESE_SUCCESS,
         ESE_SOCKCREATE,
         ESE_SOCKCONN,
         ESE_SOCKILLEGAL,
         ESE_SOCKHOSTNAME,
         ESE_SOCKSEND,
         ESE_SOCKRECV,
         ESE_SOCKBIND,
         ESE_SOCKLISTEN,
         ESE_SOCKACCEPT,
    };
    SocketException(SocketExceptionCode errcode) : Exception(errcode) {}
    std::string  GetMessage();
};

// SocketAddress - базовый абстрактный класс для представления //сетевых адресов
class SocketAddress {
protected:
    struct sockaddr * m_pAddre; //m_pAddr
public:
                SocketAddress () : m_pAddr(NULL) {}
    virtual     ~SocketAddress () {}
    virtual int GetLength() = 0;
    virtual     SocketAddress * Clone() = 0;
    operator struct sockaddr * (){
	return m_pAddre;
};
};

// UnSocketAddress - представление адреса семейства AF_UNIX
class UnSocketAddress : public SocketAddress {
public:
        UnSocketAddress (const char * SockName);
    int GetLength ();
        SocketAddress * Clone();
        ~UnSocketAddress ();
};

// InSocketAddress - представление адреса семейства AF_INET
class InSocketAddress : public SocketAddress {
public:
        InSocketAddress (const char * HostName, short PortNum){
		m_pAddre = new struct sockaddr_in;
		(*((struct sockaddr_in *)m_pAddre)).sin_family=AF_INET;
		(*((struct sockaddr_in *)m_pAddre)).sin_port=PortNum;
		(*((struct sockaddr_in *)m_pAddre)).sin_addr.s_addr=HostName;
	};
};
    int GetLength (){
	return sizeof(*((struct sockaddr_in *)m_pAddre));
};

        SocketAddress * Clone(){
		InSocketAddress * Copy = new InSocketAddress(*((struct sockaddr_in *)m_pAddre).sin_port,
*((struct sockaddr_in *)m_pAddre).sin_addr.s_addr);
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
	virtual ~BaseSocket();
	
	void Write(void * buf, int len){
		if (send(m_Socket, buf, len, 0)<0)
			throw ("Bad bufer send");
		return 1;
	};
    void            PutChar(int c);
    void            PutString(const char * str);
    void            PutString(const std::string& s)
    int             Read (void * buf, int len);
    int             GetChar();
    std::string     GetString();
    int             GetSockDescriptor();
protected:
    int             m_Socket;
    SocketAddress * m_pAddr;
    void            CheckSocket()
};

// ClientSocket - базовый класс для клиентских сокетов
class ClientSocket: public BaseSocket {
public:
	void Connect(){
		if ((connect(m_Socket, (struct sockaddr *)(* m_pAddr), m_pAddr -> GetLength()))<0)
			throw ("Bad Connection");
		return 1;
	};
};

// ServerSocket - базовый класс для серверных сокетов
class ServerSocket: public BaseSocket {
public:
	BaseSocket * Accept(){
		int soketfd = 0;
	
		if ((socketfd = accept(m_Socket, (struct sockaddr *)(* m_pAddr), m_pAddr->GetLength()))<0)
			throw ("Bad Accept");
		return (new BaseSocket(soketfd, m_pAddr));
	};
protected:

	void Bind(){
		if ((bind(m_Socket, (struct sockaddr *)(*m_pAddr), m_pAddr->GetLength))<0)
			throw ("Bad Bind");
		return 1;	
	};

    	void Listen(int BackLog){
		if ((Listen(m_Socket, Backlog)<0)
			throw ("Bad Listen");	
		return 1;
	};

	virtual void    OnAccept (BaseSocket * pConn) {}
};

// UnClientSocket - представление клиентского сокета семейства //AF_UNIX
class UnClientSocket: public ClientSocket {
public:
    UnClientSocket(const char * Address)
};

// InClientSocket - представление клиентского сокета семейства //AF_INET
class InClientSocket: public ClientSocket {
public:
	InClientSocket(const char * HostName, short PortNum){
	
		if ((m_Socket = socket(AF_INET, SOCK_STREAM, IPPRO_TCP))<0)
			throw ("BadSocket");
		if ((m_pAddr = new InSocketAddress(HostName, PortNum)) < 0)
			throw ("BadAddres");
	};
};

// UnServerSocket - представление серверного сокета семейства //AF_UNIX
class UnServerSocket: public ServerSocket {
public:
    UnServerSocket(const char * Address);
};

// InServerSocket - представление серверного сокета семейства //AF_INET
class InServerSocket: public ServerSocket {
public:
	InServerSocket(short PortNum) throw (SocketException){

		if ((m_Socket = socket(AF_INET, SOCK_STREAM, IPPRO_TCP))<0)
			throw ("BadSocket");
		if ((m_pAddr = new InSocketAddress("192.168.1.13", PortNum)) < 0)
			throw ("BadAddres");
	};
};

}; // конец namespace SockWrap

#endif
