#include "stdafx.h"
#include "Windows.h"
#include "string"
#include "list"
#include "iostream"

using namespace std;

HANDLE g_hExit = NULL;	// ������ ���Ḧ ���� ���ؽ�

typedef std::list<std::string> TP_QUE;
struct THREAD_POOL
{
	HANDLE	m_hMutx;	// enqueue/dequeue ����ȭ�� ���� ���ؽ�
	HANDLE	m_hSema;	// ������ Ǯ ������ ���� ��������
	TP_QUE	m_queue;	// ť ����� ����� STL ����Ʈ
};
typedef THREAD_POOL* PTHREAD_POOL;

// ������� �⺻������ ���� �ñ׳��� ���� ���ؽ��� ť ������ ���� �������� �� ���� ���� ���� ���� ������ ��� ���·� �����ϰ� �ȴ�.
DWORD WINAPI PoolItemProc(LPVOID pParam)
{
	PTHREAD_POOL pTQ = (PTHREAD_POOL)pParam;

	DWORD dwThId = GetCurrentThreadId();
	HANDLE arhObjs[2] = { g_hExit, pTQ->m_hSema };
	while (true)
	{
		DWORD dwWaitCode = WaitForMultipleObjects(2, arhObjs, FALSE, INFINITE);
		/* ó���� ��� ������� ������� �ͽñ׳� ���±� ������ ��� ���·� ����.
		�׷��� ��û ���� ������(���⼭�� ���� ������)���� ��û�� ������ �� ReleaseSemaphore�� ���ؼ� �ڿ������ 1 �������� �ָ�
		��� ���¿��� Ǯ�� ���� �ڵ带 �����ϰ� �ȴ�.
		���� WaitForMultipleObjects�� �ΰ��� ȿ�� ������ ��� ���¿��� Ǯ�� ���� �̹� ���������� �ڿ������ 1 ���ҵǾ� 0�� �Ǿ� ���� ���̴�.
		*/
		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}
		
		if (dwWaitCode == WAIT_OBJECT_0)
			break;	// ���ؽ��� �ñ׳� ���°� �Ǹ� ������ ���Ḧ �ǹ��ϱ� ������ ������ Ż���Ѵ�.

		std::string item;
		WaitForSingleObject(pTQ->m_hMutx, INFINITE);
		TP_QUE::iterator it = pTQ->m_queue.begin();
		item = *it;
		pTQ->m_queue.pop_front();
		ReleaseMutex(pTQ->m_hMutx);
		// ť���� �۾� �׸��� dequeue�Ѵ�. ���ؽ��� ����� dequeue�� ���� ť�� ���� ���� ������ ���´�.

		printf(" => BEGIN | Thread %d works : %s\n", dwThId, item.c_str());
		Sleep(((rand() % 3) + 1) * 1000);
		printf(" <= END   | Thread %d works : %s\n", dwThId, item.c_str());
		// ť�κ��� �۾� �׸��� ȹ���ϸ� �̿� ���� ó���� �ϰ�, �ٽ� ������ ���ư� ��� ���·� ����.
	}

	ReleaseMutex(g_hExit);	// ���ؽ��� ���� �ñ׳η� ����ϱ� ������ �����ϱ� �� ���� ������ ���ؽ��� �������� �����ش�.

	printf("......PoolItemProc Thread %d Exit!!!\n", dwThId);

	return 0;
}

