// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System.Threading.Tasks;

namespace Perf
{
    public abstract class ResultWriter
    {
        public async Task WriteHeaderAsync()
        {
            if (await NeedsHeader())
            {
                await WriteLineAsync(Metrics.GetHeader());
            }
        }

        public Task WriteAsync(Metrics m)
        {
            return WriteLineAsync(m.ToString());
        }

        public Task WriteError(string errorString)
        {
            return WriteLineAsync("Error," + errorString);
        }

        protected abstract Task<bool> NeedsHeader();

        protected abstract Task WriteLineAsync(string s);
    }
}
