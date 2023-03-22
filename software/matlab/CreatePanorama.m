function [ SP, T, zo, th ] = CreatePanorama( impath, root, LN, RN, ext, numPymd, w, alpha, AffineParam, numEdge )
%MMR_ALMI
%
%
%
%
%
% See also cpselect, cp2tform, imtransform, imread, MR_ALMI

%% FILE READING AND NAMING

tic;

L = '_L_';
R = '_R_';
SP = '_SP_';
IWSP = [ './Results/' root SP LN '_' RN ext ];

%% READ IMAGE FILES

fname = [impath root L LN ext];
f = double( imread( fname ) );
f = f(10:1:end-9,10:1:end-9, :);

gname = [impath root R RN ext];
g = double( imread( gname ) );
g = g(10:1:end-9,10:1:end-9, :);


%% MULTI RESOLUTION wMI SEARCH

[ AT, BT, At, Bt, mt, nt, dr, dc, TF, AffineOptimal ] = RegisterImages( f, g, numPymd, w, alpha, AffineParam, numEdge );

[ C, D, SL ] = Peak2Overlap( At, Bt, mt(1), nt(1), dr(1), dc(1) );
[ CT, DT, SLt ] = Peak2Overlap( AT{1}, BT{1}, mt(1), nt(1), dr(1), dc(1) );

MASKd = zeros( size(CT) );
for i = 1 : 1 : mt;
    MASKd( i, 1:1:SL(i,1) ) = 1;
end;

% BD = imclose( ceil( (D(:,:,1)+D(:,:,2)+D(:,:,3))./3 ), ones(10) );
BC = imclose( ceil( (C(:,:,1)+C(:,:,2)+C(:,:,3))./3 ), ones(10) );
MASKa = (BC + zeros(size(DT)) ./ 2);

[ SP ] = MR_Spline( C, D, [] );
[ SPD ] = MR_SplineM( C, D, MASKd );
[ SPA ] = MR_SplineM( C, D, MASKa );


%% WRITE FILES TO DISK

% Store Runtime in Seconds
T = toc;

% Save Spline Stitched Image
imwrite( SP, IWSP );

% Save all workspace variables
save( [ './Results/' root '_' LN '_' RN ] );

%% END OF FILE
end