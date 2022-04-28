#!bin/bash
# This script will setup a VPS with all the configurations I would normally use.
# It will also install all the necessary software.

# The packages to be installed are the following:
# - git
# - neovim
# - zsh
# - tmux
# - python3
# - python3-pip
# - python3-dev
# - nodejs
# - npm
# - nginx
# - docker
# - docker-compose

USERNAME="fox"
GITHUB="https://github.com/DownloadableFox"

# Check if running as root
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

echo "Creating user $USERNAME..."

# Create user fox and add to sudoers
useradd -m -s /bin/bash $USERNAME
echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

echo "Setting up SSH..."

# Create .ssh folder for the new user
mkdir -p /home/$USERNAME/.ssh

# Check if curl is installed, if not exit
if ! [ -x "$(command -v curl)" ]; then
  echo "Curl is not installed... Couldn't continue!"
  exit 1
fi

# Getting keys using curl
curl -o /home/$USERNAME/.ssh/authorized_keys ${GITHUB}.keys
chown -R $USERNAME:$USERNAME /home/$USERNAME/.ssh
chmod 700 /home/$USERNAME/.ssh
chmod 600 /home/$USERNAME/.ssh/authorized_keys

# if argument --disable-root or -d is passed, disable ssh root login
if [ "$1" == "--disable-root" ] || [ "$1" == "-d" ]; then
  sed -i 's/PermitRootLogin yes/PermitRootLogin no/g' /etc/ssh/sshd_config
fi

# Only allow ssh login using ssh keys and disable password login
sed -i 's/#PasswordAuthentication yes/PasswordAuthentication no/g' /etc/ssh/sshd_config

# Restart ssh service
systemctl restart sshd

echo "Downloading packages..."

# If apt installed, install dependencies using apt
if [ -x "$(command -v apt)" ]; then
  apt update
  apt upgrade
  apt install -y git neovim zsh tmux python3 python3-pip python3-dev nodejs npm nginx docker docker-compose
fi

# If dnf is installed, install dependencies using dnf
if [ -x "$(command -v dnf)" ]; then
  dnf update
  dnf upgrade
  dnf install -y git neovim zsh tmux python3 python3-pip python3-devel nodejs npm nginx docker docker-compose
fi

# If pacman is installed, install dependencies with pacman
if [ -x "$(command -v pacman)" ]; then
  pacman -Syu
  pacman -S --noconfirm git neovim zsh tmux python3 python3-pip python3-devel nodejs npm nginx docker docker-compose
fi

# Setting up zsh as default shell
chsh -s /bin/zsh $USERNAME

# Rebooting the system
echo "Rebooting the system..."
systemctl reboot