function [ ST ] = ProjectiveStitch( C, D, TF )
%PROJECTIVESTITCH
%
%
%
%
% See also cp2tform.
%


%% PROJECTIVE TRANSFORM WITH POINTS

[m,n,p] = size( C );

TF = fliptform( TF );

Ct = imtransform( C, TF, 'Xdata', [-n 2.*n-1], 'Ydata', [-m 2.*m-1], ...
                  'Size', [3.*m 3.*n] ...
                );
            
%% PROJECTIVE TRANSFORM WITH LINES

%% JOIN AND STITCH IMAGES

Dt = padarray( padarray( D, [0 n 0], 0, 'both' ), [m 0 0], 0, 'both' );

% ADD IMAGES
TEMP = imadd( Ct, Dt );

% CROP SPACE
[IJK] = find( TEMP ~= 0 );
[I,J,K] = ind2sub( [3.*m, 3.*n, 3], IJK );
js = min( J(:) );
jp = max( J(:) );
ip = max( I(:) );
is = min( I(:) );

Cc = Ct( is:ip, js:jp, : );
Dc = Dt( is:ip, js:jp, : );

[ ST ] = MR_Spline( Cc, Dc, [] );

%% END OF FILE
% END