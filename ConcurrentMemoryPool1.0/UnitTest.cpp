#pragma once
#include"Common.h"
#include"ConcurrentAlloc.h"
#include"PageCache.h"

void TestSize()
{
	cout << SizeClass::Index(5) << endl;

	cout << SizeClass::Roundup(10) << endl;
}

void Alloc(size_t n)
{
	std::vector<void*> v;
	for (size_t i = 0; i < n; i++)
	{
		v.push_back(ConcurrentAlloc(10));
	}
	for (size_t i = 0; i < n; i++)
	{
		ConcurrentFree(v[i], 10);
		cout << v[i] << endl;
	}
	v.clear();
	cout << endl << endl;
	for (size_t i = 0; i < n; i++)
	{
		v.push_back(ConcurrentAlloc(10));
	}
	for (size_t i = 0; i < n; i++)
	{
		ConcurrentFree(v[i], 10);
		cout << v[i] << endl;
	}
	v.clear();
}


void TestThreadCache()
{
	std::thread t1(Alloc, 1000);
	std::thread t2(Alloc, 1000);
	std::thread t3(Alloc, 1000);
	std::thread t4(Alloc, 1000);

	t1.join();
	t2.join();
	t3.join();
	t4.join();

}

void TestCentralCache()
{
	std::vector<void*> v;
	for (size_t i = 0; i < 8; ++i)
	{
		v.push_back(ConcurrentAlloc(10));
	}

	for (size_t i = 0; i < 8; ++i)
	{
		//ConcurrentFree(v[i], 10);
		cout << v[i] << endl;
	}
}

void TestPageCache()
{
	PageCache::GetInstance()->NewSpan(2);
}

void TestConcurrentAllocFree()
{
	size_t n = 10000;
	std::vector<void*> v;
	for (size_t i = 0; i < n; i++)
	{
		void* ptr = ConcurrentAlloc(12);
		v.push_back(ptr);
		printf("alloc obj:%d->%p\n", i, ptr);
	}
	for (size_t i = 0; i < n; i++)
	{
		ConcurrentFree(v[i], 12);
		printf("free obj:%d->%p\n", i, &v[i]);
	}
	v.clear();
	printf("*************************\n");
	for (size_t i = 0; i < n; i++)
	{
		void* ptr = ConcurrentAlloc(12);
		v.push_back(ptr);
		printf("alloc obj:%d->%p\n", i, ptr);
	}
	for (size_t i = 0; i < n; i++)
	{
		ConcurrentFree(v[i], 12);
		printf("free obj:%d->%p\n", i, v[i]);
	}
	v.clear();
	printf("*************************\n");
}

//int main()
//{
//	//TestSize();
//	//Alloc(10);
//	//TestThreadCache();
//	//TestCentralCache();
//	//TestPageCache();
//	//TestConcurrentAllocFree();
//	system("pause");
//	return 0;
//}