// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using Perf;
using System;
using System.Diagnostics;

namespace HttpClientPerf
{
    public class DesktopHttpClientPerfTestAsync : HttpClientPerfTestAsync
    {
        public DesktopHttpClientPerfTestAsync(
            ResultWriter writer,
            int id,
            Stopwatch wallTime,
            Uri server,
            int iterations,
            int requestSizeCharacters) 
            : base(writer, id, wallTime, server, iterations, requestSizeCharacters) { }

        public override string GetImplementationName()
        {
            return "HttpClient_Desktop";
        }
    }
}
