/*	������ �� ������ ������ ���� �̺�Ʈ ��� ����
���� �Լ����� �ַܼκ��� �Է¹޾� ���� ���۸� ���� ���� �����忡�� �� ������ �������ش�.
�ܼ� �Է��� "time"�̸� ���� �ð��� ���ؼ� SYSTEMTIME ����ü�� ��� ���� ���ۿ� ����,
"point"�� POINT ����ü�� x, y ��ǥ�� �������� �����ϰ� �� POINT ����ü�� ���� ���ۿ� ����.
�� �̿��� ���� ���ڿ��� �����ؼ� �� ���ڿ��� ���� ���ۿ� ���� ���� �����忡�� �����Ѵ�.
*/

#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

#define CMD_NONE 0
#define CMD_STR 1	 // ���ڿ�
#define CMD_POINT 2	 // POINT ����ü
#define CMD_TIME 3	 // SYSTMETIME ����ü
#define CMD_EXIT 100 // ���� ���

struct WAIT_ENV
{
	HANDLE _hevSend;   // ���� �Ϸ� ������ �̺�Ʈ
	HANDLE _hevResp;   // �б� �Ϸ� ������ �̺�Ʈ
	BYTE _arBuff[256]; // ���� ����
};

DWORD WINAPI WorkerProc(LPVOID pParam)
{
	WAIT_ENV *pwe = (WAIT_ENV *)pParam;
	DWORD dwThrId = GetCurrentThreadId();

	while (true)
	{
		DWORD dwWaitCode = WaitForSingleObject(pwe->_hevSend, INFINITE);

		// ���� �Ϸ� ������ �̺�Ʈ�� ���� ����Ѵ�. ���� ������ CMD_EXIT ������� ������ ���޵ȴ�.
		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}

		PBYTE pIter = pwe->_arBuff;
		LONG lCmd = *((PLONG)pIter);
		pIter += sizeof(LONG);
		if (lCmd == CMD_EXIT)
			break; // ���� ���ۿ��� ����� �о�鿩 ���� ����̸� �ٷ� ������ Ż���Ѵ�.

		LONG lSize = *((PLONG)pIter);
		pIter += sizeof(LONG);
		PBYTE pData = new BYTE[lSize + 1]; // lSize + 1 ��ŭ�� ���۸� �Ҵ��� ������ ���ڿ��� ��� ������ NULL ���ڸ� �����ϱ� ����
		memcpy(pData, pIter, lSize);	   // ���� ���ۿ��� ������ ũ�⸦ ��� �� ũ�⸸ŭ �ӽ� ���۸� �Ҵ��� �� ������ �κ��� �����Ѵ�.
		SetEvent(pwe->_hevResp);		   // ���� ���ۿ��� �����͸� ��� �о���� ���� ������� �Ͽ��� ���� ����� ������ �� �ֵ��� �б� �Ϸ� ������ �̺�Ʈ�� �ñ׳θ��Ѵ�.
		// �׸��� ��� ó�� ������� �о���� �����Ϳ� ���� ��� �������� ó���� �����Ѵ�.
		// �� �������� ���� �������� ��� ���� ó���� �� �������� �̹� ���ŵ� ��ɿ� ���� �۾� ó���� ���ÿ� ����ȴ�.

		switch (lCmd)
		{
			// ���ڿ� �����Ϳ� ���� ��� ó��
		case CMD_STR:
		{
			pData[lSize] = 0;
			printf("  <== R-TH %d read STR : %s\n", dwThrId, pData);
		}
		break;

		// POINT ����ü ���� ���� ��� ó��
		case CMD_POINT:
		{
			PPOINT ppt = (PPOINT)pData;
			printf("  <== R-TH %d read POINT : (%d, %d)\n", dwThrId, ppt->x, ppt->y);
		}
		break;

		// SYSTEMTIME ����ü ���� ���� ��� ó��
		case CMD_TIME:
		{
			PSYSTEMTIME pst = (PSYSTEMTIME)pData;
			printf("  <== R-TH %d read TIME : %04d-%02d-%02d %02d:%02d:%02d+%03d\n",
				   dwThrId, pst->wYear, pst->wMonth, pst->wDay, pst->wHour,
				   pst->wMinute, pst->wSecond, pst->wMilliseconds);
		}
		break;
		}
		// ���� ���۷κ��� �����͸� ������ �ӽ� ���۸� �����Ѵ�.
		delete[] pData;
	}
	cout << " *** WorkerProc Thread exits..." << endl;

	return 0;
}

