#include "serialcommunication.h"

int main(int argc, char *argv[]) {
	char buffer_recv[8];
	char buffer_send[8];
	SerialCommunication dev;
	bool is_open;

	dev.getPortList();
	if (argc > 1) {
		is_open = dev.open(argv[1],115200);
	}
	else {
		is_open = dev.open("COM6", 115200);
	}
	while (is_open) {
		for(;;){
			buffer_recv[0] = NULL;
			dev.read(buffer_recv);
			if (buffer_recv[0] == '>') {
				std::cout <<"sgn***###(p or n, speed, stroke)"<< buffer_recv[0];
				break;
			}
		}
		std::cin >> buffer_send;
		dev.write(buffer_send);
		std::cout << "q>";
		std::cin >> buffer_send;
		dev.write(buffer_send);
	}
	dev.close();

	return 0;
}