#include "ThreadCache.h"

// �����Ļ����ȡ����
void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	//return malloc(size);
}

// ������ͷ��ڴ����
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

	// �����������ͷŻ����Ļ���
}

// �ͷŶ���ʱ���������ʱ�������ڴ�ص����Ķ�
void ThreadCache::ListTooLong(FreeList* list, size_t size)
{

}