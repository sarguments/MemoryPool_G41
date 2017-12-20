#include "stdafx.h"
#include "hoxy_Header.h"

#include <new.h>
#include "MemoryPool_G41.h"

int main()
{
	CCmdStart::CmdDebugText(L"start", true);

	// placement false
	CMemoryPool memPool(10);

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

	for (int i = 0; i < iBlockNum; i++)
	{
		// 노드생성자, 블럭 생성자
		char* nowPtr = _poolBuf + i * (sizeof(st_BLOCK_NODE) + sizeof(st_TEST));
		new (nowPtr) st_BLOCK_NODE;
		new (nowPtr + sizeof(st_BLOCK_NODE)) st_TEST;

		// _nextBlock
		*(char**)(nowPtr + sizeof(__int64)) = nowPtr + (sizeof(st_BLOCK_NODE) * 2 + sizeof(st_TEST));
	}

	// 실제 블럭 포인터
	_topPtr = (st_TEST*)_poolBuf + sizeof(st_BLOCK_NODE);
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