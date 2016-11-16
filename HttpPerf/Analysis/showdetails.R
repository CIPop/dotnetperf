# Licensed to the .NET Foundation under one or more agreements.
# The .NET Foundation licenses this file to you under the MIT license.
# See the LICENSE file in the project root for more information.

print("---------------------------")
print(title)

data_httpclient_desktop = subset(data, subset = Implementation == "HttpClient_Desktop")
data_httpwebrequest_desktop = subset(data, subset = Implementation == "HWR_Desktop")
data_httpclient_winhttp_desktop = subset(data, subset = Implementation == "WinHttpHandler_Desktop")
data_httpclient_winhttp_netcore = subset(data, subset = Implementation == "WinHttpHandler_NetCore")
data_httpclient_winhttp_native_sync = subset(data, subset = Implementation == "Native_WinHTTPSync")
data_httpclient_winhttp_native_async = subset(data, subset = Implementation == "Native_WinHTTPAsync")

print(ggplot(data, aes(WallTime, IterationGetHeadersMs + IterationGetContentMs, color = Implementation)) +
    geom_point() +
    facet_grid(Implementation ~ .) +
    ggtitle(paste("Request-Times Time Series", title)))

data_httpclient_desktop$WallTime = NULL
data_httpclient_desktop$Implementation = NULL
data_httpwebrequest_desktop$WallTime = NULL
data_httpwebrequest_desktop$Implementation = NULL
data_httpclient_netcore$WallTime = NULL
data_httpclient_netcore$Implementation = NULL

print(summary(data_httpclient_desktop))
print(summary(data_httpwebrequest_desktop))
print(summary(data_httpclient_winhttp_desktop))
print(summary(data_httpclient_winhttp_netcore))
