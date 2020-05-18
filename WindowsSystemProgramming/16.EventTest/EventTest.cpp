#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(LPVOID pParam)
{
	HANDLE hEvent = (HANDLE)pParam;

	DWORD dwWaitCode = WaitForSingleObject(hEvent, INFINITE);
	// ���ؽ� ��� �ڵ� ���� �̺�Ʈ�� ���� ����Ѵ�.

	if (dwWaitCode == WAIT_FAILED)
	{
		cout << "WaitForSingleObject failed : " << GetLastError() << endl;
		return 0;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);

	cout << "..." << "SubThread " << GetCurrentThreadId() << " => ";
	cout << st.wYear << '/' << st.wMonth << '/' << st.wDay << ' ';
	cout << st.wHour << ':' << st.wMinute << ':' << st.wSecond << '+';
	cout << st.wMilliseconds << endl;
	SetEvent(hEvent);	// ReleaseMutext ȣ�� �κ��� SetEvent�� ��ü�Ѵ�.
	/* ReleaseMutex�� ���ؽ��� ���� �������� ���������ν� ���ؽ��� �ñ׳� ���·� ����� ���ÿ�
	�ٸ� ������� �Ͽ��� ���ؽ��� ������ �� �ֵ��� �Ѵ�.
	���������� ���⿡���� SetEvent�� ȣ���� �̺�Ʈ�� �ñ׳� ���·� ����� �ٸ� �����尡 ������ �� �ֵ��� ���ش�.
	���� �ڵ� ���� �̺�Ʈ�̹Ƿ� ����ϴ� ������ �� ������ �ϳ����� ���� �ڵ带 ������ ���̴�.
	�̺�Ʈ�� ���� ReleaseMutext�� �ϴ� ������ SetEvent�� ���� �ͽñ׳� ������ ��ü�� �ñ׳� ���·�
	����� ��ġ ���ؽ��� �������� ������ �Ͱ� ���� ������ �����Ѵ�.
	���� ���� ���� �̺�Ʈ���ٸ� ����ϴ� ��� �����尡 Ȱ��ȭ�Ǿ� �Ӱ豸���� ���ÿ� �����ϱ� ������ ����ȭ�� �����ϰ� �� ���̴�.	
	*/

	return 0;
}

void _tmain(void)
{
	cout << "======= Start Event Test ========" << endl;
	HANDLE hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	// ���ؽ� ��� �ڵ� ���� �̺�Ʈ�� ������ �������� �Ű������� �ѱ��.

	HANDLE arhThreads[10];
	for (int i = 0; i < 10; i++)
	{
		DWORD dwTheaID = 0;
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, hEvent, 0, &dwTheaID);
	}
	WaitForMultipleObjects(10, arhThreads, TRUE, INFINITE);
	CloseHandle(hEvent);
	cout << "======= End Event Test ==========" << endl;
}

