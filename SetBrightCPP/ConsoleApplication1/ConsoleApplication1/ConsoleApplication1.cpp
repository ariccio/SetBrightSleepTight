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
#include <strsafe.h>
#include <string>
#include <memory>


//define SCOPE_GUARD_DEBUGGING for enhanced scope guard debugging.
#include "ScopeGuard.h"

//This is an error handling function, and is intended to be called rarely!
__declspec(noinline)
void unexpected_strsafe_invalid_parameter_handler( _In_z_ PCSTR const strsafe_func_name, _In_z_ PCSTR const file_name_in, _In_z_ PCSTR const func_name_in, _In_ _In_range_( 0, INT_MAX ) const int line_number_in ) {
	std::string err_str( strsafe_func_name );
	err_str += " returned STRSAFE_E_INVALID_PARAMETER, in: file `";
	err_str += file_name_in;
	err_str += "`, function: `";
	err_str += func_name_in;
	err_str += "` line: `";
	err_str += std::to_string( line_number_in );
	err_str += "`! This (near universally) means an issue where incorrect compile-time constants were passed to a strsafe function. Thus it's probably not recoverable. We'll abort. Sorry!";
	displayWindowsMsgBoxWithMessage( err_str.c_str( ) );
	std::terminate( );
	}

//This is an error handling function, and is intended to be called rarely!
__declspec(noinline)
void write_bad_fmt_msg( _Out_writes_z_( 41 ) _Pre_writable_size_( 42 ) _Post_readable_size_( chars_written ) PWSTR psz_fmt_msg, _Out_ rsize_t& chars_written ) {
	psz_fmt_msg[  0 ] = L'F';
	psz_fmt_msg[  1 ] = L'o';
	psz_fmt_msg[  2 ] = L'r';
	psz_fmt_msg[  3 ] = L'm';
	psz_fmt_msg[  4 ] = L'a';
	psz_fmt_msg[  5 ] = L't';
	psz_fmt_msg[  6 ] = L'M';
	psz_fmt_msg[  7 ] = L'e';
	psz_fmt_msg[  8 ] = L's';
	psz_fmt_msg[  9 ] = L's';
	psz_fmt_msg[ 10 ] = L'a';
	psz_fmt_msg[ 11 ] = L'g';
	psz_fmt_msg[ 12 ] = L'e';
	psz_fmt_msg[ 13 ] = L' ';
	psz_fmt_msg[ 14 ] = L'f';
	psz_fmt_msg[ 15 ] = L'a';
	psz_fmt_msg[ 16 ] = L'i';
	psz_fmt_msg[ 17 ] = L'l';
	psz_fmt_msg[ 18 ] = L'e';
	psz_fmt_msg[ 19 ] = L'd';
	psz_fmt_msg[ 20 ] = L' ';
	psz_fmt_msg[ 21 ] = L't';
	psz_fmt_msg[ 22 ] = L'o';
	psz_fmt_msg[ 23 ] = L' ';
	psz_fmt_msg[ 24 ] = L'f';
	psz_fmt_msg[ 25 ] = L'o';
	psz_fmt_msg[ 26 ] = L'r';
	psz_fmt_msg[ 27 ] = L'm';
	psz_fmt_msg[ 28 ] = L'a';
	psz_fmt_msg[ 29 ] = L't';
	psz_fmt_msg[ 30 ] = L' ';
	psz_fmt_msg[ 31 ] = L'a';
	psz_fmt_msg[ 32 ] = L'n';
	psz_fmt_msg[ 33 ] = L' ';
	psz_fmt_msg[ 34 ] = L'e';
	psz_fmt_msg[ 35 ] = L'r';
	psz_fmt_msg[ 36 ] = L'r';
	psz_fmt_msg[ 37 ] = L'o';
	psz_fmt_msg[ 38 ] = L'r';
	psz_fmt_msg[ 39 ] = L'!';
	psz_fmt_msg[ 40 ] = 0;
	chars_written = 41;
	assert( wcslen( psz_fmt_msg ) == chars_written );
	}



