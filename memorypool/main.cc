#include <iostream>
#include "m_memory_pool.hpp"

class MyObject
{
public:
	MyObject(double x): data(x)
	{
		//std::cout << "contruct object" << std::endl;
	}

	~MyObject()
	{
		//std::cout << "destruct object" << std::endl;
	}

	double data;

	// override new and delete to use memory pool
	void* operator new(size_t size);
	void operator delete(void* p);
	void* operator new[](size_t size);
	void operator delete[](void* p);
};

// define memory pool with block size as class size
MemoryPool<sizeof(MyObject), 10> gMemPool;


void* MyObject::operator new(size_t size)
{
	//std::cout << "new object space" << std::endl;
	return gMemPool.allocate();
}

void MyObject::operator delete(void* p)
{
	//std::cout << "free object space" << std::endl;
	gMemPool.deallocate(p);
}

void* MyObject::operator new[](size_t size)
{
	// TODO: not supported continuous memoery pool for now
	//return gMemPool.allocate(size);
	return NULL;
}
void MyObject::operator delete[](void* p)
{
	// TODO: not supported continuous memoery pool for now
	//gMemPool.deallocate(p);
}

int main(int argc, char* argv[])
{
	MyObject* p1 = new MyObject(1);
	std::cout << "p1 " << p1 << " " << p1->data << " " << sizeof(*p1) <<std::endl;

	MyObject* p2 = new MyObject(2);
	std::cout << "p2 " << p2 << " " << p2->data << " " << sizeof(*p2) << std::endl;

	MyObject* p3 = new MyObject(3);
	std::cout << "p3 " << p3 << " " << p3->data << " " << sizeof(*p3) << std::endl;

	MyObject* p4 = new MyObject(4);
	std::cout << "p4 " << p4 << " " << p4->data << " " << sizeof(*p4) << std::endl;

	MyObject* p5 = new MyObject(5);
	std::cout << "p5 " << p5 << " " << p5->data << std::endl;

	MyObject* p6 = new MyObject(6);
	std::cout << "p6 " << p6 << " " << p6->data << std::endl;

	//delete p1;
	delete p2;
	delete p3;
	delete p4;
	delete p5;
    delete p6;

	getchar();
	return 0;
}