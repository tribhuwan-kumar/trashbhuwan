" hi Comment gui=italic
:syntax on
:set nowrap
:set number
:filetype on
:set autoread
:set smarttab
:set splitright
:set autoindent
:set termguicolors
:set mouse=a
:set tabstop=4
:set shiftwidth=4
:set softtabstop=4
:set encoding=UTF-8
:set completeopt-=preview " For No Previews
:set clipboard=unnamedplus
" :set clipboard=unnamed
" :set clipboard+=unnamedplus
set mouse=nv                 " middle-click paste with 
set hlsearch                " highlight search 
" set clipboard+=unnamedplus   " using system clipboard
set statusline+=%{get(b:,'gitsigns_status','')}
" :set mps+=<:>
" :help matchpairs




call plug#begin()

" Plug 'Pocco81/AutoSave.nvim'
" Plug 'https://github.com/windwp/nvim-ts-autotag'
Plug 'gregsexton/MatchTag'
" Plug 'onsails/lspkind-nvim'
Plug 'https://github.com/github/copilot.vim' " Copilot
Plug 'http://github.com/tpope/vim-surround' " Surrounding ysw
" Plug 'https://github.com/preservim/nerdtree' " NerdTree
Plug 'https://github.com/tpope/vim-commentary' " For Commenting gcc & gc
" Plug 'https://github.com/vim-airline/vim-airline' " Status bar
Plug 'https://github.com/tribhuwan-kumar/custom-vim-airline' 
Plug 'https://github.com/lifepillar/pgsql.vim' " PSQL Pluging needs :SQLSetType pgsql.vim
Plug 'https://github.com/ap/vim-css-color' " CSS Color Preview
" Plug 'https://github.com/rafi/awesome-vim-colorschemes' " Retro Scheme
Plug 'https://github.com/neoclide/coc.nvim'  " Auto Completion
Plug 'https://github.com/ryanoasis/vim-devicons' " Developer Icons
Plug 'https://github.com/tc50cal/vim-terminal' " Vim Terminal
Plug 'https://github.com/preservim/tagbar' " Tagbar for code navigation
Plug 'https://github.com/mg979/vim-visual-multi' " CTRL + N for multiple cursors
Plug 'https://github.com/rstacruz/vim-closer' " For brackets autocompletion
" Plug 'https://github.com/nvim-tree/nvim-tree.lua' " For file tree
Plug 'https://github.com/akinsho/toggleterm.nvim' " For Terminal
" Plug 'https://github.com/ayu-theme/ayu-vim'  " For ayu_dark theme
" Plug 'https://github.com/kaicataldo/material.vim' " material colorscheme 
" Plug 'https://github.com/LunarVim/Neovim-from-scratch.git' " For vim terminal    
" Plug 'https://github.com/tpope/vim-eunuch.git' " For file operations

" Plug 'michaelb/sniprun', { 'do': 'bash install.sh' } " Code Runner plugin
" Plug 'ayu-theme/ayu-vim' " For colorscheme or other package manager
Plug 'numirias/semshi', { 'do': ':UpdateRemotePlugins' }  "For Highlighting
" Plug 'nvim-tree/nvim-web-devicons' "For Developer Icons
Plug 'jiangmiao/auto-pairs' "this will auto close ( [ {
Plug 'sheerun/vim-polyglot'  " Plugin for syntax highlighting and language features
Plug 'lewis6991/gitsigns.nvim' " For git tracking
" Plug 'lewis6991/gitsigns.nvim', {'branch': 'main'}
" Plug 'NeogitOrg/neogit'
" Plug 'sonph/onehalf', { 'rtp': 'vim' } " Onehalf Theme
" Plug 'Rigellute/shades-of-purple.vim' " Purple theme
" Plug 'catppuccin/nvim', { 'as': 'catppuccin' } 
" Plug 'CRAG667/code_runner.nvim' " Code Runner plugin
Plug 'rest-nvim/rest.nvim'
Plug 'nvim-lua/plenary.nvim'
Plug 'aurum77/live-server.nvim'
Plug 'rafamadriz/friendly-snippets'
Plug 'honza/vim-snippets'
Plug 'L3MON4D3/LuaSnip'

