// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "HttpPerfTest.h"


#pragma warning( push )
#pragma warning( disable: 4100 ) // hInternet is not used.
int __cdecl wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
    const WCHAR *serverUri = L"http://localhost/dynamic/echo.ashx";
    int iterations = 1;
    int instances = 1;
    int requestSizeCharacters = 1024;
    std::wofstream outFileStream;
    std::wostream* out = &std::wcout;

    if (argc > 1)
    {
        if (wcscmp(argv[1], L"console"))
        {
            outFileStream.open(argv[1], std::wofstream::out | std::wofstream::app);
            out = &outFileStream;
        }
    }

    if (argc > 2)
    {
        serverUri = argv[2];
    }

    if (argc > 3)
    {
        iterations = _wtoi(argv[3]);
    }

    if (argc > 4)
    {
        instances = _wtoi(argv[4]);
    }
    
    if (argc > 5)
    {
        requestSizeCharacters = _wtoi(argv[5]);
    }

    if (!Metrics::StartWallTime())
    {
        wprintf(L"Cannot initialize WallTime.");
        return 1;
    }

    HttpPerfTest<WinHttpAsync> asyncTest = HttpPerfTest<WinHttpAsync>();
    asyncTest.RunTestAsync(out, serverUri, iterations, instances, requestSizeCharacters);
    
    out->flush();

    return 0;
}

#pragma warning( pop )