void _tmain()
{
	cout << "======= Start EventNotify Test ========" << endl;
	cout << "  [1] \"time\"" << endl;
	cout << "  [2] \"point\"" << endl;
	cout << "  [3] Other User Input String" << endl;
	cout << "  [4] \"quit\"" << endl
		 << endl;

	WAIT_ENV we;
	we._hevSend = CreateEvent(NULL, FALSE, FALSE, NULL);
	we._hevResp = CreateEvent(NULL, FALSE, FALSE, NULL);
	// ���� �Ϸ� �̺�Ʈ�� �б� �Ϸ� �̺�Ʈ�� ���� �ڵ� ���� �̺�Ʈ�� �����Ѵ�.

	DWORD dwThrID;
	HANDLE hThread = CreateThread(NULL, 0, WorkerProc, &we, 0, &dwThrID);
	// �����带 �����ϰ� WAIT_ENV�� �����͸� �Ű������� �ѱ��.

	char szIn[512];
	while (true)
	{
		cin >> szIn;
		// �ַܼκ��� �Է¹��� ���ڿ��� "quit"�� ��� ���α׷� ���Ḧ ���� ������ Ż���Ѵ�.
		if (_stricmp(szIn, "quit") == 0)
			break;

		LONG lCmd = CMD_NONE, lSize = 0;
		PBYTE pIter = we._arBuff + sizeof(LONG) * 2;
		// ���� ���� ������ ���ؼ� �����͸� ���� ä��� ���� �����͸� �����Ѵ�.

		if (_stricmp(szIn, "time") == 0)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			memcpy(pIter, &st, sizeof(st));
			lCmd = CMD_TIME, lSize = sizeof(st);
			// "time"�� ��� ���� �ð��� ���ؼ� ���� ���ۿ� �����ϰ�, ��� ������ SYSTEMTIME ����ü�� ũ�⸦ �����Ѵ�.
		}
		else if (_stricmp(szIn, "point") == 0)
		{
			POINT pt;
			pt.x = rand() % 1000;
			pt.y = rand() % 1000;
			*((PPOINT)pIter) = pt;
			lCmd = CMD_POINT, lSize = sizeof(pt);
			// "point"�� ��� ���� ������ ���� x,y ��ǥ�� ���ؼ� POINT ����ü�� ������ �� ���� ���ۿ� ����.
			// ���� ��� ������ POINT ����ü�� ũ�⸦ �����Ѵ�.
		}
		else
		{
			lSize = strlen(szIn);
			memcpy(pIter, szIn, lSize);
			lCmd = CMD_STR;
			// ���ڿ��� ��� ���ڿ��� ���̸� ��� �� ���̸�ŭ ���� ���ۿ� ������ �� CMD_STR ����� �����Ѵ�.
		}
		((PLONG)we._arBuff)[0] = lCmd;
		((PLONG)we._arBuff)[1] = lSize;
		// ���� ������ �� �� �κп� ��� ������ ������ ���̸� �����Ѵ�.
		// �̰����� ���� ���� ������ �������Ǿ����Ƿ� ���� ���� �Ϸ� ������ �ؾ� �Ѵ�.

		SignalObjectAndWait(we._hevSend, we._hevResp, INFINITE, FALSE);
		// ���� �Ϸ� ������ ���Ҿ� �б� �Ϸ� ������ ���� ����Ѵ�.
	}

	*((PLONG)we._arBuff) = CMD_EXIT;
	SignalObjectAndWait(we._hevSend, hThread, INFINITE, FALSE);
	// ��� ó�� �����尡 ����Ǳ⸦ ��ٸ���.
	CloseHandle(hThread);

	CloseHandle(we._hevResp);
	CloseHandle(we._hevSend);
	cout << "======= End EventNotify Test ==========" << endl;
}
