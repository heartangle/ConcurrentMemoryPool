#include"ThreadCache.h"
#include"CentralCache.h"

//�����Ļ����ȡ����
void* ThreadCache::GetFromCentralCache(size_t index, size_t size)
{
	FreeList* freelist = &_freeList[index];
	size_t maxsize = freelist->MaxSize();
	size_t numtomove = min(SizeClass::NumMoveSize(size), maxsize);

	void* start = nullptr, *end = nullptr;
	size_t batchsize = CentralCache::GetInstance()->FetchRangeObj(start, end, numtomove, size);
	if (batchsize > 1)
	{
		freelist->PushRange(NEXT_OBJ(start), end, batchsize - 1);
	}
	if (batchsize >= freelist->MaxSize())
	{
		freelist->SetMaxSize(maxsize + 1);
	}
	return start;
}
//������ͷŶ���
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
		return GetFromCentralCache(index, SizeClass::Roundup(size));
	}
}

void ThreadCache::Deallocate(void* ptr, size_t size)
{
	size_t index = SizeClass::Index(size);
	FreeList* freelist = &_freeList[index];
	freelist->Push(ptr);
	//��������(�ͷ�һ�������Ķ���)���ͷŻ����Ļ���
	if (freelist->Size() >= freelist->MaxSize())
	{
		ListTooLong(freelist, size);
	}
}

//�ͷŶ���ʱ���������ʱ�������ڴ浽���Ķ�
void ThreadCache::ListTooLong(FreeList* freelist, size_t size)
{
	void* start = freelist->PopRange();
	CentralCache::GetInstance()->ReleaseListToSpans(start, size);
}