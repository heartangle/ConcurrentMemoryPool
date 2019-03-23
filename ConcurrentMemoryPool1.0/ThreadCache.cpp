#include "ThreadCache.h"

// 从中心缓存获取对象
void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	//return malloc(size);
}

// 申请和释放内存对象
void* ThreadCache::Allocate(size_t size)
{
	size_t index = SizeClass::Index(size);
	FreeList* freelist = &_freeList[index];
	if (!freelist->Empty())
	{
		return freelist->Pop();
	}
	else
	{
		return FetchFromCentralCache(index, SizeClass::Roundup(size));
	}
}

void ThreadCache::Deallocate(void* ptr, size_t size)
{
	size_t index = SizeClass::Index(size);
	FreeList* freelist = &_freeList[index];
	freelist->Push(ptr);

	// 满足条件，释放回中心缓存
}

// 释放对象时，链表过长时，回收内存回到中心堆
void ThreadCache::ListTooLong(FreeList* list, size_t size)
{

}