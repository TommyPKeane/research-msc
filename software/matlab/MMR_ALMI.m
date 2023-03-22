function [ SP, T, zo, th ] = MMR_ALMI( impath, root, LN, RN, ext, Lnum, w, al, TH, ZO, q )
%MMR_ALMI Sets up Images and saves results for/of Multiresolution
%         Similarity Search for Peak Mutual Information
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
%

%% FILE READING AND NAMING

% Start Runtime Calculation
tic;

L = '_L_';
R = '_R_';
SP = '_SP_';
IWSP = [ impath root SP LN '_' RN ext ];

%% READ IMAGE FILES

fname = [impath root L LN ext];
f = imread( fname );

gname = [impath root R RN ext];
g = imread( gname );


%% CROP FOR CAMERA ARTIFACTS

% f = f(10:1:end-9,10:1:end-9,:);
% g = g(10:1:end-9,10:1:end-9,:);


%% HISTOGRAM EQUALIZATION

% Join Images for Equalization
FG = uint8(zeros( ([size(g)] .* [1 2 1]) ));

for i = 1 : 1 : 3;
    FG(:,:,i) = histeq( [f(:,:,i) g(:,:,i)] );
end;

% Extract Images
F = FG(:,1:size(g,2),:);
G = FG(:,size(g,2)+1:end,:);

%% MULTI RESOLUTION wMI SEARCH

[ AT, BT, At, Bt, Atc, Btc, mt, nt, dr, dc, TF, zo, th ] = MR_ALMI( f, g, F, G, Lnum, w, al, TH, ZO, q );

% Generate Translated Images based on Translation Parameters
[ C, D, SL ] = Peak2Overlap( At, Bt, mt(1), nt(1), dr(1), dc(1) );

% [ Cc, Dc, SLc ] = Peak2Overlap( Atc, Btc, mt(1), nt(1), dr(1), dc(1) );
% [ CT, DT, SLt ] = Peak2Overlap( AT{1}, BT{1}, mt(1), nt(1), dr(1), dc(1) );

% Generate Diagonal MASK
MASKd = zeros( size(CT) );
for i = 1 : 1 : mt;
    MASKd( i, 1:1:SL(i,1) ) = 1;
end;

% Apply MASKs and Generate Blended Image
[ SP ] = MR_Spline( C, D, [] );
[ SPD ] = MR_SplineM( C, D, MASKd );

%% WRITE FILES TO DISK

% Store Runtime in Seconds
T = toc;

% Save Spline Stitched Image
imwrite( SP, IWSP );

% Save all workspace variables
save( [ impath root '_' LN '_' RN ] );

%% END OF FILE
end
