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

// SocketException - класс исключений 
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
    struct sockaddr * m_pAddr;
public:
                SocketAddress () : m_pAddr(NULL) {}
    virtual     ~SocketAddress () {}
    virtual int GetLength() = 0;
    virtual     SocketAddress * Clone() = 0;
    operator struct sockaddr * (){};
/*Это я сам вписал!!!*/
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
		m_pAddr = new struct sockaddr_in;
		*m_pAddr.sin_family=AF_INET;
		*m_pAddr.sin_port=PortNum;
		*m_pAddr.sin_addr.s_addr=HostName;
	};
};
    int GetLength (){
	return sizeof(*m_pAddr);
};

        SocketAddress * Clone(){
		InSocketAddres * Copy = new InSocketAddres(*m_pAddr.sin_port, *m_pAddr.sin_addr.s_addr);
	return Copy;
};
        
	~InSocketAddress (){
		delete m_pAddr;
};

};

// BaseSocket - базовый класс для сокетов
class BaseSocket {
public:
 explicit BaseSocket(int sd = -1, SocketAddress * pAddr = NULL):
                        m_Socket(sd), m_pAddr(pAddr) {}
    virtual         ~BaseSocket();
    void            Write(void * buf, int len);
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
    void            Connect();
};

// ServerSocket - базовый класс для серверных сокетов
class ServerSocket: public BaseSocket {
public:
    BaseSocket *    Accept();
protected:
    void Bind(){
	if ((bind(m_Socket,))<0)
		throw ("Bad Bind");	
};
    void            Listen(int BackLog);
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
		m_Socket = socket(AF_INET, SOCK_STREAM, IPPRO_TCP);
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
		*m_pAddr = new InSocketAddress("192.168.1.13", 1234);
		
};
};

}; // конец namespace SockWrap

#endif
