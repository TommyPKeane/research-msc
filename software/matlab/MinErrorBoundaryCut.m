function [ MASK ] = MinErrorBoundaryCut( OVC, OVD, C, D )
%MINERRORBOUNDARYCUT Summary of this function goes here
%   Detailed explanation goes here
%
%
%
%

% %% GET C's OVERLAP REGION
% 
% [y,x] = find( Yimage(C)>0 );
% T = zeros( [size(C,1) size(C,2)] );
% T( [OVC(1,1):1:OVC(2,1)], [OVC(1,2):1:OVC(2,2)] ) = 1;
% OVi = find( T );
% XY = sub2ind( [size(C,1) size(C,2)], y, x );
% CE = zeros( size(C) );
% for i = 1 : 1 : size(C,3);
% CE(intersect( XY, OVi )+[size(C,1).*size(C,2).*(i-1)]) = C(intersect( XY, OVi )+[size(C,1).*size(C,2).*(i-1)]);
% end;
% 
% %% GET D's OVERLAP REGION
% 
% [y,x] = find( Yimage(D)>0 );
% T = zeros( [size(D,1) size(D,2)] );
% T( [OVD(2,1):1:OVD(1,1)], [OVD(2,2):1:OVD(1,2)] ) = 1;
% OVi = find( T );
% XY = sub2ind( [size(D,1) size(D,2)], y, x );
% DE = zeros( size(D) );
% for i = 1 : 1 : size(C,3);
% DE(intersect( XY, OVi )+[size(D,1).*size(D,2).*(i-1)]) = D(intersect( XY, OVi )+[size(D,1).*size(D,2).*(i-1)]);
% end;

%%

[ OCS ] = ColorEdges( C );
[ ODS ] = ColorEdges( D );

e = ( OCS - ODS ).^2;

hv = [ -1 -1 -1 ; -1 5 -1 ; 0 0 0 ];
% hh = [ -1 -1 -1 ; 0 3 0 ; 0 0 0 ].';

% Eh = imfilter( e, hh, 'replicate', 'same', 'corr' );
Ev = imfilter( e, hv, 'replicate', 'same', 'corr' );

[ m, n, p ] = size( C );

%% VERTICAL DIFF TRAVERSE


k = find( Ev(m-100,:) == min( Ev(m-100,:) ) );
K = find( k < n.*.6, 1, 'last' );

Pv = [ zeros( m-100, 1 ) ; k(K).*ones(100,1) ; NaN ];

for i = round(n./2) : -1 : 1

    j = max( Pv(i+1)-1, 1 ) : 1 : min( Pv(i+1)+1, n );
    
    ME = min(Ev(i,j));
    
    if [sum( Ev(i,j) == ME(:,ones(1,numel(j))) ) == numel(j)] & [i < (m-1)]
        
        Pv(i) = Pv(i+1);
        
    else
    
        [Pv(i)] = find( Ev(i,j) == ME, 1 ) + j(1) - 1;
    
    end;
    
end;


% %% HORIZONTAL DIFF TRAVERSE
% 
% % Pv = zeros( m, 1 );
% 
% for i = m : -1 : 1
% 
%     cols = max(OVC(1,2).*([i<OVD(1,1)]&[i>OVD(2,1)]),1):1:(max(OVD(1,2).*([i<OVD(1,1)]&[i>OVD(2,1)]),0)+n.*~([i<OVD(1,1)]&[i>OVD(2,1)]));
%     
%     [Pv(i)] = find( Ev(i,cols) == min( Ev(i,cols) ), 1 ) + cols(1) - 1;
%     
% end;


%% GENERATE MASK

MASK = zeros( m, n );

for i = m : -1 : 1

    MASK( i, 1:1:Pv(i) ) = 1;
    
end;

%% END OF FILE
end

