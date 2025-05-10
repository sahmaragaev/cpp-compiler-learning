#!/bin/bash

# Nova completion script for bash

_nova_completion() {
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD - 1]}"

    if [ $COMP_CWORD -eq 1 ]; then
        local nova_files=$(compgen -G "*.nova" 2>/dev/null)
        local options="--help -h"
        COMPREPLY=($(compgen -W "$nova_files $options" -- "$cur"))
        return 0
    fi

    if [ $COMP_CWORD -eq 2 ] && [[ "$prev" == *.nova ]]; then
        local c_files=$(compgen -G "*.c" 2>/dev/null)
        COMPREPLY=($(compgen -W "$c_files" -- "$cur"))
        return 0
    fi
}

complete -F _nova_completion nova
