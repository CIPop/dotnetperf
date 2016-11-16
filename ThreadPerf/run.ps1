$count = 3
$t1 = 0
$t2 = 0

Write-Host -ForegroundColor Yellow ".Net Desktop 4.6.2: "
for ($i = 0; $i -lt $count; $i++)
{
    Write-Host -NoNewline "($i): "
    $t = Measure-Command { 
        .\ThreadPerf\bin\Release\ThreadPerf.exe | Write-Host
    }
    
    $t1 += $t.TotalMilliseconds
}

Write-Host -ForegroundColor Yellow ".Net Core: "
for ($i = 0; $i -lt $count; $i++)
{
    Write-Host -NoNewline "($i): "
    $t = Measure-Command { 
        dotnet .\ThreadPerfCore\bin\release\netcoreapp1.0\ThreadPerfCore.dll | Write-Host
    } 
    
    $t2 += $t.TotalMilliseconds
}

$t1 /= $count
$t2 /= $count

Write-Host ".Net Core is " (($t2/$t1) * 100 - 100) "% slower (incl. process creation overheads)."
