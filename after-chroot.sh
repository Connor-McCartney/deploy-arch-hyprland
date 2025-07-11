set -e

ln -sf /usr/share/zoneinfo/Australia/Brisbane /etc/localtime
hwclock --systohc
echo "en_US.UTF-8 UTF-8" > /etc/locale.gen
locale-gen
echo "LANG=en_US.UTF-8" > /etc/locale.conf
echo "arch" > /etc/hostname

pacman -S --noconfirm hyprland
pacman -S --noconfirm networkmanager neovim sudo base-devel os-prober grub vlc htop git wget ripgrep fzf bat fd kitty
pacman -S --noconfirm pulseaudio pavucontrol
systemctl enable NetworkManager

# non-LUKS
#printf "GRUB_TIMEOUT=1\nGRUB_DISTRIBUTOR=\"Arch\"\nGRUB_CMDLINE_LINUX=\"loglevel=1 nowatchdog nvme_load=YES fsck.mode=skip modprobe.blacklist=iTCO_wdt\"\n" > /etc/default/grub
#grub-install /dev/sda # BIOS
##pacman -S efibootmgr --noconfirm; grub-install --target=x86_64-efi --efi-directory=/boot # UEFI
#grub-mkconfig -o /boot/grub/grub.cfg

# LUKS UEFI
printf "MODULES=()\nBINARIES=()\nFILES=()\nHOOKS=(base udev autodetect microcode modconf kms keyboard keymap consolefont block encrypt lvm2 filesystems fsck)\n" > /etc/mkinitcpio.conf
pacman -S --noconfirm thin-provisioning-tools lvm2
mkinitcpio -p linux
pacman -S efibootmgr --noconfirm; grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB
uuid=$(blkid -s UUID -o value /dev/nvme0n1p2)
printf "GRUB_TIMEOUT=1\nGRUB_DISTRIBUTOR=\"Arch\"\nGRUB_CMDLINE_LINUX=\"loglevel=1 nowatchdog cryptdevice=UUID="$uuid":cryptlvm root=/dev/vg1/root nvme_load=YES fsck.mode=skip modprobe.blacklist=iTCO_wdt\"\n" > /etc/default/grub
grub-mkconfig -o /boot/grub/grub.cfg


# sddm auto-login
pacman -S --noconfirm sddm
mkdir /etc/sddm.conf.d
printf "[Auto login]\nUser=connor\nSession=hyprland.desktop\n" > /etc/sddm.conf.d/autologin.conf
systemctl enable sddm


useradd -m -G users,wheel,audio,video -s /bin/bash connor
printf " \n \n" | passwd
printf " \n \n" | passwd connor
echo -e "root ALL=(ALL:ALL) ALL\n%wheel ALL=(ALL:ALL) NOPASSWD: ALL\n@includedir /etc/sudoers.d" > /etc/sudoers

cd /home/connor
wget https://raw.githubusercontent.com/Connor-McCartney/deploy-arch-hyprland/refs/heads/main/user.sh
chmod +x user.sh
sudo -u connor bash user.sh

rm /after-chroot.sh
