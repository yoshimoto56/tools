#include "sharedmemory.h"

void main()
{
	mem_int.open(key_int, num_int);
	mem_vec3d.open(key_vec3d);

	int i = 0;
	while (true) {
		for (int j = 0;j<num_int;j++)
			mem_int.getMemory()[j] = i * j;
		mem_vec3d.getMemory()[0].x = i * 2;
		mem_vec3d.getMemory()[0].y = i * 3;
		mem_vec3d.getMemory()[0].z = i * 4;

		std::cout << "-------------" << std::endl;
		for (int j = 0;j<num_int;j++)
			std::cout << i * j << std::endl;
		std::cout << i * 2 << std::endl;
		std::cout << i * 3 << std::endl;
		std::cout << i * 4 << std::endl;

		Sleep(1000);
		i++;
	}

	mem_int.close();
	mem_vec3d.close();
}