// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using Perf;
using System;
using System.Diagnostics;
using System.Net.Http;

namespace HttpClientPerf
{
    // This implementation is using the System.Net.Http.HttpClient from .Net Desktop and the WinHttpHandler 
    // implementation from NuGet.
    public class WinHttpHandlerPerfTestAsync : HttpClientPerfTestAsync
    {
        public WinHttpHandlerPerfTestAsync(ResultWriter writer, int id, Stopwatch wallTime, Uri server, int iterations, int requestSizeCharacters) 
            : base(writer, id, wallTime, server, iterations, requestSizeCharacters)
        {
        }

        public override string GetImplementationName()
        {
            return "WinHttpHandler_Desktop";
        }

        protected override void InitClient()
        {
            var handler = new WinHttpHandler();
            _client = new HttpClient(handler);
        }
    }
}
