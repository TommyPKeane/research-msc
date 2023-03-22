%%

surf( wM, 'edgecolor', 'none', 'displayname', 'Weighted Mutual Information' );

%%

surf( PC, 'edgecolor', 'none', 'displayname', 'Pixel Weighting Map' );

%%

surf( PC.*wM, 'edgecolor', 'none', 'displayname', 'Pixel Weighted Weighted Mutual Information' );

%%

surf( LwM, 'edgecolor', 'none', 'displayname', 'Absolute Laplacian of Mutual Information' );
