FullName = WScript.ScriptFullName 
ScriptName = WScript.ScriptName 
BasePath = Left(FullName,Len(FullName)-Len(ScriptName)-1)  
Set WshShell = WScript.CreateObject("WScript.Shell")
WshShell.CurrentDirectory = BasePath
WshShell.Run "Isp5856.exe",1,true
