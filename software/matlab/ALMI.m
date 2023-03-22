function [ dr, dc, P ] = ALMI( Ae, Be, m, n, R, al, Enum )
%ALMI Absolute Laplacian of Mutual Information
%
% Using an Exhaustive Search this function imitates an affine translation
% transformation between Ae and Be and Calculates the Weighted Mutual
% Information at each Translation.
%
% Ae -- Input Image 1, Quantized Color Gradient with Enum Levels
% Be -- Input Image 2, Quantized Color Gradient with Enum Levels
% m -- Input Images' Number of Rows
% n -- Input Images' Number of Columns
% R -- Resolution Layer's Bounding Box for Multiresolution Search
% al -- Laplacian Filter Parameter
% Enum -- Number of "Edge" Levels in Images
%
% dr -- Row Translation Parameter
% dc -- Column Translation Parameter
% P -- Peak Value of Search Result Map
%
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
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

% Turn ENUM-ary images into Binary Images with either 0 or 1 as their
% values for calculating Histograms to estimate Probability Densities
for i = 1 : 1 : Enum;
    
    % Ratio of Pixels per Level to use as Calculation Scale Factor
    dA(i+1) = ceil( sum( Ae(:) == i ) ./ sum( Ae(:) == (i+1) ) ) .* i;
    dB(i+1) = ceil( sum( Be(:) == i ) ./ sum( Be(:) == (i+1) ) ) .* i;
    
    Ah(:,(i-1).*n+1:1:i.*n) = (( Ae.*uint8([Ae==i]) )./i);
    Bh(:,(i-1).*n+1:1:i.*n) = (( Be.*uint8([Be==i]) )./i);
    
end;

% Eliminate INF Element at end of array used in calculation
dA = dA(1:Enum);
dB = dB(1:Enum);

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
           
           % Extract Image 1 Overlap
           A = Ah([I(1,i):1:I(2,i)],((p-1).*n)+[J(1,j):1:J(2,j)]);
           
           for q = 1 : 1 : Enum;
               
               % Extract Image 2 Overlap
               B = Bh([I(3,i):1:I(4,i)],((q-1).*n)+[J(3,j):1:J(4,j)]);
               
               % Joint Histogram
               hab(p,q) = sum(sum( A .* B ) );
               
               if p < 2;
                   % Marginal Histogram
                   hb(q) = sum(sum(B));
               end;
               
               % Avoid NULL Set Calculation
               if isempty(A)|isempty(B);
                   break;
               end;
               
           end;
           
           % Marginal Histogram
           ha(p) = sum(sum(A));
           
           % Avoid NULL Set Calculation
           if isempty(A)|isempty(B);
               break;
           end;
           
        end;
        
        % Build Array to use Pixel Counts as a Weighting Pyramid
        PC(i,j) = sum( ha ) + sum( hb );
        
        % Incase Overlaps Are Empty, Set Mutual Information to Zero
        if isempty(A)|isempty(B);
            
            wM(i,j) = 0;
            break;
        
        end;
        
        % Bivariate Histogram Added with EPS to avoid Divide by Zero
        Hab = (hab + eps);
        
        % Univariate Histograms Added with EPS to avoid Divide by Zero
        Ha = (ha + eps);
        Hb = (hb + eps);
        
        % Univariate Discrete Estimate of Probability Denisty Functions
        fa = ha ./ sum(Ha) + eps;
        fb = hb ./ sum(Hb) + eps;
        
        % Bivariate Discrete Estimate of Joint Probability Density Function
        fab = hab ./ sum(sum(Hab));
        
        % Weighted/Normalized Mutual Information
        wM(i,j) = sum( sum( fab .* log2( (fab)./(fa.'*fb) ), 1 ), 2 ) ./ ...
            min(-sum(fa.*log2(fa)), -sum(fb.*log2(fb)) );
        
    end;
        
end;

%% ABSOLUTE LAPLACIAN OF MUTUAL INFORMATION

% TURNED OFF AS TESTING CASE
% Take the Absolute Value of the Laplacian of the Weighted Mutual Information
% LwM = abs( conv2( PC .* wM, fspecial('laplacian', al), 'same' ) );

% Cut off Edge Pixels to Prevent False Peak Detection
% Restricts Minimum Overlap
PC( 1:15, : ) = 0;
PC( :, 1:15 ) = 0;
PC( end:-1:end-14, : ) = 0;
PC( :, end:-1:end-14 ) = 0;

% Normalize, Apply Weighting, And Find Peak Location
K = (PC./max(PC(:))).*wM;
[ dr, dc ] = find( K == max( K(:) ), 1, 'first' );
P = K( dr, dc );

%% END OF FILE
end
