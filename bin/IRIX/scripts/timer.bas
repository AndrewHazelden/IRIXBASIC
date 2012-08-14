10 forecolor 0, 255, 0 
20 backcolor 47, 47, 47
30 cls
50 print "Timer example"
95 s = timer 
100 t = timer-s
110 print t, "sec"
120 sleep 950
130 if timer>15 then goto 160
150 goto 100
155 print "------------------"
160 print "15 seconds elapsed"
200 end