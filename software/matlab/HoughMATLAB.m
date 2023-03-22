function [ LNC, PTC, LND, PTD ] = HoughMATLAB( CT, DT, N )
% HOUGHMATLAB
%
%
%
%
%
% See also hough, houghlines, houghpeaks.
%

%%
[HFC,THC,RHC] = hough( CT, 'RhoResolution', 3.0, 'ThetaResolution', 0.3 );
HPC = houghpeaks( HFC, N );
LNC = houghlines(CT, THC, RHC, HPC);
PTC = [ vertcat(LNC.point1)  vertcat(LNC.point2) ];
Nc = length( PTC );

[HFD,THD,RHD] = hough( DT, 'RhoResolution', 3.0, 'ThetaResolution', 0.3 );
HPD = houghpeaks( HFD, N );
LND = houghlines(DT, THD, RHD, HPD);
PTD = [ vertcat(LND.point1)  vertcat(LND.point2) ];
Nd = length( PTD );

% %% LINES
% 
% Ca = zeros(Nc,1);
% Cc = zeros(Nc,1);
% XC = zeros(Nc,10);
% YC = zeros(Nc,10);
% 
% for i = 1 : 1 : Nc;
%     
%     Ca(i) = (PTC(i,2) - PTC(i,4))./(PTC(i,1) - PTC(i,3));
%     Cc(i) = PTC(i,2) - Ca(i).*PTC(i,1);
%     
%     XC(i,:) = round( linspace(PTC(i,1),PTC(i,2),10) );
%     YC(i,:) = round( Ca(i).*XC(i,:) + Cc(i) );
%     
% end;
% 
% 
% Da = zeros(Nd,1);
% Dc = zeros(Nd,1);
% XD = zeros(Nd,10);
% YD = zeros(Nd,10);
% 
% for i = 1 : 1 : Nd;
%     
%     Da(i) = (PTD(i,2) - PTD(i,4))./(PTD(i,1) - PTD(i,3));
%     Dc(i) = PTD(i,2) - Da(i).*PTD(i,1);
%     
%     XD(i,:) = round( linspace(PTD(i,1),PTD(i,2),10) );
%     YD(i,:) = round( Da(i).*XD(i,:) + Dc(i) );
%     
% end;

%% END OF FILE
end