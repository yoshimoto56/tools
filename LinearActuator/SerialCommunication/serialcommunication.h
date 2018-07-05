#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <setupapi.h>
#include <locale.h>
#include <tchar.h>
#include <vector>
#include <iostream>

#pragma comment(lib, "SetupAPI.lib")

#define MAX_BUFFER_LENGTH 1024

class   SerialCommunication {
protected:
	DWORD num_write;
	DWORD num_error;
	DWORD num_read;

	std::string port_name;
	std::vector<std::string> port_list;

	DCB dcb;
	COMMTIMEOUTS cto;
	COMSTAT com_stat;
	HANDLE handle;
	char buffer[MAX_BUFFER_LENGTH];

public:
	SerialCommunication(void);
	SerialCommunication(std::string _port_name, int _baud_rate = 9600);
	~SerialCommunication();

	bool open(std::string _port_name, int _baud_rate = 9600);
	std::vector<std::string> getPortList(void);
	void close(void);
	void initDCB(int _baud_rate);
	void initCTO(void);
	void write(char *_buffer);
	void read(char *_buffer);
	void clear(void);
};