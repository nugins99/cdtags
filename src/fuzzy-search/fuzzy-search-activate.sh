
__insert_fuzzy_file() {
  local selected
  local screen_height=$(($(tput lines) - 5))
  selected="$(find . -type f -name .git -prune 2>/dev/null | fuzzy-search -s "" --stdin)"
  if [[ -n "$selected" ]]; then
    READLINE_LINE="${READLINE_LINE:0:$READLINE_POINT}$selected${READLINE_LINE:$READLINE_POINT}"
    READLINE_POINT=$(( READLINE_POINT + ${#selected} ))
  fi
}

__insert_fuzzy_dir() {
  local selected
  local screen_height=$(($(tput lines) - 5))
  selected="$(find . -type d -name .git -prune  2>/dev/null | fuzzy-search -s "" --stdin)"
  if [[ -n "$selected" ]]; then
    READLINE_LINE="${READLINE_LINE:0:$READLINE_POINT}$selected${READLINE_LINE:$READLINE_POINT}"
    READLINE_POINT=$(( READLINE_POINT + ${#selected} ))
  fi
}

__fzf_history_search() {
  local selected
  local screen_height=$(($(tput lines) - 5))
  selected="$(fuzzy-search -s "$READLINE_LINE" -f  ~/.bash_history -r $screen_height)"
  if [[ -n "$selected" ]]; then
    READLINE_LINE="$selected"
    READLINE_POINT=${#READLINE_LINE}
  fi
}

bind -x '"\C-r": __fzf_history_search'
bind -x '"\C-t": __insert_fuzzy_file'
bind -x '"\ef": __insert_fuzzy_file'
bind -x '"\ed": __insert_fuzzy_dir'