Param (
    [string] $outFile = "out.csv",
    [switch] $profile = $false
    )

# Configuration -------------------------------------------------

$server = "localhost"

# Fields
$uri = @(
    "http://$server/static/fixed1k.html",
    "http://$server/static/fixed100k.html",
    "http://$server/static/fixed1MB.html",
    "http://$server/dynamic/Echo.ashx"
)

$get_uri = 0;
$post_uri_idx = 3;

$getRun = @(
    # 1k
     @{"Uri" = $uri[0]; "Iterations" = "100000"; "Clients" = "1"; "PostChars" = "0"}
    ,@{"Uri" = $uri[0]; "Iterations" = "1000"; "Clients" = "100"; "PostChars" = "0"}
    ,@{"Uri" = $uri[0]; "Iterations" = "100"; "Clients" = "1000"; "PostChars" = "0"}
    
    # 100k
    ,@{"Uri" = $uri[1]; "Iterations" = "10000"; "Clients" = "1"; "PostChars" = "0"}
    ,@{"Uri" = $uri[1]; "Iterations" = "100"; "Clients" = "100"; "PostChars" = "0"}
    ,@{"Uri" = $uri[1]; "Iterations" = "10"; "Clients" = "1000"; "PostChars" = "0"}

    # 1MB
    ,@{"Uri" = $uri[2]; "Iterations" = "1000"; "Clients" = "1"; "PostChars" = "0"}
    ,@{"Uri" = $uri[2]; "Iterations" = "10"; "Clients" = "100"; "PostChars" = "0"}
    ,@{"Uri" = $uri[2]; "Iterations" = "1"; "Clients" = "1000"; "PostChars" = "0"}
)

$postRun  = @(
    # 1k total
     @{"Uri" = $uri[$post_uri_idx]; "Iterations" = "100000"; "Clients" = "1"; "PostChars" =  "512"}
    ,@{"Uri" = $uri[$post_uri_idx]; "Iterations" = "1000"; "Clients" = "100"; "PostChars" = "512"}
    ,@{"Uri" = $uri[$post_uri_idx]; "Iterations" = "100"; "Clients" = "1000"; "PostChars" = "512"}

    # 100k total
    ,@{"Uri" = $uri[$post_uri_idx]; "Iterations" = "10000"; "Clients" = "1"; "PostChars" =  "51200"}
    ,@{"Uri" = $uri[$post_uri_idx]; "Iterations" = "100"; "Clients" = "100"; "PostChars" = "51200"}
    ,@{"Uri" = $uri[$post_uri_idx]; "Iterations" = "10"; "Clients" = "1000"; "PostChars" = "51200"}

    # 1MB total
    ,@{"Uri" = $uri[$post_uri_idx]; "Iterations" = "1000"; "Clients" = "1"; "PostChars" =  "524288"}
    ,@{"Uri" = $uri[$post_uri_idx]; "Iterations" = "10"; "Clients" = "100"; "PostChars" = "524288"}
    ,@{"Uri" = $uri[$post_uri_idx]; "Iterations" = "1"; "Clients" = "1000"; "PostChars" = "524288"}
)

$largeRun = $getRun + $postRun

$smallGETSyncRun = @(
    @{"Uri" = $uri[0]; "Iterations" = "10000"; "Clients" = "1"; "PostChars" = "0"}
)

$smallGETAsyncRun = @(
    @{"Uri" = $uri[1]; "Iterations" = "100"; "Clients" = "100"; "PostChars" = "0"},
    @{"Uri" = $uri[2]; "Iterations" = "10"; "Clients" = "100"; "PostChars" = "0"}
)

$smallPOSTSyncRun = @(
    @{"Uri" = $uri[3]; "Iterations" = "10000"; "Clients" = "1"; "PostChars" = "512"}
)

$smallPOSTAsyncRun = @(
    @{"Uri" = $uri[3]; "Iterations" = "100"; "Clients" = "100"; "PostChars" = "51200"},
    @{"Uri" = $uri[3]; "Iterations" = "10"; "Clients" = "100"; "PostChars" = "524288"}
)

$smallRun = $smallGETSyncRun + $smallGETAsyncRun + $smallPOSTSyncRun + $smallPOSTAsyncRun

