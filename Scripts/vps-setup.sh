#!bin/bash
# This script will setup a VPS with all the configurations I would normally use.
# It will also install all the necessary software.

USER = "fox"
GITHUB = "https://github.com/DownloadableFox"

# Check if running as root
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

# Create user fox and add to sudoers
useradd -m -s /bin/bash $USER
echo "$USER ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Create .ssh folder for the new user
mkdir /home/$USER/.ssh

# Check if curl is installed, if not exit
if ! [ -x "$(command -v curl)" ]; then
  echo "Curl is not installed... Couldn't continue!"
  exit 1
fi

# Getting keys using curl
curl -o /home/$USER/.ssh/authorized_keys ${GITHUB}.keys
chown -R $USER:$USER /home/$USER/.ssh
chmod 700 /home/$USER/.ssh
chmod 600 /home/$USER/.ssh/authorized_keys

# if argument --disable-root or -d is passed, disable ssh root login
if [ "$1" == "--disable-root" ] || [ "$1" == "-d" ]; then
  sed -i 's/PermitRootLogin yes/PermitRootLogin no/g' /etc/ssh/sshd_config
fi

# Only allow ssh login using ssh keys and disable password login
sed -i 's/#PasswordAuthentication yes/PasswordAuthentication no/g' /etc/ssh/sshd_config

# Restart ssh service
systemctl restart sshd

