// �����ڿ��� �������� ������� �ٶ� ����

#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


#define MAX_RES_CNT		3	// �ڿ��� �ִ� ������ 3���� ���Ѵ�.

bool	g_bExit = false;	// ������ ���Ḧ ���� �÷��׸� �����Ѵ�.
BOOL	g_abUsedFlag [MAX_RES_CNT];
INT		g_anSharedRes[MAX_RES_CNT] = { 5 , 3, 7 };
// �ڿ��� ��Ÿ���� �迭�� �迭�� ���� ������ 3����.
HANDLE	g_hShareLock;	// ���� �ڿ� �˻��� ���� ���ؽ� �ڵ��̴�.

DWORD WINAPI SemaphoreProc(LPVOID pParam)
{
	HANDLE hSemaphore = (HANDLE)pParam;
	DWORD  dwThreadId = GetCurrentThreadId();

	while (!g_bExit)
	{
		DWORD dwWaitCode = WaitForSingleObject(hSemaphore, INFINITE);
		// ������� ���� ��� ���·� ����.

		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}
		// dwWaitCode�� ������ �ƴ� ���� ������� �ñ׳� ���°� �Ǿ����� �ǹ��Ѵ�.
		// ���� �����ڿ��� ȹ���� �� �ִ� ���°� �ǰ�, �װͿ� ���� �۾��� �����Ѵ�.

		int nSharedIdx = 0;
		WaitForSingleObject(g_hShareLock, INFINITE);
		for (; nSharedIdx < MAX_RES_CNT; nSharedIdx++)
		{
			if (!g_abUsedFlag[nSharedIdx])
				break;
		}
		g_abUsedFlag[nSharedIdx] = TRUE;
		ReleaseMutex(g_hShareLock);
		/* ������� �ñ׳� ���°� �Ǿ��ٴ� ���� ���� �ڿ��� �ִٴ� �͸��� �ǹ��ϱ� ������
		�� �ڿ��� ���� �ĺ� ������ ���� �������� �ʴ´�.
		���� ��� ������ �ڿ��� ã�� ���� ������ ���鼭 �� �ڿ��� ã�� ������ �ʿ��ϴ�.
		���ÿ� ���� �����尡 �� ������ ������ �� �ֱ� ������
		���� �ڿ� �˻� ���� ����ȭ ���� �ʿ��ϹǷ� ���ؽ��� ���� ����ȭ�� �����Ѵ�.
		*/

		cout << " ==> Thread " << dwThreadId <<
			" waits " << g_anSharedRes[nSharedIdx] << " seconds..." << endl;
		Sleep((DWORD)g_anSharedRes[nSharedIdx] * 1000);
		cout << " ==> Thread " << dwThreadId << " releases semaphore..." << endl;
		g_abUsedFlag[nSharedIdx] = FALSE;

		ReleaseSemaphore(hSemaphore, 1, NULL);
		// �ڿ��� ��� ����ߴٸ� ReleaseSemaphore�� ȣ���� �ڿ������ 1 ������Ų��.
		// �׷��� ��� ���� �ٸ� �����尡 ������ �ڿ��� ����� �� �ְ� �ȴ�.
	}

	cout << " *** Thread " << dwThreadId << " exits..." << endl;

	return 0;
}


void _tmain()
{
	cout << "======= Start Semaphore Test ========" << endl;
	g_hShareLock = CreateMutex(NULL, FALSE, NULL);	// ��� ������ �ʴ� �ڿ� �˻��� ���� ���ؽ��� �����Ѵ�.

	HANDLE hSemaphore = CreateSemaphore(NULL, MAX_RES_CNT, MAX_RES_CNT, NULL);	// ������� �����Ѵ�.
	// ���⼭�� ��� ������ �ִ� �ڿ������� 3�̸�, ���ÿ� ���� ��� ������ �ڿ���� ���� 3���� �ǹ��ϵ���
	// �� ��°�� �� ��° �Ű������� ���� MAX_RES_CNT�� �ѱ��. ���� �� ��������� �ñ׳� ���·� �����ȴ�.

	DWORD	dwThrId;
	HANDLE	hThreads[MAX_RES_CNT + 2];
	for (int i = 0; i<MAX_RES_CNT + 2; i++)
		hThreads[i] = CreateThread(NULL, 0, SemaphoreProc, hSemaphore, 0, &dwThrId);	// �����带 5�� �����Ѵ�.
	// �׷��� �Ǹ� ó�� 3���� ������� �ڿ��� �����ϰ� ������ �� ��° ��������ʹ� ��� ���·� �� ���̴�.
	// ������ ���� �� ���������� �ڵ��� ������ ��Ʈ�� �Լ��� �Ű������� �Ѱ��ش�.

	getchar();
	g_bExit = true;
	// ������ Ű�� �Էµ� ������ ���� �����带 ����Ų��.
	// Ű�� �ԷµǸ� ������ ���� ó���� ���� ���� �÷��׸� true�� �����Ѵ�.

	WaitForMultipleObjects(MAX_RES_CNT + 2, hThreads, TRUE, INFINITE);
	// 5���� �����尡 ��� ����� ������ ��� ���¿��� ��ٸ���.
	// �̴� �����尡 ���������� ����ǵ��� �ϴ� ���� ������ ����̴�.

	for (int i = 0; i < MAX_RES_CNT + 2; i++)
	{
		CloseHandle(hThreads[i]);
		CloseHandle(hSemaphore);
		CloseHandle(g_hShareLock);
	}
	// �������� ��������, �׸��� ���ؽ��� �ڵ��� �ݴ´�.

	cout << "======= End Semaphore Test ==========" << endl;
}