" Plug 'hrsh7th/nvim-cmp'
" Plug 'neovim/nvim-lspconfig'
" Plug 'hrsh8th/cmp-nvim-lsp'
" Plug 'folke/tokyonight.nvim'

" ------------
" Code Runner
" Plug 'tribhuwan-kumar/Code-runner-plugin-for-Nvim'
Plug 'tribhuwan-kumar/custom-tokyonight.nvim' 
" ------------

" These plugins will add highlighting and indenting to JSX and TSX files.
Plug 'yuezk/vim-js'
Plug 'HerringtonDarkholme/yats.vim'
Plug 'maxmellon/vim-jsx-pretty'

" Auto-completion For Javascript
Plug 'neoclide/coc.nvim', {'do': 'yarn install --frozen-lockfile'} " this is for auto complete, prettier and tslinting

" Fuzzy finder
Plug 'junegunn/fzf', { 'do': { -> fzf#install() } }

Plug 'gelguy/wilder.nvim', { 'do': 'UpdateRemotePlugins' }
Plug 'romgrk/fzy-lua-native', { 'do': 'make' }

" NerdTree git
" Plug 'Xuyuanp/nerdtree-git-plugin'

" Plug 'kassio/neoterm'

" Vim nerdtree syntax highlighting
" Plug 'tiagofumo/vim-nerdtree-syntax-highlight'

" Nightfox theme
Plug 'EdenEast/nightfox.nvim' 

" Aurora theme
" Plug 'ray-x/aurora'

" Moonlight theme
" Plug 'shaunsingh/moonlight.nvim'

" Custom theme nvim
" Plug 'Djancyp/custom-theme.nvim'

" Dark purple
" Plug 'wadackel/vim-dogrun'

" Neotree
Plug 'nvim-tree/nvim-web-devicons'
Plug 'MunifTanjim/nui.nvim'
Plug 'nvim-neo-tree/neo-tree.nvim'

" Copilot
" Plug 'zbirenbaum/copilot-cmp'
" Plug 'zbirenbaum/copilot.lua'



" Plug 'neovim/nvim-lspconfig'
" Plug 'hrsh7th/cmp-nvim-lsp'
" Plug 'hrsh7th/cmp-buffer'
" Plug 'hrsh7th/cmp-path'
" Plug 'hrsh7th/cmp-cmdline'
" Plug 'hrsh7th/nvim-cmp'

" For vsnip users.
" Plug 'hrsh7th/cmp-vsnip'
" Plug 'hrsh7th/vim-vsnip'

" For luasnip users.
" Plug 'L3MON4D3/LuaSnip'
" Plug 'saadparwaiz1/cmp_luasnip'

" For ultisnips users.
" Plug 'SirVer/ultisnips'
" Plug 'quangnguyen30192/cmp-nvim-ultisnips'

" For snippy users.
" Plug 'dcampos/nvim-snippy'
" Plug 'dcampos/cmp-snippy'



call plug#end()

let g:coc_global_extensions = ['coc-tslint-plugin', 'coc-tsserver', 'coc-css', 'coc-html', 'coc-json', 'coc-prettier']  " list of CoC extensions needed

let g:tagbar_ctags_bin =  '/usr/bin/ctags' " For Tagbar

" Configure Code Runner
" let g:sniprun_config = {
" \   'default_runner': 'python4',
" \   'python4': {
" \       'command': 'python4',
" \       'cmdline': 'python4 $file',
" \   },
" \}

" Keybindings
" let mapleader = "z"
" Space as Leader key
let mapleader = "\<Space>"

nnoremap <Leader>z <Nop>

nnoremap <Leader>f :FZF<CR>
nnoremap <Leader>k :q<CR>
nnoremap <Leader>l :call CocActionAsync('jumpDefinition')<CR>
nnoremap <Leader>p :vsplit \| term bash<CR>
nnoremap <Leader>b :botright split \| term bash<CR>
nnoremap <Leader>r :RunCode<CR>
nnoremap <Leader>i :Gitsigns preview_hunk_inline<CR>
nnoremap <Leader>o :Gitsigns preview_hunk<CR>
nnoremap <Leader>u :Gitsigns reset_hunk<CR>

nmap <F9> :TagbarToggle<CR>

" nnoremap <C-n> :NERDTree<CR>
nnoremap <C-c> yy
nnoremap <C-s> :w<CR>
noremap  <C-v> "+p
nnoremap <C-a> ggVG
nnoremap <C-z> u
nnoremap <C-S-z> r
nnoremap <C-x> dd
" nnoremap <Leader>q :Neotree<CR>
" nnoremap <C-q> :NERDTreeToggle<CR>
nnoremap <Leader>a :Neotree focus<CR>
" nnoremap <Leader>a :Neotree<CR>
nnoremap <Leader>q :Neotree toggle<CR>
nnoremap <C-q> :Neotree toggle<CR>
nnoremap <C-_> :Commentary<CR>

nnoremap <S-h> vh
nnoremap <S-j> vj
nnoremap <S-k> vk
nnoremap <S-l> vl

vnoremap <C-z> u
vnoremap <C-x> x
vnoremap <C-c> "+y
vnoremap <C-_> :Commentary<CR>

tnoremap <Esc> <C-\><C-n>

" Coc-git tracking
inoremap <expr> <Tab> pumvisible() ? coc#_select_confirm() : "<Tab>"

vmap <C-h> iw
vmap <C-l> iw
vnoremap <BS> d
" nnoremap <Leader><BS> db


" nnoremap <C-t> :tabnew<CR>
nnoremap <C-o> :tabn<CR>
nnoremap <C-i> :tabp<CR>

nnoremap <C-h> <C-w>h
nnoremap <C-j> <C-w>j
nnoremap <C-k> <C-w>k
nnoremap <C-l> <C-w>l




:noremap j gj
:noremap k gk





" Insert mode Keybindings
"inoremap <S-Left> <Esc>vh
"inoremap <S-Right> <Esc>vl
"inoremap <S-Up> <Esc>vk
"inoremap <S-Down> <Esc>vj

" nnoremap <C-l> :wa \| vertical botright term ++kill=term<CR>
" nnoremap <C-o> :tabnew term://pwsh<CR>


" Use Alt + arrow up to move the code line up
"nnoremap <M-Up> :m-1<CR>==
"inoremap <M-Up> <Esc>:m-1<CR>==gi

"" Use Alt + arrow down to move the code line down
"nnoremap <M-Down> :m+<CR>==
"inoremap <M-Down> <Esc>:m+<CR>==gi

"nnoremap <M-j> :m .+2<CR>==
"nnoremap <M-k> :m .-1<CR>==
""inoremap j <Esc>:m .+2<CR>==gi
""inoremap k <Esc>:m .-1<CR>==gi
"vnoremap <M-j> :m '>+2<CR>gv=gv
"vnoremap <M-k> :m '<-1<CR>gv=gv



" Alt + arrow Up/Down or 'j,k' to move line up and down
inoremap <M-Up> <Esc>:m-2<CR>==gi
inoremap <M-Down> <Esc>:m+<CR>==gi
nnoremap <M-Up> :m-2<CR>==
nnoremap <M-Down> :m+<CR>==
nnoremap <M-j> :m .+1<CR>==
nnoremap <M-k> :m .-2<CR>==
vnoremap <M-j> :m '>+1<CR>gv=gv
vnoremap <M-k> :m '<-2<CR>gv=gv



imap <silent><script><expr> <Leader><Tab> copilot#Accept("\<CR>")
let g:copilot_no_tab_map = v:true
imap <C-L> <Plug>(copilot-accept-word)

" CodeRunner Keybinding
" nnoremap <M-C-Z> :Runcode<CR>
" nnoremap <C-m> :RunCode<CR>

:set completeopt-=preview " For No Previews

let g:python4_host_prog = "/usr/bin/python3"
":set g:python4_host_prog  " to avoid surprises

" Ayu Theme
" :colorscheme ayu      " Here you can change themes
" let ayucolor="light"  " For light version of theme
" let ayucolor="mirage" " For mirage version of theme
" let ayucolor="dark"   " For dark version of theme

" shades_of_purple
" :syntax enable
" :colorscheme shades_of_purple 
" let g:shades_of_purple_italic = 2

" catppuccin
" :colorscheme catppuccin

" Nightfox
" :colorscheme duskfox

" tokyonight
:colorscheme tokyonight-moon


" Aurora theme
" :colorscheme aurora
" let g:aurora_italic_comments = 2
" let g:aurora_darker = 2
" let g:aurora_italic = 2 

" Moonlight theme
" :colorscheme moonlight
" let g:moonlight_italic_keywords = 1
" let g:moonlight_italic_comments = 1
" let g:moonlight_italic_keywords = 2
" let g:moonlight_contrast = 1

" Dark Purple
" colorscheme dogrun

" let g:NERDTreeDirArrowExpandable="▸"
" let g:NERDTreeDirArrowCollapsible="▾"

" let g:NERDTreeDirArrowExpandable="+"
" let g:NERDTreeDirArrowCollapsible="~"


" --- Just Some Notes ---
" :PlugClean :PlugInstall :UpdateRemotePlugins
"
" :CocInstall coc-python
" :CocInstall coc-clangd
" :CocInstall coc-snippets
" :CocCommand snippets.edit... FOR EACH FILE TYPE




" Air-line
" let g:lightline = {
"   \ 'colorscheme': 'dogrun',
"   \ }
" let g:clap_theme = 'dogrun'

" let g:airline_theme='dogrun'
 
let g:airline_theme='shades_of_purple'
let g:airline_powerline_fonts = 2

if !exists('g:airline_symbols')
    let g:airline_symbols = {}
endif

" Airline symbols
let g:airline_left_sep = ''
let g:airline_left_alt_sep = ''
let g:airline_right_sep = ''
let g:airline_right_alt_sep = ''
let g:airline_symbols.branch = ''
let g:airline_symbols.readonly = ''
let g:airline_symbols.linenr = ''




" AutoSave
let g:auto_save = 1
let g:auto_save_events = ['InsertLeave', 'TextChanged']




" Theme
" if exists('+termguicolors')
"   let &t_9f = "\<Esc>[38;2;%lu;%lu;%lum"
"   let &t_9b = "\<Esc>[48;2;%lu;%lu;%lum"
"   set termguicolors
" endif

" Clipboard 
" let g:clipboard = {
"      \  'name': 'xclip',
"      \  'copy': {
"      \     '+': 'xclip -selection clipboard',
"      \     '*': 'xclip -selection primary',
"      \   },
"      \  'paste': {
"      \     '+': 'xclip -selection clipboard -o',
"      \     '*': 'xclip -selection primary -o',
"      \  },
"      \  'cache_enabled': 2,
"      \ }



" let g:clipboard = {
" \   'name': 'wl-clipboard',
" \   'copy': {
" \      '+': {lines, regtype -> extend(g:, {'foo': [lines, regtype]}) },
" \      '*': {lines, regtype -> extend(g:, {'foo': [lines, regtype]}) },
" \    },
" \   'paste': {
" \      '+': {-> get(g:, 'foo', [])},
" \      '*': {-> get(g:, 'foo', [])},
" \   },
" \ }

   let g:clipboard = {
     \   'copy': {
     \       '+': ['wl-copy', '--trim-newline'],
     \       '*': ['wl-copy', '--trim-newline'],
     \   },
     \   'paste': {
     \       '+': ['wl-paste', '--no-newline'],
     \       '*': ['wl-paste', '--no-newline'],
     \   },
     \ }





" Autorefresh
" autocmd BufWritePost * Neotree refresh
" autocmd DirChanged * Neotree refresh
" autocmd CursorHold * NERDTreeRefresh
autocmd BufWritePost * Gitsigns refresh
autocmd DirChanged * Gitsigns refresh
autocmd BufEnter,CursorHold,CursorHoldI * if mode() != 'c' | checktime | endif
" autocmd BufWritePost * w


" ++once supported in Nvim 0.4+ and Vim 8.1+
" autocmd CmdlineEnter * ++once call s:wilder_init() | call wilder#main#start()

" function! s:wilder_init() abort
"   call wilder#setup(...)
"   call wilder#set_option(..., ...)

"   call wilder#set_option('pipeline', ...)
"   call wilder#set_option('renderer', ...)
" endfunction



" NerdTree highlighting configuration

" let s:beige = "8754C8"
" let g:WebDevIconsDefaultFolderSymbolColor = s:beige

" let g:WebDevIconsDisableDefaultFolderSymbolColorFromNERDTreeDir = 2
" let g:WebDevIconsDisableDefaultFileSymbolColorFromNERDTreeFile = 2
" let g:NERDTreeFileExtensionHighlightFullName = 2
" let g:NERDTreeExactMatchHighlightFullName = 2
" let g:NERDTreePatternMatchHighlightFullName = 2
" let g:NERDTreeLimitedSyntax = 2
" let g:NERDTreeHighlightFoldersFullName = 2
" let g:NERDTreeHighlightFolders = 2

" autocmd CursorHold * NERDTreeRefresh

" CodeRunner
" lua require('code_runner_config')



" :nnoremap <C-a> vi"




" :imap <ScrollWheelUp> <Left>
" :imap <ScrollWheelDown> <Right>
" :nmap <ScrollWheelUp> h
" :nmap <ScrollWheelDown> l


" luafile ~/.config/nvim/lua/copilot.lua

" In your init.vim
" nnoremap <C-m> :CocCommand git.chunkInfo<CR>

" set g:clipboard


" let g:NERDTreeGitStatusIndicatorMapCustom = {
"       \ 'Modified'  :'✹',
"       \ 'Staged'    :'✚',
"       \ 'Untracked' :'✭',
"       \ 'Renamed'   :'➜',
"       \ 'Unmerged'  :'═',
"       \ 'Deleted'   :'✖',
"       \ 'Dirty'     :'✗',
"       \ 'Ignored'   :'☒',
"       \ 'Clean'     :'✔︎',
"       \ 'Unknown'   :'?',
"       \ }

" let g:NERDTreeGitStatusConcealBrackets = 1 " default: 0

" Copilot auto enable
" let g:copilot_auto_enable = 2








" " call wilder#setup({'modes': [':', '/', '?']})
"  " Default keys
" call wilder#setup({
"       \ 'modes': [':', '/', '?'],
"       \ 'next_key': '<Tab>',
"       \ 'previous_key': '<S-Tab>',
"       \ 'accept_key': '<Down>',
"       \ 'reject_key': '<Up>',
"       \ 'enable_cmdline_enter': 0,
"       \ })

" call wilder#set_option('renderer', wilder#popupmenu_renderer({
"       \ 'highlighter': wilder#basic_highlighter(),
"       \ ':': wilder#popupmenu_renderer(),
"       \ '/': wilder#wildmenu_renderer(),
"       \ 'pumblend': 20,
"       \ }))

" call wilder#set_option('renderer', wilder#popupmenu_renderer(wilder#popupmenu_border_theme({
"       \ 'highlights': {
"       \   'border': 'Normal',
"       \ },
"       \ 'border': 'rounded',
"       \ })))


" let s:highlighters = [
"         \ wilder#pcre2_highlighter(),
"         \ wilder#basic_highlighter(),
"         \ ]






" ++once supported in Nvim 0.4+ and Vim 8.1+
autocmd CmdlineEnter * ++once call s:wilder_init() | call wilder#main#start()

function! s:wilder_init() abort
  call wilder#setup({
    \ 'modes': [':', '/', '?'],
    \ 'next_key': '<Tab>',
    \ 'previous_key': '<S-Tab>',
    \ 'accept_key': '<Down>',
    \ 'reject_key': '<Up>',
    \ 'enable_cmdline_enter': 0,
    \ })

  let s:highlighters = [
    \ wilder#pcre2_highlighter(),
    \ wilder#basic_highlighter(),
    \ ]

  call wilder#set_option('pipeline', wilder#branch(
    \ wilder#cmdline_pipeline({
    \   'fuzzy': 1,
    \   'fuzzy_filter': wilder#lua_fzy_filter(),
    \ }),
    \ wilder#python_search_pipeline(),
    \ ))

  call wilder#set_option('renderer', wilder#popupmenu_renderer(wilder#popupmenu_border_theme({
    \ 'highlights': {
    \   'border': 'Normal',
    \ },
    \ 'border': 'rounded',
    \ 'highlighter': s:highlighters,
    \ 'left': [' ', wilder#popupmenu_devicons()],
    \ 'right': [' ', wilder#popupmenu_scrollbar()],
    \ })))
