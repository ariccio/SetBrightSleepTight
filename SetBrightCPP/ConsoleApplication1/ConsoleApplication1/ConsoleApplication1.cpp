//Additional dependency? wbemuuid.lib
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#pragma strict_gs_check(on)

#include "ConsoleApplication1.h"

#include <objbase.h>
#include <windows.h>
#include <stdio.h>
#include <wbemidl.h>
#include <comdef.h>
#include <PhysicalMonitorEnumerationAPI.h>
#include <HighLevelMonitorConfigurationAPI.h>
#include <LowLevelMonitorConfigurationAPI.h>
#include <tchar.h>
//#include <dxva2api.h>
#include <stdio.h>
#include <iostream>


//define SCOPE_GUARD_DEBUGGING for enhanced scope guard debugging.
#include "ScopeGuard.h"

//what the fuck?
void printError( _In_z_ PCSTR const msg ) {
	DWORD eNum;
	TCHAR sysMsg[ 256 ];
	eNum = GetLastError( );
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, eNum, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), sysMsg, 256, NULL );
	std::cout << std::endl << msg << "--Failed with error: " << eNum << " (" << sysMsg << ")" << std::endl << std::endl;
	}

//what the double fuck?
void printError() {
	char msg[1] = { };
	printError( msg );
	}


bool ddcGetBrightness ( int getBrightInt ) {
	//HMONITOR hMonitor = NULL;

	DWORD pdwMinimumBrightness = 0;
	DWORD pdwCurrentBrightness = 0;
	DWORD pdwMaximumBrightness = 0;

	const HWND hwnd = FindWindowW( NULL, NULL );
	std::cout << "Window handle: " << hwnd << std::endl;

	const HMONITOR hMonitor = MonitorFromWindow( hwnd, MONITOR_DEFAULTTONULL );
	std::cout << "hMonitor: " << hMonitor << std::endl;

	//LPSTR pszASCIICapabilitiesString = NULL;

	DWORD cchStringLength = NULL;
	//cchStringLength = NULL;
	//C6102 warning BUG? http://www.beta.microsoft.com/VisualStudio/feedback/details/812312/incorrect-code-analysis-warning-c6102

	if ( GetCapabilitiesStringLength( hMonitor, &cchStringLength ) ) {
		std::cout << "GetCapabilitiesStringLength Succeeded!!" << std::endl;
		if ( ( cchStringLength > 0 ) && ( cchStringLength != NULL ) ) {
			LPSTR szCapabilitiesString = ( LPSTR ) malloc( cchStringLength );

			if ( szCapabilitiesString != NULL ) {

				BOOL capabilitiesRequestAndCapabilitiesReplySucces = CapabilitiesRequestAndCapabilitiesReply( hwnd, szCapabilitiesString, cchStringLength );

				if ( capabilitiesRequestAndCapabilitiesReplySucces ) {
					std::cout << "szCapabilitiesString: " << szCapabilitiesString << std::endl;
					}

				else {
					std::cout << "CapabilitiesRequestAndCapabilitiesReply failed!" << std::endl;
					}
				}
			else {
				std::cout << "Failed before CapabilitiesRequestAndCapabilitiesReply (because szCapabilitiesString == NULL)!!" << std::endl;
				}
			}
		else {
			std::cout << "\t...but cchStringLength was invalid" << std::endl;
			std::cout << "\tinvalid cchStringLength: '" << cchStringLength << "'" << std::endl;
			std::cout << "\tfull windows error message: " << std::endl;
			printError();
			}
		}
	else {
		std::cout << "Failed to GetCapabilitiesStringLength!!" << std::endl;
		std::cout << "\full windows error message: " << std::endl;
		printError( );
		return false;
		}
	BOOL getBSuccess = GetMonitorBrightness( hMonitor, &pdwMinimumBrightness, &pdwCurrentBrightness, &pdwMaximumBrightness );

	if ( getBSuccess == TRUE ) {
		std::cout << pdwMinimumBrightness << std::endl;
		std::cout << pdwCurrentBrightness << std::endl;
		std::cout << pdwMaximumBrightness << std::endl;
		
		if ( !( pdwCurrentBrightness == NULL ) ) {
			getBrightInt = pdwCurrentBrightness;
			}

		return true;
		}
	else {
		std::cout << "GetMonitorBrightness failed!" << std::endl;
		return false;
		}
	}

