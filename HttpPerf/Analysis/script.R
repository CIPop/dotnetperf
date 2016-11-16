# Licensed to the .NET Foundation under one or more agreements.
# The .NET Foundation licenses this file to you under the MIT license.
# See the LICENSE file in the project root for more information.

# install.packages('ggplot2')

# See http://www.sthda.com/english/ for ggplot2 examples.
library(ggplot2)
library(plyr)

raw = read.csv('../out_A4.csv')
print(summary(raw))

# Test
http_verb = "POST"
size = 1024
iter = 1000
data = subset(raw, subset = IterationResponseContentBytes == size/2 & Iterations == iter)
clients = max(data$ClientId) + 1
title = paste(clients, " clients / ", iter, " iter. (", http_verb, " size=", post_size + size, ")", sep = "")
source("showdetails.R")

# If 0, the request was a GET
unique_post_sizes = sort(unique(raw$RequestContentSize))

for (post_size in unique_post_sizes) {

    if (post_size == 0) {
        http_verb = "GET"
    }
    else {
        http_verb = "POST"
    }

    getVerb_data = subset(raw, subset = RequestContentSize == post_size)
    unique_contentsize = sort(unique(getVerb_data$IterationResponseContentBytes))

    for (size in unique_contentsize) {
        contentsizes_data = subset(getVerb_data, subset = IterationResponseContentBytes == size)
        unique_iterations = sort(unique(contentsizes_data$Iterations))
        for (iter in unique_iterations) {
            data = subset(contentsizes_data, subset = Iterations == iter)
            clients = max(data$ClientId) + 1
            title = paste(clients, " clients / ", iter, " iter. (", http_verb, " size=", post_size + size, ")", sep = "")
            print(title)
            source("showhistogram.R")
            source("showdetails.R")
            source("showjitter.R")
        }
    }
}
