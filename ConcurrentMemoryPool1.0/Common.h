#pragma once

#include <iostream>
#include <thread>
#include <vector>

using std::cout;
using std::endl;

const size_t MAX_BYTES = 64 * 1024;
const size_t NLISTS = MAX_BYTES / 8;


inline static void*& NEXT_OBJ(void* obj)
{
	return *((void**)obj);
}

class FreeList
{
public:
	void Push(void* obj)
	{
		NEXT_OBJ(obj) = _list;
		_list = obj;
		++_size;
	}

	void* Pop()
	{
		void* obj = _list;
		_list = NEXT_OBJ(obj);
		--_size;
		return obj;
	}

	bool Empty()
	{
		return _list == nullptr;
	}
private:
	void* _list = nullptr;
	size_t _size = 0;
};

class SizeClass
{
	inline static size_t _Index(size_t size, size_t align)
	{
		// 9-16 -> 16-23
		size_t alignnum = 1 << align;
		return ((size + alignnum-1) >> align) - 1;
	}

	inline static size_t _Roundup(size_t size, size_t align)
	{
		size_t alignnum = 1 << align;
		return (size + alignnum - 1)&~(alignnum - 1);
	}

public:
	inline static size_t Index(size_t size)
	{
		return _Index(size, 3);
	}

	inline static size_t Roundup(size_t size)
	{
		return _Roundup(size, 3);
	}
};