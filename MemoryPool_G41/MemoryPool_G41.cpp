// MemoryPool_G41.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "hoxy_Header.h"

int main()
{
	CCmdStart::CmdDebugText(L"start", true);
	// 

	/*
	정적... 미리 메모리만 확보해놓고 placement new 로 생성자만 호출..

	링크드 리스트 기반의 스택으로 포인터 관리
	구분코드/next/데이터, 포인터 연산으로 구분코드 체크

	탑이 널?

	초기 할당 크기 넘으면 새로 할당 받게
	*/

	//int test = -5 + (-1 & 10);
	int test1 = (7 - 2);
	int test2 = -((int)(7 <= 2));
	int test3 = test2 & 10;
	int test4 = test1 + test3;

	wcout << test1 << endl;
	wcout << test2 << endl;
	wcout << test3 << endl;
	wcout << test4 << endl;

    return 0;
}

