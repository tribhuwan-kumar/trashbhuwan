# Trashbhuwan

## Introduction:
A simple CLI Application for trashing & restoring files & its written in pure C. It uses the default trashcan of KDE, GNOME, XFCE, Cinnamon, MATE, and others. `trashbhuwan` remembers the original path of the trashed files & directories, so you can restore them later where they were. It is minimalistic and easy to use.

Available options:

    trashbhuwan --help                              : Show help
    trashbhuwan --empty                             : Empty the trash
    trashbhuwan --list                              : List all trashed files & directories with disk usage
    trashbhuwan --put [file/directory]              : Put files & directories in trash
    trashbhuwan --restore [file/directory]          : Restore trashed file or directory
    trashbhuwan --delete [file/directory]           : Delete trashed file or directory permanently

Pass multiple files or directories separating by double quotes or single quotes.

## Installation:

Compilation from source code & Installation:
```bash
curl -o trashbhuwan.c https://raw.githubusercontent.com/tribhuwan-kumar/trashbhuwan/main/trashbhuwan.c && gcc -o trashbhuwan trashbhuwan.c -lcurl && sudo mv trashbhuwan /usr/local/bin/
```
Precompiled executable Installation:
```bash
curl -O https://raw.githubusercontent.com/tribhuwan-kumar/trashbhuwan/main/trashbhuwan && chmod +x trashbhuwan && sudo mv trashbhuwan /usr/local/bin/
```

### Manual Installation:
```bash
git clone https://github.com/tribhuwan-kumar/trashbhuwan.git && cd trashbhuwan
```
Compile from source code or use executable file from this repo, make sure you have `libcurl` installed:
```
gcc -o trashbhuwan trashbhuwan.c -lcurl
```

### Configuration:
Now you can either move, copy or create symbolic link of `trashbhuwan` to your `$PATH`

Move:
```bash
mv trashbhuwan /usr/local/bin/
```
or Copy:
```bash
cp trashbhuwan /usr/local/bin/
```
or Create Symbolic Link:
```bash
ln -s $PWD/trashbhuwan /usr/local/bin/
```

## Usage:
If you're a [`lf`](https://github.com/gokcehan/lf) user, use this functions & mapping to intergrate `trashbhuwan` with `lf`:
```bash
# TRASHING
cmd trash ${{
  files=$(printf "$fx" | tr '\n' ';')
  while [ "$files" ]; do
    file=${files%%;*}
    trashbhuwan -p "$(basename "$file")" 
    if [ "$files" = "$file" ]; then
      files=''
    else
      files="${files#*;}"
    fi
  done
}}

# RESTORING
cmd restore_trash ${{
  files=$(printf "$fx" | tr '\n' ';')
  while [ "$files" ]; do
    file=${files%%;*}
    trashbhuwan -r "$(basename "$file")" 
    if [ "$files" = "$file" ]; then
      files=''
    else
      files="${files#*;}"
    fi
  done 
}}

# MAPPINGS
map D trash
map d restore_trash
map gt cd ~/.local/share/Trash/files
```
If you're using [`neo-tree.nvim`](https://github.com/nvim-neo-tree/neo-tree.nvim) then use my fork with `trashbhuwan` integration [`tribhuwan-kumar/neo-tree.nvim`](https://github.com/tribhuwan-kumar/neo-tree.nvim)

For more information see:
```bash
trashbhuwan --help
```
