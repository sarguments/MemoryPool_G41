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
	int _useCount;
	int _allocCount;
	st_BLOCK_NODE* _topPtr;
	char* _poolBuf;
	bool _isPlacement;
	bool _isDynamic;

	const __int64 CODE_ALLOC_NEW = 0x2525252525252525;
	const __int64 CODE_ALLOC_NOT_NEW = 0x2525252525252526;
};

// ������. iBlockNum �� 0�̸� ����(��������Ʈ)
template <class DATA>
CMemoryPool<DATA>::CMemoryPool(int iBlockNum, bool bPlacementNew)
	: _useCount(0), _allocCount(iBlockNum), _topPtr(nullptr),
	_poolBuf(nullptr), _isPlacement(bPlacementNew), _isDynamic(false)
{
	if (_allocCount == 0)
	{
		_isDynamic = true;
		return;
	}

	// �Ҵ� �� ���� ����ü[] �� �����ؼ� ��������� �ʱ�ȭ(new �Ⱦ���)
	_poolBuf = (char*)malloc(sizeof(st_BLOCK_NODE) * _allocCount);
	st_BLOCK_NODE* localNodePtr = (st_BLOCK_NODE*)_poolBuf;

	for (int i = 0; i < _allocCount; i++)
	{
		// plcaement �� ��� �ʱ�ȭ X
		if (_isPlacement == true)
		{
			localNodePtr[i]._code = CODE_ALLOC_NOT_NEW;
		}
		else
		{
			localNodePtr[i]._code = CODE_ALLOC_NEW;
			new (&localNodePtr[i]._block) DATA;
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
	if (_isDynamic == true)
	{
		return;
	}

	st_BLOCK_NODE* localNodePtr = (st_BLOCK_NODE*)_poolBuf;

	for (int i = 0; i < _allocCount; i++)
	{
		if (localNodePtr[i]._code == CODE_ALLOC_NEW)
		{
			localNodePtr[i]._block.~DATA();
		}
	}

	free(_poolBuf);
}

template <class DATA>
DATA * CMemoryPool<DATA>::Alloc(void)
{
	// �� �������� ����
	if (GetUseCount() >= _allocCount && _isDynamic == false)
	{
		return nullptr;
	}

	// ���� ����Ʈ�ε� ž�� ���������̸� �޸� �Ҵ��ϰ� ����
	if (_isDynamic == true && _topPtr == nullptr)
	{
		st_BLOCK_NODE* newNode = (st_BLOCK_NODE*)malloc(sizeof(st_BLOCK_NODE));
		new (&newNode->_block) DATA;

		newNode->_code = CODE_ALLOC_NEW;
		newNode->_nextNode = nullptr;
		_topPtr = nullptr;

		++_allocCount;
		++_useCount;

		return &(newNode->_block);
	}
	// �� �ܿ��� �� ��忡 �ִ� �޸� ����
	else
	{
		// ��尡 �ƴ� �� ����
		DATA* toReturnPtr = &(_topPtr->_block);

		// placement true�� ���
		if (_isPlacement == true)
		{
			new (toReturnPtr) DATA;
		}

		// topPtr ���� ����
		_topPtr = _topPtr->_nextNode;

		// ������� ������ ����
		++_useCount;

		// ������Ʈ ��ȯ
		return toReturnPtr;
	}
}

template <class DATA>
bool CMemoryPool<DATA>::Free(DATA * pData)
{
	// �� �����ͷ� ��� ������ ���Ѵ�.
	st_BLOCK_NODE* toFreeNode = (st_BLOCK_NODE*)((char*)pData - (sizeof(__int64) + sizeof(st_BLOCK_NODE*)));

	// �ڵ� üũ(���� ���� ������Ʈ ����)
	if (!(toFreeNode->_code == CODE_ALLOC_NEW || toFreeNode->_code == CODE_ALLOC_NOT_NEW))
	{
		return false;
	}

	// placement new �ɼ� true�̸� Free�Ҷ��� ����� �Ҹ��� ȣ��
	if (_isPlacement == true)
	{
		pData->~DATA();
	}

	// ���� ȸ���Ǵ� ����� next�� ���� topPtr
	toFreeNode->_nextNode = _topPtr;

	// topPtr ����
	_topPtr = toFreeNode;

	// ������� ������ ����
	--_useCount;

	return true;
}