bool ddcSetBrightness ( const DWORD dwNewBrightness) {
	//HMONITOR hMonitor = NULL;

	//DWORD pdwMinimumBrightness = 0;
	//DWORD pdwCurrentBrightness = 0;
	//DWORD pdwMaximumBrightness = 0;

	const HWND hwnd = FindWindowW( NULL, NULL );
	std::cout << "Window handle: " << hwnd << std::endl;

	const HMONITOR hMonitor = MonitorFromWindow( hwnd, MONITOR_DEFAULTTONULL );
	std::cout << "hMonitor: " << hMonitor << std::endl;


	const BOOL setBSuccess = SetMonitorBrightness( hMonitor,  dwNewBrightness);

	if ( setBSuccess == TRUE ) {
		std::cout << "SetMonitorBrightness " << dwNewBrightness << " succeeded!" << std::endl;
		return true;
		}
	std::cout << "SetMonitorBrightness " << dwNewBrightness << " failed!" << std::endl;
	return false;

	}

_Success_( return != -1 )
int GetBrightness ( ) {

	BSTR path      = SysAllocString( L"root\\wmi" );
	if ( path == NULL ) {
		printf( "failed to allocate path BSTR!\r\n" );
		return -1;
		}

	auto pathguard = SCOPEGUARD_INSTANCE( [ &] { SysFreeString( path ); path = NULL; } );

	BSTR ClassPath = SysAllocString( L"WmiMonitorBrightness" );
	if ( ClassPath == NULL ) {
		printf( "failed to allocate ClassPath BSTR!\r\n" );
		return -1;
		}

	auto classPathguard = SCOPEGUARD_INSTANCE( [ &] { SysFreeString( ClassPath ); ClassPath = NULL; } );


	BSTR bstrQuery = SysAllocString( L"Select * from WmiMonitorBrightness" );
	if ( bstrQuery == NULL ) {
		printf( "failed to allocate bstrQuery BSTR!\r\n" );
		return -1;
		}

	auto queryGuard = SCOPEGUARD_INSTANCE( [ &] { SysFreeString( bstrQuery ); bstrQuery = NULL; } );

	// Initialize COM and connect up to CIMOM
	const HRESULT initResult = CoInitialize( 0 );
	if ( FAILED ( initResult ) ) {
		printf( "Failed to initialize COM!\r\n" );
		if ( initResult == S_FALSE ) {
			CoUninitialize( );
			}
		return -1;
		}

	auto comGuard = SCOPEGUARD_INSTANCE( [ &] { CoUninitialize( ); } );

	//  NOTE:
	//  When using asynchronous WMI API's remotely in an environment where the "Local System" account has no network identity (such as non-Kerberos domains), the authentication level of RPC_C_AUTHN_LEVEL_NONE is needed. However, lowering the authentication level to RPC_C_AUTHN_LEVEL_NONE makes your application less secure. It is wise to use semi-synchronous API's for accessing WMI data and events instead of the asynchronous ones.

	const HRESULT initSecurityResult = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_SECURE_REFS, NULL );
	//change EOAC_SECURE_REFS to EOAC_NONE if you change dwAuthnLevel to RPC_C_AUTHN_LEVEL_NONE
	if ( FAILED( initSecurityResult ) ) {
		printf( "Failed to initialize security!\r\n" );
		return -1;
		}

	

	PVOID pLocator_temp = NULL;
	const HRESULT createInstanceResult = CoCreateInstance( CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, &pLocator_temp );
	if ( FAILED ( createInstanceResult ) ) {
		printf( "CoCreateInstance failed!\r\n" );
		return -1;
		}

	IWbemLocator* pLocator = static_cast<IWbemLocator*>( pLocator_temp );

	auto locatorGuard = SCOPEGUARD_INSTANCE( [ &] { pLocator->Release( ); } );


	IWbemServices* pNamespace = NULL;

	const HRESULT connectServerResult = pLocator->ConnectServer( path, NULL, NULL, NULL, 0, NULL, NULL, &pNamespace );
	if ( connectServerResult != WBEM_S_NO_ERROR ) {
		printf( "ConnectServer failed!\r\n" );
		return -1;
		}


	auto namespaceGuard = SCOPEGUARD_INSTANCE( [ &] { pNamespace->Release( ); } );

	const HRESULT setProxyBlanketResult = CoSetProxyBlanket( pNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );

	if ( setProxyBlanketResult != WBEM_S_NO_ERROR ) {
		printf( "CoSetProxyBlanket failed!\r\n" );
		return -1;
		}

	//Following comment kept for hilarity.
	//Yes, I'd really never seen that operator before :)

	/*
	See http://stackoverflow.com/a/1238643/625687 for info on usage of arrow ('->')
	ExecQuery:
		WQL:							Query Language
		bstrQuery:						Query to Execute
		WBEM_FLAG_RETURN_IMMEDIATELY:	Make a semi-synchronous call
		NULL:							Context
		pEnum:							Enumeration Interface
	*/
	
	IEnumWbemClassObject *pEnum = NULL;

	const HRESULT execQueryResult = pNamespace->ExecQuery (
								_bstr_t ( L"WQL" ),
								bstrQuery,
								WBEM_FLAG_RETURN_IMMEDIATELY,
								NULL,
								&pEnum
								);

	if ( execQueryResult != WBEM_S_NO_ERROR ) {
		printf( "ExecQuery failed!!\r\n" );
		goto cleanup;
		}

	
	hr = WBEM_S_NO_ERROR;

	while ( WBEM_S_NO_ERROR == hr ) {
		int ret = -1;
		ULONG ulReturned;
		IWbemClassObject* pObj;

		/*
		Next: 
			Get the Next Object from the collection
				WBEM_INFINITE:	Timeout
				1:				Number of objects requested
				pObj:			Returned Object
				ulReturned:		Number of object returned
		*/
		hr = pEnum->Next( WBEM_INFINITE, 1, &pObj, &ulReturned );

		if ( hr != WBEM_S_NO_ERROR ) {
			return -1;
			}

		VARIANT var1;
		hr = pObj->Get (
						_bstr_t ( L"CurrentBrightness" ),
						0,
						&var1,
						NULL,
						NULL
						);

		ret = V_UI1 ( &var1 );
		VariantClear ( &var1 );
		if ( hr != WBEM_S_NO_ERROR ) {
			return ret;
			}
		}

