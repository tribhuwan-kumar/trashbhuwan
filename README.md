# Trashbhuwan

## Introduction:
A simple CLI Application for trashing & restoring files & directories. It uses the default trashcan of KDE, GNOME, XFCE, Cinnamon, MATE, and others. `trashbhuwan` remembers the original path of the trashed files & directories, so you can restore them later where they were. It is so minimalistic and easy to use.

Available options:

    trashbhuwan --help                              : Show help
    trashbhuwan --empty                             : Empty the trash
    trashbhuwan --list                              : List all trashed files & directories with disk usage
    trashbhuwan --put [file/directory]              : Put files & directories in trash
    trashbhuwan --show [file/directory]             : Preview trahsed files (requires 'bat', 'feh' & 'mpv')              
    trashbhuwan --restore [file/directory]          : Restore trashed file or directory
    trashbhuwan --delete [file/directory]           : Delete trashed file or directory permanently

Pass multiple files or directories separating by double quotes or single quotes.

## Installation:
```bash
git clone https://github.com/tribhuwan-kumar/trashbhuwan.git && cd trashbhuwan
```

### Configuration:

Run `install.sh` file

#### For manual config
Move, copy or create symbolic link of `trashbhuwan` to your `$PATH`

Move:
```bash
sudo mv trashbhuwan /usr/local/bin/
```
or Copy:
```bash
sudo cp trashbhuwan /usr/local/bin/
```
or Create Symbolic Link:
```bash
sudo ln -s $PWD/trashbhuwan /usr/local/bin/
```

## Usage:
For more information see:
```bash
trashbhuwan --help
```
