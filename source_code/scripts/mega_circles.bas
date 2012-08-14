10 forecolor 0, 255, 0
20 backcolor 47, 47, 47
30 cls
90 print "Drawing Random Circles"
95 for z = 0 to 25
100 for i = 0 to 100
101 r = random
102 g = random
103 b = random
104 forecolor r, g, b 
110 x = random
111 y = random
130 fcircle x*8, y*6, 32
160 next i
165 next z
170 forecolor 0, 255, 0
180 end
