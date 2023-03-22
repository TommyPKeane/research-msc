function [] = TestAffine( varargin )
% RUN_PHASEI
%
%
%
%
% See also M_MR_ALMI, MR_ALMI, num2str.

%% SETUP VARIABLES

numPymd = 4;
w = 8;
alpha = 0.9;
dth = 1;
dzo = 0.05;
dsy = 0.5;
dsx = 0.5;
TH = -10 : dth : -7;
ZO = 0.9 : dzo : 1.1;
SX = 0 : dsx : 0;
SY = 0 : dsy : 0;
numEdge = 3;
impath = './Test Images/Lenel/';
root = 'Lenel';
ext = '.png';

AffineParam{1} = ZO;
AffineParam{2} = TH;
AffineParam{3} = SX;
AffineParam{4} = SY;

%% RUN

clc;

for i = 10 : 1 : 10;
    
    if i < 10;
    
        LN = ['00' num2str(i)];
        RN = ['00' num2str(i)];

    elseif i < 100;

        LN = ['0' num2str(i)];
        RN = ['0' num2str(i)];
        
    else
        
        LN = [num2str(i)];
        RN = [num2str(i)];

    end;
    
    [ SP, T ] = CreatePanorama( impath, root, LN, RN, ext, numPymd, w, alpha, AffineParam, numEdge );

    disp( T );
    imshow( SP, [] );
    
end;


%% END OF FILE
end