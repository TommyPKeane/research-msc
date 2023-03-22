function [] = RUN_PhaseI( varargin )
% RUN_PHASEI Runs Affine/Similarity Search Algorithm Based on Parameters
%
% LAST EDIT: 2010.05.11
% Tommy P. Keane
%

%% SETUP VARIABLES (PARAMETERS)

% ⚠️ These are the default configuration values, but they can be modified
Lnum = 5;
w = 8;
al = 0.9;
dth = 0.5;
dzo = 0.05;
TH = -10 : dth : 10;
ZO = 1 : dzo : 1;
q = 3;

impath = '../Test Images/Lenel/';  % ⚠️ Edit this value for your test images
root = 'Lenel';  % ⚠️ Edit this value for your test images
ext = '.png';  % ⚠️ Edit this value for your test images

%% RUN

for i = 9 : 1 : 10;
    
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
    
    % Outputs are already written to disk inside this function
    % These outputs are only helpful for Display on Demand
    [ SP, T ] = MMR_ALMI( impath, root, LN, RN, ext, Lnum, w, al, TH, ZO, q );

    disp( T )
%     imshow( SP, [] )
%     drawnow
    
end;

% clc;

%% END OF FILE
end
