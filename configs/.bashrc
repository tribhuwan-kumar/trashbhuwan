#
# ~/.bashrc
#

# If not running interactively, don't do anything
[[ $- != *i* ]] && return

alias ls='ls --color=auto'
alias grep='grep --color=auto'
PS1='[\u@\h \W]\$ '

export PATH="HOME/.local/bin:$PATH"

eval "$(oh-my-posh --init --shell bash --config /home/freakybytes/.cache/oh-my-posh/themes/Blade-Runner-2049.omp.json)"

# Uncomment if got theme not found error
#eval "$(oh-my-posh init bash)"

PS1="\u@\h:\w\$ "
set show-all-if-ambiguous on


# HSTR configuration
alias hh=hstr                    # hh to be alias for hstr
export HSTR_CONFIG=hicolor       # get more colors
shopt -s histappend              # append new history items to .bash_history
export HISTCONTROL=ignorespace   # leading space hides commands from history
export HISTFILESIZE=10000        # increase history file size (default is 500)
export HISTSIZE=${HISTFILESIZE}  # increase history size (default is 500)

# ensure synchronization between bash memory and history file
export PROMPT_COMMAND="history -a; history -n; ${PROMPT_COMMAND}"
function hstrnotiocsti {
    { READLINE_LINE="$( { </dev/tty hstr ${READLINE_LINE}; } 2>&1 1>&3 3>&- )"; } 3>&1;
    READLINE_POINT=${#READLINE_LINE}
}
# (C-r) for bash_history
if [[ $- =~ .*i.* ]]; then bind -x '"\C-r": "hstrnotiocsti"'; fi
export HSTR_TIOCSTI=n

# NPM
export PATH=~/.npm-global/bin:$PATH

#bash_completion
source /usr/share/bash-completion/bash_completion
bind 'set show-all-if-ambiguous on'
bind 'TAB:menu-complete'

#FZF
export FZF_DEFAULT_COMMAND='rg --files --hidden --glob "!.git/*"'
export FZF_DEFAULT_OPTS='--preview "bat --color=always --style=header,grid --line-range :500 {} 2> /dev/null"'

#Custom Aliases
alias ll='ls --color=auto'
alias gs='git status'
alias gaa='git add --all'
alias gc='git commit'
alias gp='git push'
alias gpl='git pull'
alias gcl='git clone'
alias vim='nvim'
alias vi='nvim'
alias nf='neofetch'
alias c='clear'
alias tx='tmux'
alias ex='exit'
