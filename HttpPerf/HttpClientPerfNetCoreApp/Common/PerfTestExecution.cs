// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System;
using System.Threading.Tasks;

namespace Perf
{
    public static class PerfTestExecution
    {
        public static Task RunParallel(
            ResultWriter writer,
            int parallelInstances,
            Func<int, PerfTest> factory)
        {
            Task[] tasks = new Task[parallelInstances];

            Parallel.For(
                0,
                parallelInstances,
                (i) =>
                {
                    PerfTest test = factory(i);
                    tasks[i] = test.Run().ContinueWith((t) =>
                    {
                        if (t.Status != TaskStatus.RanToCompletion)
                        {
                            writer.WriteError(t.Exception.InnerException.ToString());
                        }
                    });
                });

            return Task.WhenAll(tasks);
        }

        public static async Task RunSquential(
            ResultWriter writer,
            int instances,
            Func<int, PerfTest> factory)
        {
            for (int i = 0; i < instances; i++)
            {
                PerfTest test = factory(i);
                try
                {
                    await test.Run();
                }
                catch (Exception ex)
                {
                    await writer.WriteError(ex.ToString());
                }
            }
        }
    }
}
