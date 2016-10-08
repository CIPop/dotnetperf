// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using HttpPerf;
using Perf;
using System;
using System.Diagnostics;
using System.Net.Http;
using System.Threading.Tasks;

namespace HttpClientPerf
{
    public abstract class HttpClientPerfTestAsync : HttpPerfTest
    {
        protected HttpClient _client;
        protected string _requestContent;

        public HttpClientPerfTestAsync(
            ResultWriter writer,
            int id,
            Stopwatch wallTime,
            Uri server,
            int iterations,
            int requestSizeCharacters) : base(writer, id, wallTime, server, iterations, requestSizeCharacters)
        {
            _requestContent = GetRequestString(requestSizeCharacters);
        }

        protected override void InitClient()
        {
            _client = new HttpClient();
        }

        protected override Task<object> CreateRequestAsync()
        {
            HttpRequestMessage message;
            if (_requestContent != null)
            {
                message = new HttpRequestMessage(HttpMethod.Post, _server);
                message.Content = new StringContent(_requestContent);
            }
            else
            {
                message = new HttpRequestMessage(HttpMethod.Get, _server);
            }

            return Task.FromResult((object)message);
        }

        protected override async Task<object> GetResponseAsync(object request)
        {
            HttpResponseMessage response =
                await _client.SendAsync((HttpRequestMessage)request, HttpCompletionOption.ResponseHeadersRead);

            return (object)response;
        }

        protected override int GetHeadersSize(object response)
        {
            var r = (HttpResponseMessage)response;
            return r.Headers.ToString().Length * sizeof(char);
        }

        protected override async Task<int> GetContentAsync(object response)
        {
            HttpContent content = ((HttpResponseMessage)response).Content;

            byte[] c = await content.ReadAsByteArrayAsync();
            return c.Length;
        }

        protected override void DisposeResponse(object response)
        {
            ((HttpResponseMessage)response).Content.Dispose();
            ((HttpResponseMessage)response).Dispose();
        }

        protected override void DisposeClient()
        {
            _client.Dispose();
        }
    }
}
