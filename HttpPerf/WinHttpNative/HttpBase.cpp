// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include "stdafx.h"
#include <iostream>
#include "HttpBase.h"
#include "Metrics.h"

HttpBase::HttpBase(
    std::wostream* out, 
    PCRITICAL_SECTION outLock, 
    DWORD id, 
    PCWCHAR serverUri, 
    DWORD iterations, 
    DWORD requestSizeCharacters) :
    _out(out), _outLock(outLock), _id(id), _serverUri(serverUri), _messageLength(requestSizeCharacters), _iterations(iterations)
{
}

HttpBase::~HttpBase()
{
    _clientLevelMetrics.Start(&_clientLevelMetrics.ClientFreeMs);
    if (_connectionHandle != NULL)
    {
        WinHttpCloseHandle(_connectionHandle);
        _connectionHandle = NULL;
    }

    if (_sessionHandle != NULL)
    {
        WinHttpCloseHandle(_sessionHandle);
        _sessionHandle = NULL;
    }
    _clientLevelMetrics.Measure(&_clientLevelMetrics.ClientFreeMs);
    _clientLevelMetrics.Measure(&_clientLevelMetrics.ClientTotalMs);

    _clientLevelMetrics.Write(*_out, _outLock);

    if (_serverName)
    {
        delete[] _serverName;
    }

    if (_pathAndQuery)
    {
        delete[] _pathAndQuery;
    }

    if (_target)
    {
        delete[] _target;
    }
}

HANDLE HttpBase::GetTestCompletedEvent()
{
    return _completedEvent;
}

void HttpBase::GenerateRequestMessageBuffer()
{
    if (_messageLength > 0)
    {
        _message = new WCHAR[_messageLength + 1];
        for (unsigned i = 0; i < _messageLength; i++)
        {
            _message[i] = i % 26 + 65;
        }

        _message[_messageLength] = '\0';
    }
}

bool HttpBase::ParseUri()
{
    URL_COMPONENTS urlComp = { sizeof(urlComp) };
    urlComp.dwSchemeLength = (DWORD)-1;
    urlComp.dwHostNameLength = (DWORD)-1;
    urlComp.dwUrlPathLength = (DWORD)-1;
    urlComp.dwExtraInfoLength = (DWORD)-1;

    if (!WinHttpCrackUrl(_serverUri, static_cast<DWORD>(wcslen(_serverUri)), 0, &urlComp))
    {
        return false;
    }
    
    _serverName = new WCHAR[urlComp.dwHostNameLength + 1];
    if (!SUCCEEDED(StringCchCopyNW(_serverName, urlComp.dwHostNameLength + 1, urlComp.lpszHostName, urlComp.dwHostNameLength)))
    {
        return false;
    }

    _port = urlComp.nPort;
    
    DWORD pathAndQueryLen = urlComp.dwUrlPathLength + urlComp.dwExtraInfoLength + 1;
    _pathAndQuery = new WCHAR[pathAndQueryLen];
    if (!SUCCEEDED(StringCchCopyNW(_pathAndQuery, pathAndQueryLen, urlComp.lpszUrlPath, urlComp.dwUrlPathLength))) 
    {
        return false;
    }

    if (!SUCCEEDED(StringCchCatNW(_pathAndQuery, pathAndQueryLen, urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength)))
    {
        return false;
    }

    DWORD targetLen = urlComp.dwSchemeLength + pathAndQueryLen + 1;
    _target = new WCHAR[targetLen];
    
    if (!SUCCEEDED(StringCchCopyNW(_target, urlComp.dwSchemeLength + 1, urlComp.lpszScheme, urlComp.dwSchemeLength)))
    {
        return false;
    }

    if (!SUCCEEDED(StringCchCatNW(_target, targetLen, _pathAndQuery, pathAndQueryLen)))
    {
        return false;
    }

    return true;
}

void HttpBase::RunTestAsync()
{
    _completedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!_completedEvent)
    {
        _clientLevelMetrics.ErrorMessage = L"Failed to create _completedEvent.";
        return;
    }

    GenerateRequestMessageBuffer();
    
    InitializeMetrics(_clientLevelMetrics);
    _clientLevelMetrics.Start(&_clientLevelMetrics.ClientTotalMs);

    if (!ParseUri())
    {
        _clientLevelMetrics.ErrorMessage = L"ParseUri() failed.";
        return;
    }
    
    _clientLevelMetrics.Target = _target;
    RunTestInternalAsync();
}

void HttpBase::InitializeMetrics(Metrics& m)
{
    m.StopwatchMeasureWallTime();
    m.Implementation = ImplementationName();
    m.Target = _target;
    m.RequestContentSize = _messageLength;
    m.Iterations = _iterations;
    m.ClientId = _id;
}
