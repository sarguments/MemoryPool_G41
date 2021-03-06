﻿#include "stdafx.h"
#include "ProfilerNegev.h"

ProfileSt g_proArr[STRUCT_NUM]; // 저장용 전역 배열
double g_freq;

class CInit
{
public:
	CInit()
	{
		for (int i = 0; i < STRUCT_NUM; i++)
		{
			memset(g_proArr[i]._name, 0x00, sizeof(WCHAR) * TEXT_SIZE);
			g_proArr[i]._count = 0;
			g_proArr[i]._max[0] = 0;
			g_proArr[i]._max[1] = 0;
			g_proArr[i]._min[0] = INT64_MAX;
			g_proArr[i]._min[1] = INT64_MAX;
			g_proArr[i]._totalTime.QuadPart = 0;
			g_proArr[i]._totalTime.QuadPart = 0;
		}

		// 1초당 타이머 주파수를 얻는다, min, max
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		g_freq = (double)freq.QuadPart / 1000000;
	}
};

CInit g_Cinit; // 초기화용

void PRO_BEGIN(const WCHAR* pName) // BEGIN
{
	for (int i = 0; i < STRUCT_NUM; i++)
	{
		// 0x00 이면 새로 공간 할당
		if (g_proArr[i]._name[0] == 0x00)
		{
			wcscpy_s(g_proArr[i]._name, TEXT_SIZE, pName);
			QueryPerformanceCounter(&g_proArr[i]._begin);

			break;
		}

		// 기본적으로 이름이 맞지 않으면 continue
		if (wcscmp(g_proArr[i]._name, pName) != 0)
		{
			continue;
		}

		// begin begin 일 경우
		if (g_proArr[i]._begin.QuadPart != 0)
		{
			throw 1;
		}

		// begin QueryPerformanceCounter 구해서 넣는다.
		QueryPerformanceCounter(&g_proArr[i]._begin);

		break;
	}
}

void PRO_END(const WCHAR* pName) // END
{
	for (int i = 0; i < STRUCT_NUM; i++)
	{
		// 이름이 맞지 않으면 continue
		if (wcscmp(g_proArr[i]._name, pName) != 0)
		{
			continue;
		}

		// end QueryPerformanceCounter 구한다.
		LARGE_INTEGER endTime;
		QueryPerformanceCounter(&endTime);

		// end - begin
		__int64 diffTime = endTime.QuadPart - g_proArr[i]._begin.QuadPart;
		g_proArr[i]._totalTime.QuadPart += diffTime;
		// count++
		g_proArr[i]._count++;

		//cout << ((double)diffTime / freq.QuadPart) / 1000000 << endl;
		double microTime = (double)diffTime / g_freq;
		//wprintf(L"%.4f㎲\n", microTime);

		// min, max
		if (g_proArr[i]._min[0] > diffTime)
		{
			g_proArr[i]._min[1] = g_proArr[i]._min[0];
			g_proArr[i]._min[0] = diffTime;
		}

		if (g_proArr[i]._max[0] < diffTime)
		{
			g_proArr[i]._max[1] = g_proArr[i]._max[0];
			g_proArr[i]._max[0] = diffTime;
		}

		// end 나갈때 begin 0으로. 나중에 begin 두번 연속 선언할때 감지ㅣ
		g_proArr[i]._begin.QuadPart = 0;

		break;
	}
}

void SaveProfile()
{
	tm newtime;
	__time64_t long_time;
	_time64(&long_time);
	_localtime64_s(&newtime, &long_time);

	WCHAR timeWCHAR[100];
	StringCbPrintf(timeWCHAR, 100, L"profile_%04d%02d%02d_%02d%02d%02d.txt",
		newtime.tm_year + 1900,
		newtime.tm_mon,
		newtime.tm_mday,
		newtime.tm_hour,
		newtime.tm_min,
		newtime.tm_sec);

	FILE* fp;
	errno_t err = _wfopen_s(&fp, timeWCHAR, L"wb, ccs=UNICODE");
	if (err != 0)
	{
		wprintf(L"FILE OPEN ERROR\n");

		return;
	}

	fputws(L"\t\t\tName\t\t|\t\t\tAverage\t\t|\t\t\tMin\t\t\t|\t\t\tMax\t\t\t|\t\t\tCall\t\t|\n", fp);
	fputws(L"-------------------------------------------------------------------------------------------------------------------------\n", fp);

	for (int i = 0; i < STRUCT_NUM; i++)
	{
		if (g_proArr[i]._name[0] == 0x00)
		{
			continue;
		}

		WCHAR tempWchar[200];
		ZeroMemory(tempWchar, sizeof(WCHAR) * 200);

		// 평균
		double average = 0;
		double minusValue = 0;
		double min = 0;
		double max = 0;

		if (g_proArr[i]._min[1] == INT64_MAX)
		{
			min = (double)g_proArr[i]._min[0] / g_freq;
		}
		else
		{
			min = (double)g_proArr[i]._min[1] / g_freq;
			minusValue += (double)g_proArr[i]._min[1];
			g_proArr[i]._count--;
		}

		if (g_proArr[i]._max[1] == 0)
		{
			max = (double)g_proArr[i]._max[0] / g_freq;
		}
		else
		{
			max = (double)g_proArr[i]._max[1] / g_freq;
			minusValue += (double)g_proArr[i]._max[1];
			g_proArr[i]._count--;
		}

		average = ((double)g_proArr[i]._totalTime.QuadPart - minusValue) / g_freq / g_proArr[i]._count;

		// wsprintf 대용
		StringCbPrintf(tempWchar, 200, L"%20ws\t|\t%15.4f㎲\t|\t%15.4f㎲\t|\t%15.4f㎲\t|\t%15d\t\t|\n",
			g_proArr[i]._name, average, min, max, g_proArr[i]._count);

		// 한줄씩 파일에 쓰기
		fputws(tempWchar, fp);
	}
}