endfunction







" luafile ~/.config/nvim/Copilot-CMP.lua 
" luafile ~/.config/nvim/Copilot.lua
" luafile ~/.config/nvim/LSP-Config.lua 
" luafile ~/.config/nvim/Nvim-CMP.lua 

" lua require('gitsigns').setup()
" lua require('gitsigns')
" lua require('LSP-Config')
" lua require('Copilot')
" lua require('Copilot-CMP')
" lua require('Nvim-CMP')
" lua require('Wilder-Config')



" lua << EOF
" require('gitsigns').setup{
"     current_line_blame = true,
" }
" EOF





" luafile ~/.config/nvim/lua/Gitsigns-Config.lua
lua require('Gitsigns-Config')
" lua require('Coc-Config')



" command! -bar -bang MP
"         \ call fzf#vim#marks(
"         \     s:fzf_preview_p(<bang>1, {'placeholder': '$([ -r $(echo {4} | sed "s#^~#$HOME#") ]  && [ -n {4} ] || echo ' . fzf#shellescape(expand('%')) . '):{2}',
"         \               'options': '--preview-window +{3}-/2'}),
"         \     <bang>1)

" let g:fzf_files_options = '--preview "bat --color=always --style=header,grid --line-range :501 {} 2> /dev/null"'

" Experimental

