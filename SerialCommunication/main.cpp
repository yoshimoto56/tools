#include "serialcommunication.h"

int main() {
	char buffer_send[2048] = { "TEST\0" };
	char buffer_receive[2048];
	SerialCommunication dev;
	bool is_open;

	dev.getPortList();
	is_open = dev.open("COM3");
	while ( is_open ) {
		dev.write(buffer_send);
		dev.read(buffer_receive);
		std::cout << "SEND:" << buffer_send << std::endl;
		std::cout << "RECV:" << buffer_receive << std::endl;
	}
	dev.close();

	return 0;
}