% EE_VIDEOCAPTURE
%
% Tommy P. Keane
%
% November 30, 2009
%

%% VIEW SYSTEM INFO

% System Input Information
VidSys = imaqhwinfo;

% Chosen Adapator Info
VideoInputInfo = imaqhwinfo( 'winvideo' );

% Available File Formats
F = VideoInputInfo.DeviceInfo.SupportedFormats;

% Default Video Format (Actual: 700x476)
FileFormat = 'RGB24_720x486';

%% SETUP CAMERA OBJECTS

% Camera 001
C1 = videoinput('winvideo', 1, FileFormat, 'SelectedSourceName', 'composite1' );
set( C1, 'ROIPosition', [3 0 701 476] );

% Camera 002
C2 = videoinput('winvideo', 2, FileFormat, 'SelectedSourceName', 'composite2' );
set( C2, 'ROIPosition', [3 0 701 476] );

% Camera 003
C3 = videoinput('winvideo', 3, FileFormat, 'SelectedSourceName', 'composite3' );
set( C3, 'ROIPosition', [3 0 701 476] );

% Camera 004
C4 = videoinput('winvideo', 4, FileFormat, 'SelectedSourceName', 'composite4' );
set( C4, 'ROIPosition', [3 0 701 476] );

%% LIVE CAMERA VIEWS

% Camera 001
preview( C1 );

% Camera 002
preview( C2 );

% Camera 003
preview( C3 );

% Camera 004
preview( C4 );


%% SET TRIGGERS

triggerconfig(C1, 'hardware', 'TTL')


%% CAPTURE IMAGES

% Camera 001
A1 = getsnapshot( C1 );

% Camera 002
A2 = getsnapshot( C2 );

% Camera 003
A3 = getsnapshot( C3 );

% Camera 004
A4 = getsnapshot( C4 );

imwrite( A3, 'Track_L_001.png', 'png' );
imwrite( A4, 'Track_R_001.png', 'png' );
%% END OF FILE
