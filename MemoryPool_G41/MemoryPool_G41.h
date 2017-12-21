#pragma once

/*---------------------------------------------------------------

�޸� Ǯ Ŭ���� (������Ʈ Ǯ)
Ư�� ����Ÿ(����ü,Ŭ����,����)�� ������ �Ҵ� �� ��������.

- ����.

CMemoryPool<DATA> MemPool(300, false);
DATA *pData = MemPool.Alloc();

pData ���

MemPool.Free(pData);

----------------------------------------------------------------*/

template <class DATA>
class CMemoryPool
{
private:
	// ����ü sizeof �� ��� �е����� ���� ��� ��. pragma pack ���ص� ��
	struct st_BLOCK_NODE
	{
		__int64 _code;
		//st_TEST* _nextBlock = nullptr;
		st_BLOCK_NODE* _nextNode;
		DATA _block;
	};

public:
	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Parameters:	(int) �ִ� �� ����.
	//				(bool) ������ ȣ�� ����.
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CMemoryPool(int iBlockNum, bool bPlacementNew = false);
	virtual	~CMemoryPool();

	//////////////////////////////////////////////////////////////////////////
	// �� �ϳ��� �Ҵ�޴´�.
	//
	// Parameters: ����.
	// Return: (DATA *) ����Ÿ �� ������.
	//////////////////////////////////////////////////////////////////////////
	DATA	*Alloc(void);

	//////////////////////////////////////////////////////////////////////////
	// ������̴� ���� �����Ѵ�.
	//
	// Parameters: (DATA *) �� ������.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	bool	Free(DATA *pData);

	//////////////////////////////////////////////////////////////////////////
	// ���� Ȯ�� �� �� ������ ��´�. (�޸�Ǯ ������ ��ü ����)
	//
	// Parameters: ����.
	// Return: (int) �޸� Ǯ ���� ��ü ����
	//////////////////////////////////////////////////////////////////////////
	int		GetAllocCount(void) { return _allocCount; }

	//////////////////////////////////////////////////////////////////////////
	// ���� ������� �� ������ ��´�.
	//
	// Parameters: ����.
	// Return: (int) ������� �� ����.
	//////////////////////////////////////////////////////////////////////////
	int		GetUseCount(void) { return _useCount; }

private:
	// TODO : �ʱ�ȭ?
	int _useCount = 0;
	int _allocCount;
	//st_TEST* _topPtr;
	st_BLOCK_NODE* _topPtr = nullptr;
	char* _poolBuf;
	bool _isPlacement;
	bool _isDynamic = false;
};

// ������. iBlockNum �� 0�̸� ����(��������Ʈ)
template <class DATA>
CMemoryPool<DATA>::CMemoryPool(int iBlockNum, bool bPlacementNew)
	: _allocCount(iBlockNum), _isPlacement(bPlacementNew)
{
	/*
	// �ʱ� �Ҵ� ���� ũ�� = (��� + ��) * ������
	_poolBuf = (char*)malloc((sizeof(st_BLOCK_NODE) + sizeof(st_TEST)) * _allocCount);
	*/
	/*
	for (int i = 0; i < iBlockNum; i++)
	{
	// ��������, �� ������
	char* nowPtr = _poolBuf + i * (sizeof(st_BLOCK_NODE) + sizeof(st_TEST));
	new (nowPtr) st_BLOCK_NODE;
	new (nowPtr + sizeof(st_BLOCK_NODE)) st_TEST;

	// _nextBlock
	//*(char**)(nowPtr + sizeof(__int64)) = nowPtr + (sizeof(st_BLOCK_NODE) * 2 + sizeof(st_TEST));
	st_BLOCK_NODE* localNodePtr = (st_BLOCK_NODE*)(nowPtr + sizeof(__int64));
	st_TEST* nextBlockPtr = (st_TEST*)(nowPtr + (sizeof(st_BLOCK_NODE) * 2 + sizeof(st_TEST)));
	localNodePtr->_nextBlock = nextBlockPtr;
	}

	// ���� �� ������
	_topPtr = (st_TEST*)_poolBuf + sizeof(st_BLOCK_NODE);
	*/

	if (_allocCount == 0)
	{
		// TODO :  ��������Ʈ
		_isDynamic = true;
		return;
	}

	// �Ҵ� �� ���� ����ü[] �� �����ؼ� ��������� �ʱ�ȭ(new �Ⱦ���)
	_poolBuf = (char*)malloc(sizeof(st_BLOCK_NODE) * _allocCount);
	st_BLOCK_NODE* localNodePtr = (st_BLOCK_NODE*)_poolBuf;

	// plcaement �� ��� �ʱ�ȭ X
	if (_isPlacement == true)
	{
		for (int i = 0; i < _allocCount; i++)
		{
			localNodePtr[i]._code = 0x2525252525252525;
			++_useCount;
		}
	}
	else
	{
		for (int i = 0; i < _allocCount; i++)
		{
			localNodePtr[i]._code = 0x2525252525252525;
			new (&localNodePtr[i]._block) DATA;
			++_useCount;
		}
	}

	// �̱� ����Ʈ ����
	for (int i = 0; i < _allocCount - 1; i++)
	{
		// <top>
		// localNodePtr[0] --> localNodePtr[1] --> localNodePtr[2] ...
		localNodePtr[i]._nextNode = &localNodePtr[i + 1];
	}

	// ���� nextNode�� nullptr
	localNodePtr[_allocCount - 1]._nextNode = nullptr;

	// �ʱ� topPtr�� ù ���
	_topPtr = &localNodePtr[0];
}

