0 backcolor 47, 47, 47
1 forecolor 80, 80, 255
5  cls
10 print "Countdown Sequence"
20 for x = 0 to 10
25 forecolor x*10+155, 0, 0
30 print "Get ready to go in ... ";10-x
31 sleep 1000
35 next x
40 forecolor 0, 255, 0
50 print "Launch!"
60 end
