#cs Copyright (C) 2017 Georgios Katevainis-Bitzos

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 
#ce

#NoTrayIcon
#include <MsgBoxConstants.au3>

local $screen_delay = 60000
local $lightshot_path = "C:\Program Files (x86)\Skillbrains\lightshot\Lightshot.exe"

HotKeySet("{PRINTSCREEN}", take_screen)

check_lightshot()

while(1)
	Sleep(1000)
wend

func take_screen()
	check_lightshot()
	
	Send("^{PRINTSCREEN}")
	wait_screen()
endfunc

func check_lightshot()
	if(not ProcessExists("lightshot.exe")) then 
		ShellExecute('"' & $lightshot_path & '"')
		Sleep(200)
	endif
endfunc

func wait_screen()
	local $timer = TimerInit()
	local $start_clip = ClipGet()
	
	while(TimerDiff($timer) <= $screen_delay)
		local $clip = ClipGet()
		
		if($clip <> $start_clip and StringInStr($clip, "http://prntscr.com/")) then
			local $buf
			local $r = DllCall("analyze.dll", "int", "analyze_result", "str", $clip, "str", $buf, "int", 65536)
			
			if(@error) then
				MsgBox($MB_ICONERROR, "Lightshot Direct", "DllCall failed with error code " & @error)
			else
				if($r[0] == 0) then 
					ClipPut($r[2])
				else 
					MsgBox($MB_ICONERROR, "Lightshot Direct", "String Analyzer for '" & $clip & "' failed with error code " & $r[0])
				endif
			endif
			
			exitloop
		endif
		
		Sleep(50)
	wend
endfunc
