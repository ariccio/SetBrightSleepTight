#pragma once

#include "resource.h"
#include <Windows.h>
#include <cassert>


//from my altWinDirStat repo, macros_that_scare_small_children.h
#ifndef WDS_ASSERT_EXPECTED_STRING_FORMAT_FAILURE_HRESULT

//I don't think anything in strsafe actually ever returns STRSAFE_E_END_OF_FILE,
//so I use this after I've handled the other error conditions (STRSAFE_E_INSUFFICIENT_BUFFER, STRSAFE_E_INVALID_PARAMETER),
//to catch unexpected errors. NOTE that these are still handled by the calling function via SUCCESSS( ),
//but this macro helps catch the issue closer to the function that returned the unexpected value;
#define WDS_ASSERT_EXPECTED_STRING_FORMAT_FAILURE_HRESULT( res ) {                                                \
	static_assert( SUCCEEDED( S_OK ), "This macro depends on SUCCEEDED( S_OK ) returning true" );                 \
	static_assert( std::is_same<decltype( res ), const HRESULT>::value, "This macro depends on an HRESULT res" ); \
	assert( ( res ) != STRSAFE_E_END_OF_FILE );                                                                   \
	assert( FAILED( res ) );                                                                                      \
	assert( !SUCCEEDED( res ) );                                                                                  \
	}

#else
#error already defined!
#endif



#ifndef WDS_STRSAFE_E_INVALID_PARAMETER_HANDLER
//std::terminate( )s if ( res == STRSAFE_E_INVALID_PARAMETER ), as this is usually an issue with an incorrect compile-time constant.
//Is a macro and not an inline function because of the use of file name, function signature, and line number.
#define WDS_STRSAFE_E_INVALID_PARAMETER_HANDLER( res, strsafe_func_name )                                             \
	if ( ( res ) == STRSAFE_E_INVALID_PARAMETER ) {                                                                   \
		static_assert( std::is_same<decltype( res ), const HRESULT>::value, "This macro depends on an HRESULT res" ); \
		unexpected_strsafe_invalid_parameter_handler( ( strsafe_func_name ), __FILE__, __FUNCSIG__, __LINE__ );       \
		}
#else
#error already defined!!
#endif

#ifndef WDS_WRITES_TO_STACK
#define WDS_WRITES_TO_STACK( strSize, chars_written ) _Out_writes_z_( strSize ) _Pre_writable_size_( strSize ) _Post_readable_size_( chars_written ) _Pre_satisfies_( strSize >= chars_written ) _Post_satisfies_( _Old_( chars_written ) <= chars_written )
#else
#error already defined!
#endif


//On returning E_FAIL, call GetLastError for details. That's not my idea!
_Success_( SUCCEEDED( return ) ) HRESULT CStyle_GetLastErrorAsFormattedMessage( WDS_WRITES_TO_STACK( strSize, chars_written ) PWSTR psz_formatted_error, _In_range_( 128, 32767 ) const rsize_t strSize, _Out_ rsize_t& chars_written, const DWORD error = GetLastError( ) );

//This is an error handling function, and is intended to be called rarely!
__declspec(noinline)
void displayWindowsMsgBoxWithMessage( PCSTR const message ) {
	MessageBoxA( NULL, message, "Error", MB_OK );
	}
