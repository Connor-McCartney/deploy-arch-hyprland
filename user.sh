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
paru -S --noconfirm brave-bin discord obsidian sagemath google-chrome
paru -S --noconfirm obs-studio wlrobs libmpeg2 vlc-plugins-all
paru -S --noconfirm gvfs usbutils
paru -S --noconfirm file-roller thunar-archive-plugin

# just for audio effect ('play file.mp3')
paru -S --noconfirm sox twolame libmad

# wireshark needs minor workaround https://askubuntu.com/questions/968948/how-to-run-wireshark-on-ubuntu-17-10
paru -S --noconfirm xorg-xhost
# xhost +si:localuser:root # can't run in chroot, run later

# python setup
paru -S --noconfirm python-pip
python -m venv /home/connor/.p
/home/connor/.p/bin/python -m pip install --upgrade pip
/home/connor/.p/bin/pip install pycryptodome gmpy2 pwntools

# sage libs
#/usr/bin/sage --pip install pycryptodome pwntools --break-system-packages # depracated since 10.7 ?!
paru -S --noconfirm python-pycryptodome python-pwntools

# LSPs
paru -S --noconfirm clang basedpyright-bin lua-language-server asm-lsp rust-analyzer # rust gets installed as a dependancy


paru -S --noconfirm arc-gtk-theme # pair with env = GTK_THEME,Arc-Dark    in hyprland.conf to set
paru -S --noconfirm papirus-folders
papirus-folders -C magenta
gsettings set org.gnome.desktop.interface icon-theme "Papirus"
#paru -S --noconfirm kvantum nwg-look # change gtk themes by running nwg-look


# file previews 
paru -S --noconfirm tumbler ffmpegthumbnailer libopenraw

rm /home/connor/user.sh
