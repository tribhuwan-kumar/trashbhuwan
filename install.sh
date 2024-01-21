#!/bin/bash

# Global variables
CLONE_DIRECTORY="$HOME/Downloads"



#------<log file>-------#
touch log.log

#------<ARCH DEPS>------#
echo "Updating System"
sudo pacman -Syu