static_assert( !SUCCEEDED( E_FAIL ), "CStyle_GetLastErrorAsFormattedMessage doesn't return a valid error code!" );
static_assert( SUCCEEDED( S_OK ), "CStyle_GetLastErrorAsFormattedMessage doesn't return a valid success code!" );
//On returning E_FAIL, call GetLastError for details. That's not my idea! //TODO: mark as only returning S_OK, E_FAIL
_Success_( SUCCEEDED( return ) ) HRESULT CStyle_GetLastErrorAsFormattedMessage( WDS_WRITES_TO_STACK( strSize, chars_written ) PWSTR psz_formatted_error, _In_range_( 128, 32767 ) const rsize_t strSize, _Out_ rsize_t& chars_written, const DWORD error ) {
	//const auto err = GetLastError( );
	const auto err = error;
	const auto ret = FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), psz_formatted_error, static_cast<DWORD>( strSize ), NULL );
	if ( ret != 0 ) {
		chars_written = ret;
		return S_OK;
		}
	const DWORD error_err = GetLastError( );
	
	const rsize_t err_msg_buff_size = 512;
	_Null_terminated_ char err_msg_buff[ err_msg_buff_size ] = { 0 };
	const HRESULT output_error_message_format_result = StringCchPrintfA( err_msg_buff, err_msg_buff_size, "WDS: FormatMessageW failed with error code: `%lu`!!\r\n", error_err );
	if ( SUCCEEDED( output_error_message_format_result ) ) {
		OutputDebugStringA( err_msg_buff );
		}
	else {
		WDS_ASSERT_EXPECTED_STRING_FORMAT_FAILURE_HRESULT( output_error_message_format_result );
		WDS_STRSAFE_E_INVALID_PARAMETER_HANDLER( output_error_message_format_result, "StringCchPrintfA" );
		OutputDebugStringA( "WDS: FormatMessageW failed, and THEN formatting the error message for FormatMessageW failed!\r\n" );
		}
	if ( strSize > 41 ) {
		write_bad_fmt_msg( psz_formatted_error, chars_written );
		return E_FAIL;
		}
	chars_written = 0;
	return E_FAIL;
	}




//what the fuck?
void printError( _In_z_ PCSTR const msg ) {
	//TCHAR sysMsg[ 256 ];
	const DWORD lastErr = GetLastError( );
	//FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lastErr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), sysMsg, 256, NULL );
	//std::cout << std::endl << msg << "--Failed with error: " << lastErr << " (" << sysMsg << ")" << std::endl << std::endl;

	const rsize_t strBufferSize = 512u;
	wchar_t errBuffer[ strBufferSize ] = { 0 };
	rsize_t chars_written = 0;
	const HRESULT fmt_res = CStyle_GetLastErrorAsFormattedMessage( errBuffer, strBufferSize, chars_written, lastErr );
	if ( SUCCEEDED( fmt_res ) ) {
		//OutputDebugStringW( errBuffer );
		wprintf( L"%S--failed with error: %u\r\n\ttext: %s\r\n\r\n", msg, lastErr, errBuffer );
		return;
		}
	printf( "printError - DOUBLE FAULT!\r\n" );
	std::terminate( );
	}

//what the double fuck?
void printError() {
	//char msg[1] = { };
	printError( "" );
	}

bool doesMonitorSupportBrightnessConfigurationViaDDC( _In_ const HANDLE hPhysicalMonitor ) {
	DWORD capabilitiesFlags_temp = 0;
	DWORD colorTempsFlags_junk = 0;
	const BOOL capabilitiesResult = GetMonitorCapabilities( hPhysicalMonitor, &capabilitiesFlags_temp, &colorTempsFlags_junk );
	if ( capabilitiesResult != TRUE ) {
		printf( "failed to get monitor capabilities!\r\n" );
		printError( );
		return false;
		}

	const DWORD capabilitiesFlags = capabilitiesFlags_temp;
	if ( ( capabilitiesFlags & MC_CAPS_BRIGHTNESS ) == 0 ) {
		printf( "monitor DOES NOT support GetMonitorBrightness/SetMonitorBrightness!\r\n" );
		return false;
		}
	return true;
	}


