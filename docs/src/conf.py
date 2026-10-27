from dataclasses import asdict

from sphinxawesome_theme import ThemeOptions
from sphinxawesome_theme.postprocess import Icons

# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'ENGR 100 Project Documentation'
copyright = '2026, ENGR 100 Project Team'
author = 'Hill-billies'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "sphinx_new_tab_link",
    "sphinx.ext.extlinks",
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
]

templates_path = ['_templates']
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinxawesome_theme'
html_title = project
html_permalinks_icon = Icons.permalinks_icon # nice icons
html_static_path = ['_static']
html_favicon = "favicon2.png"

theme_options = ThemeOptions(
    awesome_external_links=True,
    show_breadcrumbs=True,
    breadcrumbs_separator=".",
    show_scrolltop=True,
    logo_dark=html_favicon,
    logo_light=html_favicon,
)

html_theme_options = asdict(theme_options)