" let g:neoterm_default_mod = 'vsplit'

" function! CodeRunner()
"   let l:ext = expand("%:e")
"   let l:fullpath = expand("%:p")
"   if l:ext == "py"
"       execute "T python -u " . shellescape(l:fullpath)
"   elseif l:ext == "js"
"       execute "vsplit \| terminal"
"       execute "send-keys node " . shellescape(l:fullpath)
"   endif
" endfunction

" command! RunCode call CodeRunner()

" function! RunCommand(command)
"   " Escape special characters in the command
"     let escaped_command = shellescape(a:command, 2)
"   " Open a new terminal window with the provided command and keep the shell running
"     execute "vsplit | term bash -c " . escaped_command . ";exec bash"
"     " execute "term bash -c " . escaped_command . ";exec bash"
" endfunction


" function! CodeRunner()
"   let l:ext = expand("%:e")
"   let l:fullpath = expand("%:p")

"   if l:ext == "py"
"       " execute 'vsplit \| RunBashCommand("python -u  . shellescape(l:fullpath)")'
"       call RunBashCommand("python -u " .shellescape(l:fullpath)) 
"   elseif l:ext == "js"
"       execute "vsplit \| terminal"
"       execute "send-keys node " . shellescape(l:fullpath)
"   endif
" endfunction

