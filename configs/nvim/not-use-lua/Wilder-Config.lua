-- Autocommand to start wilder on CmdlineEnter
vim.api.nvim_create_autocmd("CmdlineEnter", {
  once = true,
  callback = function()
    require('wilder').start()
  end,
})

-- Function to initialize wilder
local function wilder_init()
  require('wilder').setup({
    modes = {':', '/', '?'},
    next_key = '<Tab>',
    previous_key = '<S-Tab>',
    accept_key = '<Down>',
    reject_key = '<Up>',
    enable_cmdline_enter = 0,
  })

  local highlighters = {
    require('wilder').pcre2_highlighter(),
    require('wilder').basic_highlighter(),
  }

  require('wilder').set_option('pipeline', require('wilder').branch(
    require('wilder').cmdline_pipeline({
      fuzzy = 1,
      fuzzy_filter = require('wilder').lua_fzy_filter(),
    }),
    require('wilder').python_search_pipeline()
  ))

  require('wilder').set_option('renderer', require('wilder').popupmenu_renderer(require('wilder').popupmenu_border_theme({
    highlights = {
      border = 'Normal',
    },
    border = 'rounded',
    highlighter = highlighters,
    left = {' ', require('wilder').popupmenu_devicons()},
    right = {' ', require('wilder').popupmenu_scrollbar()},
  })))
end

-- Call the init function
wilder_init()