bool ddcGetBrightness ( ) {
	//HMONITOR hMonitor = NULL;

	DWORD pdwMinimumBrightness = 0;
	DWORD pdwCurrentBrightness = 0;
	DWORD pdwMaximumBrightness = 0;

	const HWND hwnd = FindWindowW( NULL, NULL );
	printf( "Window handle: %p\r\n", hwnd );

	const HMONITOR hMonitor = MonitorFromWindow( hwnd, MONITOR_DEFAULTTOPRIMARY );
	printf( "hMonitor: %p\r\n", hMonitor );

	DWORD physicalMonitors_temp = 0;
	const BOOL numberMonitorsResult = GetNumberOfPhysicalMonitorsFromHMONITOR( hMonitor, &physicalMonitors_temp );
	if ( numberMonitorsResult != TRUE ) {
		printf( "GetNumberOfPhysicalMonitorsFromHMONITOR failed!\r\n" );
		printError( );
		return false;
		}

	const DWORD physicalMonitors = physicalMonitors_temp;
	
	if ( physicalMonitors == 0 ) {
		printf( "No monitors? What??\r\n" );
		return false;
		}
	
	_Field_size_( physicalMonitors ) std::unique_ptr<PHYSICAL_MONITOR[]> monitors = std::make_unique<PHYSICAL_MONITOR[ ]>( physicalMonitors );

	static_assert( std::is_pod<PHYSICAL_MONITOR>::value, "can't memset!" );
	memset( monitors.get( ), 0, ( sizeof( PHYSICAL_MONITOR ) * physicalMonitors ) );

	const BOOL getPhysicalMonitors = GetPhysicalMonitorsFromHMONITOR( hMonitor, physicalMonitors, monitors.get( ) );
	if ( getPhysicalMonitors != TRUE ) {
		printf( "GetPhysicalMonitorsFromHMONITOR failed!\r\n" );
		printError( );
		return false;
		}

	auto monitorsGuard = SCOPEGUARD_INSTANCE( [ &] {
		const BOOL destroyRes = DestroyPhysicalMonitors( physicalMonitors, monitors.get( ) );
		if ( destroyRes != TRUE ) {
			printf( "DestroyPhysicalMonitors failed!\r\n" );
			printError( );
			std::terminate( );
			}
		} );

	printf( "Got %u physical monitors\r\n", physicalMonitors );

	for ( DWORD i = 0; i < physicalMonitors; ++i ) {
		printf( "Monitor #%u description: %S\r\n", i, monitors[ i ].szPhysicalMonitorDescription );
		}

	const bool doesSupport = doesMonitorSupportBrightnessConfigurationViaDDC( monitors[ 0 ].hPhysicalMonitor );
	if ( !doesSupport ) {
		return false;
		}

	//LPSTR pszASCIICapabilitiesString = NULL;

	DWORD cchStringLength = NULL;
	//cchStringLength = NULL;
	//C6102 warning BUG? http://www.beta.microsoft.com/VisualStudio/feedback/details/812312/incorrect-code-analysis-warning-c6102




	const BOOL capabilityStrLen = GetCapabilitiesStringLength( monitors[ 0 ].hPhysicalMonitor, &cchStringLength );

	if ( capabilityStrLen != TRUE ) {
		printf( "GetCapabilitiesStringLength failed!!\r\n" );
		printError( );
		return false;
		}

	printf( "GetCapabilitiesStringLength Succeeded!!\r\n" );

	std::unique_ptr<_Null_terminated_ char[ ]> capabilitiesString = std::make_unique<char[ ]>( cchStringLength );

	const BOOL capabilitiesRequestAndCapabilitiesReplySucces = CapabilitiesRequestAndCapabilitiesReply( hwnd, capabilitiesString.get( ), cchStringLength );
	if ( capabilitiesRequestAndCapabilitiesReplySucces != TRUE ) {
		printf( "CapabilitiesRequestAndCapabilitiesReply failed!\r\n" );
		printError( );
		return false;
		}

	printf( "szCapabilitiesString: %s\r\n", capabilitiesString.get( ) );

	const BOOL getBSuccess = GetMonitorBrightness( hMonitor, &pdwMinimumBrightness, &pdwCurrentBrightness, &pdwMaximumBrightness );

	if ( getBSuccess != TRUE ) {
		printf( "GetMonitorBrightness failed!\r\n" );
		return false;
		}

	std::cout << pdwMinimumBrightness << std::endl;
	std::cout << pdwCurrentBrightness << std::endl;
	std::cout << pdwMaximumBrightness << std::endl;

	return true;

	}

