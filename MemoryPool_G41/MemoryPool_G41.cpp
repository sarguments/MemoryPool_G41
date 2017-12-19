#include "stdafx.h"
#include "hoxy_Header.h"

#include <new.h>
#include "MemoryPool_G41.h"

int main()
{
	CCmdStart::CmdDebugText(L"start", true);
	
	// placement false
	CMemoryPool memPool(10);

	// placement new 여부 옵션 의 차이는 적용 여부 차이가 아니라 적용 시기의 차이인가? 
	// 동적할당을 한 다음에 그걸로 생성자로 초기화 하려면 결국 placement new 써야하는거 아닌가?
	// 배열로 만든다고 했는데 (code + next + data) 을 배열 하나 크기로 만드는 건가?
	// 배열로 만들어도 placement new 써야하는거 아닌가?

	/*
		일정 갯수의 노드와 데이터 크기 만큼 메모리를 동적할당 한 후 
		노드를 생성자로 초기화 하고, 데이터는 placement 옵션에 따라 초기화 여부를 결정한다.
		(일단 false 인 경우 생성자로 초기화 한다.)

		각 노드는 싱글 리스트로 연결되어있고 스택의 형태이다.

		<인터페이스>
		Alloc은 Pop위치에 있는 노드를 리턴 한 후 Pop포인터가 리턴되기 전 노드의 다음 노드를
		가리키게 한다.

		Free는 인자로 들어온 포인터의 노드가 현재 Pop위치의 노드를 가리키게 하고 현재 Pop포인터가
		인자로 들어온 포인터의 노드를 가리키게 한다.
	*/


	return 0;
}

CMemoryPool::CMemoryPool(int iBlockNum, bool bPlacementNew)
	: _allocCount(iBlockNum)
{
	// 초기 할당 버퍼 크기 = (노드 + 블럭) * 블럭갯수
	_poolBuf = (char*)malloc((sizeof(st_BLOCK_NODE) + sizeof(st_TEST)) * _allocCount);
}

CMemoryPool::~CMemoryPool()
{
}

st_TEST * CMemoryPool::Alloc(void)
{
	return nullptr;
}

bool CMemoryPool::Free(st_TEST * pData)
{
	return false;
}
