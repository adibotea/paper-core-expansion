function [v,invalidityType] = isValidGrid(grid)
%% Checks if a given grid is valid
% Vadim Bulitko
% Jan 23, 2022

arguments
    grid (:,:) logical
end

invalidityType = '';

%% Do we have the right number of black cells?
n = size(grid,1);

if (nnz(grid) > maxNumBC(n))
    v = false;
    invalidityType = 'too many black cells';
    return;
end

%% Are any of the black cells adjacent cardinally?
borderedGrid = borderGrid(grid,false); % need to border it with empty cells

for y = 2:n+1
    for x = 2:n+1

        % Do we have a wall at (x,y)?
        if (~borderedGrid(y,x))
            % no, move on
            continue;
        end

        % Yes, check its cardinal neighbors
        if (borderedGrid(y-1,x) || borderedGrid(y+1,x) || borderedGrid(y,x-1) || borderedGrid(y,x+1))
            % we have an adjacent black cell in a cardinal direction
            % the puzzle is not valid
            v = false;
            invalidityType = 'cardinal adjacency';
            return
        end
    end
end


%% Check for connectivity
borderedGrid = borderGrid(grid,true);  % need to border it with walls

if (~isConnectedGrid(borderedGrid,0))
    v = false;
    invalidityType = 'disconnected';
    return
end

%% Check for semi-closures
if (hasSemiclosuresGrid(borderedGrid))
    v = false;
    invalidityType = 'has semiclosures';
    return
end

%% Passed all tests
v = true;

end
