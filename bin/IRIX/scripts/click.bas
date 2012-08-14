0 backcolor 0,0,0
5 cls
6 forecolor 255,255,255
10 print "Mouse input Demo"
15 print "Click to turn the circle green"
16 print ""
17 print "Press ESC to quit"
20 b = button
30 if b = 1 then goto 100
40 if b = 0 then goto 200 
100 forecolor 0,255,0
120 fcircle 320, 240, 64
130 sleep 100
130 goto 20
200 forecolor 255,0,0
220 fcircle 320, 240, 64
225 sleep 100
230 goto 20
500 end