void _tmain()
{
	cout << "======= Start TPSemaphore Test ========" << endl;

	g_hExit = CreateMutex(NULL, TRUE, NULL);	// ������ ���Ḧ ���ؼ� ���ؽ��� �����Ѵ�.
	// ���� ���� ���� �̺�Ʈ�� ó��������, ���ؽ��� �̿��� ���� ������ �Ѵ�.

	THREAD_POOL tp;
	tp.m_hSema = CreateSemaphore(NULL, 0, INT_MAX, NULL);	// ������� �����Ѵ�.
	/* ��� ������ �ʱ� ��������� 0���� �ѱ�� �� ��������� �ͽñ׳� ���·� �����ǹǷ�
	�� ������� ����ϰ����ϴ� ��������� ��� ��� ���·� ���� �ȴ�.
	���⼭�� ��� ������ �ִ� �ڿ� ���� INT_MAX�� ����������,
	ť�� ������ Ŀ���� ���� ������ �Ѵٸ� ���� �Ѱ�ġ�� ���� �Ѱ��ָ� �ȴ�.
	*/

	tp.m_hMutx = CreateMutex(NULL, FALSE, NULL);	// enqueue/dequeue ����ȭ�� ���� ���ؽ��� �����Ѵ�.

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	// Ǯ�� �� ������ ���� �����ϱ� ���� �ý��� CPU�� ������ ȹ���Ѵ�.
	// ������ Ǯ���� ����� ������ ���� CPU�� ������ŭ ������ ���̴�.

	PHANDLE parPoolThs = new HANDLE[si.dwNumberOfProcessors];
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++)
	{
		DWORD dwThreadId;
		parPoolThs[i] = CreateThread(NULL, 0, PoolItemProc, &tp, 0, &dwThreadId);
		// CPU ������ŭ �����带 �����Ѵ�.
		// ������ ��������� ������� �ͽñ׳� �����̹Ƿ� ��� ��� ���·� �����ϰ� �ȴ�.
	}
	cout << "*** MAIN -> ThreadPool Started, count=" << si.dwNumberOfProcessors << endl;

	char szIn[1024];
	while (true)
	{
		cin >> szIn;
		if (_stricmp(szIn, "quit") == 0)
			break;
		// ������ ���� �ֿܼ��� �����͸� �Է��ϸ� �� �����͸� ��� �����尡 ó���ϵ��� �ñ��.
		// �� ������ Ŭ�����̾�Ʈ�� ���� ��û�� �� ������ �ùķ��̼��� ���̴�.
		// �� �κ��� ���� �������� ����Ѵٸ� Ŭ���̾�Ʈ�� ���� ������ �ϰ� �ִ� ������ ��ü�� �� ���� ���̴�.

		WaitForSingleObject(tp.m_hMutx, INFINITE);
		tp.m_queue.push_back(szIn);
		ReleaseMutex(tp.m_hMutx);
		// ���ڿ��� ť�� enqueue�Ѵ�.
		// ���ؽ� ��ü�� �̿��� ť�� ���� �������� ȹ���� �� �۾� �׸��� ť�� �߰��Ѵ�.

		ReleaseSemaphore(tp.m_hSema, 1, NULL);
		/* ��� ���� �����忡�� �۾� �׸��� �߰��Ǿ����� �˸��� ����̴�.
		�� �������� CPU ����ŭ�� ��������� ���� m_hSema�� ������� ����ϰ� �ִ�.
		���� �ڿ������ 0�̹Ƿ� ��û�� ���� ReleaseSemaphore�� ȣ���� 1��ŭ �ڿ� ���� �������������ν�
		��� ������ �� ������ �ϳ��� Ȱ��ȭ���� ��û�� ó���ϵ��� �Ѵ�.
		ReleaseSemaphore�� ȣ���ϴ� ������� ���� �������̴�.
		���� �����带 CPU ����ŭ �������� �־��� ������ �۾��� ��� �߰��Ǵ��� �׻� CPU ����ŭ�� �����常�� Ȱ��ȭ�ȴ�.
		*/
	}

	ReleaseMutex(g_hExit);	// ���Ḧ ���� ���� ���� ���ؽ��� �ñ׳��Ѵ�.
	WaitForMultipleObjects(si.dwNumberOfProcessors, parPoolThs, TRUE, INFINITE);
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++)
		CloseHandle(parPoolThs[i]);
	delete[] parPoolThs;

	CloseHandle(tp.m_hSema);
	CloseHandle(tp.m_hMutx);
	CloseHandle(g_hExit);

	cout << "======= End TPSemaphore Test ==========" << endl;
}
