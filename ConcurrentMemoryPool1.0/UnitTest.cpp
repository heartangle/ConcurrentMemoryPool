#include "Common.h"
#include "ConcurrentAlloc.h"

void TestSize()
{
	cout << SizeClass::Index(10) << endl;
	cout << SizeClass::Index(16) << endl;

	cout << SizeClass::Roundup(10) << endl;
	cout << SizeClass::Roundup(12) << endl;
	cout << SizeClass::Roundup(16) << endl;
	cout << SizeClass::Roundup(17) << endl;
}

void Alloc(size_t n)
{
	std::vector<void*> v;
	for (size_t i = 0; i < n; ++i)
	{
		v.push_back(ConcurrentAlloc(10));
	}

	for (size_t i = 0; i < n; ++i)
	{
		ConcurrentFree(v[i], 10);
		cout << v[i] << endl;
	}
	v.clear();

	cout << endl << endl;
	for (size_t i = 0; i < n; ++i)
	{
		v.push_back(ConcurrentAlloc(10));
	}

	for (size_t i = 0; i < n; ++i)
	{
		ConcurrentFree(v[i], 10);
		cout << v[i] << endl;
	}
	v.clear();
}

void TestThreadCache()
{
	std::thread t1(Alloc, 5);
	//std::thread t2(Alloc, 5);
	//std::thread t3(Alloc, 5);
	//std::thread t4(Alloc, 5);

	t1.join();
	//t2.join();
	//t3.join();
	//t4.join();

}

int main()
{
	TestThreadCache();

	return 0;
}