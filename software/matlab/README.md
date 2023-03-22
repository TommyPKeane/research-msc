# MATLAB Prototype Implementation

<!-- MarkdownTOC -->

- [MATLAB Version](#matlab-version)
- [Entrypoint\(s\)](#entrypoints)
- [Third-Party Libraries](#third-party-libraries)

<!-- /MarkdownTOC -->


<a id="matlab-version"></a>
## MATLAB Version

This code was originally developed between 2009 and 2010, using [MATLAB R2009b](https://blogs.mathworks.com/community/2009/09/07/new-and-updated-desktop-features-in-matlab-r2009b/).

<a id="entrypoints"></a>
## Entrypoint(s)

As research code, and arguably my first major software development project (prior to most of my professional Software Engineering experience), this directory is a bit of a mess of various MATLAB modules with minimal documentation.

To run the "Weighted and Filtered Mutual Information (WFMI)" Algorithm the process would be to use the `TestAffine.m` module as a MATLAB script and call the function without any arguments -- this is considered as "Phase I".

The `RUN_PhaseII.m` module provides the second stage post-processing of the results to use the estimated image overlap from `TestAffine.m` and use the [Coherent Point Drift](https://arxiv.org/abs/0905.2635) Algorithm to attempt to do the 3D Image Registration for the creation of a more accurate Panorama.

This second stage ("Phase II") was being investigated as part of the prototype research, but was not part of the novel algorithm delivered as the Intellectual Property of this Thesis.

<a id="third-party-libraries"></a>
## Third-Party Libraries

All of the code provided here was developed by me, Tommy P. Keane, in the course of the research, except for the code in the `libs/` directory per the [`libs/README.md`](./libs/README.md) documentation.

⚠️ Per the README, please note that I do not have license to share certain of the libraries that were used in the "Phase II" prototype research, so they were redacted before this code was published to GitHub.
