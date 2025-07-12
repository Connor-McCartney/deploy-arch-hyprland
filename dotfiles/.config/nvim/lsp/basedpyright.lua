return {
  cmd = { 'basedpyright-langserver', '--stdio' },
  filetypes = { 'python' },

  settings = {
    python = {
      analysis = {
        typeCheckingMode = "standard"
      }
    }
  }

}
