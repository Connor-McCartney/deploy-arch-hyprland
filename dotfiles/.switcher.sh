# https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h
# sudo ydotool key 29:1 56:1 59:1 59:0 56:0 29:0
# sudo ydotool type 'echo Hey guys. This is Austin.'

hyprctl dispatch layoutmsg focusmaster master

sudo ydotool key 125:0 # windows up
#sleep 0.1
sudo ydotool key 1:0 # esc up
#sleep 0.1
sudo ydotool key 1:1 # esc down
#sleep 0.1
sudo ydotool key 1:0 # esc up
#sleep 0.1
sudo ydotool type ':w\n'
#sleep 0.1

#sudo ydotool key 125:1 15:1 125:0 15:0 # switch (win-tab)
hyprctl dispatch layoutmsg focusmaster slave
#sleep 0.1

#sleep 0.1

# ctrl-l
sudo ydotool key 29:1 
#sleep 0.1
sudo ydotool type 'l'
#sleep 0.1
sudo ydotool key 29:0
#sleep 0.1

sudo ydotool key 103:1 103:0 # up
#sleep 0.1
sudo ydotool key 28:1 28:0 # enter
#sleep 0.1
#sudo ydotool key 125:1 15:1 125:0 15:0 # switch (win-tab)
hyprctl dispatch layoutmsg focusmaster master
