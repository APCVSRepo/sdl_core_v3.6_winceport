#include <string> 
#include <Windows.h>

#ifndef ACCESSORY_DLL_H
#define ACCESSORY_DLL_H

//Input process name to Accessory DLL
typedef enum CallingProcess
{
	eAOAChannel = 0,
	eIOSChannel,
	eVideoStreamerChannel, // Read shared memory
	eBTChannel,
	eVideoStreamerAPP, // Write shared memory
	eNaviAPP,
	eGracenoteAPP,
	ePandoraAPP,
	eiPodApp,
	eSDLApp,
	enumCPNum
}enumCallingProcess, enumDataDirection;

typedef void (CALLBACK* ONRECEIVEDATA)(enumCallingProcess iMsgType, const BYTE* chBuff, int iBufLen);

struct AccessoryInfo {
	// DLL version
	int version;
	// specify calling process
	IN enumCallingProcess eCallPro;
	// specify call back function
	ONRECEIVEDATA pCallbackfunc;
};

//CALLBACK function definition
typedef void (*FUN_AccessoryAPI_Init)(OUT struct AccessoryInfo* functions);
typedef void (*FUN_AccessoryAPI_DeInit)(enumDataDirection eCallPro);
typedef size_t (*FUN_AccessoryAPI_Write)(enumDataDirection eCallPro, enumDataDirection iDirection, const BYTE* data, size_t dataLen);

const TCHAR Accessory_DLL[] = _T("\\NandFlash\\Application\\iPod\\AccessoryDLL.dll");

class CAccessory_DLL
{
public:
	/**
	 * Construction. Initialize all the variables
	 */
	CAccessory_DLL(void)
	{
		m_hDllModule = NULL;
		m_bIsInit = false;
		pRegister = NULL;
		pDeDeInit = NULL;
		pWriteAccessory = NULL;
	}
	/**
	 * Deconstruction. Release library and initialize the variables
	 */
	~CAccessory_DLL(void)
	{
		FreeLibrary();
		m_hDllModule = NULL;
		m_bIsInit = false;
		pRegister = NULL;
		pDeDeInit = NULL;
		pWriteAccessory = NULL;
	}
	/**
	 * Prepare the interface and register
	 */
	BOOL AccessoryAPI_Init(AccessoryInfo* accessoryInfo)
	{
		if (m_bIsInit)
		{
			return TRUE;
		}
		m_bIsInit = true;
		if (_LoadLibrary() == FALSE)
		{
			return FALSE;
		}
		
		pRegister = (FUN_AccessoryAPI_Init)GetProcAddress(m_hDllModule, _T("AccessoryAPI_Init"));
		pDeDeInit = (FUN_AccessoryAPI_DeInit)GetProcAddress(m_hDllModule, _T("AccessoryAPI_DeInit"));
		pWriteAccessory = (FUN_AccessoryAPI_Write)GetProcAddress(m_hDllModule, _T("AccessoryAPI_Write"));

		if (pRegister == NULL || pDeDeInit == NULL || pWriteAccessory == NULL)
		{
			return FALSE;
		}
		pRegister(accessoryInfo);
		return TRUE;
	}
	
	BOOL AccessoryAPI_DeInit(enumDataDirection eCallPro)
	{
		if (pDeDeInit)
		{
			pDeDeInit(eCallPro);
		}
		return TRUE;
	}
	BOOL  AccessoryAPI_Write(enumDataDirection eCallPro, enumDataDirection iDirection, const BYTE* data, size_t dataLen)
	{
		if (pWriteAccessory)
		{
			pWriteAccessory(eCallPro, iDirection, data, dataLen);
		}
		return TRUE;
	}

	/**
	 * Release library IAPFilter.dll
	 */
	void FreeLibrary()
	{
		if (m_hDllModule != NULL)
		{
			::FreeLibrary(m_hDllModule); 
			m_hDllModule = NULL; 
		}
		m_bIsInit = false;
		pRegister = NULL;
		pDeDeInit = NULL;
		pWriteAccessory = NULL;
	}

private:
	/**
	 * Load library IAPFilter.dll
	 */
	BOOL _LoadLibrary()
	{
		if (m_hDllModule == NULL)
		{
			std::wstring dll_path = Accessory_DLL;
			m_hDllModule = ::LoadLibrary( dll_path.c_str() ); 
			if (m_hDllModule == NULL)
			{
				RETAILMSG(1, (L"Load library %s failed, error code %d", dll_path.c_str(), GetLastError()));
				return FALSE; 
			}
		}
		return TRUE;
	}

public:
	HMODULE m_hDllModule;
	bool m_bIsInit;
	FUN_AccessoryAPI_Init pRegister;
	FUN_AccessoryAPI_DeInit pDeDeInit;
	FUN_AccessoryAPI_Write pWriteAccessory;
};

#endif // ACCESSORY_DLL_H