cleanup:
	return -1;
	}

bool SetBrightness( int val ) {
	
	std::cout << "Attempting to set brightness " << val << " via WMI" << std::endl;
	bool bRet = true;
	
	IWbemLocator         *pLocator   = NULL;
	IWbemClassObject     *pClass     = NULL;
	IWbemClassObject     *pInClass   = NULL;
	IWbemClassObject     *pInInst    = NULL;
	IEnumWbemClassObject *pEnum      = NULL;
	IWbemServices        *pNamespace = 0;
	//HRESULT hr = S_OK;

	BSTR path       = SysAllocString( L"root\\wmi" );
	BSTR ClassPath  = SysAllocString( L"WmiMonitorBrightnessMethods" );
	BSTR MethodName = SysAllocString( L"WmiSetBrightness" );
	BSTR ArgName0   = SysAllocString( L"Timeout" );
	BSTR ArgName1   = SysAllocString( L"Brightness" );
	BSTR bstrQuery  = SysAllocString( L"Select * from WmiMonitorBrightnessMethods" );

	if ( !path || !ClassPath || !MethodName || !ArgName0 ) {
		std::cout << "\tSomething went wrong when initializing path, ClassPath, MethodName, and ArgName0." << std::endl;
		bRet = false;
		goto cleanup;
		}

	// Initialize COM and connect up to CIMOM

	const HRESULT initResult = CoInitialize( 0 );
	if ( FAILED( initResult ) ) {
		std::cout << "\tSomething went wrong in CoInitialize!" << std::endl;
		bRet = false;
		goto cleanup;
		}

	//  When using asynchronous WMI API's remotely in an environment where the "Local System" account has no network identity (such as non-Kerberos domains), the authentication level of RPC_C_AUTHN_LEVEL_NONE is needed. However, lowering the authentication level to RPC_C_AUTHN_LEVEL_NONE makes your application less secure. It is wise to use semi-synchronous API's for accessing WMI data and events instead of the asynchronous ones.

	const HRESULT initSecurity = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_SECURE_REFS, NULL );
	if ( FAILED( initSecurity ) )
	{
		printf( "Failed to init security!\r\n" );
		goto cleanup;
	}
	//change EOAC_SECURE_REFS to EOAC_NONE if you change dwAuthnLevel to RPC_C_AUTHN_LEVEL_NONE
	const HRESULT createInstanceResult = CoCreateInstance( CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, ( LPVOID * ) &pLocator );
	if ( FAILED( createInstanceResult ) ) {
		std::cout << "\tSomething went wrong in CoCreateInstance!" << std::endl;
		bRet = false;
		goto cleanup;
		}

	const HRESULT connectServerResult = pLocator->ConnectServer( path, NULL, NULL, NULL, 0, NULL, NULL, &pNamespace );
	if ( connectServerResult != WBEM_S_NO_ERROR ) {
		std::cout << "\tSomething went wrong in pLocator->ConnectServer!" << std::endl;
		bRet = false;
		goto cleanup;
		}

	const HRESULT setBlanketResult = CoSetProxyBlanket( pNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );

	if ( setBlanketResult != WBEM_S_NO_ERROR ) {
		std::cout << "\tSomething went wrong in CoSetProxyBlanket!" << std::endl;
		bRet = false;
		goto cleanup;
		}

	//Following comment kept for hilarity.
	//Yes, I'd really never seen that operator before :)
	/*
	See http://stackoverflow.com/a/1238643/625687 for info on usage of arrow ('->')
	ExecQuery:
	WQL:							Query Language
	bstrQuery:						Query to Execute
	WBEM_FLAG_RETURN_IMMEDIATELY:	Make a semi-synchronous call
	NULL:							Context
	pEnum:							Enumeration Interface
	*/

	const HRESULT execQueryResult = pNamespace->ExecQuery( 
								_bstr_t( L"WQL" ),
								bstrQuery,
								WBEM_FLAG_RETURN_IMMEDIATELY,
								NULL,
								&pEnum
								);

	if ( execQueryResult != WBEM_S_NO_ERROR ) {
		std::cout << "\tSomething went wrong in pNamespace->ExecQuery!" << std::endl;
		bRet = false;
		goto cleanup;
		}

	hr = WBEM_S_NO_ERROR;

	while ( WBEM_S_NO_ERROR == hr ) {
		ULONG ulReturned;
		IWbemClassObject *pObj;

		//Get the Next Object from the collection
		hr = pEnum->Next(	WBEM_INFINITE, //Timeout
							1, //No of objects requested
							&pObj, //Returned Object
							&ulReturned //No of object returned
							);

		if ( hr != WBEM_S_NO_ERROR ) {
			std::cout << "\tSomething went wrong in pEnum->Next!" << std::endl;
			bRet = false;
			goto cleanup;
			}

		// Get the class object
		hr = pNamespace->GetObject( ClassPath, 0, NULL, &pClass, NULL );
		if ( hr != WBEM_S_NO_ERROR ) {
			std::cout << "\tSomething went wrong in pNamespace->GetObject!" << std::endl;
			bRet = false;
			goto cleanup;
			}

		// Get the input argument and set the property
		hr = pClass->GetMethod( MethodName, 0, &pInClass, NULL );
		if ( hr != WBEM_S_NO_ERROR ) {
			std::cout << "\tSomething went wrong in pClass->GetMethod!" << std::endl;
			bRet = false;
			goto cleanup;
			}

		hr = pInClass->SpawnInstance( 0, &pInInst );
		if ( hr != WBEM_S_NO_ERROR ) {
			std::cout << "\tSomething went wrong in pInClass->SpawnInstance!" << std::endl;
			bRet = false;
			goto cleanup;
			}

		VARIANT var1;
		VariantInit( &var1 );

		V_VT( &var1 ) = VT_BSTR;
		V_BSTR( &var1 ) = SysAllocString( L"0" );
		hr = pInInst->Put( ArgName0, 0, &var1, CIM_UINT32 ); //CIM_UINT64

		VariantClear( &var1 );
		if ( hr != WBEM_S_NO_ERROR ) {
			std::cout << "\tSomething went wrong in pInInst->Put!" << std::endl;
			bRet = false;
			goto cleanup;
			}

		VARIANT var;
		VariantInit( &var );

		V_VT( &var ) = VT_BSTR;
		WCHAR buf[ 10 ] = { 0 };
		_stprintf_s( buf, _countof( buf ), L"%ld", val );
		V_BSTR( &var ) = SysAllocString( buf );
		hr = pInInst->Put( ArgName1, 0, &var, CIM_UINT8 );

		VariantClear( &var );
		
		if ( hr != WBEM_S_NO_ERROR ) {
			std::cout << "\tSomething went wrong in pInInst->Put!" << std::endl;
			bRet = false;
			goto cleanup;
			}
		
		// Call the method
		VARIANT pathVariable;
		VariantInit( &pathVariable );

		hr = pObj->Get( _bstr_t( L"__PATH" ), 0, &pathVariable, NULL, NULL );
		
		if ( hr != WBEM_S_NO_ERROR ) {
			std::cout << "\tSomething went wrong in pObj->Get!" << std::endl;
			bRet = false;
			goto cleanup;
			}
		
		hr = pNamespace->ExecMethod( pathVariable.bstrVal, MethodName, 0, NULL, pInInst, NULL, NULL );
		
		VariantClear( &pathVariable );
		
		if ( hr != WBEM_S_NO_ERROR ) {
			std::cout << "\tSomething went wrong in pNamespace->ExecMethod!" << std::endl;
			bRet = false;
			goto cleanup;
			}
		}

	cleanup:
		std::cout << "initiated SetBrightness cleanup!" << std::endl;
		SysFreeString( path       );
		SysFreeString( ClassPath  );
		SysFreeString( MethodName );
		SysFreeString( ArgName0   );
		SysFreeString( ArgName1   );
		SysFreeString( bstrQuery  );

		if ( pClass		)	{ pClass->Release( );	 }
		if ( pInInst	)	{ pInInst->Release( );   }
		if ( pInClass	)	{ pInClass->Release( );  }
		if ( pLocator	)	{ pLocator->Release( );  }
		if ( pNamespace )	{ pNamespace->Release( );}
	
		CoUninitialize ( );

		return bRet;
	}

