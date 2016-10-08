// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#pragma once
#include "Metrics.h"

class HttpBase
{
public:
    HttpBase(
        std::wostream* out, 
        PCRITICAL_SECTION outLock, 
        DWORD id, 
        PCWCHAR serverUri, 
        DWORD iterations, 
        DWORD requestSizeCharacters);
    ~HttpBase();

    void RunTestAsync();
    HANDLE GetTestCompletedEvent();

protected:
    virtual void RunTestInternalAsync() = 0;
    virtual PCWSTR ImplementationName() = 0;
    void InitializeMetrics(Metrics& m);

    HINTERNET _sessionHandle = NULL;
    HINTERNET _connectionHandle = NULL;
    
    PCWCHAR _serverUri = NULL;
    INTERNET_PORT _port = INTERNET_DEFAULT_HTTP_PORT;
    WCHAR *_serverName = NULL;
    WCHAR *_pathAndQuery = NULL;
    WCHAR *_target = NULL;
    WCHAR *_message = NULL;
    DWORD _messageLength = 0;

    std::wostream* _out = NULL;
    PCRITICAL_SECTION _outLock = NULL;
    DWORD _iterations = MAXDWORD;
    DWORD _id = MAXDWORD;
    HANDLE _completedEvent = NULL;
    Metrics _clientLevelMetrics = {};

private:
    void GenerateRequestMessageBuffer();
    bool ParseUri();
};
