#include "handle.h"
#include "lib.h"
#include <sys/wait.h>

using namespace std;

int mkline(int LineSize=100, char ch='*'){
	cout << "\n";
	for (int j=0; j < LineSize; j++) cout << ch ;
	cout << "\n";
	return 0;
};

int main(int argc, char* argv[])
{
	int pid,keep=1, status;
	char Answer='Y';

	mkline();
	

	try {
      		MyServerSocket sock;
		ReqHandle handler;
		BaseSocket * pConn = NULL;

		cout << "Accept new connection? (Y/N)" << endl;
		cin >> Answer;
		(Answer=='Y' || Answer=='y') ? keep=1 : keep=0;


		while (keep){
			pConn = sock.Accept();
			switch (pid=fork()){
//			case -1:
//				delete pConn;
//				throw (SocketException(SocketException::ESE_FORK));
			case 0:
				handler.ParseRequest(pConn);
				cout << "\n--RESPONSE--\n" << endl;
				handler.MakeResponse(pConn);	
		
				delete pConn;
				return 0;
			default:
				wait(&status);
				delete pConn;
				cout << "Accept new connection? (Y/N)" << endl;
				cin >> Answer;
				(Answer=='Y' || Answer=='y') ? keep=1 : keep=0;
			};
		};
   	}
	catch (Exception& e){
		e.Report();
	};	

	mkline();

    return 0;
}
