// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using HttpPerf;
using Perf;
using System;
using System.Diagnostics;

namespace HttpClientPerf
{
    public class Program
    {
        public static void Main(string[] args)
        {
            string outFile = null;
            Uri server = new Uri("http://localhost/static/fixed1k.html");
            int iterations = 1;
            int instances = 1;
            int requestSizeCharacters = 0;
            bool sync = false;
            ResultWriter writer = new ResultWriterConsole();
            Stopwatch wallTime = new Stopwatch();

            // outFile
            if ((args.Length > 0) && (args[0].ToLowerInvariant() != "console"))
            {
                outFile = args[0];
                writer = new ResultWriterFile(outFile);
            }

            // server
            if (args.Length > 1)
            {
                server = new Uri(args[1]);
            }

            // iterations
            if (args.Length > 2)
            {
                int.TryParse(args[2], out iterations);
            }

            // client instances
            if (args.Length > 3)
            {
                int.TryParse(args[3], out instances);
            }

            // postcharacters
            if (args.Length > 4)
            {
                int.TryParse(args[4], out requestSizeCharacters);
            }

            // [sync/async]
            if (args.Length > 5)
            {
                if (args[5][0] == 's')
                {
                    sync = true;
                }
            }

            // type [null|httpclient]
            Func<int, PerfTest> factory = (index) =>
            {
                return new NetCoreAppHttpClientPerfTestAsync(writer, index, wallTime, server, iterations, requestSizeCharacters);
            };

            if (args.Length > 6)
            {
                string type = args[6].ToLowerInvariant();

                if (type == "null")
                {
                    factory = (index) =>
                    {
                        return new NullHttpPerfTestAsync(writer, index, wallTime, server, iterations, requestSizeCharacters);
                    };
                }
            }

            writer.WriteHeaderAsync().GetAwaiter().GetResult();

            wallTime.Start();
            if (sync || (instances == 1))
            {
                PerfTestExecution.RunSquential(writer, instances, factory).GetAwaiter().GetResult();
            }
            else
            {
                PerfTestExecution.RunParallel(writer, instances, factory).GetAwaiter().GetResult();
            }

            wallTime.Stop();

            Console.WriteLine("Done: {0}", wallTime.Elapsed);
        }
    }
}
