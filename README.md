# trashbhuwan

## introduction:
CLI application for managing recycle bin. its cross-platform, supports Windows & Linux

## demo

https://github.com/user-attachments/assets/a1add3cf-85d8-4dce-896a-6d4baf427880

## why?
i'm a termit (terminal+hermit), i mostly time spend in terminal and leaving terminal is too painful for me, mostly when i just need to put some files in recycle bin.
and the verity is i usually don't delete files bcz there is always a certainty of "what if the file i just deleted is might be needed after sometimes!" that's
why i created this cli application, which solves the problem of putting and restoring file from recycle bin.

i know there are multiple tools already created for this purpose, but they are written in python/javaScript & they aren't cross platform, which doesn't seem okay to me!

## installation 

for linux:
```bash
curl -sSL https://raw.githubusercontent.com/tribhuwan-kumar/trashbhuwan/refs/heads/main/install.sh | bash
```

on windows:
```ps1
Invoke-Expression (New-Object Net.WebClient).DownloadString('https://raw.githubusercontent.com/tribhuwan-kumar/trashbhuwan/refs/heads/main/install.ps1')
```

options:

    trashbhuwan --help                              : Show help
    trashbhuwan --empty                             : Empty the recycle bin
    trashbhuwan --list                              : List all files of recycle bin with disk usage
    trashbhuwan --put [file/directory]              : Put files in recycle bin
    trashbhuwan --restore [file/directory]          : Restore a file from recycle bin
    trashbhuwan --restore-dest [file/directory]     : Restore a file or directory to specified destination
    trashbhuwan --delete [file/directory]           : Delete files from recycle bin, permanently!

pass multiple files separating by double quotes or single quotes.

## integrations:
<details>
<summary>

### [`lf`](https://github.com/gokcehan/lf) terminal file manager:
</summary>

###### use this functions & mapping to intergrate `trashbhuwan` with `lf`:

```bash
# for putting a file in recycle bin
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

# for restoring a file from recycle bin
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

# mappings
map D trash
map d restore_trash

# only for linux
map gt cd ~/.local/share/Trash/files
```

</details>

<details>
<summary>

### [`neo-tree.nvim`](https://github.com/nvim-neo-tree/neo-tree.nvim) neovim file manager:
</summary>

use my fork of [`neo-tree.nvim`](https://github.com/tribhuwan-kumar/neo-tree.nvim) with `trashbhuwan` integration [`tribhuwan-kumar/neo-tree.nvim`](https://github.com/tribhuwan-kumar/neo-tree.nvim). it'll help you managing recycle bin directly from neovim.

</details>


## architecture
it uses the native api of windows so it'll work same as the file explorer of windows<br>
on linux, it manages trashcan same as the dolphin file manager!

for more information see:
```bash
trashbhuwan --help
```
