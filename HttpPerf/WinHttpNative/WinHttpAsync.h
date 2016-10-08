// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#pragma once
#include "HttpBase.h"

class WinHttpRequestContext;

class WinHttpAsync : public HttpBase
{

public:
    WinHttpAsync(
        std::wostream* out, 
        PCRITICAL_SECTION outLock, 
        DWORD id, 
        PCWCHAR serverUri, 
        DWORD iterations, 
        DWORD requestSizeCharacters)
        : HttpBase(out, outLock, id, serverUri, iterations, requestSizeCharacters)
    {
    };
    ~WinHttpAsync();

    virtual DWORD Initialize();
    virtual DWORD Connect();
    virtual DWORD SendReceive();

protected:
    virtual void RunTestInternalAsync();
    virtual PCWSTR ImplementationName();

private:
    static VOID CALLBACK WinHttpCallback(
        IN HINTERNET hInternet, 
        IN DWORD_PTR dwContext, 
        IN DWORD dwInternetStatus, 
        IN LPVOID lpvStatusInformation OPTIONAL, 
        IN DWORD dwStatusInformationLength);
    DWORD SendRequest();
    DWORD ReadContent(WinHttpRequestContext* context);
    void OnHttpSendRequestComplete(WinHttpRequestContext* context);
    void OnHeadersAvailable(WinHttpRequestContext* context);
    void OnReadComplete(WinHttpRequestContext* context, DWORD bytesRead);
    void OnError(WinHttpRequestContext* context, DWORD error);
    void OnHandleClosed(WinHttpRequestContext* context);

    volatile DWORD _responsesReceived = 0;
};
