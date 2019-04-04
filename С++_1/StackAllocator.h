#pragma once
#include <iostream>
#include <cstdint>
#include <memory>

const int BlockSize = 1e7;
 
class memoryBlock{

public:
	typedef std::size_t size_type;
	typedef uint8_t* ptr;

	std::shared_ptr<memoryBlock> previousBlock;
	size_type offsetBlock;
	ptr buffer;


	memoryBlock():
		previousBlock(nullptr), buffer(nullptr), offsetBlock(0) {};
	~memoryBlock() = default;
	
};


template < class T >
class StackAllocator {

public:
	std::shared_ptr<memoryBlock> finishBlocks;

	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef uint8_t* ptr;
	typedef uint8_t var;
	typedef int count_type;
	
	template < class U >
	struct rebind {
		typedef StackAllocator<U> other;
	};

	StackAllocator() :
		finishBlocks(nullptr) {};
	
	template < class U >
	StackAllocator(const StackAllocator<U>& other):
		finishBlocks(other.finishBlocks) {};

	pointer allocate(size_type n);
	void deallocate(pointer p, size_type n);

	~StackAllocator() = default;
};

template < class T >
T* StackAllocator<T>::allocate(size_type n) {

	count_type cnt_byte = n * sizeof(value_type);

	if (finishBlocks == nullptr) {
		std::shared_ptr<memoryBlock> newBlock (new memoryBlock);
		newBlock.get()->buffer = new var[BlockSize];
		finishBlocks = newBlock;
		finishBlocks.get()->offsetBlock = cnt_byte;
		return reinterpret_cast<pointer>(finishBlocks.get()->buffer);
	}

	count_type temp = BlockSize - finishBlocks.get()->offsetBlock;

	if (cnt_byte > temp) {
		std::shared_ptr<memoryBlock> newBlock(new memoryBlock);
		newBlock.get()->buffer = new var[BlockSize];
		newBlock.get()->previousBlock = finishBlocks;
		newBlock.get()->offsetBlock = cnt_byte;
		finishBlocks = newBlock;
		finishBlocks.get()->previousBlock.get()->offsetBlock = BlockSize;
		return reinterpret_cast<pointer> (finishBlocks.get()->buffer);
	}

	count_type begin = finishBlocks.get()->offsetBlock;
	finishBlocks.get()->offsetBlock += cnt_byte;
	return reinterpret_cast<pointer>(finishBlocks.get()->buffer + begin);
};

template < class T > 
void StackAllocator<T>::deallocate(pointer p, size_type n) {}
