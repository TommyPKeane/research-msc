function [ g ] = LOG2( f )
%LOG2 Summary of this function goes here
%   Detailed explanation goes here

F = log( f ) ./ log( 2 );

F( isnan(F) ) = 0;
F( isinf(F) ) = 0;

end
