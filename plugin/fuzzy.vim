" ============================================================================

let s:fz_job = 0
let s:results_buf = -1
let s:option_prefix = '- '
let s:results = 10
let s:window_open = 0

function! FuzzyFiles() abort
  " Create top mini input buffer
  if s:window_open > 0
    return
  endif
  let s:window_open = 1

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
    autocmd WinClosed <buffer> call s:close_window()
  augroup END

  " Start job
  call s:start_fz_job()
  
  " Key to open the highlighted file
  nnoremap <buffer> <CR> :call <SID>open_selected()<CR>

  " Remap <c-j> and <c-k> to go up or down in list
  inoremap <silent> <buffer> <c-j> <esc>j
  inoremap <silent> <buffer> <c-k> <esc>k
  noremap <silent> <buffer> <c-j> j
  noremap <silent> <buffer> <c-k> k

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

  " Save the search root so other functions can compute relative vs absolute
  let s:search_root = l:start_dir
  " Map of displayed result line number -> full path
  let s:results_map = {}

  if executable('fuzzy-search')
    " Ask `fuzzy-search` to list files recursively from repository root
    let cmd = ['fuzzy-search', '--files', '--search-root=' . l:start_dir, '--jsonrpc', '--results=' . s:results, '--reverse']
  else 
    echom "Error: 'fuzzy-search' executable not found in PATH."
  endif
  let s:fz_job = job_start(cmd, {
        \ 'out_cb': 's:on_stdout',
        \ 'err_cb': 's:on_stderr',
        \ 'exit_cb':'s:on_exit',
        \ })
  let s:fz_channel = job_getchannel(s:fz_job)
endfunction

function s:close_window()
    let s:window_open = 0
    # TODO - do I need to do more?
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
            let l:orig = r['line']
            " Compute absolute path for the result.
            if len(l:orig) > 0 && l:orig[0] !=# '/'
              let l:full = fnamemodify(s:search_root . '/' . l:orig, ':p')
            else
              let l:full = fnamemodify(l:orig, ':p')
            endif
            " Compute display path relative to the search root.
            let l:display = substitute(l:full, '^' . escape(s:search_root . '/', '\'), '', '')
            call append(l:lastLine  - 1, s:option_prefix .. l:display)
            " Record mapping from the buffer line number to full path.
            let l:newln = line('$')
            let s:results_map[l:newln] = l:full
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
  if exists('s:fz_job')
    unlet s:fz_job
  endif
  if exists('s:fz_channel')
    unlet s:fz_channel
  endif
  if exists('s:search_root')
    unlet s:search_root
  endif
  if exists('s:results_map')
    unlet s:results_map
  endif
  if exists('g:search_match_id')
    unlet g:search_match_id
  endif
endfunction

" --- When user hits <Enter> in results buffer -------------------------------
function! s:open_selected() abort
  let lnum = line('.')
  let ltext = getline(lnum)
  let display = substitute(ltext, '^' . escape(s:option_prefix, '\'), '', '')
  " Prefer to look up the full path recorded for this displayed line.
  if exists('s:results_map') && has_key(s:results_map, lnum)
    let file = s:results_map[lnum + 1]
  else
    let file = display
  endif
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
  let s:window_open = 0

  if filereadable(file)
    execute 'edit' fnameescape(file)
  else
    echoerr "Not a readable file: " . file
  endif
endfunction

command! FuzzyFiles call FuzzyFiles()
nn <silent> <c-p> :FuzzyFiles<cr>
" ============================================================================

