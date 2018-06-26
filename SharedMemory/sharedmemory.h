#include <windows.h>
#include <iostream>

template <typename T>
class SharedMemory {
	T *memory;
	HANDLE handle;
public:
	SharedMemory() {}
	~SharedMemory() {}
	void open(std::string _key, unsigned int _num = 1)
	{
		handle = CreateFileMapping(NULL, NULL, PAGE_READWRITE, NULL, sizeof(T)*_num, _key.c_str());
		memory = (T*)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, NULL, NULL, sizeof(T)*_num);
	}
	void close() 
	{
		UnmapViewOfFile(memory);
		CloseHandle(handle);
	}
	T* getMemory()
	{
		return memory;
	}
};

struct Vec3d_TEST{
	double x, y, z;
};

int num_int = 5;
SharedMemory<int> mem_int;
std::string key_int = "key_int";
SharedMemory<Vec3d_TEST> mem_vec3d;
std::string key_vec3d = "key_vec3d";