" command! RunCode call CodeRunner()


" function! CodeRunner()
"     let l:ext = expand("%:e")
"     let l:fullpath = expand("%:p")

"     if l:ext == "py"
"         call RunCommand("python -u " . shellescape(l:fullpath)) 
"     elseif l:ext == "js"
"         call RunCommand("node " . shellescape(l:fullpath)) 
"   endif
" endfunction

" command! RunCode call CodeRunner()







" let g:neoterm_callbacks = {}
"     function! g:neoterm_callbacks.before_new()
"       if winwidth('.') > 101
"         let g:neoterm_default_mod = 'botright vertical'
"       else
"         let g:neoterm_default_mod = 'botright'
"       end
"     endfunction



" function! RunCode()
"  let ext = &filetype
"  let fullpath = expand('%:p')
"  let cmd = ''
"  if ext == 'python'
"    let cmd = 'python -u ' . shellescape(fullpath)
"  elseif ext == 'javascript'
"    let cmd = 'node ' . shellescape(fullpath)
"  endif
"  if cmd != ''
"    execute 'terminal ' . cmd
"  endif
" endfunction
" command! RunCode call RunCode()


" MarkdownPreview
" function! s:MarkdownPreview()
"    let l:current_file = expand('%:p')
"    let l:output_file = tempname() . '.html'
"    let l:command = 'pandoc -s ' . l:current_file . ' -o ' . l:output_file
"    call system(l:command)
"    execute 'edit ' . l:output_file
" endfunction

