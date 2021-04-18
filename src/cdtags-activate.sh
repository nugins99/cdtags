function d
{
    cd "$(cdtags cd $1)"
}

_cdtags() {
    declare cur=${COMP_WORDS[COMP_CWORD]}
    if [[ ${COMP_CWORD} -eq 1 ]]
    then
        COMPREPLY+=($(compgen -W "add remove cd complete list --help --verbose" -- $cur))
    fi
}


_cdt() {
    declare cur=${COMP_WORDS[COMP_CWORD]}
    if [[ ${COMP_CWORD} -eq 1 ]] 
    then
        COMPREPLY+=($(compgen -W "$(cdtags complete $cur 2>> ~/.cdtags.log)"))
    else 
        COMPREPLY+=($(compgen -W "$(cdtags list 2>> ~/.cdtags.log )"))
    fi
}

complete -o dirnames -F _cdtags cdtags
complete -o dirnames -o nospace -F _cdt d

