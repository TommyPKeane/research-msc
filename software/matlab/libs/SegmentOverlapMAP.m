function [ S ] = SegmentOverlapMAP( I )
% SEGMENTOVERLAP Calls RIT MAPSEG Algorithm for Color Image Segmentation
%
% Reference:
% "Two and three dimensional segmentation of multimodal imagery"
% https://scholarworks.rit.edu/theses/2959/
%
% See also.

%% VARIABLE SETUP

global j1 AlgorithmTime NumRes RdOr GnOr BlOr InfoCou ImgName clrmap;

addpath './SegmentationMAP/'  % ⚠️ License-Restricted Code has been Redacted

%% RUN SEGMENTATION

[ Segmentations, SizeInfo ] = MAPGSEG( I );

clrmap = rand( 10000, 3 );

SegmentationsTmp = Segmentations( 1 : prod( SizeInfo(1,:) ) );
    
S = reshape( SegmentationsTmp, SizeInfo(1,:) );

%% END OF FILE
end