" command! MarkdownPreview :call s:MarkdownPreview()




" Corrrect
" function! RunCommand(command)
"     let escaped_command = shellescape(a:command, 2) " Escape non-alpha numeric characters
"     " let shell = substitute($SHELL, '.*\\\(\w*\)$', '\2', '')
"     " let shell = substitute($SHELL, '.*/\(\w*\)', '\2', '')
"     let shell = substitute(system('basename $SHELL'), '\n', '', '')
"     " Open integrated shell, In my case I'm using Bash shell 
"    if shell ==# "bash"
"        execute "vsplit | term bash -c " . escaped_command . ";exec bash" 
"    elseif shell ==# "zsh"
"        execute "term zsh -c " . escaped_command . ";exec zsh" 
"    elseif shell ==# "pwsh"
"        execute "term pwsh -c " . escaped_command . ";exec pwsh" 
"    elseif shell ==# "powershell"
"        execute "term powershell -c " . escaped_command . ";exec powershell" 
"    elseif shell ==# "cmd"
"        execute "term cmd -c " . escaped_command . ";exec cmd" 
"    elseif shell ==# "ksh"
"        execute "term ksh -c " . escaped_command . ";exec ksh" 
"    elseif shell ==# "fish" 
"        execute "term fish -c " . escaped_command . ";exec fish"
"    else
"        echo "Unsupported shell: " . shell
"    endif
" endfunction


