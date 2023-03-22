function [ AT, BT, mt, nt, TForm ] = AffineTransform( A, B, AffineOptimal )
% AFFINETRANSFORM Performs Affine Transformation based on Parameters
%
% Images AT and BT are adjusted to be the same size: mt-by-nt
%
% TForm is the MATLAB imtransform compatible struct
%
% See also imtransform, maketform, padarray.

%% RUN

zo = AffineOptimal(1);
th = AffineOptimal(2);
sx = AffineOptimal(3);
sy = AffineOptimal(4);

% Get Original Image Sizes
[m,n,p] = size(A);

% Shear, Shear, Scale, Rotate
ShearX = [ 1 sx 0 ; 0 1 0 ; 0 0 1 ];
ShearY = [ 1 0 0 ; sy 1 0 ; 0 0 1 ];
Scale = [ zo 0 0 ; 0 zo 0 ; 0 0 1 ];
Rotate = [ cosd(th) sind(th) 0 ; -sind(th) cosd(th) 0 ; 0 0 1 ];

T = ShearX * ShearY * Scale * Rotate ;

% Create MATLAB TForm Object
TForm = maketform( 'affine', T );

% Transform Image
AT = imtransform( A, TForm );

% Get New Image Size
[mt,nt,p] = size( AT );

% Generate Values to Apply New Size to Non-Transformed Image
ind = [ mt-m, nt-n ];
Odd = [2.*round(ind./2) ~= ind];
Pos = logical( sign(ind) + 1 );

Bsub = ( ~Pos .* ( abs(ind) + Odd ) ) ./ 2;
Bpad = ( Pos .* ( ind + Odd ) ) ./ 2; 

% Adjust Second Image Size
BT = B(Bsub(1)+1:1:end-Bsub(1),Bsub(2)+1:1:end-Bsub(2),:);
BT = padarray( BT, [Bpad(1) Bpad(2) 0], 0, 'both' );

% Adjust Second Image Size In Case of Odd Pixel Counts
AT = AT(1:1:end-(~Pos(1).*Odd(1)),1:1:end-(~Pos(2).*Odd(2)),:);
AT = padarray( AT, [Pos(1).*Odd(1) Pos(2).*Odd(2) 0], 0, 'pre' );


% Final Transformed Image Sizes
[mt,nt,p] = size( AT );

%% END OF FILE
end