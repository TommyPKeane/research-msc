function [ AT, BT, At, Bt, mT, nT, dr, dc, TForm, AffineOptimal ] = RegisterImages( f, g, numPymd, w, alpha, AffineParam, numEdge )
%MR_ALMI Performs Multiresolution Search for ALMI
%
% ALMI = Absolute Value of Laplacian of Mutual Information
%
% Assumes f and g are of same size and type.
%

%% SETUP EMPTY VARIABLES FOR EFFICIENCY

% Color Gradient Maps
Ag = cell( 1, numPymd );
Bg = cell( 1, numPymd );

% Thresholded q-ary Edge Maps
Ae = cell( 1, numPymd );
Be = cell( 1, numPymd );

% Transformed q-ary Edge Maps
AT = cell( 1, numPymd );
BT = cell( 1, numPymd );

% Color Gradient Maps' Number of Rows and Colums
m = zeros( 1, numPymd );
n = zeros( 1, numPymd );
mT = zeros( 1, numPymd );
nT = zeros( 1, numPymd );

% Rectangular Peak Search Window Bounds From Layer to Layer
R = zeros( 2, 2, numPymd );

% Peak Locations
dr = zeros( 1, numPymd );
dc = zeros( 1, numPymd );


%% MULTI-RESOLUTION RGB GRADIENT MAPS

% Initialize Cell Arrays for Multiple Resolution Images
Ag{1} = ColorEdges( f );
Bg{1} = ColorEdges( g );

% Get Size Of Image (Assume Same for Both)
[m(1), n(1)] = size( f(:,:,1) );


% Create Gaussian Pyramid for MultiResolution Search
for i = 2 : 1 : numPymd;
    
    % Subsampled Arrays (Gaussian Pyramid Reduction)
    Ag{i} = impyramid( Ag{i-1}, 'reduce' );
    
    Bg{i} = impyramid( Bg{i-1}, 'reduce' );
    
    [m(i), n(i)] = size( Ag{i} );
    
end;

%% AFFINE TRANSFORMATION BY ABSOLUTE MAXIMUM OF LAPLACIAN OF MUTUAL INFORMATION

% Find Peak Row and Column Shifts And Affine Rotation Angle
[ dr(numPymd),...
  dc(numPymd), ...
  TForm, ...
  AT{numPymd}, ...
  BT{numPymd}, ...
  AffineOptimal, ...
  mT(numPymd), ...
  nT(numPymd) ] = AffineSearch( Ag{numPymd}, ...
                                Bg{numPymd}, ...
                                alpha, ...
                                AffineParam, ...
                                numEdge );

% Initialization for the Array Of Search Window Coordinates At Each Level
R(:,:,numPymd) = [1 1 ; 2.*mT(numPymd)-1 2.*nT(numPymd)-1];

% Start at Lowest Resolution and Work up to Original Resolution
for i = numPymd-1 : -1 : 1;

    [ AT{i}, BT{i}, mT(i), nT(i), TForm ] = AffineTransform( Ag{i}, Bg{i}, AffineOptimal );
    
    [ AMASK, BMASK, mT(i), nT(i), TForm ] = AffineTransform( ones([size(Ag{i})]), ones([size(Bg{i})]), AffineOptimal );        
    
    AT{i} = round( 255 .* (AT{i} ./ max(AT{i}(:))) );
    BT{i} = round( 255 .* (BT{i} ./ max(BT{i}(:))) );
    
    [Ae{i}, Be{i}] = EdgeThreshold( AT{i}, round(AMASK), BT{i}, round(BMASK), mT(i), nT(i), numEdge );
     
    % Calculate Absolute value of Laplacian of Mutual Information (WFMI)
    SCL = [ mT(i) ; nT(i) ] ./ [ mT(i+1) ; nT(i+1) ];
        
    R(:,:,i) = round( [SCL SCL].*[ dr(i+1) dc(i+1) ; dr(i+1) dc(i+1) ] + [ -w -w ; w w ] );
    
    [ dr(i), dc(i), P ] = ALMI( Ae{i}, Be{i}, mT(i), nT(i), R(:,:,i), alpha, numEdge );
        
    disp( ['Finished Layer ' num2str(i)] );
    
end;

A = double(f) ./ 255;
B = double(g) ./ 255;

[ At, Bt, mT(1), nT(1), TForm ] = AffineTransform( A, B, AffineOptimal );

%% END OF FILE
end