#include <MsgBoxConstants.au3>

local $screen_delay = 60000
local $lightshot_path = "C:\Program Files (x86)\Skillbrains\lightshot\Lightshot.exe"

local $running = 0

HotkeySet("{PRINTSCREEN}", run_lightshot)

while(1)
	if(not $running and ProcessExists("lightshot.exe")) then
		kill_lightshot()
	endif

	Sleep(1000)
wend

func run_lightshot()
	HotKeySet("{PRINTSCREEN}")
	
	ShellExecute('"' & $lightshot_path & '"')
	$running = 1
	
	Sleep(200)
	Send("{PRINTSCREEN}")
	wait_screen()
	
	kill_lightshot()
	HotkeySet("{PRINTSCREEN}", run_lightshot)
endfunc

func wait_screen()
	local $timer = TimerInit()
	local $start_clip = ClipGet()
	
	while(TimerDiff($timer) <= $screen_delay)
		local $clip = ClipGet()
		
		if($clip <> $start_clip and StringInStr($clip, "http://prntscr.com/")) then
			local $buf
			ClipPut("")
			local $r = DllCall("analyze.dll", "int", "analyze_result", "str", $clip, "str", $buf, "int", 65536)
			
			if(@error) then
				MsgBox($MB_ICONERROR, "Lightshot Direct", "DllCall failed with error code " & @error)
			else
				if($r[0] == 0) then 
					ClipPut($r[2])
				else 
					MsgBox($MB_ICONERROR, "Lightshot Direct", "String Analyzer failed with error code " & $r[0])
				endif
			endif
			
			exitloop
		endif
		
		$last_clip = $clip
		Sleep(100)
	wend
endfunc

func kill_lightshot()
	ProcessClose("lightshot.exe")
	Run("run_silent cleanup_tray")
	$running = 0
endfunc
