#ifndef LinearAllocator_h__
#define LinearAllocator_h__

/// \brief lienar allocator allocate sequential objects, object cannot be destoied arbitrairly
class LinearAllocator
{
public:

	//\ brief allocate a linear memory pool with the given size
	LinearAllocator(int isize);

	/// \brief detroy
	~LinearAllocator();

	/// \brief release the given quantity of memoty
	void* AllocateBlock(int size);

	/// \brief return the base address
	void* GetBase();

	/// \brief reset this pool
	void    Flush();

private:

	char	*m_pBuffer;

	char	*m_pNextBlock;

	unsigned int	 m_bufferSize;

	unsigned int	 m_usedMemory;

	unsigned int	 m_freeMemory;


};


#endif // LinearAllocator_h__
