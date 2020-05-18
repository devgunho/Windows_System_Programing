#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(LPVOID pParam)
{
	HANDLE hMutex = (HANDLE)pParam;

	DWORD dwWaitCode = WaitForSingleObject(hMutex, INFINITE);
	// �������� ȹ���� ������ ����Ѵ�.
	if (dwWaitCode == WAIT_FAILED)
	{
		cout << "WaitForSingleObject failed : " << GetLastError() << endl;
		return 0;
	}
	if (dwWaitCode == WAIT_ABANDONED)
		cout << "Abandoned Mutex acquired!!!" << GetLastError() << endl;

	SYSTEMTIME st;
	GetLocalTime(&st);

	cout << "..." << "SubThread " << GetCurrentThreadId() << " => ";
	cout << st.wYear << '/' << st.wMonth << '/' << st.wDay << ' ';
	cout << st.wHour << ':' << st.wMinute << ':' << st.wSecond << '+';
	cout << st.wMilliseconds << endl;
	//ReleaseMutex(hMutex);
	// ȹ���� �������� �����Ѵ�.
	/* ���� ���ؽ��� �������� ȹ���� �� ReleaseMutex�� ȣ������ �ʰ� �ش� �����带 �����ϵ��� ó���� ���,
	�ƴϸ� �����尡 ���ؽ��� ������ �� �۾� ó�� ���̸� ReleaseMutex�� ȣ���ϱ� ���� ��Ȳ���� �ٸ� �����忡��
	TerminateThread�� ȣ���� �� �����带 ������ ������� ���� ��쿡 �ü���� �����尡 ����� ��
	���ؽ��� ���� ������ �ִ����� �˻��ϰ� �ǰ� ���ؽ��� ����� ���� ����Ǿ����� �˸��� ����
	���ϰ��� WAIT_ABANDONED�� ���� ��� �Լ����� �����ϵ��� �Ѵ�.
	�� ���, ��� �������� ���� ä �����尡 ����Ǵ��� �� ������ ���ؽ��� �ٸ� �����尡 �̾ ����� �� �ְ� ���ش�.
	*/

	return 0;
}

#define MAX_THR_CNT	10
void _tmain(void)
{
	cout << "======= Start Mutex Test ========" << endl;
	HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
	// Mutex ����.
	// �� ��° �Ű������� FALSE�� �Ѱ��־� ���� �����尡 ���ؽ��� �������� ������ ����Ѵ�.
	// ������ ���ؽ��� �ڵ��� ������ ��Ʈ�� �Լ��� �Ű������� �Ѱ��ش�.

	//HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
	/*
	CreateMutex �� bInitialOwner �Ű������� �ռ� ������ ��� �� �Լ��� ȣ���� �����尡 �����Ǵ� ���ؽ��� ������ �������� ���θ� �����Ѵ�.
	���� �� �ҽ����� �� �Ű������� TRUE�� �Ѱ��ָ� CreateMutex�� ȣ���� ���α׷��� ���� �����尡
	���Ӱ� ������ �� ���ؽ��� ������ ���ÿ� �����ϰ� �ȴ�.
	���� ��Ʈ�� �Լ� ������ WaitForXXX�� ȣ���ϰ� ��� ���� 10���� ��������� ���� �� ���ؽ��� �������� ȹ������ ���ϰ�
	������ ����ϰ� �ȴ�.
	*/

	HANDLE arhThreads[MAX_THR_CNT];
	for (int i = 0; i < MAX_THR_CNT; i++)
	{
		DWORD dwTheaID = 0;
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, hMutex, 0, &dwTheaID);
	}

	//if (ReleaseMutex(hMutex) == FALSE)
	//	printf("~~~ ReleaseMutex error, code=%d\n", GetLastError());

	WaitForMultipleObjects(MAX_THR_CNT, arhThreads, TRUE, INFINITE);
	// ������ ��� �����尡 ����� ������ ����Ѵ�.

	for (int i = 0; i < MAX_THR_CNT; i++)
		CloseHandle(arhThreads[i]);
	CloseHandle(hMutex);
	cout << "======= End Mutex Test ==========" << endl;
}

