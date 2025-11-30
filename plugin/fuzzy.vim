" ============================================================================

let s:fz_job = 0
let s:results_buf = -1
let s:option_prefix = '- '
let s:results = 10

function! FuzzyFiles() abort
  " Create top mini input buffer
  botright new
  resize 12
  let s:prompt_buf = bufnr('%')
  setlocal buftype=prompt
  setlocal buftype=nofile bufhidden=wipe nobuflisted noswapfile
  setlocal nonumber norelativenumber

  " Autocmd to send input on every keystroke
  augroup FuzzyFiles
    au!
    autocmd TextChanged,TextChangedI <buffer> call s:send_query()
  augroup END

  " Start job
  call s:start_fz_job()
  
  " Key to open the highlighted file
  nnoremap <buffer> <CR> :call <SID>open_selected()<CR>
  " Start accepting input
  startinsert
endfunction

" --- Start fuzzy backend job ------------------------------------------------
function! s:start_fz_job() abort
  " Prefer using the repository's `fuzzy-search` JSON-RPC aware binary when
  " available. Fall back to a simple line-based tool if not.
  " Determine a sensible start directory: prefer the git repo top-level
  " when available, otherwise use the current working directory.
  let l:start_dir = getcwd()
  if executable('git')
    let l:git_root = systemlist('git rev-parse --show-toplevel')
    if v:shell_error == 0 && len(l:git_root) > 0
      let l:start_dir = l:git_root[0]
    endif
  endif

  if executable('fuzzy-search')
    " Ask `fuzzy-search` to list files recursively from repository root
    let cmd = ['fuzzy-search', '--files', '--search-root=' . l:start_dir, '--jsonrpc', '--results=' . s:results, '--reverse']
  else 
    echom "Error: 'fuzzy-search' executable not found in PATH."
  endif
  let s:fz_job = job_start(cmd, {
        \ 'out_cb': function('s:on_stdout'),
        \ 'err_cb': function('s:on_stderr'),
        \ 'exit_cb':   function('s:on_exit'),
        \ })
  let s:fz_channel = job_getchannel(s:fz_job)
endfunction

" --- Highlight matched characters in results buffer -------------------------
function! s:highlight_search_chars(searchString) abort
  " Create a highlight group for matched characters (bold / colored).
  if !hlexists('SearchChars')
    "execute 'highlight SearchChars cterm=bold ctermfg=Red gui=bold guifg=Red'
    execute 'highlight SearchChars gui=bold guifg=Yellow cterm=bold ctermfg=Yellow'
  endif
  " Clear any previous match highlights
  if exists('g:search_match_id')
    call matchdelete(g:search_match_id)
  endif

  let class = escape(a:searchString, '^-]\')
  let g:search_match_id = matchadd('SearchChars', '\v[' . class . ']')
endfunction

" --- Get current search string from prompt buffer ---------------------------
function! s:current_search_string() abort
  let l:search = ''
  if exists('s:prompt_buf') && bufloaded(s:prompt_buf)
    let l:prompt_line= getbufoneline(s:prompt_buf, '$')
    let l:prompt = prompt_getprompt(s:prompt_buf)
    let l:search = substitute(l:prompt_line, '^' . escape(l:prompt, '\'), '', '')
  endif
  return l:search
endfunction

" --- Handle stderr output from backend tool -------------------------------
function! s:on_stderr(job, data) abort
  " For now, ignore stderr output
  " echom "Fuzzy search stderr: " . a:data 
endfunction

" --- Update selection window as job outputs lines ---------------------------
function! s:on_stdout(job, data) abort
  " Clear the prompt buffer except for the final line (the user's input).
  if exists('s:prompt_buf') && bufloaded(s:prompt_buf)
    let l:all = getbufline(s:prompt_buf, 1, '$')
    if len(l:all) > 0
      let l:last = l:all[len(l:all) - 1]
      " Replace the first line with the last line, then delete the rest.
      call setbufline(s:prompt_buf, 1, l:last)
      if len(l:all) > 1
        call deletebufline(s:prompt_buf, 2, len(l:all))
      endif
    endif
  endif

  let msg = json_decode(a:data)
  
  let l:search = s:current_search_string()
  call s:highlight_search_chars(l:search)

  
  if type(msg) == type({}) && has_key(msg, 'method') && msg['method'] ==# 'results' && has_key(msg, 'params')
    let params = msg['params']
    if type(params) == type({}) && has_key(params, 'results')
      for r in params['results']
        if type(r) == type({}) && has_key(r, 'line')
          let l:lastLine = line('$')
          let l:text = r['line']
          call append(l:lastLine  - 1, s:option_prefix .. l:text)
        endif
      endfor
    else
      echom "Malformed results params: " . string(params)
    endif
  elseif type(msg) == type({}) && has_key(msg, 'method') && msg['method'] ==# 'finalResult' && has_key(msg, 'params')
    " Final result received; can exit the job now.
  else
    echom "Unknown JSON-RPC message: " . string(msg)
  endif
  return
endfunction

" --- Send query to backend tool --------------------------------------------
function! s:send_query() 
  let l:status = job_status(s:fz_job)
  if l:status !=? 'run' 
    return
  endif
  let line = s:current_search_string()
  let payload = {'jsonrpc': '2.0', 'method': 'input', 'params': {'type': 'SearchString', 'searchString': line}}
  call ch_sendraw(s:fz_channel, json_encode(payload) . "\n")
endfunction

" --- Handle job exit --------------------------------------------------------
function! s:on_exit(channel, exit_status) abort
  " silent cleanup OK
  unlet s:fz_job
  unlet s:fz_channel
  unlet g:search_match_id
endfunction

" --- When user hits <Enter> in results buffer -------------------------------
function! s:open_selected() abort
  let lnum = line('.')
  let file = getline(lnum)
  let file = substitute(file, '^' . escape(s:option_prefix, '\'), '', '')
  " If the fuzzy backend is running, send the accept/submit input
  " so it can emit its finalResult and exit, then wait for it to finish.
  let l:status = job_status(s:fz_job)
  if l:status ==# 'run'
    if exists('s:fz_channel')
      let payload = {'jsonrpc': '2.0', 'method': 'input', 'params': {'type': 'Newline'}}
      call ch_sendraw(s:fz_channel, json_encode(payload) . "\n")
    endif
  endif

  unlet g:search_match_id
  quit!

  if filereadable(file)
    execute 'edit' fnameescape(file)
  else
    echoerr "Not a readable file: " . file
  endif
endfunction

command! FuzzyFiles call FuzzyFiles()
nn <silent> <c-p> :FuzzyFiles<cr>
" ============================================================================

