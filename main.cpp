/*
 * main.cpp
 *
 *  Created on: Mar 4, 2010
 *      Author: chmzc
 *      https://forum.arduino.cc/index.php/topic,28167.0.html
 */


#include "stdio.h"
#include <windows.h>
#include <iostream>

using namespace std;

void PrintError( LPCSTR str)
{
	LPVOID lpMessageBuffer;
	int error = GetLastError();
	FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM,
	NULL,
	error,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
	(LPTSTR) &lpMessageBuffer,
	0,
	NULL
	);
	printf("%s: (%d) %s\n\n",str,error,lpMessageBuffer);
	LocalFree( lpMessageBuffer );
}

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	cout << "Main entry\n";
	cout.flush();
	// open port for I/O
	HANDLE h = CreateFile("com7",
	GENERIC_READ|GENERIC_WRITE,
	0,NULL,
	OPEN_EXISTING,0,NULL); // my adruino on com4 but this value can be read from argv

	if(h == INVALID_HANDLE_VALUE)
	{
		PrintError("E012_Failed to open port");
	}
	else
	{
		// set timeouts
		COMMTIMEOUTS cto = { 1, 100, 1000, 0, 0 };
		DCB dcb;
		if(!SetCommTimeouts(h,&cto))
		PrintError("E013_SetCommTimeouts failed");

		// set DCB
		memset(&dcb,0,sizeof(dcb));
		dcb.DCBlength = sizeof(dcb);
		dcb.BaudRate = 115200;
		dcb.fBinary = 1;
		dcb.fDtrControl = DTR_CONTROL_DISABLE; // avoid reset of arduino board
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fOutxCtsFlow = 0;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.ByteSize = 8;

		if(!SetCommState(h,&dcb))
		PrintError("E014_SetCommState failed");

		char buf[8] = "";


		DWORD read = 0;
		cout << "before loop\n";
		cout.flush();
		while (ReadFile(h,buf,1,&read,NULL))
		{
			if (1 == read)
			{
				cout << (unsigned char)buf[0];
				cout.flush();
			}
		}

		cout << "Enter upto 7 characters to send : ";
		cin.getline(buf, 8);

		DWORD write = 8; // Number of bytes to write to serial port

		WriteFile(h,buf,write,&write,NULL); // write is updated with the number of bytes written

		DWORD i;
		for (i=0; i<write; i++)
		{
			cout << buf[i]; // repeat written chars back in console
		}

		CloseHandle(h);
	}
	return 0;
}


