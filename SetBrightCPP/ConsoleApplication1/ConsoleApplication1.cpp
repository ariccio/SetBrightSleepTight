//Additional dependency? wbemuuid.lib
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
#include <dxva2api.h>
#include <stdio.h>
#include <iostream>

using namespace std;


void printError( char msg[ ] ) {
	DWORD eNum;
	TCHAR sysMsg[ 256 ];
	eNum = GetLastError( );
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, eNum, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), sysMsg, 256, NULL );
	cout << endl << msg << " failed with error: " << eNum << " (" << sysMsg << ")" << endl << endl;
	}


bool ddcGetBrightness ( int getBrightInt ) {
	HMONITOR hMonitor = NULL;

	DWORD pdwMinimumBrightness = 0;
	DWORD pdwCurrentBrightness = 0;
	DWORD pdwMaximumBrightness = 0;

	HWND hwnd = FindWindow ( NULL, NULL );
	cout << "Window handle: " << hwnd << endl;

	hMonitor = MonitorFromWindow ( hwnd, MONITOR_DEFAULTTONULL );
	cout << "hMonitor: " << hMonitor << endl;

	//LPSTR pszASCIICapabilitiesString = NULL;


	DWORD cchStringLength = 0;
	BOOL getCapabilitiesStringLengthSuccess = GetCapabilitiesStringLength ( hMonitor, &cchStringLength );

	if ( getCapabilitiesStringLengthSuccess && (cchStringLength != (NULL|| 0)) ) {

		LPSTR szCapabilitiesString = ( LPSTR ) malloc ( cchStringLength );

		if ( szCapabilitiesString != NULL) {

			BOOL capabilitiesRequestAndCapabilitiesReplySucces = CapabilitiesRequestAndCapabilitiesReply ( hwnd, szCapabilitiesString, cchStringLength);

			if ( capabilitiesRequestAndCapabilitiesReplySucces ) { cout << "szCapabilitiesString: " << szCapabilitiesString << endl; }
			
			else												 { cout << "CapabilitiesRequestAndCapabilitiesReply failed!" << endl; }
		}
		else { cout << "Failed before CapabilitiesRequestAndCapabilitiesReply (because szCapabilitiesString == NULL)!!" << endl; }
	}
	else {
		cout << "Failed to GetCapabilitiesStringLength!!" << endl;
		return false;
		}
	BOOL getBSuccess = GetMonitorBrightness ( hMonitor, &pdwMinimumBrightness, &pdwCurrentBrightness, &pdwMaximumBrightness );

	if ( getBSuccess == TRUE ) {
		cout << pdwMinimumBrightness << endl;
		cout << pdwCurrentBrightness << endl;
		cout << pdwMaximumBrightness << endl;
		
		if ( !( pdwCurrentBrightness == NULL ) ) {
			getBrightInt = pdwCurrentBrightness;
			}

		return true;
		}
	else {
		cout << "GetMonitorBrightness failed!" << endl;
		return false;
		}
	}
bool ddcSetBrightness ( ) {
	return true;
	// TODO: write this damned function
	}

