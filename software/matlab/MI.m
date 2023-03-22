function [ dr, dc, P ] = MI( Ae, Be, m, n, R, Enum )
%ALMI Absolute Laplacian of Mutual Information
%
% Using an Exhaustive Search this function imitates an affine translation
% transformation between Ae and Be and Calculates the Weighted Mutual
% Information at each Translation.
%
% See also max, min, size, sum, zeros, fspecial, hist.
%

%% INITIATE MAP AND OVERLAP WINDOWS

% Correct Bounding Box if Calculated Out Of Input's Bounds
R( :, : ) = ~[R(:,1:2) < 1] .* (R(:,1:2)-1) + 1;
RE = [ [ R(:,1), [2.*m-1;2.*m-1] ]; [ R(:,2), [2.*n-1;2.*n-1] ] ];
R( :, 1 ) = RE(sub2ind([4 2],[1;2],[[R(:,1) > 2.*m-1] + 1]));
R( :, 2 ) = RE(sub2ind([4 2],[3;4],[[R(:,2) > 2.*n-1] + 1]));

% Arrays Holding the Translation Rectangles' Indices
% This avoids calculating Overlaps on the fly
I(1,:) = max( m - [1:1:2.*m-1] + 1 , ones( 1, 2.*m-1 ) );
I(2,:) = min( m .* ones( 1, 2.*m-1 ), 2.*m - [1:1:2.*m-1] );
J(1,:) = max( n-[1:1:2.*n-1]+1, ones( 1, ceil(2.*n-1) ) );
J(2,:) = min( n .* ones( 1, 2.*n-1 ), 2.*n - [1:1:2.*n-1] );
I(3,:) = max( [1:1:2.*m-1] - m+1 , ones( 1, 2.*m-1 ) );
I(4,:) = min( m .* ones( 1, 2.*m-1 ), [1:1:2.*m-1] );
J(3,:) = max( [1:1:2.*n-1] - n+1, ones( 1, 2.*n-1 ) );
J(4,:) = min( n .* ones( 1, 2.*n-1 ), [1:1:2.*n-1] );

% Predefine Weighted Mutual Information Map
wM = zeros( 2.*m-1, 2.*n-1 );
PC = zeros( 2.*m-1, 2.*n-1 );

% Create Place to Store ENUM-ary Edge Images
Ah = zeros( m, n.*Enum );
Bh = zeros( m, n.*Enum );

dA = ones(1,Enum+1);
dB = ones(1,Enum+1);

% Turn ENUM-ary images into Binary Images with either Zero or ENUM as their
% values for calculating Histograms to estimate Probability Densities
for i = 1 : 1 : Enum;
    
    dA(i+1) = ceil( sum( Ae(:) == i ) ./ sum( Ae(:) == (i+1) ) );
    dB(i+1) = ceil( sum( Be(:) == i ) ./ sum( Be(:) == (i+1) ) );
    
    Ah(:,(i-1).*n+1:1:i.*n) = (( Ae.*uint8([Ae==i]) )./i);
    Bh(:,(i-1).*n+1:1:i.*n) = (( Be.*uint8([Be==i]) )./i);
    
end;

dA = dA(1:Enum);
dB = dB(1:Enum);

W = dA.'*dB;

%% WEIGHTED MUTUAL INFORMATION CALCULATION

% Loop through entire set of overlaps
for i = R(1,1) : 1 : R(2,1);
    
    for j = R(1,2) : 1 : R(2,2);
        
        % Predefine Histograms as Empty Arrays
        ha = zeros( 1, Enum );
        hb = zeros( 1, Enum );
        hab = zeros( Enum, Enum );
        
        % Go through each Level and Manually Calculate Binary Histograms
        for p = 1 : 1 : Enum;
            
           A = Ah([I(1,i):1:I(2,i)],((p-1).*n)+[J(1,j):1:J(2,j)]);
           
           for q = 1 : 1 : Enum;
               
               B = Bh([I(3,i):1:I(4,i)],((q-1).*n)+[J(3,j):1:J(4,j)]);
               
               hab(p,q) = sum(sum( A .* B ) );
               
               if p < 2;
                   hb(q) = sum(sum(B));
               end;
               
               if isempty(A)|isempty(B);
                   break;
               end;
               
           end;
           
           ha(p) = sum(sum(A));
           
           if isempty(A)|isempty(B);
               break;
           end;
           
        end;
        
        PC(i,j) = sum( ha ) + sum( hb );
        
        % Incase Overlaps Are Empty, Set Mutual Information to Zero
        if isempty(A)|isempty(B);
            
            wM(i,j) = 0;
            break;
        
        end;
        
        if i == 17 & j == 13;
            disp('');
        end;
        
        % Bivariate Histogram Added with EPS so Zero doesn't Equal Zero
        Hab = (hab + eps);
        
        % Univariate Histograms Added with EPS so Zero doesn't Equal Zero
        Ha = (ha + eps);
        Hb = (hb + eps);
        
        % Univariate Discrete Estimate of Probability Denisty Functions
        fa = ha ./ sum(Ha) + eps;
        fb = hb ./ sum(Hb) + eps;
        
        % Bivariate Discrete Estimate of Joint Probability Density Function
        fab = hab ./ sum(sum(Hab));
        
        % Weighted Mutual Information
        wM(i,j) = sum( sum( W .* Hab .* fab .* log2( (fab)./(fa.'*fb) ), 1 ), 2 );
        
    end;
        
end;

%% ABSOLUTE LAPLACIAN OF MUTUAL INFORMATION

% Take the Absolute Value of the Laplacian of the Weighted Mutual Information
% LwM = abs( conv2( PC .* wM, fspecial('laplacian', al), 'same' ) );

LwM = PC .* wM;

[ dr, dc ] = find( LwM == max( max( LwM ) ), 1, 'first' );

P = LwM( dr, dc );

%% END OF FILE
end