/*
int main(int argc, char* argv[])
	main �̶�� Ư���Լ��� ��� c�� c++ ���α׷��� ������ ���۵Ǵ� ����


int wmain(int argc, wchar_t* argv[])
	"�����ڵ� ���α׷��ָ��� ������ �ڵ带 �ۼ��ϴ°�� main �� ���̵� ���ڹ����� wmain�� ����� �� �ֽ��ϴ�" (MSDN)


int _tmain(int argc, _TCHAR *argv[])
	_tmain�� _UNICODE�� ���ǵ��� ���� �� main���� Ȯ�εȴ�.
	_UNICODE�� ���ǵ� ��쿡�� _tmain�� wmain���� Ȯ�ε˴ϴ�.
	�����ڵ尡 ���ǵȰ�� �����Ͻ� wmain����, ���ǵ��� ������� main���� �����ϵȴ�.
	��, _tmain ��������ؼ� TCHAR.h ��������� �������־�� �Ѵ�.


int ����� void������ �����ϴ� ���?
	main �� wmain �Լ��� void(��ȯ �� ����)�� ��ȯ�ϴ� ������ ����� �� �ִ�.
	void�� ��ȯ�ϴ� ������ main �Ǵ� wmain�� �����ϴ� ��� return ���� ����Ͽ� �θ� ���μ����� � ü���� ���� �ڵ带 ��ȯ�� �� ����.
	main �Ǵ� wmain�� void�� ��ȯ�ϴ� ������ ����� ���, ���� �ڵ带 ��ȯ�Ϸ��� exit �Լ��� ����ؾ� �Ѵ�.
*/

#include "stdafx.h"
#include "Windows.h"
#include "iostream"

using namespace std;

DWORD WINAPI ThreadProc(PVOID pParam)
{
	DWORD dwDelay = (DWORD)pParam;
	// CreateThread�� �Ű������� �Ѱ���� 5000�̶� ���� pParam �Ű������κ��� ȹ���Ѵ�.

	cout << ">>>>> Thread " << GetCurrentThreadId() << " enter." << endl;
	Sleep(dwDelay);
	cout << "<<<<< Thread " << GetCurrentThreadId() << " leave." << endl;
	// �Ű������� ���޹��� ���� ������ 5�� ���� Sleep ó���� �� �� �����带 �����Ѵ�.

	return dwDelay;
	// ������ ���� �ڵ带 5000���� �����ش�. ���� �Լ����� Ȯ���ߴ� ��ó��, GetExitCodeThread �Լ��� ���� �� ���� ȹ���� �� �ִ�.
	// ��� ��κ��� ��� 0�� ������ ���̴�.
}

void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread
	(
		NULL,			// ���� �ĺ��ڸ� NULL�� �Ѱ��.
		0,				// 0���� �Ѱܼ� ����Ʈ ���� ũ��(1MB)�� �����ߴ�.
		ThreadProc,		// ������ ������ ������ ��Ʈ�� �Լ��� ���� �����Ϳ�
		(PVOID)5000,	// �Ѱ��� �Ű������� �����Ѵ�. �Ű������� 5000 ���� �־���.
		0,				// ������ ���ÿ� ����ǵ��� �����Ѵ�.
		&dwThreadID		// dwThreadID ������ �������� ID�� ����Ǿ� ��ȯ�ȴ�.
	);
	if (hThread == NULL)	// CreateThread �Լ� ȣ�� ����� ������ ��쿡�� NULL�̸�, GetLastError�� ���� ���� �ڵ带 ����Ѵ�.
	{
		cout << "~~~ CreateThread failed, error code : "
			<< GetLastError() << endl;
		return;
	}

	WaitForSingleObject(hThread, INFINITE);

	DWORD dwExitCode = 0;
	GetExitCodeThread(hThread, &dwExitCode);
	// ������ ���� �ڵ�, �� ��Ʈ�� �Լ��� ���ϰ��� ȹ���Ѵ�.

	cout << "Sub thread " << dwThreadID
		<< " terminated with ExitCode " << dwExitCode << endl;
	CloseHandle(hThread);
	// ������ ���� Ŀ�� ��ü�̹Ƿ�, CloseHandle�� ȣ���� �ݾ���� Ŀ�ο� ���� ������ �����ϴ�.
}
