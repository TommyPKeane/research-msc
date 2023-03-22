function [ th, zo, thK, zoK ] = AffineRobustness( impath, root, ext, num, thK, zoK )
%AFFINEROBUSTNESS
%
%
%
%
%

%% VARIABLE SETUP

% Gaussian Pyramid Levels
Lnum = 5;

% Gaussian Reduction Search Window Radius (Square)
w = 8;

% Laplacian Filter Factor in Mutual Information Absolute Valued Filtering
al = 0.9;

% Rotation Search Range
dth = 0.5;
TH = -8 : dth : 8;

% Scaling Search Range
dzo = 0.05;
ZO = 1 : dzo : 1;

% Gradient Quantization Levels
q = 3;


%% RUN
   
if num < 10;
    
    LN = ['00' num2str(num)];
    RN = ['00' num2str(num)];
    
elseif num < 100;
    
    LN = ['0' num2str(num)];
    RN = ['0' num2str(num)];
    
else
    
    LN = [num2str(num)];
    RN = [num2str(num)];
    
end;

%% FILE READING AND NAMING

tic;

L = '_L_';
R = '_R_';
SP = '_SP_';
IWSP = [ './Results/' root SP LN '_' RN ext ];

%% READ IMAGE FILES

fname = [impath root L LN ext];
f = imread( fname );

gname = [impath root R RN ext];
g = imread( gname );

[ f, g, ff, gg, TFK ] = SimilarityTransform( f, g, zoK, thK, 1 );

%% CROP FOR CAMERA ARTIFACTS

% f = f(10:1:end-9,10:1:end-9,:);
% g = g(10:1:end-9,10:1:end-9,:);

%% HISTOGRAM EQUALIZATION

% FG = zeros( ([size(g)] .* [1 2 1]) );
% 
% for i = 1 : 1 : 3;
%     
%     FG(:,:,i) = histeq( [f(:,:,i) g(:,:,i)] );
%     
% end;
% 
% f = FG(:,1:size(g,2),:);
% g = FG(:,size(g,2)+1:end,:);


%% LAB TRANSFORM

cform = makecform('srgb2lab');
F = applycform(f,cform);
G = applycform(g,cform);

%% MULTI RESOLUTION wMI SEARCH

[ AT, BT, At, Bt, Atc, Btc, mt, nt, dr, dc, TF, zo, th ] = MR_ALMI( F, G, Lnum, w, al, TH, ZO, q );

[ C, D, SL ] = Peak2Overlap( At, Bt, mt(1), nt(1), dr(1), dc(1) );
% [ Cc, Dc, SLc ] = Peak2Overlap( Atc, Btc, mt(1), nt(1), dr(1), dc(1) );
% [ CT, DT, SLt ] = Peak2Overlap( AT{1}, BT{1}, mt(1), nt(1), dr(1), dc(1) );

% MASKd = zeros( size(CT) );
% for i = 1 : 1 : mt;
%     MASKd( i, 1:1:SL(i,1) ) = 1;
% end;

% BD = imclose( ceil( (D(:,:,1)+D(:,:,2)+D(:,:,3))./3 ), ones(10) );
% BC = imclose( ceil( (C(:,:,1)+C(:,:,2)+C(:,:,3))./3 ), ones(10) );
% MASKa = (BC + zeros(size(DT)) ./ 2);

[ SP ] = MR_Spline( C, D, [] );
% [ SPD ] = MR_SplineM( C, D, MASKd );
% [ SPA ] = MR_SplineM( C, D, MASKa );


%% WRITE FILES TO DISK

% Store Runtime in Seconds
T = toc;

% Save Spline Stitched Image
imwrite( SP, IWSP );

% Save all workspace variables
save( [ './Results/Robustness/' root '_' LN '_' RN '_' num2str(zoK) '_' num2str(thK) '.mat' ] );

disp( T );

%% END OF FILE
end

