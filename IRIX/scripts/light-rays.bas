10 forecolor 0, 255, 0
20 backcolor 0, 0, 0
30 cls
40 w = 640
50 h = 480
100 for x = 1 to 20
130 r = random % 127
135 g = random % 127
140 b = random % 127
150 forecolor r+127, g+127, b+127
160 moveto 320, 240
170 lineto x*32, 0
180 next x
200 for x = 1 to 20
230 r = random % 127
235 g = random % 127
240 b = random % 127
250 forecolor r+127, g+127, b+127
260 moveto 320, 240
270 lineto x*32, 480
280 next x
300 forecolor 0, 255, 0
400 end
