// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System;
using System.Text;

namespace Perf
{
    public struct Metrics
    {
        public double? WallTime;
        public string Implementation;
        public Uri Target;
        public int? RequestContentSize;
        public int? Iterations;
        public int? ClientId;
        public double? ClientInitMs;
        public double? ClientSendRecvMs;
        public double? ClientFreeMs;
        public double? ClientTotalMs;
        public double? IterationCreateRequestMs;
        public double? IterationGetHeadersMs;
        public double? IterationGetContentMs;
        public double? IterationFreeMs;
        public int? IterationResponseHeadersBytes;
        public int? IterationResponseContentBytes;
        public string ErrorMessage;

        public static string GetHeader()
        {
            return
                "WallTime," +
                "Implementation," +
                "Target," +
                "RequestContentSize," +
                "Iterations," +
                "ClientId," +
                "ClientInitMs," +
                "ClientSendRecvMs," +
                "ClientFreeMs," +
                "ClientTotalMs," +
                "IterationCreateRequestMs," +
                "IterationGetHeadersMs," +
                "IterationGetContentMs," +
                "IterationFreeMs," +
                "IterationResponseHeadersBytes," +
                "IterationResponseContentBytes," +
                "ErrorMessage,";
        }

        public override string ToString()
        {
            var sb = new StringBuilder();
            Add(sb, WallTime);
            Add(sb, Implementation);
            Add(sb, Target.Scheme + Target.PathAndQuery);
            Add(sb, RequestContentSize.ToString());
            Add(sb, Iterations.ToString());
            Add(sb, ClientId.ToString());
            Add(sb, ClientInitMs.ToString());
            Add(sb, ClientSendRecvMs.ToString());
            Add(sb, ClientFreeMs.ToString());
            Add(sb, ClientTotalMs.ToString());
            Add(sb, IterationCreateRequestMs.ToString());
            Add(sb, IterationGetHeadersMs.ToString());
            Add(sb, IterationGetContentMs.ToString());
            Add(sb, IterationFreeMs.ToString());
            Add(sb, IterationResponseHeadersBytes.ToString());
            Add(sb, IterationResponseContentBytes.ToString());
            Add(sb, ErrorMessage);

            return sb.ToString();
        }

        private void Add(StringBuilder sb, object data)
        {
            if (data != null)
            {
                sb.Append(data.ToString());
            }

            sb.Append(',');
        }
    }
}
