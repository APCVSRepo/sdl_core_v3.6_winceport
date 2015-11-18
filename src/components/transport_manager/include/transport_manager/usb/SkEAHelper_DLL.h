#ifndef _SK_EA_HELPER_DLL_H_
#define _SK_EA_HELPER_DLL_H_

#include "string"
#include <Windows.h>

typedef void (*onDeviceConnect)();
typedef void (*onDeviceDisconnect)();
typedef void (*onReceiveData)(const BYTE* chBuff, int iBufLen);

const TCHAR SkEAHelper_DLL[] = _T("\\NandFlash\\Application\\iPod\\SkEAHelper.dll");

class EACallBack
{
public:
	EACallBack() {};
	virtual ~EACallBack() {};
	virtual void onDeviceConnect() = 0;
	virtual void onDeviceDisconnect() = 0;
	virtual void onEASessionConnect() {};
	virtual void onEASessionDisconnect() {};
	virtual void onReceiveData(const BYTE* chBuff, int iBufLen) = 0;
};

//CALLBACK function definition
typedef void (*FUN_EAHelper_Init)(onDeviceConnect pConnect, onDeviceDisconnect pDisconnect, onReceiveData pReceiveData);
typedef void (*FUN_EAHelper_DeInit)();
typedef void (*FUN_EAHelper_Write)(const BYTE* chBuff, int iBufLen);
typedef void (*FUN_EAHelper_LaunchSDL)();


class SkEAHelperDLL
{
public:
	SkEAHelperDLL()
	{
		clear();
	}
	~SkEAHelperDLL()
	{
		freeLibrary();
	}

	bool SP_EAHelperInit(onDeviceConnect pConnect, onDeviceDisconnect pDisconnect, onReceiveData pReceiveData)
	{
		if (m_bInit)
		{
			return TRUE;
		}
		m_bInit = true;
		if (loadLibrary() == FALSE)
		{
			return FALSE;
		}

		pEAHelperInit = (FUN_EAHelper_Init)GetProcAddress(m_hDllModule, _T("SP_EAHelperInit"));
		pEAHelperDeInit = (FUN_EAHelper_DeInit)GetProcAddress(m_hDllModule, _T("SP_EAHelperDeInit"));
		pEAHelperWrite = (FUN_EAHelper_Write)GetProcAddress(m_hDllModule, _T("SP_EAHelperWrite"));
		pEAHelperLaunchSDL = (FUN_EAHelper_LaunchSDL)GetProcAddress(m_hDllModule, _T("SP_EAHelperLaunchSDL"));

		if (pEAHelperInit == NULL || pEAHelperDeInit == NULL || pEAHelperWrite == NULL || pEAHelperLaunchSDL == NULL)
		{
			return FALSE;
		}

		pEAHelperInit(pConnect, pDisconnect, pReceiveData);

		return TRUE;
	}

	void SP_EAHelperDeInit()
	{
		if (pEAHelperDeInit)
		{
			pEAHelperDeInit();
		}
	}

	void SP_EAHelperWrite(const BYTE* chBuff, int iBufLen)
	{
		if (pEAHelperWrite)
		{
			pEAHelperWrite(chBuff, iBufLen);
		}
	}

	void SP_EAHelperLaunchSDL()
	{
		if (pEAHelperLaunchSDL)
		{
			pEAHelperLaunchSDL();
		}
	}


	void freeLibrary()
	{
		if (m_hDllModule != NULL)
		{
			::FreeLibrary(m_hDllModule); 
			m_hDllModule = NULL; 
		}

		clear();
	}

private:
	bool loadLibrary()
	{
		if (m_hDllModule == NULL)
		{
			std::wstring dll_path = SkEAHelper_DLL;
			m_hDllModule = ::LoadLibrary( dll_path.c_str() ); 
			if (m_hDllModule == NULL)
			{
				return FALSE; 
			}
		}
		return TRUE;
	}

	void clear()
	{
		m_hDllModule = NULL;
		m_bInit = false;
		pEAHelperInit = NULL;
		pEAHelperDeInit = NULL;
		pEAHelperWrite = NULL;
		pEAHelperLaunchSDL = NULL;
	}
private:
	HMODULE m_hDllModule;
	bool m_bInit;
	FUN_EAHelper_Init pEAHelperInit;
	FUN_EAHelper_DeInit pEAHelperDeInit;
	FUN_EAHelper_Write pEAHelperWrite;
	FUN_EAHelper_LaunchSDL pEAHelperLaunchSDL;
};



#endif