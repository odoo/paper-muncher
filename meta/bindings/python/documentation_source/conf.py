extensions = [
    "myst_parser",
    "sphinx.ext.autodoc",
    "sphinx.ext.napoleon",
]
extensions.append("sphinx_markdown_builder")
source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}
master_doc = 'readme'
exclude_patterns = [
    'examples.rst',
    'framework_examples.rst'
] 
