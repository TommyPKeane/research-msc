function [ AT, BT, At, Bt, Atc, Btc, mt, nt, dr, dc, TF, zo, th ] = MR_ALMI( F, G, f, g, Lnum, w, al, TH, ZO, q )
%MR_ALMI Performs Multiresolution Search for ALMI
%
% ALMI = Absolute Value of Laplacian of Mutual Information
%
% Assumes f and g are of same size and type.
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
%

%% SETUP EMPTY VARIABLES FOR EFFICIENCY

% Color Gradient Maps
Ag = cell( 1, Lnum );
Bg = cell( 1, Lnum );

% Thresholded q-ary Edge Maps
Ae = cell( 1, Lnum );
Be = cell( 1, Lnum );

% Transformed q-ary Edge Maps
AT = cell( 1, Lnum );
BT = cell( 1, Lnum );

% Color Gradient Maps' Number of Rows and Colums
m = zeros( 1, Lnum );
n = zeros( 1, Lnum );
mt = zeros( 1, Lnum );
nt = zeros( 1, Lnum );

% Rectangular Peak Search Window Bounds From Layer to Layer
R = zeros( 2, 2, Lnum );

% Peak Locations
dr = zeros( 1, Lnum );
dc = zeros( 1, Lnum );

%%% CONVERT IMAGES TO DOUBLE AND NORMALIZE TO [0,1]


%% MULTI-RESOLUTION RGB GRADIENT MAPS

% Pad Then Remove Duplicate Rows/Columns To Avoid High Image Edge Gradients
Ag{1} = uint8( ColorEdges( padarray( double(f), [1 1], 'replicate', 'both' ) ) );
Ag{1} = Ag{1}(2:1:end-1,2:1:end-1);
Ae{1} = Ag{1};

% Get Size Of Image (Assume Same for Both)
[m(1), n(1)] = size( Ae{1} );

Bg{1} = uint8( ColorEdges( padarray( double(g), [1 1], 'replicate', 'both' ) ) );
Bg{1} = Bg{1}(2:1:end-1,2:1:end-1);
Be{1} = Bg{1};

for i = 2 : 1 : Lnum;
    
    % Subsampled Arrays (Gaussian Pyramid Reduction)
    Ag{i} = impyramid( Ag{i-1}, 'reduce' );
    Ae{i} = round(Ag{i}); 
    
    Bg{i} = impyramid( Bg{i-1}, 'reduce' );
    Be{i} = round(Bg{i});
   
    [m(i), n(i)] = size( Ae{i} );
    
end;

%% GRADIENT TO EDGE MAP QUANTIZATION

[Ae, Be] = LGrad2Edge( Ae, Be, m, n, Lnum, q-1 );

%% AFFINE TRANSFORMATION BY ABSOLUTE MAXIMUM OF LAPLACIAN OF MUTUAL INFORMATION

% Find Peak Row and Column Shifts And Affine Rotation Angle
[dr(Lnum), dc(Lnum), TF, AT{Lnum}, BT{Lnum}, zo, th, mt(Lnum), nt(Lnum) ] = SimilaritySearch( Ae{Lnum}, Be{Lnum}, al, TH, ZO, q );

% Initialization for the Array Of Search Window Coordinates At Each Level
R(:,:,Lnum) = [1 1 ; 2.*mt(Lnum)-1 2.*nt(Lnum)-1];

disp( ['Finished Layer ' num2str(Lnum)] );

% Start at Lowest Resolution and Work up to Original Resolution
for i = (Lnum - 1) : -1 : 1;

    % Scale
    [ AT{i}, BT{i}, mt(i), nt(i), TF ] = SimilarityTransform( Ae{i}, Be{i}, zo, th, 0 );
  
    SCL = [mt(i);nt(i)] ./ [mt(i+1);nt(i+1)];
    
    % Calculate Search Window Bounds
    R(:,:,i) = round( [SCL SCL].*[ dr(i+1) dc(i+1) ; dr(i+1) dc(i+1) ] + [ -w -w ; w w ] );
    
    % Calculate Absolute value of Laplacian of Mutual Information(ALMI)
    [ dr(i), dc(i), P ] = ALMI( AT{i}, BT{i}, mt(i), nt(i), R(:,:,i), al, q );
        
    disp( ['Finished Layer ' num2str(i)] );
    
end;

A = double(F) ./ 255;
B = double(G) ./ 255;

% Apply Search Result Transform to Images as Final Result
[ At, Bt, mt(1), nt(1), TF ] = SimilarityTransform( A, B, zo, th, 0 );
[ Atc, Btc, mt(1), nt(1), TF ] = SimilarityTransform( (Yimage(A)), (Yimage(B)), zo, th, 0 );

%% END OF FILE
end
