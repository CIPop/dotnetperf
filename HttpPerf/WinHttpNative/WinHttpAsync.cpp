// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include "stdafx.h"
#include "WinHttpAsync.h"
#include "WinHttpRequestContext.h"

#ifdef TRACE
#define trace(...) wprintf(__VA_ARGS__)
#else
#define trace(...)
#endif

PCWSTR WinHttpAsync::ImplementationName()
{
    return L"Native_WinHTTPAsync";
}

void WinHttpAsync::RunTestInternalAsync()
{
    _clientLevelMetrics.Start(&_clientLevelMetrics.ClientInitMs);

    if (Initialize() != ERROR_SUCCESS)
    {
        _clientLevelMetrics.ErrorMessage = L"Failed to Initialize().";
        goto error;
    }

    if (Connect() != ERROR_SUCCESS)
    {
        _clientLevelMetrics.ErrorMessage = L"Failed to Connect().";
        goto error;
    }
    
    _clientLevelMetrics.Measure(&_clientLevelMetrics.ClientInitMs);

    _clientLevelMetrics.Start(&_clientLevelMetrics.ClientSendRecvMs);
    if (SendReceive() != ERROR_SUCCESS)
    {
        _clientLevelMetrics.ErrorMessage = L"Failed to SendReceive()";
        goto error;
    }

    return;

error:
    trace(L"Error: RunTest\n");
    if (!SetEvent(_completedEvent))
    {
        trace(L"Error: Cannot set testCompletedEvent!\n");
    }
}

WinHttpAsync::~WinHttpAsync()
{
    trace(L"Destructor: session: %lx, connection: %lx, client: %lx\n",
        PtrToUlong(_sessionHandle),
        PtrToUlong(_connectionHandle),
        PtrToUlong(this));
}

DWORD WinHttpAsync::Initialize()
{
    DWORD error = 0;

    _sessionHandle = WinHttpOpen(NULL,
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC);
    if (_sessionHandle == NULL)
    {
        error = GetLastError();
        trace(L"ERROR: WinHttpOpen %x\n", error);
        return error;
    }

    trace(L"%lx: Session handle opened\n", PtrToUlong(_sessionHandle));

    BOOL optionAssuredNonBlocking = TRUE;
    BOOL fStatus;

    fStatus = WinHttpSetOption(_sessionHandle,
        WINHTTP_OPTION_ASSURED_NON_BLOCKING_CALLBACKS,
        &optionAssuredNonBlocking,
        sizeof(optionAssuredNonBlocking));
    if (!fStatus)
    {
        error = GetLastError();
        trace(L"ERROR: WinHttpSetOption(WINHTTP_OPTION_ASSURED_NON_BLOCKING_CALLBACKS) %x\n", error);
        return error;
    }

    return ERROR_SUCCESS;
}

DWORD WinHttpAsync::Connect()
{
    DWORD error = 0;

    _connectionHandle = WinHttpConnect(_sessionHandle,
        _serverName,
        _port,
        0);
    if (_connectionHandle == NULL)
    {
        error = GetLastError();
        trace(L"ERROR: WinHttpSetOption %x\n", error);
        return error;
    }

    trace(
        L"%lx: Connection handle opened (session: %lx)\n",
        PtrToUlong(_connectionHandle),
        PtrToUlong(_sessionHandle));
    return ERROR_SUCCESS;
}

DWORD WinHttpAsync::SendReceive()
{
    return SendRequest();
}