int GetBrightness ( ) {
	int ret = -1;

	IWbemLocator         *pLocator    = NULL;
	IEnumWbemClassObject *pEnum       = NULL;
	HRESULT               hr          = S_OK;
	IWbemServices        *pNamespace  = 0;

	BSTR path      = SysAllocString ( L"root\\wmi" );
	BSTR ClassPath = SysAllocString ( L"WmiMonitorBrightness" );
	BSTR bstrQuery = SysAllocString ( L"Select * from WmiMonitorBrightness" );

	if ( !path || !ClassPath || !bstrQuery) {
		cout << "Something went wrong when initializing path, ClassPath, and bstrQuery." << endl;
		goto cleanup;
		}

	// Initialize COM and connect up to CIMOM

	hr = CoInitialize ( 0 );
	if ( FAILED ( hr ) ) {
		goto cleanup;
		}

	//  NOTE:
	//  When using asynchronous WMI API's remotely in an environment where the "Local System" account has no network identity (such as non-Kerberos domains), the authentication level of RPC_C_AUTHN_LEVEL_NONE is needed. However, lowering the authentication level to RPC_C_AUTHN_LEVEL_NONE makes your application less secure. It is wise to use semi-synchronous API's for accessing WMI data and events instead of the asynchronous ones.

	hr = CoInitializeSecurity ( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_SECURE_REFS, NULL );
	//change EOAC_SECURE_REFS to EOAC_NONE if you change dwAuthnLevel to RPC_C_AUTHN_LEVEL_NONE

	hr = CoCreateInstance ( CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, ( LPVOID * ) &pLocator );
	if ( FAILED ( hr ) ) {
		goto cleanup;
		}
	hr = pLocator->ConnectServer ( path, NULL, NULL, NULL, 0, NULL, NULL, &pNamespace );
	if ( hr != WBEM_S_NO_ERROR ) {
		goto cleanup;
		}

	hr = CoSetProxyBlanket ( pNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );

	if ( hr != WBEM_S_NO_ERROR ) {
		goto cleanup;
		}

	/*
	See http://stackoverflow.com/a/1238643/625687 for info on usage of arrow ('->')
	ExecQuery:
		WQL:							Query Language
		bstrQuery:						Query to Execute
		WBEM_FLAG_RETURN_IMMEDIATELY:	Make a semi-synchronous call
		NULL:							Context
		pEnum:							Enumeration Interface
	*/
	hr = pNamespace->ExecQuery (
								_bstr_t ( L"WQL" ),
								bstrQuery,
								WBEM_FLAG_RETURN_IMMEDIATELY,
								NULL,
								&pEnum
								);

	if ( hr != WBEM_S_NO_ERROR ) {
		goto cleanup;
		}

	hr = WBEM_S_NO_ERROR;

	while ( WBEM_S_NO_ERROR == hr ) {
		ULONG ulReturned;
		IWbemClassObject *pObj;

		/*
		Next: 
			Get the Next Object from the collection
				WBEM_INFINITE:	Timeout
				1:				Number of objects requested
				pObj:			Returned Object
				ulReturned:		Number of object returned
		*/
		hr = pEnum->Next ( WBEM_INFINITE, 1, &pObj, &ulReturned );

		if ( hr != WBEM_S_NO_ERROR ) {
			goto cleanup;
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
			goto cleanup;
			}
		}

cleanup:
	cout << "GetBrightness cleanup initiated!" << endl;
	SysFreeString ( path );
	SysFreeString ( ClassPath );
	SysFreeString ( bstrQuery );

	if ( pLocator ) {
		pLocator->Release( );
		}
	if ( pNamespace ) {
		pNamespace->Release( );
		}
	CoUninitialize ( );

	return ret;
	}

