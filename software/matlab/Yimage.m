function [ Y ] = Yimage( g )
%GETIMAGE Takes an RGB image name and returns the Y channel.
%   Assume image as a matrix of type double in the YES colorspace,
%   given by the system:
%
%  | Y |    | 0.253    0.684   0.063 |    | R |
%  | E | =  | 0.500   -0.500   0.000 |  * | G |
%  | S |    | 0.250    0.250  -0.500 |    | B |
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
%

%% LUMINANCE CHANNEL IMAGE

Y = 0.253 .* g(:,:,1) + 0.684 .* g(:,:,2) + 0.063 .* g(:,:,3);

%% END OF FILE
end
