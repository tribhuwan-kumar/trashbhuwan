# Trashbhuwan

## Introduction:
A simple CLI Application for trashing & restoring files & directories. It uses the default trashcan of KDE, GNOME, XFCE, Cinnamon, MATE, and others. `trashbhuwan` remembers the original path of the trashed files & directories, so you can restore them later where they were. Its so minimalistic and easy to use.

Available options:

    trashbhuwan --help                              : Show help
    trashbhuwan --empty                             : Empty the trash
    trashbhuwan --list                              : List all trashed files & directories with disk usage
    trashbhuwan --restore <file/directory>          : Restore trashed file or directory
    trashbhuwan --delete <file/directory>           : Delete trashed file or directory permanently

Pass multiple files or directories seprating by double quotes 

## Installation:
```bash
git clone https://github.com/tribhuwan-kumar/trashbhuwan.git && cd trashbhuwan
```

### Configuration:
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
```bash
trashbhuwan --help
```