bool ddcSetBrightness ( const DWORD dwNewBrightness) {
	//HMONITOR hMonitor = NULL;

	//DWORD pdwMinimumBrightness = 0;
	//DWORD pdwCurrentBrightness = 0;
	//DWORD pdwMaximumBrightness = 0;


	const HWND hwnd = FindWindowW( NULL, NULL );
	printf( "Window handle: %p\r\n", hwnd );

	const HMONITOR hMonitor = MonitorFromWindow( hwnd, MONITOR_DEFAULTTOPRIMARY );
	printf( "hMonitor: %p\r\n", hMonitor );

	DWORD physicalMonitors_temp = 0;
	const BOOL numberMonitorsResult = GetNumberOfPhysicalMonitorsFromHMONITOR( hMonitor, &physicalMonitors_temp );
	if ( numberMonitorsResult != TRUE ) {
		printf( "GetNumberOfPhysicalMonitorsFromHMONITOR failed!\r\n" );
		printError( );
		return false;
		}

	const DWORD physicalMonitors = physicalMonitors_temp;
	
	if ( physicalMonitors == 0 ) {
		printf( "No monitors? What??\r\n" );
		return false;
		}
	
	_Field_size_( physicalMonitors ) std::unique_ptr<PHYSICAL_MONITOR[]> monitors = std::make_unique<PHYSICAL_MONITOR[ ]>( physicalMonitors );

	static_assert( std::is_pod<PHYSICAL_MONITOR>::value, "can't memset!" );
	memset( monitors.get( ), 0, ( sizeof( PHYSICAL_MONITOR ) * physicalMonitors ) );

	const BOOL getPhysicalMonitors = GetPhysicalMonitorsFromHMONITOR( hMonitor, physicalMonitors, monitors.get( ) );
	if ( getPhysicalMonitors != TRUE ) {
		printf( "GetPhysicalMonitorsFromHMONITOR failed!\r\n" );
		printError( );
		return false;
		}

	auto monitorsGuard = SCOPEGUARD_INSTANCE( [ &] {
		const BOOL destroyRes = DestroyPhysicalMonitors( physicalMonitors, monitors.get( ) );
		if ( destroyRes != TRUE ) {
			printf( "DestroyPhysicalMonitors failed!\r\n" );
			printError( );
			std::terminate( );
			}
		} );

	printf( "Got %u physical monitors\r\n", physicalMonitors );

	for ( DWORD i = 0; i < physicalMonitors; ++i ) {
		printf( "Monitor #%u description: %S\r\n", i, monitors[ i ].szPhysicalMonitorDescription );
		}

	const bool doesSupport = doesMonitorSupportBrightnessConfigurationViaDDC( monitors[ 0 ].hPhysicalMonitor );
	if ( !doesSupport ) {
		return false;
		}


	const BOOL setBSuccess = SetMonitorBrightness( monitors[ 0 ].hPhysicalMonitor, dwNewBrightness);

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
	
	IEnumWbemClassObject* pEnum = NULL;

	const HRESULT execQueryResult = pNamespace->ExecQuery (
								_bstr_t ( L"WQL" ),
								bstrQuery,
								WBEM_FLAG_RETURN_IMMEDIATELY,
								NULL,
								&pEnum
								);

	if ( execQueryResult != WBEM_S_NO_ERROR ) {
		printf( "ExecQuery failed!!\r\n" );
		return -1;
		}

	
	//hr = WBEM_S_NO_ERROR;

	//https://code.google.com/p/stexbar/source/browse/trunk/Misc/AAClr/src/Utils.cpp?spec=svn926&r=926
	//while ( WBEM_S_NO_ERROR == hr ) {
		ULONG ulReturned = 0;
		IWbemClassObject* pObj = NULL;

		/*
		Next: 
			Get the Next Object from the collection
				1:				Number of objects requested
		*/
		const HRESULT nextResult = pEnum->Next( WBEM_INFINITE, 1, &pObj, &ulReturned );

		if ( nextResult != WBEM_S_NO_ERROR ) {
			printf( "pEnum->Next failed!! (done?)\r\n" );
			return -1;
			}

		VARIANT var1;
		const HRESULT wmiBrightnessResult = pObj->Get( L"CurrentBrightness", 0, &var1, NULL, NULL );
		if ( FAILED( wmiBrightnessResult ) ) {
			printf( "pObj->Get( CurrentBrightness ) failed!\r\n" );
			return -1;
			}

		const int ret = V_UI1( &var1 );
		const HRESULT clearResult = VariantClear( &var1 );
		if ( FAILED( clearResult ) ) {
			printf( "VariantClear failed!!\r\n" );
			return -1;
			}
		printf( "'ret' is valid: %i\r\n", ret );
		return ret;//?
		//}
	//return -1;
	}

_Success_( return )
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

	if ( path == NULL ) {
		printf( "failed to allocate path BSTR!\r\n" );
		return false;
		}

	auto pathguard = SCOPEGUARD_INSTANCE( [ &] { SysFreeString( path ); path = NULL; } );

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

	HRESULT hr = WBEM_S_NO_ERROR;

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
	bool getBrightSucess = ddcGetBrightness( );

	if ( !getBrightSucess ) {
		printf( "Failed to get brightness via DDC/CI!\r\n" );
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