bool SetBrightness( int val ) {
	
	cout << "Attempting to set brightness " << val << " via WMI" << endl;
	bool bRet = true;
	

	IWbemLocator         *pLocator   = NULL;
	IWbemClassObject     * pClass    = NULL;
	IWbemClassObject     * pInClass  = NULL;
	IWbemClassObject     * pInInst   = NULL;
	IEnumWbemClassObject *pEnum      = NULL;
	IWbemServices        *pNamespace = 0;
	HRESULT hr = S_OK;

	BSTR path       = SysAllocString( L"root\\wmi" );
	BSTR ClassPath  = SysAllocString( L"WmiMonitorBrightnessMethods" );
	BSTR MethodName = SysAllocString( L"WmiSetBrightness" );
	BSTR ArgName0   = SysAllocString( L"Timeout" );
	BSTR ArgName1   = SysAllocString( L"Brightness" );
	BSTR bstrQuery  = SysAllocString( L"Select * from WmiMonitorBrightnessMethods" );

	if ( !path || !ClassPath || !MethodName || !ArgName0 ) {
		cout << "Something went wrong when initializing path, ClassPath, MethodName, and ArgName0." << endl;
		bRet = false;
		goto cleanup;
		}

	// Initialize COM and connect up to CIMOM

	hr = CoInitialize( 0 );
	if ( FAILED( hr ) ) {
		bRet = false;
		goto cleanup;
		}

	//  NOTE:
	//  When using asynchronous WMI API's remotely in an environment where the "Local System" account has no network identity (such as non-Kerberos domains), the authentication level of RPC_C_AUTHN_LEVEL_NONE is needed. However, lowering the authentication level to RPC_C_AUTHN_LEVEL_NONE makes your application less secure. It is wise to use semi-synchronous API's for accessing WMI data and events instead of the asynchronous ones.


	hr = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_SECURE_REFS, NULL );
	//change EOAC_SECURE_REFS to EOAC_NONE if you change dwAuthnLevel to RPC_C_AUTHN_LEVEL_NONE
	hr = CoCreateInstance( CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, ( LPVOID * ) &pLocator );
	if ( FAILED( hr ) ) {
		bRet = false;
		goto cleanup;
		}
	hr = pLocator->ConnectServer( path, NULL, NULL, NULL, 0, NULL, NULL, &pNamespace );
	if ( hr != WBEM_S_NO_ERROR ) {
		bRet = false;
		goto cleanup;
		}

	hr = CoSetProxyBlanket( pNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );

	if ( hr != WBEM_S_NO_ERROR ) {
		bRet = false;
		goto cleanup;
		}

	/*
	See http://stackoverflow.com/a/1238643/625687 for info on usage of arrow ('->')
	ExecQuery:
	WQL:							Query Language
	bstrQuery:						Query to Execute
	WBEM_FLAG_RETURN_IMMEDIATELY:	Make a semi-synchronous call
	NULL:							Context
	pEnum:							Enumeration Interface
	*/

	hr = pNamespace->ExecQuery( 
								_bstr_t( L"WQL" ),
								bstrQuery,
								WBEM_FLAG_RETURN_IMMEDIATELY,
								NULL,
								&pEnum
								);

	if ( hr != WBEM_S_NO_ERROR ) {
		bRet = false;
		goto cleanup;
		}

	hr = WBEM_S_NO_ERROR;

	while ( WBEM_S_NO_ERROR == hr ) {
		ULONG ulReturned;
		IWbemClassObject *pObj;

		//Get the Next Object from the collection
		hr = pEnum->Next( WBEM_INFINITE, //Timeout
			1, //No of objects requested
			&pObj, //Returned Object
			&ulReturned //No of object returned
			);

		if ( hr != WBEM_S_NO_ERROR ) {
			bRet = false;
			goto cleanup;
			}

		// Get the class object
		hr = pNamespace->GetObject( ClassPath, 0, NULL, &pClass, NULL );
		if ( hr != WBEM_S_NO_ERROR ) {
			bRet = false;
			goto cleanup;
			}

		// Get the input argument and set the property
		hr = pClass->GetMethod( MethodName, 0, &pInClass, NULL );
		if ( hr != WBEM_S_NO_ERROR ) {
			bRet = false;
			goto cleanup;
			}

		hr = pInClass->SpawnInstance( 0, &pInInst );
		if ( hr != WBEM_S_NO_ERROR ) {
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
			bRet = false;
			goto cleanup;
			}
		// Call the method

		VARIANT pathVariable;
		VariantInit( &pathVariable );

		hr = pObj->Get( _bstr_t( L"__PATH" ), 0, &pathVariable, NULL, NULL );
		
		if ( hr != WBEM_S_NO_ERROR ) {
			goto cleanup;
			}
		
		hr = pNamespace->ExecMethod( pathVariable.bstrVal, MethodName, 0, NULL, pInInst, NULL, NULL );
		
		VariantClear( &pathVariable );
		
		if ( hr != WBEM_S_NO_ERROR ) {
			bRet = false;
			goto cleanup;
			}
		}

	cleanup:
		cout << "initiated SetBrightness cleanup!" << endl;
		SysFreeString( path       );
		SysFreeString( ClassPath  );
		SysFreeString( MethodName );
		SysFreeString( ArgName0   );
		SysFreeString( ArgName1   );
		SysFreeString( bstrQuery  );

		if ( pClass		)		{ pClass->Release( );		}
		if ( pInInst	)		{ pInInst->Release( );		}
		if ( pInClass	)		{ pInClass->Release( );		}
		if ( pLocator	)		{ pLocator->Release( );		}
		if ( pNamespace )		{ pNamespace->Release( );	}
	
		CoUninitialize ( );

		return bRet;
	}

void main ( ) {
	int getBrightInt = NULL;

	HMODULE hModule = ::GetModuleHandle ( NULL );

	if ( hModule != NULL ) {
		int ass = GetBrightness (  );

		cout << "Got brightness: " << ass << " via WMI" << endl;


		bool getBrightSucess = ddcGetBrightness ( getBrightInt );

		if ( !getBrightSucess ) {
			char msg[] = "Failed to get brightness via DDC/CI!";
			printError ( msg );
			}
		
		else if ( getBrightSucess ) { cout << "got brightness: " << getBrightInt << " via DDC/CI" << endl; }
		SetBrightness ( 0 );
		Sleep ( 100 );
		SetBrightness ( 100 );
		Sleep ( 100 );
		SetBrightness ( 0 );
		Sleep ( 100 );
		SetBrightness ( ass );
		}
	else {
		cout << "Fatal Error: GetModuleHandle failed" << endl;
		}
	}