DWORD WinHttpAsync::SendRequest()
{
    BOOL fStatus = FALSE;
    DWORD error = 0;
    HINTERNET handle;

    WinHttpRequestContext* context = new WinHttpRequestContext(this);
    InitializeMetrics(context->Metrics);
    
    context->Metrics.Start(&context->Metrics.IterationCreateRequestMs);
    handle = WinHttpOpenRequest(_connectionHandle,
        _messageLength > 0 ? L"POST" : L"GET",
        _pathAndQuery,
        NULL,
        NULL,
        NULL,
        0);
    if (handle == NULL)
    {
        error = GetLastError();
        trace(L"ERROR: WinHttpOpenRequest %x\n", error);
        context->Metrics.ErrorMessage = L"WinHttpOpenRequest() failed.";
        return error;
    }

    context->RequestHandle(handle);

    trace(L"%lx: Request handle opened (connection: %lx, session: %lx, client: %lx, requestContext: %lx)\n",
        PtrToUlong(handle),
        PtrToUlong(_connectionHandle),
        PtrToUlong(_sessionHandle),
        PtrToUlong(this),
        PtrToUlong(context));
            
    WINHTTP_STATUS_CALLBACK callback = WinHttpSetStatusCallback(handle,
        WinHttpCallback,
        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS
        | WINHTTP_CALLBACK_FLAG_HANDLES
        | WINHTTP_CALLBACK_FLAG_REDIRECT
        | WINHTTP_CALLBACK_FLAG_SEND_REQUEST,
        0);
    if (callback != NULL)
    {
        error = GetLastError();
        trace(L"ERROR: WinHttpSetStatusCallback %x\n", error);
        context->Metrics.ErrorMessage = L"WinHttpSetStatusCallback() failed.";
        return error;
    }

    context->Metrics.Measure(&context->Metrics.IterationCreateRequestMs);

    context->Metrics.Start(&context->Metrics.IterationGetHeadersMs);
    fStatus = WinHttpSendRequest(handle,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        _message,
        _messageLength,
        _messageLength,
        reinterpret_cast<DWORD_PTR>(context));         // Important: must be set! 

    if (!fStatus)
    {
        error = GetLastError();
        trace(L"ERROR: WinHttpSendRequest %x\n", error);
        context->Metrics.ErrorMessage = L"WinHttpSendRequest() failed.";
        return error;
    }

    return ERROR_SUCCESS;
}

void WinHttpAsync::OnHttpSendRequestComplete(WinHttpRequestContext* context)
{
    DWORD error = 0;
    trace(L"%lx: OnHttpSendRequestComplete\n", PtrToUlong(context->RequestHandle()));

    BOOL fStatus = WinHttpReceiveResponse(context->RequestHandle(), 0);
    if (!fStatus)
    {
        error = GetLastError();
        trace(L"ERROR: WinHttpReceiveResponse %x\n", error);
        context->Metrics.ErrorMessage = L"WinHttpReceiveResponse() failed.";
    }
}

void WinHttpAsync::OnHeadersAvailable(WinHttpRequestContext* context)
{
    trace(L"%lx: OnHeadersAvailable\n", PtrToUlong(context->RequestHandle()));
    DWORD error = 0;

    // Query length.
    DWORD index = 0;
    DWORD bufferLen = 0;

    if (!WinHttpQueryHeaders(context->RequestHandle(),
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        NULL,
        &bufferLen,
        &index))
    {
        error = GetLastError();
        if (error != ERROR_INSUFFICIENT_BUFFER)
        {
            trace(L"ERROR: WinHttpQueryHeaders %x\n", error);
            context->Metrics.ErrorMessage = L"WinHttpQueryHeaders() failed to retrieve header length.";
        }
    }

    char* buffer = new char[bufferLen];
    context->Metrics.IterationResponseHeadersBytes = bufferLen;
    trace(L"%lx: OnHeadersAvailable(bufferLen: %lu)\n", PtrToUlong(context->RequestHandle()), bufferLen);

    if (!WinHttpQueryHeaders(context->RequestHandle(),
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        buffer,
        &bufferLen,
        &index))
    {
        error = GetLastError();
        trace(L"ERROR: WinHttpQueryHeaders %x\n", error);
        context->Metrics.ErrorMessage = L"WinHttpQueryHeaders() failed to retrieve headers.";
        OnError(context, error);
    }

    delete[] buffer;

    context->Metrics.Measure(&context->Metrics.IterationGetHeadersMs);

    context->Metrics.Start(&context->Metrics.IterationGetContentMs);

    context->Metrics.IterationResponseContentBytes = 0;
    error = ReadContent(context);
    if (error != ERROR_SUCCESS)
    {
        trace(L"ERROR: ReadContent %x\n", error);
    }
}

DWORD WinHttpAsync::ReadContent(WinHttpRequestContext* context)
{
    trace(L"%lx: ReadContent\n", PtrToUlong(context->RequestHandle()));

    if (!WinHttpReadData(context->RequestHandle(), context->Buffer, WinHttpRequestContext::BufferSize, NULL))
    {
        context->Metrics.ErrorMessage = L"WinHttpReadData() failed.";
        return GetLastError();
    }

    return ERROR_SUCCESS;
}

