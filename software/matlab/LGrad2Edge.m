function [Ae, Be] = LGrad2Edge( Ae, Be, m, n, Lnum, q )
%LGRAD2EDGE Performs q-ary Quantization of Gradient Images
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
%

%% (q+1)-ARY EDGE MAP

% Quantization Uses Histograms of Images to Label to 80 Percent of Pixels
% with q-Levels, forcing the top 10% to be the qth Level.

for i = Lnum : -1 : 1;
    
    kA = hist( double( Ae{i}(:) ), [1:1:256] );
    iAp = find( cumsum(kA) < (0.9.*m(i).*n(i)), 1, 'last' );
    iAs = find(cumsum(kA) > (0.2.*m(i).*n(i)), 1, 'first' );
    qA = [iAs:floor((iAp-iAs)./q):iAp inf inf];
    
    for j = 1 : 1 : q + 1;
        
        Ae{i}( [intersect( find(Ae{i}>=qA(j)), find(Ae{i}<qA(j+1)) )] ) = j;
        
    end;
    
    if ~isempty( Be );
        
        kB = hist( double( Be{i}(:) ), [1:1:256] );
        iBp = find( cumsum(kB) < (0.9.*m(i).*n(i)), 1, 'last' );
        iBs = find(cumsum(kA) > (0.2.*m(i).*n(i)), 1, 'first' );
        qB = [iBs:floor((iBp-iBs)./q):iBp inf inf];
    
        for j = 1 : 1 : q + 1;
            
            Be{i}( [intersect( find(Be{i}>=qB(j)), find(Be{i}<qB(j+1)) )] ) = j;
            
        end;
        
    end;
    
    
end;

%% END OF FILE
end