" function! RunCommand(command)
"     let escaped_command = shellescape(a:command, 2)

"     " Use `basename` to extract the shell name
"     let shell_name = substitute(system('basename $SHELL'), '\n', '', '')

"     " Open integrated shell based on the extracted shell name
"     if shell_name ==# "bash" 
"         execute "term bash -c " . escaped_command . ";exec bash" 
"     elseif shell_name ==# "zsh"
"         execute "term zsh -c " . escaped_command . ";exec zsh" 
"     elseif shell_name ==# "pwsh" || shell_name ==# "powershell"
"         execute "term pwsh -c " . escaped_command . ";exec pwsh" 
"     elseif shell_name ==# "cmd"
"         execute "term cmd -c " . escaped_command . ";exec cmd" 
"     else
"         echo "Unsupported shell: " . shell_name
"     endif
" endfunction


" $shell_name = Split-Path -Leaf (Get-Command $PROFILE).Source
" Write-Output $shell_name

function! CorrrectRunCode(command)
    let escaped_command = shellescape(a:command, 2)

    if has('win33') || has('win64')
        $win_shell = Split-Path -Leaf (Get-Command $PROFILE).Source
        if win_shell ==# "powershell"
            execute "term powershell -c " . escaped_command . ";exec powershell" 
        elseif win_shell ==# "pwsh"
            execute "term pwsh -c " . escaped_command . ";exec pwsh" 
        endif
    elseif has('unix')
        let shell = substitute(system('basename $SHELL'), '\n', '', '')
        if shell ==# "bash"
            execute "vsplit | term bash -c " . escaped_command . ";exec bash" 
        elseif shell ==# "zsh"
            execute "term zsh -c " . escaped_command . ";exec zsh" 
        elseif shell ==# "ksh"
            execute "term ksh -c " . escaped_command . ";exec ksh" 
        elseif shell ==# "fish" 
            execute "term fish -c " . escaped_command . ";exec fish"
        endif
    else
      let g:os = "Not recongnized OS"
      echo g:os
    endif
