using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;

namespace ThreadPerfCore
{
    public class Program
    {
        public static void Main(string[] args)
        {
            Stopwatch sw = new Stopwatch();

            sw.Start();
            ThreadPerfTest.RunParallelInstances().GetAwaiter().GetResult();
            sw.Stop();

            Console.WriteLine("{0}", sw.Elapsed);
        }
    }
}
