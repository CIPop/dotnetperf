// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System.IO;
using System.Threading.Tasks;

namespace Perf
{
    public class ResultWriterFile : ResultWriter
    {
        private StreamWriter _writer;
        private bool _needsHeader;
        private object _lockObject = new object();

        public ResultWriterFile(string fileName)
        {
            if (!File.Exists(fileName))
            {
                _needsHeader = true;
            }

            _writer = File.AppendText(fileName);
            _writer.AutoFlush = true;
        }

        protected override Task<bool> NeedsHeader()
        {
            return Task.FromResult(_needsHeader);
        }

        protected override Task WriteLineAsync(string s)
        {
            lock (_lockObject)
            {
                _writer.WriteLine(s);
            }

            return Task.CompletedTask;
        }
    }
}
