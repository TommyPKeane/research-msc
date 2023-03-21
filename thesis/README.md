# Thesis $\LaTeX$ Document

<!-- MarkdownTOC -->

- [Build Steps](#build-steps)
    - [Setup](#setup)
    - [Build Document](#build-document)

<!-- /MarkdownTOC -->

<a id="build-steps"></a>
## Build Steps

macOS Build Process using `pdflatex` installed _via_ [MacTeX](https://www.tug.org/mactex/) ([CTAN mirror](https://ctan.math.illinois.edu/systems/mac/mactex/)) from `brew`:

<a id="setup"></a>
### Setup

1. `brew install --cask mactex-no-gui` (or install from MacTeX CTAN Mirror linked above)
1. `sudo -E tlmgr update --self`
1. `sudo -E tlmgr update --all`

<a id="build-document"></a>
### Build Document

1. `pdflatex TommyKeane_MS_Thesis.tex`
1. `biber TommyKeane_MS_Thesis.bib`
1. `pdflatex TommyKeane_MS_Thesis.tex`
1. `pdflatex TommyKeane_MS_Thesis.tex`

