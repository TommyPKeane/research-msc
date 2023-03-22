function [ OVC, OVD ] = DetermineOverlaps( m, n, dr, dc )
% DETERMINEOVERLAPS
%
%
%
%

%% WHAT ?!?!!? No Trust Me, It Works

I(1,:) = max( m - [1:1:2.*m-1] + 1 , ones( 1, 2.*m-1 ) );
I(2,:) = min( m .* ones( 1, 2.*m-1 ), 2.*m - [1:1:2.*m-1] );

J(1,:) = max( n-[1:1:2.*n-1]+1, ones( 1, ceil(2.*n-1) ) );
J(2,:) = min( n .* ones( 1, 2.*n-1 ), 2.*n - [1:1:2.*n-1] );

I(3,:) = max( [1:1:2.*m-1] - m+1 , ones( 1, 2.*m-1 ) );
I(4,:) = min( m .* ones( 1, 2.*m-1 ), [1:1:2.*m-1] );

J(3,:) = max( [1:1:2.*n-1] - n+1, ones( 1, 2.*n-1 ) );
J(4,:) = min( n .* ones( 1, 2.*n-1 ), [1:1:2.*n-1] );

mp = abs(dr - m);
np = abs(dc - n);

OVC = [ I(1,dr) J(1,dc) ; I(2,dr) J(2,dc) ];
OVD = [ I(3,dr) J(3,dc) ; I(4,dr) J(4,dc) ] + [ mp np ; mp np ];

%% END OF FILE
end