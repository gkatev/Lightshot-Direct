#cs Copyright (C) 2017 George Katevainis

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
#include <Inet.au3>

; Key that activates the script
local $script_key = "{PRINTSCREEN}"

; Key that lightshot uses for screenshots. Cannot be the same as $script_key
local $lightshot_key = "^{PRINTSCREEN}"

; Time taken to analyze the webpage will be displayed when this is true
local $debug_mode = 0

; Time for which to listen after the lightshot combination has been sent
local $screen_delay = 30000

; Path to lightshot. Set to empry string if automatic execution of lightshot is undesirable
local $lightshot_path = "C:\Program Files (x86)\Skillbrains\lightshot\Lightshot.exe"

; Program Start

; Only allow one instance of lightshot_direct.exe at a time
if(ProcessList("lightshot_direct.exe")[0][0] > 1) then 
	Exit 
endif

; Register the script key
HotKeySet($script_key, take_screen)

; Start lightshot if nescessary
check_lightshot()

while(1)
	Sleep(1000)
wend

; When called sends the combination so that lightshot may
; 	be activated and calls wait_screen() to listen for changes
func take_screen()
	check_lightshot()
	
	Send($lightshot_key)
	wait_screen()
endfunc


; Executes lightshot if it's not already running
; Does nothing when $lightshot_path is an empty string
func check_lightshot()
	if(not $lightshot_path) then return endif

	if(not ProcessExists("lightshot.exe")) then 
		ShellExecute('"' & $lightshot_path & '"')
		Sleep(200)
	endif
endfunc

; Wait for changes to the clipboard for $screen_delay ms
; When a lightshot produced link is detected it is analyzed and
; 	the direct link to the image is put in the clipboard
func wait_screen()
	local $timer = TimerInit()
	local $start_clip = ClipGet()
	
	; Wait for $screen_delay seconds
	while(TimerDiff($timer) <= $screen_delay)
		local $clip = ClipGet()
		
		; If the clipboard was changed and the new data is of lightshot link format analyze the data
		if($clip <> $start_clip and StringRegExp($clip, "http://prntscr\.com/\w+")) then
			ClipPut("")
			
			local $d_timer = TimerInit()
			
			; The link produced by lightshot redirects to another page using a simple formula
			local $url = "https://prnt.sc/" & StringRegExp($clip, "\/(\w+)$", 1)[0]
			local $data = _INetGetSource($url)
			
			; Analyze the page and look for the image code
			$r = StringRegExp($data, "([\w\-]{22}\.jpeg)", 1)
			
			; Add the code to the clipboard if found, display an error window otherwise
			if(@error) then 
				MsgBox($MB_ICONERROR, "Lightshot Direct", "String analyzer failed with error code " & @error)
			else 
				ClipPut("https://image.prntscr.com/image/" & $r[0])
				
				if($debug_mode) then 
					MsgBox($MB_ICONINFORMATION, "Lightshot Direct", "Time taken to analyze: " & TimerDiff($d_timer) & " ms") 
				endif
			endif
			
			exitloop
		endif
		
		Sleep(50)
	wend
endfunc
