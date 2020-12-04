#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


class MyClass
{
	static LONG ExpFilter(LPEXCEPTION_POINTERS pEx);
	static DWORD WINAPI MyThreadProc(PVOID pParam);
	void InnerThreadProc();
	// Ŭ���� ������ ������ ��Ʈ�� �Լ��� ���� �Լ��� �����Ѵ�.
	// �׸��� Ŭ���� ��� ������ ���� ����� ���� ������ ��Ʈ�� �Լ� ���ο��� ȣ����
	// ��Ʈ�� �Լ��� ������ ����� InnerThreadProc �Լ��� �����Ѵ�.

	HANDLE	m_hThread;
	DWORD	m_dwThreadId;
	bool	m_bExit;
	DWORD	m_dwDelay;

public:
	MyClass() : m_hThread(NULL), m_bExit(false), m_dwDelay(1000)
	{
	}
	~MyClass()
	{
		if (m_hThread != NULL)
			CloseHandle(m_hThread);
	}

public:
	HRESULT Start()
	{
		m_hThread = CreateThread(NULL, 0, MyThreadProc, this, 0, &m_dwThreadId);	// �����带 �����Ѵ�.
		//���� �� �Ű������� �� Ŭ������ �ν��Ͻ� ������, �� this�� �Ѱ��ش�.
		if (m_hThread == NULL)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

	void Stop()
	{
		m_bExit = true;	// �����忡�� ����� �����ϴ� ������ �Ѵ�.
		while (m_dwThreadId > 0)
			Sleep(100);	// �����尡 ����� ������ ��� - �����尡 ����� ������ ����ϴ� ó���� �߿�!
			// ���� ��� ������ ���� ��� ó��
	}
};


// ���� ���� ��� �Լ� ����:
LONG MyClass::ExpFilter(LPEXCEPTION_POINTERS pEx)
{
	PEXCEPTION_RECORD pER = pEx->ExceptionRecord;
	printf("~~~~ Exception : Code = %x, Address = %p",
		pER->ExceptionCode, pER->ExceptionAddress);
	// ���� ���� ������ �����ϰ� ���

	return EXCEPTION_EXECUTE_HANDLER;
}


// ������ ��Ʈ�� �Լ� ����:
DWORD WINAPI MyClass::MyThreadProc(PVOID pParam)
{
	MyClass* pThis = (MyClass*)pParam;
	__try
	{
		pThis->InnerThreadProc();	// ������ ��Ʈ�� �Լ��� ����
		// �Ű������� ���޹��� this �����͸� Ÿ�� ��ȯ�� ���Ͽ� ������ ������ �븮�ϴ� ��� �Լ� InnerThreadProc�� ȣ��
	}
	__except (ExpFilter(GetExceptionInformation()))
	{
		pThis->m_dwThreadId = 0;
		// SEH�� �̿��� ���� �߻� ���� ó���� �߰�
		// ������ ���� ó�� �� m_dwThreadId�� �̿��ϱ� ������,
		// ���� �ÿ��� ���� ���Ḧ ���� ���� ó������ m_dwThreadId�� 0���� �������ش�.
	}
	return 0;
}

void MyClass::InnerThreadProc()
{
	while (!m_bExit)	// �������� ���� ������ �ޱ� ���� m_bExit�� ����Ѵ�.
	// m_bExit�� true�� ������ Ż���Ѵ�.
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		printf("Thread %d, delay %d => %5d/%02d/%02d-%02d:%02d:%02d+%03d\n",
			m_dwThreadId, m_dwDelay, st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		Sleep(m_dwDelay);
		// �������� �۾� ó�� �κ��� Sleep�� ���� ���ķ��̼��Ѵ�.
	}
	m_dwThreadId = 0;
	// �����尡 ����Ǿ����� �˸��� ���� m_dwThreadId�� 0���� �����Ѵ�.
	// �ҿ����� ó������ ���� �߿��� �κ��̴�.
}


// ���� �Լ� ����:
void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	MyClass mc;
	HRESULT hr = mc.Start();
	// MyClass�� �����ϰ� Start ��� �Լ��� ȣ���� ���� �����带 �����Ѵ�.

	if (FAILED(hr))
	{
		cout << "MyClass start failed, error code is " << hr;
		return;
	}

	getchar();

	mc.Stop();
	// ������ Ű�� �Է��� ������ Stop�� ȣ���� MyClass�� ������ ������ �����Ѵ�.
	// Stop ���� ������ ���� ��� ó���� ��� �ִ�.

	cout << "Main thread creating sub thread..." << endl;
}
