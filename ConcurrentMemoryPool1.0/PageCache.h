#include"Common.h"

class PageCache
{
public:
	static PageCache* GetInstance()
	{
		return &_inst;
	}
	Span* NewSpan(size_t n);
	//获取从对象到span的映射
	Span* MapObjectToSapn(void* obj);
	//释放空闲span回到page cache,并合并相邻的span
	void ReleaseSpanToPageCache(Span* cur);
private:
	SpanList _spanlist[NPAGES];
	std::map<PageID, Span*> _idspanmap;
private:
	PageCache()
	{}
	PageCache(const PageCache&) = delete;
	static PageCache _inst;
};