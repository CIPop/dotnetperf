# Licensed to the .NET Foundation under one or more agreements.
# The .NET Foundation licenses this file to you under the MIT license.
# See the LICENSE file in the project root for more information.

data_httpclient_desktop = subset(data, subset = Implementation == "HttpClient_Desktop")
d = data_httpclient_desktop
window(data_httpclient_desktop$WallTime, deltat = 1)

aggregate(d$WallTime, FUN = mean, by )

cut(d$WallTime, breaks = 10)

d=d$WallTime

# Ensure that the proper adjustment is done in case we are looking at data from multiple experiments.
adjust = max(data_httpclient_desktop$Iterations) / length(d)
bins = 100

plot(adjust * 1000/bins * table(cut(d, breaks = seq(from = 0, to = max(d), by = bins), include.lowest = TRUE)))

# TODO plot by facets.