$warmupConfig = @(
    @{"Uri" = $uri[0]; "Iterations" = "1"; "Clients" = "1"; "PostChars" = "0"},
    @{"Uri" = $uri[$post_uri_idx]; "Iterations" = "1"; "Clients" = "1"; "PostChars" = "10"}
)

$testConfig = $smallRun
#$testConfig = $largeRun

# NullHandler runs (overhead comparison)
#.\HttpClientPerfDesktop\bin\Release\HttpClientPerf.exe out_null.csv http://null 1000 1000 0 async null
#dotnet HttpClientPerfNetCoreApp\bin\Release\netcoreapp1.0\HttpClientPerfNetCoreApp.dll out_null.csv http://null 1000 1000 0 async null

# --------------------------------------------------------------
Function RunTest
{
    Param (
        $config,
        $profile,
        $name,
        $exePath,
        $otherArgs = "",
        [switch]$dotnet = $false
    )

    Write-Host -NoNewline "$name, $($config.Iterations), $($config.Clients), $($config.PostChars), "

    $logName = Split-Path -Leaf $exePath
    $logName += "_$name"

    $exeArguments = "$outFile $($config.Uri) $($config.Iterations) $($config.Clients) $($config.PostChars) $otherArgs"
    
    if ($dotnet)
    {
        # When using the 'dotnet' command, the dll path must be added to the arguments:
        $exeArguments = "$exePath $exeArguments"
        $exePath = "dotnet"
    }

    $perfViewOptions = "/logFile:$logName.log /dataFile:$logName.data /AcceptEula /ThreadTime /zip:true /KernelEvents:Default+VirtualAlloc" # -DotNetAllocSampled -DotNetCallsSampled"

    $t = Measure-Command {
        if ($profile)
        {
            Start-Process -WindowStyle Hidden -Wait perfview -ArgumentList "$perfViewOptions run $exePath $exeArguments"
        }
        else {
            if (-not [String]::IsNullOrEmpty($exeArguments) )
            {
                Start-Process -WindowStyle Hidden -Wait $exePath -ArgumentList $exeArguments
            }
            else
            {
                Start-Process -WindowStyle Hidden -Wait $exePath
            }
        }
    }

    Write-Host $t.TotalMilliseconds
}

Function Test
{
    Param (
            $config,
            $performanceProfiling = $false
        )
    
    RunTest $config $performanceProfiling "HttpClient_Desktop" ".\HttpClientPerfDesktop\bin\Release\HttpClientPerf.exe" "async HttpClient"
    RunTest $config $performanceProfiling "HWR_Desktop" ".\HttpClientPerfDesktop\bin\Release\HttpClientPerf.exe" "async HttpWebRequest"
    RunTest $config $performanceProfiling "HttpClientOOB_Desktop" ".\HttpClientPerfDesktopWinHttpOOB\bin\Release\HttpClientPerfDesktopWinHttpOOB.exe" "async HttpClient"
    RunTest $config $performanceProfiling "WinHttpHandler_NetCore" "HttpClientPerfNetCoreApp\bin\Release\netcoreapp1.0\HttpClientPerfNetCoreApp.dll" "async HttpClient" -dotnet
    RunTest $config $performanceProfiling "Native_WinHTTPSync" "x64\Release\WinHttpNative.exe" "sync"
    RunTest $config $performanceProfiling "Native_WinHTTPAsync" "x64\Release\WinHttpNative.exe" "async"

    if ($performanceProfiling)
    {
        $folder = "Perf_$($config.Clients)_$($config.Iterations)_$($config.PostChars)"
        mkdir $folder | Out-Null
        mv *.log $folder\ | Out-Null
        mv *.zip $folder\ | Out-Null
    }
}

rm $outFile -ErrorAction SilentlyContinue

#warmup
Write-Host -ForegroundColor Yellow "Warm-up"
foreach ($c in $warmupConfig)
{
    Test $c
}

rm $outFile

#run test
Write-Host
Write-Host -ForegroundColor Yellow "Running test"
foreach ($c in $testConfig)
{
    Write-Host -ForegroundColor Gray "`tIterations = $($c.Iterations); Clients = $($c.Clients); Uri = $($c.Uri)"
    Test $c $profile
}
