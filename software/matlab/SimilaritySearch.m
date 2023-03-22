function [ dr, dc, TF, AT, BT, zo, th, mt, nt ] = SimilaritySearch(Ae, Be, al, TH, ZO, q)
%SIMILARITYSEARCH Runs the ALMI Algorithm based on Similarity Features
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
%

%% 

% Preallocate Arrays
DR = zeros( length(ZO), length(TH) );
DC = zeros( length(ZO), length(TH) );
peak = zeros( length(ZO), length(TH) );

Count = length( ZO ) .* length( TH );
count = 0;

% Display a waitbar while running
wbh = waitbar(0,['Searching (' num2str(Count) ' left)...']);

for i = 1 : 1 : length( ZO );
    
    for j = 1 : 1 : length( TH );

        count = count + 1;
        
        zo = ZO(i);
        th = TH(j);
        
        [ AT, BT, mt, nt, TF ] = SimilarityTransform( Ae, Be, zo, th, 0 );
        
        % Determine New Bounding Box Based on Resultant Image Sizes
        R = [1 1 ; 2.*mt-1 2.*nt-1];
        
        [ DR(i,j), DC(i,j), peak(i,j) ] = ALMI( AT, BT, mt, nt, R, al, q );
        
        waitbar( count ./ Count, wbh, ['Searching (' num2str(Count-count) ' left)...'] );
                
    end;
    
end;

close( wbh );

clear i j;

[i,j] = find( peak == max(peak(:)), 1, 'first' );

% Final Similarity Features
zo = ZO(i);
th = TH(j);

[ AT, BT, mt, nt, TF ] = SimilarityTransform( Ae, Be, zo, th, 0 );

% Final Translation Parameters
dr = DR(i,j);
dc = DC(i,j);

%% END OF FILE
end
