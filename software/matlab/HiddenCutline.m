function [ MASK ] = HiddenCutline( OVC, OVD, C, D )
%MINERRORBOUNDARYCUT Summary of this function goes here
%   Detailed explanation goes here
%
%
%
%

%%

[ OCS ] = ColorEdges( C );
[ ODS ] = ColorEdges( D );

e = ( OCS - ODS ).^2;

[ m, n, p ] = size( C );

%% MINIMUM DIFF REGION

[ OVC, OVD ] = DetermineOverlaps( mt(1), nt(1), dr(1), dc(1) );

k = find( e(OVD(1,1):OVC(2,1),OVD(1,2):OVC(2,2)) == min(min( e(OVD(1,1):OVC(2,1),OVD(1,2):OVC(2,2))) ) );
K = find( k < n.*.6, 1, 'last' );

Pv = [ zeros( m-100, 1 ) ; k(K).*ones(100,1) ; NaN ];

for i = m-100 : -1 : 1

    j = max( Pv(i+1)-1, 1 ) : 1 : min( Pv(i+1)+1, n );
    
    ME = min(e(i,j));
    
    if [sum( e(i,j) == ME(:,ones(1,numel(j))) ) == numel(j)] & [i < (m-1)]
        
        Pv(i) = Pv(i+1);
        
    else
    
        [Pv(i)] = find( e(i,j) == ME, 1 ) + j(1) - 1;
    
    end;
    
end;


%% GENERATE MASK

MASK = zeros( m, n );

for i = m : -1 : 1

    MASK( i, 1:1:Pv(i) ) = 1;
    
end;

%% END OF FILE
end

