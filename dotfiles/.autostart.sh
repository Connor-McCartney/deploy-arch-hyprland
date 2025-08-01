# wget https://www.desktophut.com/files/SjKfUeLTflOpVjR_Purple%20Eyes%20Live%20Wallpaper_5_103713.mp4
# ffmpeg -i 'SjKfUeLTflOpVjR_Purple Eyes Live Wallpaper_5_103713.mp4' -vf "fps=30," purple_eyes.gif
swww-daemon &
swww img ~/.wallpapers/purple_eyes.gif --transition-duration=1 --transition-type wipe

hyprctl dispatch workspace 9
hyprctl dispatch workspace 8
hyprctl dispatch workspace 7
hyprctl dispatch workspace 6
hyprctl dispatch workspace 5
hyprctl dispatch workspace 4
hyprctl dispatch workspace 3
hyprctl dispatch workspace 2
hyprctl dispatch workspace 1
sudo ydotoold
