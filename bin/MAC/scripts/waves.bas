10 forecolor 0, 255, 0
15 backcolor 47, 47, 47
20 cls
40 print "Drawing waves"
50 c = 0
100 for x = 1 to 127
101 if x = 1 then moveto 0, 128
110 if x % 4 = 0 then c=c+1
110 y = cos c
130 lineto x*5, y/4+128
190 next x
201 forecolor 255, 0, 255
202 c = 0
210 for x = 1 to 127
211 if x = 1 then moveto 0, 256
220 if x % 2 = 0 then c=c+1
230 y = cos c
240 lineto x*5, y/4+256
290 next x
301 forecolor 255, 0, 255
302 c = 0
310 for x = 1 to 127
311 if x = 1 then moveto 0, 384
320 if x % 5 = 0 then c=c+1
330 y = cos c
340 b = random % 8
350 lineto x*5, y/4+b+384
390 next x
500 end
