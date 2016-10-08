// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#pragma once

class WinHttpAsync;

class WinHttpRequestContext
{
public:
    WinHttpRequestContext(WinHttpAsync* client) : _client(client)
    {
        Buffer = new char[BufferSize];
    }

    ~WinHttpRequestContext()
    {
        delete Buffer;
    }
    
    WinHttpAsync* Client() const { return _client; }
    HINTERNET RequestHandle() const { return _requestHandle; }
    void RequestHandle(HINTERNET val) { _requestHandle = val; }

    static const DWORD BufferSize = 8 * 1024;

    Metrics Metrics;
    PCHAR Buffer;
    
private:
    WinHttpAsync* _client;
    HINTERNET _requestHandle;
};
