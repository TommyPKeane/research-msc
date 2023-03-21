# "Weighted and Filtered Mutual Information: A metric for the automated creation of panoramas from views of complex scenes"

Thesis for M.Sc. (_Magister Scientiae_) Degree in Electrical Engineering. Originally defended and published in June of 2011.

<!-- MarkdownTOC -->

- [Motivation](#motivation)
- [Algorithm Summary](#algorithm-summary)
- [Directory Structure](#directory-structure)
- [Copyright and Affiliations](#copyright-and-affiliations)

<!-- /MarkdownTOC -->

<a id="motivation"></a>
## Motivation

The original motivation for this research was to provide a novel algorithm for performing automated [Image Registration](https://en.wikipedia.org/wiki/Image_registration) of surveillance cameras positioned along the exteriors of buildings or along fences, in order to create a visually cohesive panoramic view.

At the time of this research, the state-of-the-art algorithms -- [SIFT](https://en.wikipedia.org/wiki/Scale-invariant_feature_transform), [SURF](https://en.wikipedia.org/wiki/Speeded_up_robust_features), _etc._ -- were all patented and under limited Software Licenses which would've required negotiation of commercial licensing. So it was proposed to develop a novel algorithm, and I was successful in selling the Intellectual Property (while retaining the rights to publish academically, non-commercially) to the Lenel Systems, Inc. (a UTC Fire & Security company; now Carrier) with the support of the Rochester Institute of Technology (RIT).

<a id="algorithm-summary"></a>
## Algorithm Summary

The approach presented here relies on an assumption of statistical similarity in the pixel content between overlapping views, even when there are non-trivial 3D rotations between the cameras.

Based-on that assumption, the algorithm uses [Information Theory](https://en.wikipedia.org/wiki/Information_theory) concepts like the measure of [Mutual Information](https://en.wikipedia.org/wiki/Mutual_information) to create a statistical map of the overlap similarity.

That "map" is a two-dimensional function in a three-dimensional space, which is assumed to create a smooth, differentiable manifold that can then be "smoothed" with a [Laplacian](https://en.wikipedia.org/wiki/Laplace_operator) [convolution kernel](https://en.wikipedia.org/wiki/Kernel_(image_processing)).

The outcome of this "smoothing" operation should then provide a manifold with a trivially unique "peak" (statistical "mode") whose $\left(x, y\right)$ coordinates refer to the pixel overlap of the two images.

Overall, this process provides a quick-to-compute algorithm to give an estimated registration between two images, which is somewhat robust against lighting and orientation variability between the distinct views.

<a id="directory-structure"></a>
## Directory Structure

- [`data`](./data/): Associated Data Files for the Research and Documents
- [`publications`](./publications/): Pre-Print $\LaTeX$ versions of related Conference and Journal Publications
- [`software`](./software/): Related Code created during the Research Process
- [`thesis`](./thesis/): $\LaTex$ Thesis Document Code

<a id="copyright-and-affiliations"></a>
## Copyright and Affiliations

Copyright (c) Tommy P. Keane, 2011

This work was developed as part of the Electrical Engineering M.Sc. degree program at the Rochester Institute of Technology (RIT) by Tommy P. Keane, from April 2009 through to June 2011. The thesis was published by RIT after being defended through an oral presentation by Tommy Keane in June of 2011, and accompanied by several Conference Publications and an accepted publication in the SPIE Journal of Electronic Imaging (JEI).

Funding for this research was supported by Lenel Systems, Inc. (a UTC Fire & Security company; now Carrier) through RIT during the April 2009 to May 2010 timeframe. The novel algorithm developed in this research was ported from MATLAB to C++ with OpenCV during the Summer of 2010 while I worked as a full-time Co-Op Student Employee of Lenel Systems, Inc. in Pittsford, NY.

In September 2010, I started the Ph.D. Program at RIT in Imaging Science, and I was funded as a Teaching Assistant variously between the Electrical Engineering, Computer Engineering, and Imaging Science departments from September 2010 through September 2011. During that first year in the Ph.D. Program while completing the core curriculum, I wrote (and re-wrote) the M.Sc. Thesis which was successfully defended and published in June 2011.

> _Fun Tommy Trivia 👻🤓: Due to the fact that I was in the Dual-Degree B.Sc. + M.Sc. Co-Op Program in Electrical Engineering, I was not able to receive my B.Sc. Degree Diploma, officially, until my M.Sc. Thesis was accepted and published. This means that I technically started the Ph.D. program with only a High School diploma (which complicated the paperwork), and I was technically working on all 3 degrees at the same time in 2010 and 2011._
