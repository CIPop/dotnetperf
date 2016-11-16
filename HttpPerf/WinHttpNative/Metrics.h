// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#pragma once
#include "stdafx.h"
#include <iostream>

struct Metrics
{
public:
    PCWSTR Implementation = NULL;
    PWSTR Target = NULL;
    DWORD RequestContentSize = MAXDWORD;
    DWORD Iterations = MAXDWORD;
    DWORD ClientId = MAXDWORD;
    LARGE_INTEGER ClientInitMs = {};
    LARGE_INTEGER ClientSendRecvMs = {};
    LARGE_INTEGER ClientFreeMs = {};
    LARGE_INTEGER ClientTotalMs = {};
    LARGE_INTEGER IterationCreateRequestMs = {};
    LARGE_INTEGER IterationGetHeadersMs = {};
    LARGE_INTEGER IterationGetContentMs = {};
    LARGE_INTEGER IterationFreeMs = {};
    DWORD IterationResponseHeadersBytes = MAXDWORD;
    DWORD IterationResponseContentBytes = MAXDWORD;
    PCWSTR ErrorMessage = NULL;

    static bool StartWallTime();
    void GetHeader(PCWSTR str);
    void Write(std::wostream& out, PCRITICAL_SECTION outLock);
    void Start(PLARGE_INTEGER m);
    void Measure(PLARGE_INTEGER m);
    void StopwatchMeasureWallTime();

private:
    static __int64 s_perfTimerFrequency;
    LARGE_INTEGER _wallTime = {};
    static LARGE_INTEGER _timeStart;
    void Add(std::wostringstream& sb, LARGE_INTEGER i);
    void Add(std::wostringstream& sb, DWORD i);
    void Add(std::wostringstream & sb, PCWSTR str);
    bool Initialize();
};
