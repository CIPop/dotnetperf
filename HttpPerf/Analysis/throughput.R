# Licensed to the .NET Foundation under one or more agreements.
# The .NET Foundation licenses this file to you under the MIT license.
# See the LICENSE file in the project root for more information.

data_summary = ddply(data, c("Implementation"), summarise,
    mean = mean(IterationCreateRequestMs + IterationGetHeadersMs + IterationGetContentMs, na.rm = TRUE),
    med = median(IterationCreateRequestMs + IterationGetHeadersMs + IterationGetContentMs, na.rm = TRUE),
    sd = sd(IterationCreateRequestMs + IterationGetHeadersMs + IterationGetContentMs, na.rm = TRUE),
    min = min(IterationCreateRequestMs + IterationGetHeadersMs + IterationGetContentMs, na.rm = TRUE),
    max = max(IterationCreateRequestMs + IterationGetHeadersMs + IterationGetContentMs, na.rm = TRUE),
    q99 = quantile(IterationCreateRequestMs + IterationGetHeadersMs + IterationGetContentMs, 0.99, na.rm = TRUE),
    total_execution = max(WallTime, na.rm = TRUE),
    mean_creation = mean(IterationCreateRequestMs, na.rm = TRUE),
    sd_creation = sd(IterationCreateRequestMs, na.rm = TRUE),
    mean_getheaders = mean(IterationGetHeadersMs, na.rm = TRUE),
    sd_getheaders = sd(IterationGetHeadersMs, na.rm = TRUE),
    mean_getcontent = mean(IterationGetContentMs, na.rm = TRUE),
    sd_getcontent = sd(IterationGetContentMs, na.rm = TRUE),
    requests = ((max(ClientId, na.rm = TRUE) + 1) * mean(Iterations, na.rm = TRUE)),
    total_data_mb = sum(IterationResponseContentBytes + RequestContentSize) / (1024*1024) * 8,
    connections_per_sec = ((max(ClientId, na.rm = TRUE) + 1) * min(Iterations, na.rm = TRUE)) / total_execution,
    throughput_mbps = (sum(IterationResponseContentBytes + RequestContentSize) / (1024 * 1024) * 8) / (max(WallTime, na.rm = TRUE) / 1000)
    )
print(data_summary)