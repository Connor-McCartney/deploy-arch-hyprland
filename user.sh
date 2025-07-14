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

paru -S --noconfirm  noto-fonts noto-fonts-cjk noto-fonts-emoji ttf-hack-nerd ttf-comic-mono-git
paru -S --noconfirm brave-bin discord obsidian # sagemath google-chrome
paru -S --noconfirm gvfs usbutils

# wireshark needs minor workaround https://askubuntu.com/questions/968948/how-to-run-wireshark-on-ubuntu-17-10
paru -S --noconfirm xorg-xhost wireshark-qt
xhost +si:localuser:root

# LSPs
paru -S --noconfirm clang basedpyright-bin lua-language-server asm-lsp rust-analyzer # rust gets installed as a dependancy


paru -S --noconfirm arc-gtk-theme # pair with env = GTK_THEME,Arc-Dark    in hyprland.conf to set
paru -S --noconfirm papirus-folders
papirus-folders -C magenta
gsettings set org.gnome.desktop.interface icon-theme "Papirus"
#paru -S --noconfirm kvantum nwg-look # change gtk themes by running nwg-look

rm /home/connor/user.sh
