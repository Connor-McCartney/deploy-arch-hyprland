return {
    "nvim-treesitter/nvim-treesitter",
    event = { "BufReadPre", "BufNewFile" },
    build = ":TSUpdate",
    branch = 'main',
    lazy = false,
    dependencies = {
        "windwp/nvim-ts-autotag",
    },

    config = function()
        local treesitter = require("nvim-treesitter")

        treesitter.install({

                "odin",
                "json",
                "javascript",
                "typescript",
                "tsx",
                "yaml",
                "html",
                "css",
                "markdown",
                "markdown_inline",
                "bash",
                "lua",
                "vim",
                "dockerfile",
                "c",
                "cpp",
                "python",
                "rust",
                "cmake",
        })
    end,
}
