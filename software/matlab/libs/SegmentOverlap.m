function [ S ] = SegmentOverlap( I )
% SEGMENTOVERLAP Calls RIT MAPSEG Algorithm for Color Image Segmentation
%
% Reference:
% "Two and three dimensional segmentation of multimodal imagery"
% https://scholarworks.rit.edu/theses/2959/
%
% See also.

%% VARIABLE SETUP
global clrmap j1;

addpath './Segmentation/'  % ⚠️ License-Restricted Code has been Redacted

%% RUN SEGMENTATION

clrmap = rand(10000,3);

[ S ] = GSeg(I);

% ClrS = label2rgb( S, clrmap);

%% END OF FILE
end
