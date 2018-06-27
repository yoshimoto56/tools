#include "serialcommunication.h"

SerialCommunication::SerialCommunication(): handle( NULL )
{
}

SerialCommunication::SerialCommunication( std::string _port_name, int _baud_rate )
{
	handle = NULL;
	open( _port_name );
}

SerialCommunication::~SerialCommunication()
{
	close();
}

bool SerialCommunication::open( std::string _port_name, int _baud_rate )
{
	port_name = _port_name;
	try{
		handle = CreateFile((LPCSTR)port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle == INVALID_HANDLE_VALUE)
			throw std::exception();
	}catch (std::exception e) {
		std::cerr<<"SerialCommunication::open(): cannot open "<<port_name.c_str()<<std::endl;
		return false;
	}
	initDCB(_baud_rate);
	initCTO();
	return true;
}

std::vector<std::string> SerialCommunication::getPortList()
{
	HDEVINFO HDI;
	DWORD index_member = 0;
	DWORD size;
	DWORD type = 0;
	char name[256];
	SP_DEVINFO_DATA data = { sizeof(SP_DEVINFO_DATA) };
	int max = 0;
	size = sizeof(name);

	port_list.clear();
	HDI = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	if (HDI == 0)
		return port_list;
	data.cbSize = sizeof(data);

	while (SetupDiEnumDeviceInfo(HDI, max++, &data)) {
		HKEY key = SetupDiOpenDevRegKey(HDI, &data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
			if (key != INVALID_HANDLE_VALUE) {
				RegQueryValueExA(key, "PortName", NULL, &type, (LPBYTE)name, &size);
				if (strstr(name, "COM") == NULL) {
					goto PORT_LIST;
				}
				for (unsigned int i = 0; i < port_list.size(); i++) {
					if (strstr(port_list[i].c_str(), name)!=NULL) {
						goto PORT_LIST;
					}
			}
			port_list.push_back(name);
			PORT_LIST:
			;
		}
	}
	SetupDiDestroyDeviceInfoList(HDI);

#ifdef _DEBUG
	for (unsigned int i = 0; i < port_list.size(); i++) {
		std::cout << port_list[i].c_str() << std::endl;
	}
#endif
	return port_list;
}

void SerialCommunication::initDCB( int _baud_rate )
{
	dcb.BaudRate = _baud_rate;
	dcb.fParity = FALSE;
	dcb.Parity = NOPARITY;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fTXContinueOnXoff = TRUE;
	dcb.XonLim = 2048;
	dcb.XoffLim = 2048;
	dcb.XonChar = 0x11;
	dcb.XoffChar = 0x13;
	dcb.fBinary = TRUE;
	dcb.fNull = FALSE;
	dcb.fAbortOnError = TRUE;
	dcb.fErrorChar = FALSE;
	dcb.ErrorChar = 0x00;
	dcb.EofChar = 0x03;
	dcb.EvtChar = 0x02;
	dcb.DCBlength = sizeof( DCB );
	dcb.wReserved = 0;
	SetCommState( handle, &dcb );
}

void SerialCommunication::initCTO( void )
{
	cto.ReadIntervalTimeout = 500;
	cto.ReadTotalTimeoutMultiplier = 1;
	cto.ReadTotalTimeoutConstant = 500;
	cto.WriteTotalTimeoutMultiplier = 1;
	cto.WriteTotalTimeoutConstant = 500;
	SetCommTimeouts( handle, &cto );
}

void SerialCommunication::close( void )
{
	if ( handle != NULL ) {
		CloseHandle( handle );
	}
	handle = NULL;
}

void SerialCommunication::write( char *_buffer )
{
	if (handle != INVALID_HANDLE_VALUE && handle !=NULL)
		WriteFile( handle, _buffer, strlen( _buffer ), &num_write, NULL );
}

void SerialCommunication::read( char *_buffer )
{
	unsigned int to_read;
	if (handle != INVALID_HANDLE_VALUE && handle != NULL) {
		memset(buffer, '\0', sizeof(char) * MAX_BUFFER_LENGTH);
		ClearCommError(handle, &num_error, &com_stat);

		if (this->com_stat.cbInQue > 0)
		{
			if (this->com_stat.cbInQue > MAX_BUFFER_LENGTH) {
				to_read = MAX_BUFFER_LENGTH;
			}
			else {
				to_read = com_stat.cbInQue;
			}
			ReadFile(handle, buffer, to_read, &num_read, NULL);
			memcpy(_buffer, buffer, sizeof(char) * MAX_BUFFER_LENGTH);
		}
	}
}

void SerialCommunication::clear( void )
{
	if (handle != INVALID_HANDLE_VALUE && handle != NULL)
		PurgeComm( handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}
