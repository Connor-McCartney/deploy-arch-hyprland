set -e

cd /tmp
git clone https://github.com/Connor-McCartney/deploy-arch-hyprland

mv /tmp/deploy-arch-hyprland/dotfiles/.autostart.sh /home/connor
chmod +x /home/connor/.autostart.sh

rm -rf /home/connor/.config
mv /tmp/deploy-arch-hyprland/dotfiles/.config /home/connor

rm -rf /tmp/deploy-arch-hyprland