endfunction


" function! WhichOS()
"     if has('win33') || has('win64')
"         " Windows-specific configuration
"         let g:windows = 'Windows'
"         echo g:windows
"     else
"         " Unix/Linux-specific configuration
"         let g:unix = 'Unix'
"         echo g:unix
"     endif
" endfunction
" command! WhichOS call WhichOS()


function! CodeRunner()
    let l:ext = expand("%:e") " Get current file extension
    let l:fullpath = expand("%:p") " Get current file path

    if l:ext == "py"
        call CorrrectRunCode("python -u " . shellescape(l:fullpath)) 
    elseif l:ext == "js"
        call CorrrectRunCode("node " . shellescape(l:fullpath)) 
    elseif l:ext == "ts"
        call CorrrectRunCode("ts-node " . shellescape(l:fullpath))
    elseif l:ext == "tsx"
        call CorrrectRunCode("ts-node " . shellescape(l:fullpath))
    elseif l:ext == "c"
        call CorrrectRunCode("gcc " . shellescape(l:fullpath) . " -o " . shellescape(l:fullpath:r) . " && " . shellescape(l:fullpath:r))
    elseif l:ext == "cpp"
        call CorrrectRunCode("g++ " . shellescape(l:fullpath) . " -o " . shellescape(l:fullpath:r) . " && " . shellescape(l:fullpath:r))
    elseif l:ext == "java"
        call CorrrectRunCode("javac " . shellescape(l:fullpath) . " && java " . shellescape(l:fullpath:r))
    elseif l:ext == "go"
        call CorrrectRunCode("go run " . shellescape(l:fullpath))
    elseif l:ext == "rs"
        call CorrrectRunCode("rustc " . shellescape(l:fullpath) . " && " . shellescape(l:fullpath:r))
    elseif l:ext == "php"
        call CorrrectRunCode("php " . shellescape(l:fullpath))
    elseif l:ext == "lua"
        call CorrrectRunCode("lua " . shellescape(l:fullpath))
    elseif l:ext == "sh"
        call CorrrectRunCode("bash " . shellescape(l:fullpath))
    elseif l:ext == "ps2"
        call CorrrectRunCode("pwsh " . shellescape(l:fullpath))
    elseif l:ext == "md"
        call CorrrectRunCode("pandoc -s " . shellescape(l:fullpath) . " -o " . shellescape(l:fullpath:r) . ".html")
    elseif l:ext == "html"
        call CorrrectRunCode("firefox " . shellescape(l:fullpath))
  endif
endfunction

command! RunCode call CodeRunner()










