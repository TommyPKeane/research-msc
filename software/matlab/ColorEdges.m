function [ g ] = ColorEdges( f )
% COLOREDGES Generates Lee And Cok Described Vector Gradient Edge Map.
%
% Input is an MxNx3 Array of type 'Double'
%
% Output is 8-bit Quantized MxN Array
%

%% CHECK INPUT

f = padarray( f, [ 2 2 0 ], 'replicate', 'both' );

[m,n,c] = size(f);

if c ~= 3;
    
    disp( 'Incorrect Array Format, See Help' );
    return;
    
end;

%% LAYER GRADIENTS

h_x = [ -3 0 3 ; -10 0 10 ; -3 0 3 ];
h_y = [ -3 -10 -3 ; 0 0 0 ; 3 10 3 ];

Rp = zeros( m, n, c, 'double' );
Rq = zeros( m, n, c, 'double' );

for i = 1 : c;
    
    Rp(:,:,i) = conv2( f(:,:,i), h_x, 'same' );
    Rq(:,:,i) = conv2( f(:,:,i), h_y, 'same' );
    
end;

%% JACOBIAN ARRAY COMPONENTS

p = sum( (Rp .^ 2), 3 );
t = sum( (Rp .* Rq), 3 );
q = sum( (Rq .^ 2), 3 );


%% MAXIMUM EIGENVALUE NORMALIZED

g = sqrt( 0.5 .* ( p + q + sqrt( ( p + q ) .^2 - 4 * (p .* q - t .^ 2 ) ) ) );
g = double( g ./ max(max( g )) );

g = g(3:end-2,3:end-2);

end