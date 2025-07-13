# https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h
# sudo ydotool key 29:1 56:1 59:1 59:0 56:0 29:0
# sudo ydotool type 'echo Hey guys. This is Austin.'


sudo ydotool key 125:0 # windows up
sudo ydotool key 1:0 # esc up
sudo ydotool type ':w'
sudo ydotool key 1:1 # esc down
sudo ydotool key 1:0 # esc up
sudo ydotool type '\n'

sudo ydotool key 125:1 15:1 125:0 15:0 # switch (win-tab)

#sleep 0.1

# ctrl-l
sudo ydotool key 29:1 
sudo ydotool type 'l'
sudo ydotool key 29:0

sudo ydotool key 103:1 103:0 # up
sudo ydotool key 28:1 28:0 # enter
sudo ydotool key 125:1 15:1 125:0 15:0 # switch (win-tab)
