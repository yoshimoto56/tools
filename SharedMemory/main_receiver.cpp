#include "sharedmemory.h"

void main()
{
	mem_int.open(key_int, num_int);
	mem_vec3d.open(key_vec3d);

	int i = 0;
	while(true){
		std::cout << "-------------" << std::endl;
		for(int j=0;j<num_int;j++)
			std::cout << mem_int.getMemory()[j] << std::endl;
		std::cout << mem_vec3d.getMemory()[0].x << std::endl;
		std::cout << mem_vec3d.getMemory()[0].y << std::endl;
		std::cout << mem_vec3d.getMemory()[0].z << std::endl;

		Sleep(1000);
		i++;
	}

	mem_int.close();
	mem_vec3d.close();
}