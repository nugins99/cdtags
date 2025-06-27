FUZZY_FIND_OPTS="-not -path '*/\.*';"
FUZZZY_SEARCH_ERROR_LOG=~/.cache/fuzzy-search-error.log

__insert_fuzzy_file() {
  local last_word=${READLINE_LINE:$READLINE_POINT}
  selected="$(find . -type f ${FUZZY_FIND_OPTS}  2>/dev/null | fuzzy-search -s "$last_word" --stdin 2> $FUZZZY_SEARCH_ERROR_LOG)"
  if [[ -n "$selected" ]]; then
    READLINE_LINE="${READLINE_LINE:0:$READLINE_POINT}$selected${READLINE_LINE:$READLINE_POINT}"
    READLINE_POINT=$(( READLINE_POINT + ${#selected} ))
  fi
}

__insert_fuzzy_dir() {
  local selected
  # get last WORD from the current line
  local last_word=${READLINE_LINE:$READLINE_POINT}
  selected="$(find . -type d ${FUZZY_FIND_OPTS} 2>/dev/null | fuzzy-search -s "$last_word" --stdin 2> $FUZZZY_SEARCH_ERROR_LOG)"
  if [[ -n "$selected" ]]; then
    READLINE_LINE="${READLINE_LINE:0:$READLINE_POINT}$selected${READLINE_LINE:$READLINE_POINT}"
    READLINE_POINT=$(( READLINE_POINT + ${#selected} ))
  fi
}

__fzf_history_search() {
  selected="$(fuzzy-search -s "$READLINE_LINE" -f  ~/.bash_history 2> $FUZZZY_SEARCH_ERROR_LOG)"
  if [[ -n "$selected" ]]; then
    READLINE_LINE="$selected"
    READLINE_POINT=${#READLINE_LINE}
  fi
}

bind -x '"\C-r": __fzf_history_search'
bind -x '"\C-f": __insert_fuzzy_file'
bind -x '"\C-t": __insert_fuzzy_dir'
