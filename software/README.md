# Thesis Software

This directory contains the original MATLAB prototype code written for my M.Sc. Thesis: __"Weighted and Filtered Mutual Information: A metric for the automated creation of panoramas from views of complex scenes"__.

<!-- MarkdownTOC -->

- [Code Directories](#code-directories)
    - [`matlab` \(MATLAB R2009b\)](#matlab-matlab-r2009b)
    - [`cpp` \(C++\)](#cpp-c)
- [`python` \(Python 3.11\)](#python-python-311)
- [License](#license)

<!-- /MarkdownTOC -->

<a id="code-directories"></a>
## Code Directories

This code is a blend of organized software and prototype scripting, so these subsections explain the content of each directory. You can also refer to the local `README.md` for each, in order to get the details on how to run or use the code.

<a id="matlab-matlab-r2009b"></a>
### `matlab` (MATLAB R2009b)

The `matlab/` directory provides the MATLAB code for the original prototype of the algorithm from 2009 and 2010.

<a id="cpp-c"></a>
### `cpp` (C++)

The `cpp/` directory provides the C++ code for the refactored prototype implementation of the algorithm from my 2010 Co-Op (Paid Internship) role and any clean-up refactoring I had done while writing the Thesis document itself.

<a id="python-python-311"></a>
## `python` (Python 3.11)

The `python/` directory contains a recent re-implementation using Python 3.11 as a custom package with a dependency on `numpy` and `pillow` (the Python Image Library), including Unit-Testing with `pytest` and an interactive Jupyter Notebook for demonstration purposes.

<a id="license"></a>
## License

The Intellectual Property (IP) for the commercial use of this algorithm belongs to [Lenel Systems, Inc. (a UTC Fire & Security company; now Carrier)](https://www.lenels2.com/en/us/), so the code provided here is only for documentation and Academic (non-commercial) research purposes to accompany the $\LaTeX$ code of the dissertation.

The MATLAB and C++ prototypes were developed in the course of the research, including a Summer Co-Op (Paid Internship) position with Lenel in the Summer of 2010.

The Python implementation was developed retroactively as another example after I had published this work on GitHub in 2023.
