// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#pragma once
#include "HttpBase.h"
#include "WinHttpAsync.h"
#include "Metrics.h"

#define MAX_TIMEOUT_MS 60000

template <class T>
class HttpPerfTest
{
public:
    
    void RunTestAsync(std::wostream* out, const WCHAR *server, int iterations, int instances, int requestSizeCharacters)
    {
        T **clientArray = new T*[instances];
        CRITICAL_SECTION outLock;
        InitializeCriticalSection(&outLock);

        Concurrency::parallel_for(
            0,
            instances,
            [&out, &outLock, &server, &iterations, &clientArray, &requestSizeCharacters](int i)
        {
            try
            {
                clientArray[i] = new T(out, &outLock, i, server, iterations, requestSizeCharacters);
                clientArray[i]->RunTestAsync();
            }
            catch (...)
            {
                assert(false);
            }
        });

        // This is a simple way of working around WaitForMultipleObjects/MAXIMUM_WAIT_OBJECTS.
        for (int i = 0; i < instances; i++)
        {
            assert(clientArray[i] != NULL);
            assert(WAIT_OBJECT_0 == WaitForSingleObject(clientArray[i]->GetTestCompletedEvent(), MAX_TIMEOUT_MS));
            delete clientArray[i];
        }

        delete[] clientArray;
        DeleteCriticalSection(&outLock);
    }
};
