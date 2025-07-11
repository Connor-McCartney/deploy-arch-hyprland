set -e

cd /tmp
git clone https://github.com/Connor-McCartney/deploy-arch-hyprland

mv /tmp/deploy-arch-hyprland/dotfiles/.config/hypr /home/connor/.config


rm -rf /tmp/deploy-arch-hyprland
