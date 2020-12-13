#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(LPVOID pParam)
{	
	/* ������ ��Ʈ�� �Լ������� 4���� Ŀ�� ��ü�� ���� ���� ��⸦ �ϸ�,
	�� ��ü�� �ñ׳� ���°� �ɶ����� ��� ���¿��� Ż���ؼ� �ش� �۾��� ó���Ѵ�.
	*/

	PHANDLE parSyncs = (PHANDLE)pParam;

	while (true)
	{
		DWORD dwWaitCode = WaitForMultipleObjects(4, parSyncs, FALSE, INFINITE);
		// WaitForMultipleObjects�� �̿��� 4���� ����ȭ ��ü�� ���� ���ÿ� ��⿡ �� ��
		// ������ ������ ���� �� �����忡�� ��� ó���� �����Ѵ�.

		if (dwWaitCode == WAIT_FAILED)
		{
			cout << "WaitForSingleObject failed : " << GetLastError() << endl;
			return 0;
		}
		if (dwWaitCode == WAIT_OBJECT_0) // ���� �̺�Ʈ �ñ׳�
			break;

		dwWaitCode -= WAIT_OBJECT_0;
		switch (dwWaitCode)
		{
			case 1 : // �̺�Ʈ �ñ׳�
				cout << " +++++ Event Singnaled!!!" << endl;
				ResetEvent(parSyncs[dwWaitCode]);
			break;
			case 2:	// ���ؽ� �ñ׳�
				cout << " ***** Mutex Singnaled!!!" << endl;
				ReleaseMutex(parSyncs[dwWaitCode]);
			break;
			case 3:	// �������� �ñ׳�
				cout << " ##### Semaphore Singnaled!!!" << endl;
			break;
		}
	}
	return 0;
}

void _tmain(void)
{
	cout << "======= Start MultiSyncWaits Test ========" << endl;

	HANDLE arhSyncs[4];
	arhSyncs[0] = CreateEvent(NULL, TRUE, FALSE, NULL);	
	// ���Ḧ ���� �̺�Ʈ�� ���� ���� �̺�Ʈ�� �����ϰ� �迭�� �ε��� 0�� �Ҵ��Ѵ�.

	//arhSyncs[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	arhSyncs[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	arhSyncs[2] = CreateMutex(NULL, TRUE, NULL);
	arhSyncs[3] = CreateSemaphore(NULL, 0, 1, NULL);
	// ���� Ư�� ����� �뺯�ϴ� �̺�Ʈ, ���ؽ�, �������� ��ü�� ������ ���� �迭�� ������� �Ҵ��Ѵ�.
	// �̺�Ʈ�� ��� �ڵ� ���� �̺�Ʈ�� �������״�.

	DWORD dwTheaID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, arhSyncs, 0, &dwTheaID);
	// ����ȭ ��ü �ڵ� �迭�� �������� ��Ʈ�� �Լ� �Ű������� �����Ѵ�.

	char szIn[32];
	while (true)
	{
		cin >> szIn;
		if (_stricmp(szIn, "quit") == 0)
			break;
		// ���� ó����, ������ �������� ���� �̺�Ʈ�� �ñ׳θ��Ѵ�.

		if (_stricmp(szIn, "event") == 0)
			SetEvent(arhSyncs[1]);
		// �̺�Ʈ�� ���� �ð��� �����Ѵ�.

		else if (_stricmp(szIn, "mutex") == 0)
		{
			ReleaseMutex(arhSyncs[2]);
			WaitForSingleObject(arhSyncs[2], INFINITE);
			// ���ؽ��� ���� ����� �����Ѵ�.
			// ���ؽ��� ��� ������ ��ȣ�� ����ȭ�Ǿ� ������ ���� ������� �����ص� ��� �����ϴ�.
		}

		else if (_stricmp(szIn, "semaphore") == 0)
			ReleaseSemaphore(arhSyncs[3], 1, NULL);
		// ������� ���� ����� �����Ѵ�.
	}
	SetEvent(arhSyncs[0]);	// ������ ���Ḧ �����Ѵ�.
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	for (int i = 0; i < 4; i++)
		CloseHandle(arhSyncs[i]);

	cout << "======= End MultiSyncWaits Test ==========" << endl;
}

