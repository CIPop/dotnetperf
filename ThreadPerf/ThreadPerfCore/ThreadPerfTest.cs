using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace ThreadPerfCore
{
    public class ThreadPerfTest
    {
        // Tune to run the code for 10 seconds.
        public static int ParallelTasks = 100000;
        public static int IterationPerTask = 1000;

        public static async Task RunParallelInstances()
        {
            var tasks = new Task[ParallelTasks];

            Parallel.For(0, ParallelTasks, (i) =>
            {
                var test = new ThreadPerfTest();
                tasks[i] = test.Run().ContinueWith((t) =>
               {
                   if (t.Status != TaskStatus.RanToCompletion)
                   {
                       Console.Error.WriteLine(t.Exception.InnerException.ToString());
                   }
               });
            });

            await Task.WhenAll(tasks);
        }

        public async Task Run()
        {
            for (int i = 0; i < IterationPerTask; i++)
            {
                await DoNoWork();
                await DoNoWork();
                await DoNoWork();
                await DoNoWork();
                await DoNoWork();
                await DoNoWork();
                await DoNoWork();
            }
        }

        public Task<object> DoNoWork()
        {
            return Task.FromResult<object>(null);
        } 
    }
}
