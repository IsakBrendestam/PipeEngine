#pragma once

#include <string>
#include <fstream>

#define FS_READ 0b00000001
#define FS_WRITE 0x00000010
#define FS_BINARAY 0x00100000

class FileSystem
{
public:
	FileSystem(std::string path);
	~FileSystem() = default;

	bool Open(const char flags);
	bool Write(void* data, size_t size);
	bool Read(void* &data, size_t size);

private:
	std::string m_path;
	char m_flags;
	std::ofstream m_stream;
};
