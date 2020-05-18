#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(LPVOID pParam)
{
	PHANDLE	parWaits = (PHANDLE)pParam;
	DWORD	dwThrId = GetCurrentThreadId();

	while (true)
	{
		DWORD dwWaitCode = WaitForMultipleObjects(2, parWaits, FALSE, INFINITE);
		// ���ؽ� 2���� ���� ���� ����Ѵ�.
		// ù ��° ���ؽ��� ���� ������ �����̰�, �� ��° ���ؽ��� �Ʒ� �ð� ��� �ڵ��� ����ȭ�� ���� ���ȴ�.
		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}

		if (dwWaitCode == WAIT_OBJECT_0)
		{
			printf(" ===> SubThread %d exits....\n", dwThrId);
			ReleaseMutex(parWaits[0]);
			break;
			// ù ��° ���ؽ��� �ñ׳� ���°� �Ǿ���, �̴� ���� ������ �ǹ��ϹǷ� ������ Ż���Ѵ�.
			// Ż���ϱ� ���� ReleaseMutex�� ȣ���� Ż���ϰ� �� ������ ���� �������� ������� �Ѵ�.
			// �̷��� �������� �����ָ� ��ġ ��̳�ó�� ������ ���̿���
			// ���ؽ��� ���� ������ ������ ȹ���� ���ʴ�� �ݺ� ���ĵǾ� ���� �����忡�Ա��� ���޵ȴ�.
		}
		//if (dwWaitCode == WAIT_ABANDONED)
		//	cout << " **** SubThread " << dwThrId << " : Abandoned Mutex"  << endl;
		//if (dwWaitCode == WAIT_OBJECT_0 || dwWaitCode == WAIT_ABANDONED)
		//	break;

		SYSTEMTIME st;
		GetLocalTime(&st);
		cout << "..." << "SubThread " << dwThrId << " => ";
		cout << st.wYear << '/' << st.wMonth << '/' << st.wDay << ' ';
		cout << st.wHour << ':' << st.wMinute << ':' << st.wSecond << '+';
		cout << st.wMilliseconds << endl;

		if (!ReleaseMutex(parWaits[1]))
		{
			cout << " ~~~ Thread " << dwThrId <<
				" : ReleaseMutex failed, Error Code=" << GetLastError() << endl;
		}
		Sleep(1000);
	}
	return 0;
}

#define MAX_THR_CNT	3
void _tmain(void)
{
	cout << "======= Start MutexNoti Test ========" << endl;
	HANDLE arWaits[2];
	arWaits[0] = CreateMutex(NULL, TRUE, NULL);
	// ������ ���� ������ ���ؽ��� �����ϱ� ���� ���� �����尡 �켱 �����带 �����ϵ���
	// bInitialOwner �Ű������� TRUE�� �����Ѵ�.
	arWaits[1] = CreateMutex(NULL, FALSE, NULL);
	// �Ӱ豸�� ������ ���� �� �ٸ� ���ؽ��� �����Ѵ�.

	HANDLE arhThreads[MAX_THR_CNT];
	for (int i = 0; i < MAX_THR_CNT; i++)
	{
		DWORD dwTheaID = 0;
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, arWaits, 0, &dwTheaID);
	}

	getchar();
	ReleaseMutex(arWaits[0]);
	// Ű�� �Է¹����� ���� ���� ��������� �����ϱ� ���ؼ� ������ ���ؽ��� �����Ѵ�.

	WaitForMultipleObjects(MAX_THR_CNT, arhThreads, TRUE, INFINITE);
	for (int i = 0; i < MAX_THR_CNT; i++)
		CloseHandle(arhThreads[i]);
	for (int i = 0; i < 2; i++)
		CloseHandle(arWaits[i]);
	cout << "======= End MutexNoti Test ==========" << endl;
}

