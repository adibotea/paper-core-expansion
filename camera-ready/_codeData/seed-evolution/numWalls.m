function nw = numWalls(grid)
%% Computes the number of wall components in a grid
% Vadim Bulitko
% March 3, 2022

arguments
    grid (:,:) logical
end

%% Preliminaries
% invert the grid
grid = ~grid;

% border it with walls
grid = borderGrid(grid,true);

%% Compute connected components of the empty cells (obstacles before inversion) using diagonal connectivity
cc = connectedComponents_mex(grid);

nw = max(cc,[],'all');

end
