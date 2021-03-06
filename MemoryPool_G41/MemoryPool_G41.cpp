#include "stdafx.h"
#include "hoxy_Header.h"

#include <new.h>
#include "MemoryPool_G41.h"
//#include "ProfilerNegev.h"

/* ///////////////////////////////////////////////////////////////////////////////////////////

일정 갯수의 노드와 데이터 크기 만큼 메모리를 동적할당 한 후
노드를 생성자로 초기화 하고, 데이터는 placement 옵션에 따라 초기화 여부를 결정한다.
크기가 0인 경우 프리리스트 모드 이다.

각 노드는 싱글 리스트로 연결되어있고 스택의 형태이다.

<인터페이스>
Alloc은 top위치에 있는 노드를 리턴 한 후 top포인터가 리턴되기 전 노드의 다음 노드를
가리키게 한다.
프리리스트인 경우 새로 동적할당 한 후 바로 리턴한다.

Free는 인자로 들어온 노드의 next가 기존의 top위치에 있는 노드를 가리키고
인자로 들어온 노드가 top이 된다.
프리리스트인 경우 기존에 들어온게 없으면 top은 현재 반환한 노드가 된다.
기존에 들어온게 있으면 기존 top과 연결한다.

*/ ///////////////////////////////////////////////////////////////////////////////////////////

// TODO : 프리리스트에서 할당한 것 중에 누수되는 메모리?

struct st_TEST
{
	st_TEST()
	{
		_test1 = 123;
		_test2 = 789;
	}

	~st_TEST()
	{
		_test1 = 0;
		_test2 = 0;
	}

	int _test1;
	short _test2;
};

int main()
{
	CMemoryPool<st_TEST> memPool(0, true);

	/*
	P_BEGIN(L"pool");
	for (int i = 0; i < 100; i++)
	{
		st_TEST* testPtr222 = memPool.Alloc();
		memPool.Free(testPtr222);
	}
	P_END(L"pool");

	P_BEGIN(L"new");
	for (int i = 0; i < 100; i++)
	{
		st_TEST* testPtr222 = new st_TEST;
		delete testPtr222;
	}
	P_END(L"new");

	P_SAVE();
	*/

	st_TEST* testPtr1 = memPool.Alloc();
	std::wcout << testPtr1->_test1 << endl;
	std::wcout << testPtr1->_test2 << endl;

	st_TEST* testPtr2 = memPool.Alloc();
	std::wcout << testPtr2->_test1 << endl;
	std::wcout << testPtr2->_test2 << endl;

	st_TEST* testPtr3 = memPool.Alloc();
	std::wcout << testPtr3->_test1 << endl;
	std::wcout << testPtr3->_test2 << endl;

	memPool.Free(testPtr1);
	memPool.Free(testPtr2);
	memPool.Free(testPtr3);

	//testPtr1 = memPool.Alloc();
	//std::wcout << testPtr1->_test1 << endl;
	//std::wcout << testPtr1->_test2 << endl;

	//testPtr2 = memPool.Alloc();
	//std::wcout << testPtr2->_test1 << endl;
	//std::wcout << testPtr2->_test2 << endl;

	//memPool.Free(testPtr1);
	//memPool.Free(testPtr2);

	//testPtr3 = memPool.Alloc();
	//std::wcout << testPtr3->_test1 << endl;
	//std::wcout << testPtr3->_test2 << endl;

	//memPool.Free(testPtr3);

	//testPtr1 = memPool.Alloc();
	//std::wcout << testPtr1->_test1 << endl;
	//std::wcout << testPtr1->_test2 << endl;

	//testPtr2 = memPool.Alloc();
	//std::wcout << testPtr2->_test1 << endl;
	//std::wcout << testPtr2->_test2 << endl;

	//testPtr3 = memPool.Alloc();
	//std::wcout << testPtr3->_test1 << endl;
	//std::wcout << testPtr3->_test2 << endl;

	//testPtr1 = memPool.Alloc();
	//std::wcout << testPtr1->_test1 << endl;
	//std::wcout << testPtr1->_test2 << endl;

	//testPtr2 = memPool.Alloc();
	//std::wcout << testPtr2->_test1 << endl;
	//std::wcout << testPtr2->_test2 << endl;

	//testPtr3 = memPool.Alloc();
	//std::wcout << testPtr3->_test1 << endl;
	//std::wcout << testPtr3->_test2 << endl;

	//testPtr1 = memPool.Alloc();
	//std::wcout << testPtr1->_test1 << endl;
	//std::wcout << testPtr1->_test2 << endl;

	//testPtr2 = memPool.Alloc();
	//std::wcout << testPtr2->_test1 << endl;
	//std::wcout << testPtr2->_test2 << endl;

	//testPtr3 = memPool.Alloc();
	//std::wcout << testPtr3->_test1 << endl;
	//std::wcout << testPtr3->_test2 << endl;

	return 0;
}