template <class DATA>
CMemoryPool<DATA>::~CMemoryPool()
{
	// TODO : �� delete�� ��� X?
	free(_poolBuf);
}

template <class DATA>
DATA * CMemoryPool<DATA>::Alloc(void)
{
	if (_isDynamic == true)
	{
		// ��������Ʈ�� �̹� �Ҵ��� �� �߿� ���� �ִ°� ������ ���� ���� ��.. top�� ������ Ȯ��
		if (_topPtr == nullptr)
		{
			// top�� ���̸� ���� ���� �޸� �Ҵ�
			++_allocCount;
			++_useCount;

			char* newMem = (char*)malloc(sizeof(st_BLOCK_NODE));

			st_BLOCK_NODE* newNode = new st_BLOCK_NODE;
			newNode->_code = 0x2525252525252525;
			newNode->_nextNode = nullptr;

			return &newNode->_block;
		}

		// ���� �ƴ� ��� ��������Ʈ�� �ΰ��̻� �ִ��� Ȯ��
		if (_topPtr->_nextNode == nullptr)
		{
			// TODO : ���� �Ҵ� �ߴ� ���� Ȯ��
			if (_topPtr->_code != 0x2525252525252525)
			{
				int* errorPtr = nullptr;
				*errorPtr = 999;
				return nullptr;
			}

			DATA* toReturnPtr = &_topPtr->_block;
			_topPtr = _topPtr->_nextNode;
			++_useCount;

			return toReturnPtr;
		}

		// �ϳ��� �ִ� ���
		// TODO : ���� �Ҵ� �ߴ� ���� Ȯ��
		if (_topPtr->_code != 0x2525252525252525)
		{
			int* errorPtr = nullptr;
			*errorPtr = 999;
			return nullptr;
		}

		DATA* toReturnPtr = &_topPtr->_block;
		_topPtr = nullptr;
		++_useCount;

		return toReturnPtr;
	}

	// TODO : �ڵ� üũ �ϴ°� �³�?
	if (_topPtr->_code != 0x2525252525252525)
	{
		int* errorPtr = nullptr;
		*errorPtr = 999;
		return nullptr;
	}

	// ��尡 �ƴ� �� ����
	DATA* toReturnPtr = &(_topPtr->_block);

	// placement true�� ���
	if (_isPlacement == true)
	{
		new (toReturnPtr) DATA;
	}

	// topPtr ���� ����
	_topPtr = _topPtr->_nextNode;

	// Ǯ ���� ����
	--_useCount;

	// ������Ʈ ��ȯ
	return toReturnPtr;
}

template <class DATA>
bool CMemoryPool<DATA>::Free(DATA * pData)
{
	// �� �����ͷ� ��� ������ ���Ѵ�.
	st_BLOCK_NODE* toFreeNode = (st_BLOCK_NODE*)((char*)pData - (sizeof(__int64) + sizeof(st_BLOCK_NODE*)));

	// �ڵ� üũ(���� ���� ������Ʈ ����)
	if (toFreeNode->_code != 0x2525252525252525)
	{
		return false;
	}

	// ��������Ʈ(�������) Ȯ��
	if (_isDynamic == true)
	{
		// top�� ������ �ƴ���
		if (_topPtr == nullptr)
		{
			_topPtr = toFreeNode;

			// TODO : ���� ����?
			_topPtr->_nextNode = nullptr;
			--_useCount;

			return true;
		}

		// �̹� �ϳ� �̻� ���� ���
		st_BLOCK_NODE* oldTop = _topPtr;
		_topPtr = toFreeNode;
		_topPtr->_nextNode = oldTop;
		--_useCount;

		return true;
	}

	// ���� ȸ���Ǵ� ����� next�� ���� topPtr
	toFreeNode->_nextNode = _topPtr;

	// topPtr ����
	_topPtr = toFreeNode;

	// ������� ������ ����
	--_useCount;

	return true;
}