#!/bin/bash

# Global variables
CLONE_DIRECTORY="$HOME/Downloads"



#------<log file>-------#
touch log.log

#------<ARCH DEPS>------#
echo "Updating System"
sudo pacman -Syu

# Will use later
# sudo pacman -S --needed $(< ~/{backup-location}/package_list.txt)
# konsoleprofile colors=YourColorSchemeName
