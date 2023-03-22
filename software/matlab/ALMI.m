function [ dr, dc, P ] = ALMI( Ae, Be, m, n, R, al, Enum )
%ALMI Absolute Laplacian of Mutual Information
%
% Using an Exhaustive Search this function imitates an affine translation
% transformation between Ae and Be and Calculates the Weighted Mutual
% Information at each Translation.
%
% See also max, min, size, sum, zeros, fspecial, hist.
%

%% INITIATE MAP AND OVERLAP WINDOWS


Ae = uint8( Ae );
Be = uint8( Be );

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
Ah = zeros( m, n.*Enum, 'uint8' );
Bh = zeros( m, n.*Enum, 'uint8' );

dA = ones( 1, Enum+1, 'uint8' );
dB = ones( 1, Enum+1, 'uint8' );

% Turn ENUM-ary images into Binary Images with either Zero or ENUM as their
% values for calculating Histograms to estimate Probability Densities
for i = 1 : 1 : Enum;
    
    dA(i+1) = ceil( sum( Ae(:) == i ) ./ sum( Ae(:) == (i+1) ) ) .* i;
    dB(i+1) = ceil( sum( Be(:) == i ) ./ sum( Be(:) == (i+1) ) ) .* i;
    
    Ah(:,(i-1).*n+1:1:i.*n) = (( Ae.*([Ae==i]) )./i);
    Bh(:,(i-1).*n+1:1:i.*n) = (( Be.*([Be==i]) )./i);
    
end;


%% WEIGHTED MUTUAL INFORMATION CALCULATION

% Loop through entire set of overlaps
for i = R(1,1) : 1 : R(2,1);
    
    for j = R(1,2) : 1 : R(2,2);
        
        % Predefine Histograms as Empty Arrays
        hab = zeros( Enum, Enum, 'uint8' );
        
        % Go through each Level and Manually Calculate Binary Histograms
        for p = 1 : 1 : Enum;
            
           A = Ah([I(1,i):1:I(2,i)],((p-1).*n)+[J(1,j):1:J(2,j)]);
           
           for q = 1 : 1 : Enum;
               
               B = Bh([I(3,i):1:I(4,i)],((q-1).*n)+[J(3,j):1:J(4,j)]);
               
               hab(p,q) = sum(sum( A .* B ) );
               
           end;
           
        end;
        
        PC(i,j) = sum( sum( hab ) );
        
        % Incase Overlaps Are Empty, Set Mutual Information to Zero
        if ( isempty(A) || isempty(B) || (sum(hab,1) == 0) || (sum(hab,2) == 0) );
            
            wM(i,j) = 0;
            continue;
        
        end;
         
        % Univariate Discrete Estimate of Probability Denisty Functions
        fa = ( sum(hab,1) ./ ( sum(sum(hab)) ) );
        fb = ( sum(hab,2) ./ ( sum(sum(hab)) ) );
        
        if ~[sum(sum(fa.'*fb))];
        	
        	wM(i,j) = 0;
            continue;
        
        end;
        
        % Bivariate Discrete Estimate of Joint Probability Density Function
        fab = hab ./ sum(sum(hab));
        
        % Weighted Mutual Information
        wM(i,j) = 2 .* sum( ...
        					sum( ...
        						fab .* log2( (fab)./(fa.'*fb) )...
        					, 1 ) ...
        			   , 2 ) ./ ( -sum(fa.*log2(fa)) - sum(fb.*log2(fb)) );
        
    end;
        
end;

%% ABSOLUTE LAPLACIAN OF MUTUAL INFORMATION

% Take the Absolute Value of the Laplacian of the Weighted Mutual Information
PW = PC ./ (M.*N);

LwM = abs( conv2( PW .* wM, fspecial('laplacian', al), 'same' ) );
[ dr, dc ] = find( LwM == max( max( LwM ) ), 1, 'first' );

P = PW( dr, dc ) .* wM( dr, dc );

%% END OF FILE
end