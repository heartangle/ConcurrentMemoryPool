#include"PageCache.h"

PageCache PageCache::_inst;

Span* PageCache::NewSpan(size_t n)
{
	assert(n < NPAGES);
	if (!_spanlist[n].Empty())
	{
		return _spanlist[n].PopFront();
	}
	for (size_t i = n + 1; i < NPAGES; i++)
	{
		if (!_spanlist[i].Empty())
		{
			Span* span = _spanlist[i].PopFront();
			Span* split = new Span;

			split->_pageid = span->_pageid;
			split->_npage = n;
			span->_pageid = span->_pageid + n;
			span->_npage = span->_npage - n;
			for (size_t i = 0; i < n; i++)
			{
				_idspanmap[span->_pageid + i] = split;
			}
			_spanlist[span->_npage].PushFront(span);
			return split;
		}
	}
	Span* span = new Span;
	void* ptr = VirtualAlloc(0, (NPAGES - 1)*(1 << PAGE_SHIFT), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	span->_pageid = (PageID)ptr >> PAGE_SHIFT;
	span->_npage = NPAGES - 1;
	for (size_t i = 0; i < span->_npage; ++i)
	{
		_idspanmap[span->_pageid + i] = span;
	}
	_spanlist[span->_npage].PushFront(span);
	return NewSpan(n);
}

//获取从对象到span的映射
Span* PageCache::MapObjectToSapn(void* obj)
{
	PageID id = (PageID)obj >> PAGE_SHIFT;
	auto it = _idspanmap.find(id);
	if (it != _idspanmap.end())
	{
		return it->second;
	}
	else
	{
		assert(false);
		return nullptr;
	}
}

void PageCache::ReleaseSpanToPageCache(Span* cur)
{
	//向前合并
	while (1)
	{
		//超过128页，就不合并
		if (cur->_npage >= NPAGES - 1)
			break;
		PageID curid = cur->_pageid;
		PageID previd = curid - 1;
		auto it = _idspanmap.find(previd);
		//没有找到
		if (it == _idspanmap.end())
			break;
		//前一个span不空闲
		if (it->second->_usecount != 0)
			break;
		Span* prev = it->second;
		//先把prev从链表中移除
		_spanlist[prev->_npage].Erase(prev);
		//合并
		prev->_npage += cur->_npage;
		for (size_t i = 0; i < cur->_npage; i++)
		{
			_idspanmap[cur->_pageid + i] = prev;
		}
		delete cur;
		cur = prev;
	}
	//向后合并
	while (1)
	{

		//超过128页，就不合并
		if (cur->_npage >= NPAGES - 1)
			break;
		PageID  curid = cur->_pageid;
		PageID nextid = curid + cur->_npage;
		std::map<PageID, Span*>::iterator it = _idspanmap.find(nextid);
		//没找到
		if (it == _idspanmap.end())
			break;
		//后一个span不空闲
		if (it->second->_usecount != 0)
			break;
		//合并
		Span* next = it->second;
		_spanlist[next->_npage].Erase(next);
		//修正id->span的映射关系
		for (size_t i = 0; i < next->_npage; i++)
		{
			_idspanmap[next->_pageid + i] = cur;
		}
		cur->_npage += next->_npage;
		delete next;
	}
	cur->_list = nullptr;
	cur->_objsize = 0;

	_spanlist[cur->_npage].PushFront(cur);
}