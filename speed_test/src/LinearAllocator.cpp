#include "LinearAllocator.h"
#include "common.h"
#include <new>
LinearAllocator::LinearAllocator(int isize)
{
	KASSERT(isize>0, "error\n");
	m_pBuffer = new char[isize];
	KASSERT(m_pBuffer,"error");
	m_bufferSize = isize;
	m_usedMemory = 0;
	m_freeMemory = isize;
	m_pNextBlock = m_pBuffer;
}


LinearAllocator::~LinearAllocator()
{
	delete[] m_pBuffer;
}

void* LinearAllocator::GetBase()
{
	return m_pBuffer;
}

void* LinearAllocator::AllocateBlock(int size)
{
	KASSERT((unsigned int)size < m_freeMemory, "error");
	char* pNextBlock = m_pNextBlock ;
	m_pNextBlock = pNextBlock + size;
	m_usedMemory += size;
	return pNextBlock;
}

void LinearAllocator::Flush()
{
	m_pNextBlock = m_pBuffer;
	m_usedMemory = 0;
	m_freeMemory = m_bufferSize;
}
