// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System.Diagnostics;
using System.Threading.Tasks;

namespace Perf
{
    public abstract class PerfTest
    {
        protected ResultWriter _writer;
        protected int _iterations;
        protected Stopwatch _wallTime;
        protected int _id;

        public PerfTest(ResultWriter writer, int id, Stopwatch wallTime, int iterations)
        {
            _writer = writer;
            _id = id;
            _iterations = iterations;
            _wallTime = wallTime;
        }

        public abstract Task Run();

        public abstract string GetImplementationName();

        public async Task WriteMetrics(Metrics m)
        {
            await _writer.WriteAsync(m);
        }
    }
}