void main ( ) {
	int getBrightInt = NULL;

	HMODULE hModule = ::GetModuleHandleW( NULL );
	if ( hModule == NULL ) {
		printf( "Fatal Error: GetModuleHandle failed\r\n" );
		std::terminate( );
		}

	int ass = GetBrightness(  );

	std::cout << "Got brightness: " << ass << " via WMI" << std::endl;
	Sleep( 100 );
	bool getBrightSucess = ddcGetBrightness( getBrightInt );

	if ( !getBrightSucess ) {
		char msg[] = "Failed to get brightness via DDC/CI!";
		printError( msg );
		Sleep( 100 );
		}	

	else if ( getBrightSucess ) {
		std::cout << "Got brightness: " << getBrightInt << " via DDC/CI" << std::endl;
		Sleep( 100 );
		}
	DWORD newBrightness = 55;
	if ( !ddcSetBrightness( newBrightness ) ) {
		std::cout << "Failed to set brightness " << newBrightness << " via DDC/CI!" << std::endl << std::endl;
		Sleep( 100 );
		}
	else {
		std::cout << "Successfully set brightness " << newBrightness << "via DDC/CI!" << std::endl;
		Sleep( 100 );
		if ( !ddcSetBrightness( DWORD( ass ) ) ) {
			std::cout << "\tFailed to reset brightness to " << ass << "via DDC/CI!" << std::endl << std::endl;
			}
		}
	SetBrightness ( 0 );
	Sleep ( 100 );
	SetBrightness ( 100 );
	Sleep ( 100 );
	SetBrightness ( 0 );
	Sleep ( 100 );
	SetBrightness ( ass );
	}

