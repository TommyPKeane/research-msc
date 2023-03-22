function [ H ] = DLT( L, Lp )
% DLT Normalized Direct Linear Transform for 2D Homography
%
%
%
% See also imtransform.
%

%% SETUP

N = size( L, 1 );

if size(L,2) < 3;
    
    L = [ L ones(N,1) ];
    
end;

if size( Lp, 2 ) < 3;
    
    Lp = [ Lp ones(N,1) ];
    
end;

%% NORMALIZE L

L = L ./ repmat( L(:,3), [1 3] );

sc = sqrt(2) ./ std( L(:,1:2) );

tr = -mean( L(:,1:2) ) .* sc;

TL = [ sc(1) 0 0 ; 0 sc(2) 0 ; tr 1 ];
% TL = [ sc(1) 0 tr(1) ; 0 sc(2) tr(2) ; 0 0 1 ];

%% NORMALIZE Lp

Lp = Lp ./ repmat( Lp(:,3), [1 3] );

scp = sqrt(2) ./ std( Lp(:,1:2) );

trp = -mean( Lp(:,1:2) ) .* scp;

TLp = [ scp(1) 0 0 ; 0 scp(2) 0 ; trp 1 ];
% TLp = [ scp(1) 0 trp(1) ; 0 scp(2) trp(2) ; 0 0 1 ];

%% DLT ALGORITHM

A = zeros( 2.*size(L,1), 9 );

j = 0;

for i = 1 : 2 : 2.*N;
    
    j = j + 1;
    
%     A(i:i+1,:) = [ [0 0 0] [-Lp(j,3).*L(j,:)] [Lp(j,2).*L(j,:)] ; ...
%                    [Lp(j,3).*L(j,:)] [0 0 0] [-Lp(j,1).*L(j,:)] ...
%                  ];

    A(i:i+1,:) = [ [Lp(j,3).*L(j,:)] [0 0 0] [-Lp(j,1).*L(j,:)]; ...
                   [0 0 0] [Lp(j,3).*L(j,:)] [-Lp(j,2).*L(j,:)] ...
                 ];

end;

[U,S,V] = svd(A,0);

h = V(:,end);

H = reshape( h, [3 3] );

%% DESCALE AND ADJUST HOMOGRAPHY

H = inv(TLp) * H * TL;

H = H ./ H(3,3);

%% END OF FILE
end