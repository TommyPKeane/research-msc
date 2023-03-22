function [ g ] = ColorEdges( f )
% COLOREDGES Generates Lee And Cok Described Vector Gradient Edge Map.
%
% Input is an MxNx3 Array of type 'Double'
%
% Output is 8-bit Quantized MxN Array
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
%

%% CHECK INPUT

% Replicate Frame Border to avoid Edges along Border
f = padarray( f, [ 2 2 0 ], 'replicate', 'both' );

% Extract Frame Size
[m,n,c] = size(f);

% Only Coded for 3 Channel Images, Preferably RGB
if c ~= 3;
    
    disp( 'Incorrect Array Format, See Help' );
    return;
    
end;

%% LAYER GRADIENTS

% Horizontal and Vertical Derivative/Difference Kernals
h_x = [ -1 0 1 ; -1 0 1 ; -1 0 1 ];
h_y = [ 1 1 1 ; 0 0 0 ; -1 -1 -1 ];

% Preallocate Memory
Rp = zeros( m, n, c );
Rq = zeros( m, n, c );

% Use Convolution with Sobel Masks to Generate Partial Derivative Maps
for i = 1 : c;
    
    Rp(:,:,i) = conv2( f(:,:,i), h_x, 'same' );
    Rq(:,:,i) = conv2( f(:,:,i), h_y, 'same' );
    
end;

%% JACOBIAN ARRAY COMPONENTS

% Create Variables to Generate Maximum Eigenvalue
p = sum( (Rp .^ 2), 3 );
t = sum( (Rp .* Rq), 3 );
q = sum( (Rq .^ 2), 3 );


%% MAXIMUM EIGENVALUE NORMALIZED

% Equation for Maximum Eigenvalue, Normalized and Quantized
g = sqrt( 0.5 .* ( p + q + sqrt( ( p + q ) .^2 - 4 * (p .* q - t .^ 2 ) ) ) );
g = round( 255 .* (g / max( max( g ) )) );

% Cutoff Duplicate Data Used to Avoid False Frame Border Edges
g = g(3:end-2,3:end-2);

end
