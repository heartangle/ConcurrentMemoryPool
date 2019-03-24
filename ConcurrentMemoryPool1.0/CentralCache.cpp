#include"CentralCache.h"
#include"PageCache.h"

CentralCache CentralCache::_inst;
Span* CentralCache::GetOneSpan(SpanList& spanlist, size_t byte_size)
{
	Span* span = spanlist.Begin();
	while (span != spanlist.End())
	{
		if (span->_list != nullptr)
			return span;
		else
			span = span->_next;
	}
	Span* newspan = PageCache::GetInstance()->NewSpan(SizeClass::NumMovePage(byte_size));
	//将span页分成需要的对象并连接起来
	char* cur = (char*)(newspan->_pageid << PAGE_SHIFT);
	char* end = cur + (newspan->_npage << PAGE_SHIFT);
	newspan->_list = cur;
	newspan->_objsize = byte_size;
	while (cur + byte_size < end)
	{
		char* next = cur + byte_size;
		NEXT_OBJ(cur) = next;
		cur = next;
	}
	NEXT_OBJ(cur) = nullptr;
	spanlist.PushFront(newspan);
	return newspan;
}

size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t n, size_t byte_size)
{
	size_t index = SizeClass::Index(byte_size);
	SpanList& spanlist = _spanlist[index];
	
	//加锁
	spanlist.Lock();
	Span* span = GetOneSpan(spanlist, byte_size);

	//从span中获取对象
	size_t batchsize = 0;
	void* prev = nullptr;
	void* cur = span->_list;
	for (size_t i = 0; i < n; i++)
	{
		prev = cur;
		cur = NEXT_OBJ(cur);
		++batchsize;
		if (cur == nullptr)
			break;
	}
	start = span->_list;
	end = prev;
	span->_list = cur;
	span->_usecount += batchsize;
	//将空的span移动到最后,保持非空的span在前面
	if (span->_list == nullptr)
	{
		spanlist.Erase(span);
		spanlist.PushBack(span);
	}
	spanlist.Unlock();
	return batchsize;
}

void CentralCache::ReleaseListToSpans(void* start, size_t size)
{
	size_t index = SizeClass::Index(size);
	SpanList& spanlist = _spanlist[index];
	while (start)
	{
		void* next = NEXT_OBJ(start);
		//加锁
		spanlist.Lock();

		Span* span = PageCache::GetInstance()->MapObjectToSapn(start);
		NEXT_OBJ(start) = span->_list;
		//当一个span的对象全部回来，将span还给page cache,并且坐页合并
		if (--span->_usecount == 0)
		{
			PageCache::GetInstance()->ReleaseSpanToPageCache(span);
			spanlist.Erase(span);
		}

		spanlist.Unlock();
		start = next;
	}
}