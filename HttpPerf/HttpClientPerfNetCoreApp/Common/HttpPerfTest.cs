// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using Perf;
using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace HttpPerf
{
    public abstract class HttpPerfTest : PerfTest
    {
        protected int _requestContentSize;
        protected Uri _server;

        public HttpPerfTest(
            ResultWriter writer,
            int id,
            Stopwatch wallTime,
            Uri server,
            int iterations,
            int requestSizeCharacters) : base(writer, id, wallTime, iterations)
        {
            _server = server;
            _requestContentSize = requestSizeCharacters;
            Debug.Assert(Stopwatch.IsHighResolution);
        }

        public override async Task Run()
        {
            Metrics clientLevelMetrics = CreateMetrics();

            Stopwatch _totalTime = new Stopwatch();
            Stopwatch _sendRecvTime = new Stopwatch();
            Stopwatch _swTime = new Stopwatch();

            _totalTime.Restart();

            _swTime.Restart();
            InitClient();
            _swTime.Stop();
            clientLevelMetrics.ClientInitMs = _swTime.Elapsed.TotalMilliseconds;

            _sendRecvTime.Restart();
            for (int i = 0; i < _iterations; i++)
            {
                Metrics iterationLevelMetrics = CreateMetrics();

                try
                {
                    _swTime.Restart();
                    object request = await CreateRequestAsync();
                    _swTime.Stop();
                    iterationLevelMetrics.IterationCreateRequestMs = _swTime.Elapsed.TotalMilliseconds;

                    _swTime.Restart();
                    object response = await GetResponseAsync(request);
                    iterationLevelMetrics.IterationResponseHeadersBytes = GetHeadersSize(response);
                    _swTime.Stop();

                    iterationLevelMetrics.IterationGetHeadersMs = _swTime.Elapsed.TotalMilliseconds;

                    _swTime.Restart();
                    iterationLevelMetrics.IterationResponseContentBytes = await GetContentAsync(response);
                    _swTime.Stop();
                    iterationLevelMetrics.IterationGetContentMs = _swTime.Elapsed.TotalMilliseconds;

                    _swTime.Restart();
                    DisposeResponse(response);
                    _swTime.Stop();
                    iterationLevelMetrics.IterationFreeMs = _swTime.Elapsed.TotalMilliseconds;
                }
                catch (Exception ex)
                {
                    iterationLevelMetrics.ErrorMessage = "\"" + ex.Message + "\"";
                    Debug.Assert(true);
                }

                await WriteMetrics(iterationLevelMetrics);
            }

            _sendRecvTime.Stop();
            clientLevelMetrics.ClientSendRecvMs = _sendRecvTime.Elapsed.TotalMilliseconds;

            _swTime.Restart();
            DisposeClient();
            _swTime.Stop();
            clientLevelMetrics.ClientFreeMs = _swTime.Elapsed.TotalMilliseconds;

            _totalTime.Stop();
            clientLevelMetrics.ClientTotalMs = _totalTime.Elapsed.TotalMilliseconds;

            await WriteMetrics(clientLevelMetrics);
        }

        protected abstract void InitClient();

        protected abstract Task<object> CreateRequestAsync();

        protected abstract Task<object> GetResponseAsync(object request);

        protected abstract int GetHeadersSize(object response);

        protected abstract Task<int> GetContentAsync(object response);

        protected abstract void DisposeResponse(object response);

        protected abstract void DisposeClient();

        protected string GetRequestString(int characters)
        {
            if (characters <= 0)
            {
                return null;
            }

            char[] charBuffer = new char[characters];

            for (int i = 0; i < characters; i++)
            {
                checked
                {
                    charBuffer[i] = (char)(i % 26 + 65);
                }
            }

            return new string(charBuffer);
        }

        private Metrics CreateMetrics()
        {
            Metrics m = new Metrics();
            m.WallTime = _wallTime.Elapsed.TotalMilliseconds;
            m.Implementation = GetImplementationName();
            m.Target = _server;
            m.RequestContentSize = _requestContentSize;
            m.Iterations = _iterations;
            m.ClientId = _id;

            return m;
        }
    }
}
