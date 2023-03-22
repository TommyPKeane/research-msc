function [Ae, Be] = EdgeThreshold( Ae, AMASK, Be, BMASK, m, n, q )
%GRAD2EDGE
%
%
% See also edge, hist.

%% (q+1)-ARY EDGE MAP

MN = length( nonzeros( Ae(:).*AMASK(:) ) );

kA = hist( double( nonzeros( Ae(:).*AMASK(:) ) ), [1:1:256] );
iAp = find( cumsum(kA) < (0.8.*MN), 1, 'last' );
iAs = find(cumsum(kA) > (0.1.*MN), 1, 'first' );
qA = [0 iAs:floor((iAp-iAs)./q):iAp inf];

for j = 1 : 1 : q+2;
    
    Ae( [intersect( find(Ae>=qA(j)), find(Ae<qA(j+1)) )] ) = j-1;
    
end;

if ~isempty( Be );
    
    MN = length( nonzeros( Ae(:).*AMASK(:) ) );
    
    kB = hist( double( nonzeros( Be(:).*BMASK(:) ) ), [1:1:256] );
    iBp = find( cumsum(kB) < (0.8.*MN), 1, 'last' );
    iBs = find(cumsum(kA) > (0.1.*MN), 1, 'first' );
    qB = [0 iBs:floor((iBp-iBs)./q):iBp inf];
    
    for j = 1 : 1 : q+2;
        
        Be( [intersect( find(Be>=qB(j)), find(Be<qB(j+1)) )] ) = j-1;
        
    end;
    
end;
        

%% END OF FILE
end