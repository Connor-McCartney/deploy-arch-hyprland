set -e

cd /tmp
git clone https://github.com/Connor-McCartney/deploy-arch-hyprland

mv /tmp/deploy-arch-hyprland/dotfiles/.bashrc /home/connor
mv /tmp/deploy-arch-hyprland/dotfiles/.fdignore /home/connor
mv /tmp/deploy-arch-hyprland/dotfiles/.wallpapers /home/connor
mv /tmp/deploy-arch-hyprland/dotfiles/.switcher.sh /home/connor
mv /tmp/deploy-arch-hyprland/dotfiles/.volume_change.mp3 /home/connor

mv /tmp/deploy-arch-hyprland/dotfiles/.autostart.sh /home/connor
chmod +x /home/connor/.autostart.sh

rm -rf /home/connor/.config
mv /tmp/deploy-arch-hyprland/dotfiles/.config /home/connor

mv /tmp/deploy-arch-hyprland/dotfiles/.bongocat /home/connor
cd /home/connor/.bongocat
make install

cd /home/connor/.wallpapers
wget https://www.desktophut.com/files/SjKfUeLTflOpVjR_Purple%20Eyes%20Live%20Wallpaper_5_103713.mp4
ffmpeg -i 'SjKfUeLTflOpVjR_Purple Eyes Live Wallpaper_5_103713.mp4' -vf "fps=30," purple_eyes.gif

rm -rf /tmp/deploy-arch-hyprland
