function [ C, D, SL ] = Peak2Overlap( A, B, m, n, dr, dc )
%PEAK2OVERLAP Generates "Translated" Images from Translation Parameters
%
% C -- Top/Left Image
% D -- Bottom/Right Image
% SL -- Diagonal Straight Line Pixel List for a Diagonal MASK
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
%

%%

DM = dr - m;
DN = dc - n;

mp = abs(DM);
np = abs(DN);

j = [dr <= m];
k = [dc <= n];

p = {'pre';'post'};

As = padarray( A, [mp 0 0], p{j+1} );
Bs = padarray( B, [mp 0 0], p{~j+1} );

CD = {'Bs';'As'};

C = eval( [CD{k+1} ';'] );
C = padarray( C, [ 0 np 0 ], p{k+1} );

D = eval( [CD{~k+1} ';'] );
D = padarray( D, [ 0 np 0 ], p{~k+1} );

SL = flipud( [round(linspace(np, n, m - mp)).' round(linspace( 1, n - np + 1, m - mp)).'] );
SL = [ n(1).*ones(mp,2) ; SL ; np.*ones(mp,2) ];
SL(1:1:mp, 2 ) = n - np + 1;
SL(end-mp+1:1:end, 2 ) = 1;

%% END OF FILE
end
