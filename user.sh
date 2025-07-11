set -e

cd /home/connor
mkdir Documents t

# Paru
cd /tmp
git clone https://aur.archlinux.org/paru-bin.git
cd paru-bin
makepkg -si --noconfirm

# dotfiles
cd /tmp
wget https://raw.githubusercontent.com/Connor-McCartney/deploy-arch-hyprland/refs/heads/main/dotfiles/install.sh
chmod +x install.sh
./install.sh

paru -S --noconfirm waybar
paru -S --noconfirm  noto-fonts noto-fonts-cjk noto-fonts-emoji ttf-hack-nerd ttf-comic-mono-git
paru -S --noconfirm brave-bin discord obsidian flameshot thunar alacritty # sagemath google-chrome
paru -S --noconfirm gvfs usbutils





rm /home/connor/user.sh
