function [] = RUN_PhaseII( varargin )
% RUN_PHASEI
%
%
%
%
% See also M_MR_ALMI, MR_ALMI, num2str.

addpath './libs/'

%% LOAD PHASE I RESULTS

load '../[] RESULTS/[] RESULTS [2010.03.11]/Lenel_010_010'

%% DETERMINE OVERLAPS

[ OVC, OVD ] = DetermineOverlaps( mt(1), nt(1), dr(1), dc(1) );

MC = zeros( [size(C)] );
MD = zeros( [size(D)] );
MC( OVC(1,1):1:OVC(2,1), OVC(1,2):1:OVC(2,2), : ) = 1;
MD( OVD(1,1):1:OVD(2,1), OVD(1,2):1:OVD(2,2), : ) = 1;

OC = C .* MC;
OD = D .* MD;

%% GET SEGMENTATION MAP FEATURES

[ OCS ] = SegmentOverlapMAP( OC );
[ ODS ] = SegmentOverlapMAP( OD );

% [ CSc ] = phasecongmono( OCS );
% [ DSc ] = phasecongmono( ODS );

[ CSc ] = edge( OCS, 'canny' );
[ DSc ] = edge( ODS, 'canny' );

[IJC] = find( Yimage(OC) > 0 );
[IJD] = find( Yimage(OD) > 0 );

cB = zeros( size(CSc) );
dB = zeros( size(DSc) );

cB( IJC ) = 1;
dB( IJD ) = 1;

CSc = CSc .* ~( imdilate( edge(cB,'sobel'), strel('square', 7) ) );
DSc = DSc .* ~( imdilate( edge(dB,'sobel'), strel('square', 7) ) );


%% FEATURE MATCHING

[ LNC, PTC, LND, PTD ] = HoughMATLAB( CT, DT, 10 );

%% COHERENT POINT DRIFT

opt.method = 'nonrigid';
opt.viz = 1;
opt.fgt = 2;

[Transform, CDi]=cpd_register(Ic(:,1:2), Id(:,1:2), opt);

%% END OF FILE
end
