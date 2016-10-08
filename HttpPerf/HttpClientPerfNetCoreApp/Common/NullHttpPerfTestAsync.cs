// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using Perf;
using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace HttpPerf
{
    // NO-OP implementation used to baseline the harness overheads.
    public class NullHttpPerfTestAsync : HttpPerfTest
    {
        public NullHttpPerfTestAsync(ResultWriter writer, int id, Stopwatch wallTime, Uri server, int iterations, int requestSizeCharacters) 
            : base(writer, id, wallTime, server, iterations, requestSizeCharacters) { }

        protected override Task<object> CreateRequestAsync()
        {
            return Task.FromResult<object>(null);
        }

        protected override void DisposeClient() { }

        protected override void DisposeResponse(object response) { }

        protected override Task<int> GetContentAsync(object response)
        {
            return Task.FromResult<int>(0);
        }

        protected override Task<object> GetResponseAsync(object request)
        {
            return Task.FromResult<object>(null);
        }

        public override string GetImplementationName()
        {
            return "NullImplementation";
        }

        protected override void InitClient() { }

        protected override int GetHeadersSize(object response)
        {
            return 0;
        }
    }
}
