function [ SP ] = MR_Spline( A, B, MASK )
% MR_SPLINE Returns the Splined Image from A and B
%
%
%
% See also impyramid.

%% SETUP SIZE

[m,n,p] = size( A );

N = round( log2( [m n] ) );

%% MULTIPLE OF POWER OF 2

% MP = 2^N(1) - m;
% NP = 2^N(2) - n;

% A = padarray( A, [MP.*[MP>0] NP.*[NP>0] 0], 0, 'pre' );
% A = A( 1:1:end-abs(MP.*[MP<0]), 1:1:end-abs(NP.*[NP<0]), : );
% 
% B = padarray( B, [MP.*[MP>0] NP.*[NP>0] 0], 0, 'pre' );
% B = B( 1:1:end-abs(MP.*[MP<0]), 1:1:end-abs(NP.*[NP<0]), : );

%% BUILD LAPLACIANS

GA = cell( min(N), 1 );
GB = cell( min(N), 1 );
LA = cell( min(N), 1 );
LB = cell( min(N), 1 );
LS = cell( min(N), 1 );

GR = cell( min(N), 1 );
GR{1} = repmat(MASK,[1 1 size(A,3)] );

LS{1} = zeros( [size(A)] );

GA{1} = A;
GB{1} = B;

for i = 1 : 1 : min(N);
    
    if i < min(N);
    
        % Gaussian Pyramid Reduction
        GA{i+1} = impyramid( GA{i}, 'reduce' );
        GB{i+1} = impyramid( GB{i}, 'reduce' );
        GR{i+1} = impyramid( GR{i}, 'reduce' );
        
        % Laplacian Pyramid
        tGA = impyramid( GA{i+1}, 'expand' );
        T = size( GA{i} ) - size( tGA );
        tGA = padarray( tGA, [T(1).*[T(1)>0] T(2).*[T(2)>0] 0], 0, 'pre' );
        tGA = tGA( 1:1:end-abs(T(1).*[T(1)<0]), 1:1:end-abs(T(2).*[T(2)<0]), : );
        LA{i} = GA{i} - tGA;
                
        tGB = impyramid( GB{i+1}, 'expand' );
        T = size( GB{i} ) - size( tGB );
        tGB = padarray( tGB, [T(1).*[T(1)>0] T(2).*[T(2)>0] 0], 0, 'pre' );
        tGB = tGB( 1:1:end-abs(T(1).*[T(1)<0]), 1:1:end-abs(T(2).*[T(2)<0]), : );
        LB{i} = GB{i} - tGB;
        
        % Spline
        LS{i+1} = impyramid( LS{i}, 'reduce' );
%         LS{i}(:,1:round((end./2)-1),:) = LA{i}(:,1:round((end./2)-1),:);
%         LS{i}(:,round((end./2)+1):end,:) = LB{i}(:,round((end./2)+1):end,:);
%         LS{i}(:,round(end./2),:) = ( LA{i}(:,round(end./2),:) + LB{i}(:,round(end./2),:) ) ./ 2;
        LS{i} = GR{i}.*LA{i} + (1 - GR{i}).*LB{i};
        
    else;
    
        % Laplacian Pyramid
        LA{i} = GA{i};
        LB{i} = GB{i};
        
        % Spline
        LS{i}(:,1:round((end./2)-1),:) = LA{i}(:,1:round((end./2)-1),:);
        LS{i}(:,round((end./2)+1):end,:) = LB{i}(:,round((end./2)+1):end,:);
        LS{i}(:,round(end./2),:) = ( LA{i}(:,round(end./2),:) + LB{i}(:,round(end./2),:) ) ./ 2;
        LS{i} = GR{i}.*LA{i} + (1 - GR{i}).*LB{i};
        
        
    end;
    
end;

%% MERGE AS SPLINE

SP = LS{min(N)};

for i = min(N) : -1 : 2;
    
    tLS = impyramid( SP, 'expand' );
    T = size( LS{i-1} ) - size( tLS );
    tLS = padarray( tLS, [T(1).*[T(1)>0] T(2).*[T(2)>0] 0], 0, 'pre' );
    tLS = tLS( 1:1:end-abs(T(1).*[T(1)<0]), 1:1:end-abs(T(2).*[T(2)<0]), : );
    SP = LS{i-1} + tLS;
    
end;

SP = SP ./ max( SP(:) );

%% END OF FILE
end