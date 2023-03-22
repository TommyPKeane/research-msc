function [ C, D, SL, mp, np ] = WorldImages( A, B, m, n, dr, dc )
% WORLDIMAGES Puts Images A and B into Registered, World Coordinates
%
% Black Regions Placed Around A and B So That They Are The Same Size
% And Can Be Combined As Overlapping 2-D Arrays Via Their Third Dimension.
%
%
% See also ColorEdges, padarray.
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
% Co = zeros( [size(C)] );
% Co(mp+1:end,np+1:end, :) = C(mp+1:end,np+1:end, :);

D = eval( [CD{~k+1} ';'] );
D = padarray( D, [ 0 np 0 ], p{~k+1} );
% Do = zeros( [size(D)] );
% Do(1:end-mp,1:end-np, :) = D(1:end-mp,1:end-np, :);

SL = flipud( [round(linspace(np, n, m - mp)).' round(linspace( 1, n - np + 1, m - mp)).'] );
SL = [ n(1).*ones(mp,2) ; SL ; np.*ones(mp,2) ];
SL(1:1:mp, 2 ) = n - np + 1;
SL(end-mp+1:1:end, 2 ) = 1;

%% END OF FILE
end