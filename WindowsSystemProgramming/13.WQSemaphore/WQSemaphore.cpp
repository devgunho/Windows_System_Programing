#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "Windows.h"
#include "list"
#include "iostream"
using namespace std;


#define WQI_CMD_EXIT	0	// ���� ó���� ���� ���
// ������ Ǯ������ ���� ó���� ���� ������ ���� ���ؽ��� ���������,
// ���⼭�� ��� �ĺ������� ��ü�Ѵ�. ���� ���Ḧ ���� ������ ��ü�� �ʿ� ����.
#define WQI_CMD_STR		1	// ���ڿ� ó���� ���� ���
#define WQI_CMD_INT		2	// ������ ó���� ���� ���

struct WQUE_ITEM
{
	INT		_cmd;
	PVOID	_data;
};
typedef std::list<WQUE_ITEM> ITEM_QUE;
// ť�� �׸��� �����Ѵ�. _cmd�� _data�� Ÿ���� �ĺ��ϰ� _data�� ó���� ���� �����͸� ��� �ȴ�.

struct WAIT_QUE
{
	HANDLE	 _hMutx;
	HANDLE	 _hSema;
	ITEM_QUE _queue;
};
typedef WAIT_QUE* PWAIT_QUE;
// ����� ť�� �ʿ��� �׸��� ��� �ִ�. ������ Ǯ�� ���� ������Ҵ� �����ϴ�.

// enqueue ó�� �κ��� ������ �Լ��� �����Ѵ�. ������ Ǯ ���������� ��ť ó���� �����ϴ�.
void WQ_Enqueue(PWAIT_QUE pWQ, INT cmd, PVOID data)
{
	WQUE_ITEM wqi;
	wqi._cmd = cmd, wqi._data = data;

	WaitForSingleObject(pWQ->_hMutx, INFINITE);
	pWQ->_queue.push_back(wqi);
	ReleaseMutex(pWQ->_hMutx);

	ReleaseSemaphore(pWQ->_hSema, 1, NULL);
}

// dequeue ó�� �κ��� ������ �Լ��� �����Ѵ�. ������ Ǯ ���������� dequeue ó���� �����ϴ�.
INT WQ_Dequeue(PWAIT_QUE pWQ, PVOID& data)
{
	WaitForSingleObject(pWQ->_hSema, INFINITE);

	WQUE_ITEM wqi;
	WaitForSingleObject(pWQ->_hMutx, INFINITE);
	ITEM_QUE::iterator it = pWQ->_queue.begin();
	wqi = *it;
	pWQ->_queue.pop_front();
	ReleaseMutex(pWQ->_hMutx);

	data = wqi._data;
	return wqi._cmd;
}


DWORD WINAPI QueueWorkerProc(LPVOID pParam)
{
	PWAIT_QUE pWQ = (PWAIT_QUE)pParam;

	while (true)
	{
		/*
		DWORD dwWaitCode = WaitForSingleObject(pWQ->_hSema, INFINITE);
		// ������� ���� ����Ѵ�.
		// WQI_CMD_EXIT�� ���� ó���� �����ϱ� ������ �������� �ϳ��� ���ؼ��� ����ص� �������.
		// ����, �ʿ��ϴٸ� �ٸ� ���� ����ȭ ��ü�� ���Ҿ�
		// WaitForMultipleObjects�� ȣ���� ������ ���� ���� ������ ���� ó�� ���� �����ϴ�.
		*/

		PVOID data = NULL;
		INT cmd = WQ_Dequeue(pWQ, data);	// ť�κ��� �׸��� �����Ѵ�.

		if (cmd == WQI_CMD_EXIT)
			break;
		// ���� ����̸� ������ Ż���ϰ� �����带 �����Ѵ�. ������ ���� ������ ��ü ���� �Ϲ� ó�� �׸�� ���� ����� �� �ִ�.

		if (cmd == WQI_CMD_INT)
			printf(" => CMD_INT queued : %d\n", (int)data);
		else
		{
			printf(" => CMD_STR queued: %s\n", (char*)data);
			delete[]((char*)data);
		}
		// ��ɿ� �°� �����͸� ����� �� �۾��� �Ϸ��� ���� �ٽ� ������ ���� ��� �Լ��� ȣ���Ѵ�.
	}
	printf("......QueueWorkerProc Thread Exit!!!\n");

	return 0;
}

void _tmain()
{
	cout << "======= Start WQSemaphore Test ========" << endl;

	WAIT_QUE wq;
	wq._hSema = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	wq._hMutx = CreateMutex(NULL, FALSE, NULL);
	// �Ӱ豸�� ������ ���ؽ��� ť ������ ���� ������� �����Ѵ�.
	// �������� ���� �ÿ� ���������� �ʱⰪ�� 0���� �����Ѵ�.

	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, QueueWorkerProc, &wq, 0, &dwThreadId);
	// �����带 �ϳ��� �����Ѵ�. enqueue�� ť�� �׸��� ó���ϴ� �������̴�.

	char szIn[1024];
	while (true)
	{
		cin >> szIn;

		INT   cmd = 0;
		PVOID data = NULL;
		if (_stricmp(szIn, "quit") == 0)
			cmd  = WQI_CMD_EXIT;	// ���ڿ��� �Է¹޾� quit�̸� ���Ḧ �˸��� WQI_CMD_EXIT�� �������ش�.
		else
		{
			int nVal = atoi(szIn);
			if (nVal == 0)	// ������������ ��ȯ ����� 0�� �ƴϸ� ������ �����ϰ� WQI_CMD_INT�� �������� �����Ѵ�.
			{
				cmd = WQI_CMD_STR;
				data = new char[strlen(szIn) + 1];
				strcpy((char*)data, szIn);
				// ������������ ��ȯ ����� 0�̸� ���ڿ��� �����ϰ�, WQI_CMD_STR�� �Է¹��� ���ڿ��� �����Ѵ�.
			}
			else
			{
				cmd = WQI_CMD_INT;
				data = (PVOID)nVal;
			}
		}

		WQ_Enqueue(&wq, cmd, data);	// ť �׸��� ť�� enqueue�Ѵ�. �̿� ���ÿ� ������� Ȱ��ȭ �ȴ�.
		if (cmd == WQI_CMD_EXIT)
			break;
	}

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(wq._hSema);
	CloseHandle(wq._hMutx);

	cout << "======= End WQSemaphore Test ==========" << endl;
}
