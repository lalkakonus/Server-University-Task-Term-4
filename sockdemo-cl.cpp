#include <iostream>
#include "sock_wrap.h" 

using namespace std;
using namespace SockWrap;

#define PORT_NUM 1234	     // номер порта процесса-сервера 

//В этом примере клиент и сервер выполняются на одном компьютере,
//но программа легко обобщается на случай разных компьютеров. Для
//этого можно, например, использовать сетевое имя не собственного
//компьютера, как ниже, а имя компьютера, на котором выполняется
//процесс-сервер

int main(int argc, char* argv[])
{
    try {
      char host[64];

    // запрос сетевого имени собственной ЭВМ
    if (gethostname(host, sizeof host) < 0) {
        // ошибка --- досрочно завершаем выполнение
        cerr << GetLastError();
        perror("Host name");
        exit (-1);
      }

    // создаём сокет 
    InClientSocket sock(host, PORT_NUM);

    // устанавливаем соединение 
    sock.Connect();

    // отправляем серверу строку
    sock.PutString("Hello from client!");

    // печатаем на экран ответ от сервера
     cout << "Read from server: " << sock.GetString() << endl;

    // продолжаем диалог с сервером, пока в этом есть необходимость
    // ...

    } catch (Exception& e) {
        // ошибка --- выводим текст сообщения на экран
        e.Report();
    }
    return 0;
}

