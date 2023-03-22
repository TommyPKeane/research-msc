function [ deltaM, deltaN, TForm, AT, BT, AffineOptimal, mT, nT ] = AffineSearch( Ae, Be, alpha, AffineParam, numEdge )
%AFFINESEARCH
%
%
% See also imrotate, imtransform, maketform.

%% 

ZO = AffineParam{1};
TH = AffineParam{2};
SX = AffineParam{3};
SY = AffineParam{4};

AffineOptimal = [ 1 0 0 0 ];

DeltaM = zeros( length(ZO), length(TH), length(SX), length(SY) );
DeltaN = zeros( length(ZO), length(TH), length(SX), length(SY) );
Peak = zeros( length(ZO), length(TH), length(SX), length(SY) );

SearchTotal = length( ZO ) .* length( TH ) .* length(SX) .* length(SY);
SearchCrrnt = 0;

wbh = waitbar(0,['Searching (' num2str(SearchTotal) ' left)...']);

% for l = 1 : 1 : length( SX );
%     
%     for k = 1 : 1 : length( SY );
        
        for i = 1 : 1 : length( ZO );
            
            for j = 1 : 1 : length( TH );

                SearchCrrnt = SearchCrrnt + 1;
                
                AffineOptimal(1) = ZO(i);
                AffineOptimal(2) = TH(j);
%                 AffineOptimal(3) = SX(k);
%                 AffineOptimal(4) = SY(l);
                
                [ AT, BT, mT, nT, TForm ] = AffineTransform( Ae, Be, AffineOptimal );
                [ AMASK, BMASK, mT, nT, TForm ] = AffineTransform( ones([size(Ae)]), ones([size(Be)]), AffineOptimal );
                
                Range = [1 1 ; 2.*mT-1 2.*nT-1];
                
                AT = round( 255 .* (AT ./ max(AT(:))) );
                BT = round( 255 .* (BT ./ max(BT(:))) );
                
                [ATe, BTe] = EdgeThreshold( AT, round(AMASK), BT, round(BMASK), mT, nT, numEdge );
                
                [ DeltaM(i,j), DeltaN(i,j), Peak(i,j) ] = ALMI( ATe, BTe, mT, nT, Range, alpha, numEdge );
                
                waitbar( SearchCrrnt ./ SearchTotal, wbh, ['Searching (' num2str(SearchTotal-SearchCrrnt) ' left)...'] );
                
%             end;
%             
%         end;
        
    end;
    
end;

close( wbh );

clear i j;

[ i, j ] = find( Peak == max(Peak(:)), 1, 'first' );

AffineOptimal(1) = ZO(i);
AffineOptimal(2) = TH(j);
% AffineOptimal(3) = SX(k);
% AffineOptimal(4) = SY(l);

[ AT, BT, mT, nT, TForm ] = AffineTransform( Ae, Be, AffineOptimal );

deltaM = DeltaM(i,j);
deltaN = DeltaN(i,j);

%% END OF FILE
end