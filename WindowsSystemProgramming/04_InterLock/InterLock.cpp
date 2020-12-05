#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

//#define USE_VOLATILE

#define OFF	 0
#define ON	 1

#ifdef USE_VOLATILE
void AcquireLock(volatile LONG& lSign)
{
	while (lSign == OFF);
	// lSign�� OFF�� ���� ��� ������ ���ٰ� ON�� �Ǵ� ���� �������� �ڵ带 �����Ѵ�.
	lSign = OFF;
	// ����ϰ� ������ �˸��� ���� OFF ���·� �����.
}
#else
void AcquireLock(LONG& lSign)
{
	while (lSign == OFF);
	lSign = OFF;
}
#endif

void ReleaseLock(LONG& lSign)
{
	lSign = ON;
	// lSign�� ON���� ����� �ٸ� �����尡 ���� �ڵ带 ������ �� �ֵ��� �Ѵ�.
}


LONG g_lCSSign = ON;
// ON/OFF ǥ������ ���� ���� �����̴�. ���� ON ���·� �����Ͽ� �����ڿ��� ������ ����� �� ������ ǥ������� �Ѵ�.
int  g_nValue = 0;
// ���ڼ��� Ȯ������� ��, ������ ���̿��� ���� �����Ǵ� ���� �����̴�.

DWORD WINAPI ThreadProc(PVOID pParam)
{
	INT dwCurId = (INT)pParam;

	AcquireLock(g_lCSSign);	// <- �����ڿ� ��� ���� üũ �� ���

	cout << " ==> Thread " << dwCurId << " Enter : " << g_nValue << endl;
	g_nValue++;
	cout << "  <= Thread " << dwCurId << " Leave : " << g_nValue << endl;

	ReleaseLock(g_lCSSign);	// <- �����ڿ� ��� �Ϸ� ����

	return 0;
}

#define MAX_THR_CNT	6
void _tmain()
{
	HANDLE arhThrs[MAX_THR_CNT];
	for (int i = 0; i < MAX_THR_CNT; i++)
	{
		DWORD dwTheaID = 0;
		arhThrs[i] = CreateThread(NULL, 0, ThreadProc, (PVOID)i, 0, &dwTheaID);
	}

	getchar();

	for (int i = 0; i < MAX_THR_CNT; i++)
		CloseHandle(arhThrs[i]);
}

// �̿� ���� ������δ� Acquire/ReleaseLock ������ �ڵ忡 ���� ���ڼ��� Ȯ���� �� ����. = ����ȭ�� �̷��� ���Ѵ�.