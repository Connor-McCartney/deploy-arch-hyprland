set -e

cd /tmp
git clone https://github.com/Connor-McCartney/deploy-arch-hyprland

mv /tmp/deploy-arch-hyprland/dotfiles/.autostart.sh /home/connor

mv /tmp/deploy-arch-hyprland/dotfiles/.config/hypr /home/connor/.config
mv /tmp/deploy-arch-hyprland/dotfiles/.config/alacritty /home/connor/.config
mv /tmp/deploy-arch-hyprland/dotfiles/.config/waybar /home/connor/.config


rm -rf /tmp/deploy-arch-hyprland
