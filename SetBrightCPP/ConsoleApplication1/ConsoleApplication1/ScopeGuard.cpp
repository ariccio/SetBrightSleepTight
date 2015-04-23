// see `file_header_text.txt` for licensing & contact info. If you can't find that file, then assume you're NOT allowed to do whatever you wanted to do.

#include "ScopeGuard.h"

#ifndef SCOPEGUARD_CPP_INCLUDED
#define SCOPEGUARD_CPP_INCLUDED

//intentionally NOT defined as part of ScopeGuard, to reduce code duplication. //Also, produces cleaner `TRACE` output.
#ifdef DEBUG
void trace_out( _In_z_ PCSTR const file_name, _In_z_ PCSTR const func_name, _In_ _In_range_( 0, INT_MAX ) const int line_number ) {
#ifdef SCOPE_GUARD_DEBUGGING
	TRACE( L"Scope guard triggered!"
			L"\r\n\t\tScope guard initialized in file: `%S`,"
			L"\r\n\t\tfunction:                        `%S`,"
			L"\r\n\t\tline:                            `%i`\r\n", file_name, func_name, line_number );
#else
	UNREFERENCED_PARAMETER( file_name );
	UNREFERENCED_PARAMETER( func_name );
	UNREFERENCED_PARAMETER( line_number );
#endif
	}
#endif

#else

#endif