#include "stdafx.h"

#include "FileSystem.h"

FileSystem::FileSystem(std::string path) :
	m_path(path), m_flags(0)
{
}

bool FileSystem::Open(const char flags)
{
	m_flags = flags;

	if ((m_flags & FS_READ) && (m_flags & FS_WRITE))
		return false;

	// Set mode
	std::ios_base::openmode mode = std::ios::app;
	mode |= m_flags;

	m_stream = std::ofstream(m_path, mode);

	if (!m_stream)
		return false;
}

bool FileSystem::Write(void* data, size_t size)
{
	if (!(m_flags & FS_WRITE))
		return false;

	m_stream.write((char*)data, size);
}

bool FileSystem::Read(void*& data, size_t size)
{
	if (!(m_flags & FS_READ))
		return false;

}
