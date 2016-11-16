// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using HttpPerf;
using System;
using Perf;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Text;

namespace HttpClientPerf
{
    public class DesktopHttpWebRequestPerfTestAsync : HttpPerfTest
    {
        protected string _requestContent;

        public DesktopHttpWebRequestPerfTestAsync(
            ResultWriter writer,
            int id,
            Stopwatch wallTime,
            Uri server,
            int iterations,
            int requestSizeCharacters) : base(writer, id, wallTime, server, iterations, requestSizeCharacters)
        {
            _requestContent = GetRequestString(requestSizeCharacters);
        }

        public override string GetImplementationName()
        {
            return "HWR_Desktop";
        }

        protected override void InitClient()
        {
            ServicePointManager.Expect100Continue = true;
        }

        protected override async Task<object> CreateRequestAsync()
        {
            HttpWebRequest message = WebRequest.CreateHttp(_server);

            if (_requestContent != null)
            {
                message.Method = "POST";
                byte[] buffer = Encoding.UTF8.GetBytes(_requestContent);
                message.ContentLength = buffer.Length;

                using (Stream requestStream = await message.GetRequestStreamAsync())
                {
                    await requestStream.WriteAsync(buffer, 0, buffer.Length);
                }
            }

            return message;
        }

        protected override async Task<object> GetResponseAsync(object request)
        {
            var hwr = (HttpWebRequest)request;

            WebResponse response = await hwr.GetResponseAsync();
            return response;
        }

        protected override int GetHeadersSize(object response)
        {
            var r = (HttpWebResponse)response;
            return r.Headers.ToByteArray().Length;
        }

        protected override void DisposeClient()
        {
            // No-op.
        }

        protected override void DisposeResponse(object response)
        {
            ((HttpWebResponse)response).Dispose();
        }

        protected override async Task<int> GetContentAsync(object response)
        {
            var r = (HttpWebResponse)response;

            Debug.Assert(r.ContentLength < int.MaxValue);

            using (Stream s = r.GetResponseStream())
            using (var m = new MemoryStream((int)r.ContentLength))
            {
                await s.CopyToAsync(m);
                return m.GetBuffer().Length;
            }
        }
    }
}
