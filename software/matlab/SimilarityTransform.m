function [ AT, BT, mt, nt, TF ] = SimilarityTransform( A, B, zo, th, varargin )
%SIMILARITYTRANSFORM Applies Similarity Transform to Images
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
%

%% RUN

% Get Original Image Sizes
[m,n,p] = size(A);

% Similarity Transformation Structure Generation
T = [ zo.*cosd(th) -zo.*sind(th) 0 ; zo.*sind(th) zo.*cosd(th) 0 ; 0 0 1 ];        
TF = maketform( 'affine', T );

% Use Variable Binary Input Argument to Restrict Outputs to Original Sizes
if varargin{1}
    AT = imtransform( A, TF, 'Xdata', [1 n], 'Ydata', [1 m] );
else
    AT = imtransform( A, TF );
end;

% Get Transformed Image Size
[mt,nt,p] = size( AT );

% Generate Values to Apply New Size to Non-Transformed Image
ind = [ mt-m, nt-n ];
s = logical( sign(ind) + 1 );
O = [2.*round(ind./2) == ind];
C = O.*round( ~s .* abs(ind) ./ 2 );
P = round( s .* abs(ind) ./ 2 );

% Adjust Second Image Size
BT = B(C(1)+1:1:end-C(1)-1,C(2)+1:1:end-C(2)-1,:);
BT = padarray( padarray( BT, [0 P(2) 0], 0, 'both' ), [P(1) 0 0], 0, 'both' );

% Crop First Image In Case of Odd Pixel Counts
AT = AT(1:1:end-O(1),1:1:end-O(2),:);

% Final Transformed Image Sizes
[mt,nt,p] = size( AT );

%% END OF FILE
end
