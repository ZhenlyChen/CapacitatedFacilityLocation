
$sw = [Diagnostics.Stopwatch]::StartNew()
for($i=1;$i -le 13;$i++)
{
  echo $i
  type .\data\p$i | .\bin\main.exe
}
$sw.Stop()
$sw.Elapsed
