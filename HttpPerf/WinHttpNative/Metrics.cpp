// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include "stdafx.h"
#include "Metrics.h"
#include <sstream>
#include <iomanip>
#include <iostream>

LARGE_INTEGER Metrics::_timeStart = {};

void Metrics::GetHeader(PCWSTR str)
{
    str = L"WallTime,"
        L"Implementation,"
        L"Target,"
        L"RequestContentSize,"
        L"Iterations,"
        L"ClientId,"
        L"ClientInitMs,"
        L"ClientSendRecvMs,"
        L"ClientFreeMs,"
        L"ClientTotalMs,"
        L"IterationCreateRequestMs,"
        L"IterationGetHeadersMs,"
        L"IterationGetContentMs,"
        L"IterationFreeMs,"
        L"IterationResponseHeadersBytes,"
        L"IterationResponseContentBytes,"
        L"ErrorMessage,";
}

bool Metrics::StartWallTime()
{
    if (!QueryPerformanceCounter(&_timeStart))
    {
        return false;
    }

    return true;
}

void Metrics::Start(PLARGE_INTEGER m)
{
    if (!QueryPerformanceCounter(m))
    {
        ErrorMessage = L"Cannot start measure performance counter.";
    }
}

void Metrics::Measure(PLARGE_INTEGER m)
{
    LARGE_INTEGER now;
    if (!QueryPerformanceCounter(&now))
    {
        ErrorMessage = L"Cannot measure performance counter.";
    }

    m->QuadPart = now.QuadPart - m->QuadPart;
}

void Metrics::StopwatchMeasureWallTime()
{
    _wallTime.QuadPart = _timeStart.QuadPart;
    Measure(&_wallTime);
}

void Metrics::Write(std::wostream& out, PCRITICAL_SECTION outLock)
{
    Initialize();

    std::wostringstream sb;
    Add(sb, _wallTime);
    Add(sb, Implementation);
    Add(sb, Target);
    Add(sb, RequestContentSize);
    Add(sb, Iterations);
    Add(sb, ClientId);
    Add(sb, ClientInitMs);
    Add(sb, ClientSendRecvMs);
    Add(sb, ClientFreeMs);
    Add(sb, ClientTotalMs);
    Add(sb, IterationCreateRequestMs);
    Add(sb, IterationGetHeadersMs);
    Add(sb, IterationGetContentMs);
    Add(sb, IterationFreeMs);
    Add(sb, IterationResponseHeadersBytes);
    Add(sb, IterationResponseContentBytes);
    Add(sb, ErrorMessage);
    
    EnterCriticalSection(outLock);

    try 
    {
        out << sb.str() << std::endl;
    }
    catch (...) 
    { 
        ErrorMessage = L"Metrics::Write: Cannot write to output stream.";
    }

    LeaveCriticalSection(outLock);
}

void Metrics::Add(std::wostringstream& sb, LARGE_INTEGER i)
{
    try
    {
        if (i.QuadPart != 0)
        {
            double msec = (double)i.QuadPart / ((double)s_perfTimerFrequency / 1000.0);
            sb << std::fixed << std::setprecision(4) << msec;
        }
    
        sb << L',';
    }
    catch (...)
    {
        ErrorMessage = L"Metrics::Add(LARGE_INTEGER): Cannot write to output stream.";
    }
}

void Metrics::Add(std::wostringstream & sb, DWORD i)
{
    try
    {

        if (i != MAXDWORD)
        {
            sb << i;
        }

        sb << L',';
    }
    catch (...)
    {
        ErrorMessage = L"Metrics::Add(DWORD): Cannot write to output stream.";
    }
}

void Metrics::Add(std::wostringstream& sb, PCWSTR str)
{
    try
    {

        if (str != NULL)
        {
            sb << str;
        }

        sb << L',';
    }
    catch (...)
    {
        ErrorMessage = L"Metrics::Add(PCWSTR): Cannot write to output stream.";
    }
}

__int64 Metrics::s_perfTimerFrequency = 0;

bool Metrics::Initialize()
{
    if (s_perfTimerFrequency == 0)
    {
        LARGE_INTEGER freq;
        if (!QueryPerformanceFrequency(&freq))
        {
            ErrorMessage = L"QueryPerformanceFrequency failed. All time measurements are not correctly scaled.";
            return false;
        }

        InterlockedExchange64(&s_perfTimerFrequency, freq.QuadPart);
    }

    return true;
}