void WinHttpAsync::OnReadComplete(WinHttpRequestContext* context, DWORD bytesRead)
{
    trace(L"%lx: OnReadComplete(bytesRead:%lu)\n", PtrToUlong(context->RequestHandle()), bytesRead);
    DWORD error = 0;

    if (bytesRead == 0)
    {
        context->Metrics.Measure(&context->Metrics.IterationGetContentMs);

        context->Metrics.Start(&context->Metrics.IterationFreeMs);
        WinHttpCloseHandle(context->RequestHandle());
    }
    else
    {
        // Important: assuming at most one WinHttpReadData pending operation for this handle.
        context->Metrics.IterationResponseContentBytes += bytesRead;

        error = ReadContent(context);
        if (error)
        {
            trace(L"ERROR: ReadContent %x\n", error);
            OnError(context, error);
        }
    }
}

#pragma warning( push )
#pragma warning( disable: 4100 ) // error is used only when tracing.
void WinHttpAsync::OnError(WinHttpRequestContext* context, DWORD error)
{
    trace(L"ERROR: OnError(handle:%lx) %x\n", PtrToUlong(context->RequestHandle()), error);
    if (!WinHttpCloseHandle(context->RequestHandle()))
    {
        trace(L"ERROR: WinHttpCloseHandle() %x\n", GetLastError());
        context->Metrics.ErrorMessage = L"WinHttpCloseHandle() failed.";
    }
}
#pragma warning( pop )

void WinHttpAsync::OnHandleClosed(WinHttpRequestContext* context)
{
    if (context != nullptr)
    {
        trace(L"%lx: OnHandleClosed\n", PtrToUlong(context->RequestHandle()));

        context->Metrics.Measure(&context->Metrics.IterationFreeMs);
        InterlockedIncrement(&context->Client()->_responsesReceived);

        context->Metrics.Write(*context->Client()->_out, _outLock);

        if (context->Client()->_responsesReceived == context->Client()->_iterations)
        {
            context->Client()->_clientLevelMetrics.Measure(&context->Client()->_clientLevelMetrics.ClientSendRecvMs);
            
            if (!SetEvent(_completedEvent))
            {
                trace(L"Error: Cannot set testCompletedEvent!\n");
                context->Metrics.ErrorMessage = L"SetEvent() failed.";
            }
        }
        else
        {
            context->Client()->SendRequest();
        }
        
        delete context;
    }
}

#pragma warning( push )
#pragma warning( disable: 4100 ) // hInternet is not used.
VOID CALLBACK WinHttpAsync::WinHttpCallback(
    IN HINTERNET hInternet,
    IN DWORD_PTR dwContext,
    IN DWORD dwInternetStatus,
    IN LPVOID lpvStatusInformation OPTIONAL,
    IN DWORD dwStatusInformationLength)
{
    try
    {
        WinHttpRequestContext* requestContext = reinterpret_cast<WinHttpRequestContext*>(dwContext);
        WinHttpAsync * thisPtr = requestContext->Client();

        switch (dwInternetStatus)
        {
            // WinHttpSendRequest completed:
            case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
            {
                thisPtr->OnHttpSendRequestComplete(requestContext);
                break;
            }

            // WinHttpReceiveResponse completed:
            case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
            {
                thisPtr->OnHeadersAvailable(requestContext);
                break;
            }

            // WinHttpReadData completed
            case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
            {
                thisPtr->OnReadComplete(requestContext, dwStatusInformationLength);
                break;
            }

            // WinHttpClose completed:
            case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
            {
                trace(L"%lx WinHttpCallback(WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING).\n", PtrToUlong(hInternet));
                if (thisPtr)
                {
                    thisPtr->OnHandleClosed(requestContext);
                }

                break;
            }

            case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
            {
                if (thisPtr)
                {
                    WINHTTP_ASYNC_RESULT* err = static_cast<WINHTTP_ASYNC_RESULT*>(lpvStatusInformation);
                    requestContext->Metrics.ErrorMessage = L"WINHTTP_CALLBACK_STATUS_REQUEST_ERROR received.";
                    thisPtr->OnError(requestContext, err->dwError);
                }

                break;
            }
        }
    }
    catch (...)
    {
        // It's illegal to leak exceptions in WinHttp.
        assert(true);
    }
}
#pragma warning( pop )
