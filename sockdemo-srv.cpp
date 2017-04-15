#include <iostream>
#include "sock_wrap.h"

using namespace std;
using namespace SockWrap;

#define PORT_NUM 1234  // номер порта процесса-сервера 

class MyServerSocket : public InServerSocket {
public:
    MyServerSocket () : InServerSocket (PORT_NUM) {}
protected:
    void OnAccept (BaseSocket * pConn)
    {
    // установлено соединение с клиентом, читаем сообщение
    cout << "Read from client: " << pConn->GetString() << endl;
    // отправляем ответ
    pConn->PutString("Hello from server.");

    // продолжаем диалог с клиентом, пока в этом есть необходимость
    // ...

    delete pConn;
    }
};

int main(int argc, char* argv[])
{
    try {
      // создаём серверный сокет
      MyServerSocket sock;

      for (;;)
          // слушаем запросы на соединение
          sock.Accept();
    } catch (Exception& e) {
      // ошибка --- выводим текст сообщения на экран
      e.Report();
    }	
    return 0;
}
