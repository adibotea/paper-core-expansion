function connected = isConnectedGrid(puzzle,allowIslandSize)
%% Check if the grid is connected (cardinally)
% We assume that puzzle is bordered with true
% Vadim Bulitko
% Jan 23, 2022

% arguments
%     puzzle (:,:) logical
%     allowIslandSize (1,1) double
% end

%% Label one component
a = find(~puzzle);

%% Flood fill from one of open cells
cc = zeros(size(puzzle),'int64');
cc(puzzle) = -1;  % black cells
cc = floodFill(cc,a(1),int64(1));

%% Assuming there are only two connected components, we check if the size of either is under the allowable limit
connected = (nnz(cc == int64(0)) <= allowIslandSize) || (nnz(cc == int64(1)) <= allowIslandSize);

end


function cc = floodFill(cc,i,ccI)
%% Floods fills from a given index i
% Vadim Bulitko
% May 12, 2020

% arguments
%     cc (:,:) int64
%     i (1,1) double
%     ccI (1,1) int64
% end


%% Preliminaries
mapHeight = size(cc,1);
childrenI = int64([mapHeight 1 -mapHeight -1]);

% set up the frontier
frontierMaxSize = uint64(numel(cc));
frontier = zeros(1,frontierMaxSize,'int64');
fStartI = uint64(1);
fNextI = uint64(2);
frontier(fStartI) = int64(i);
cc(i) = ccI;

%% Run BFS from the initial state
while (fStartI ~= fNextI)
    % pop the state from the frontier
    i = frontier(fStartI);
    fStartI = fStartI + 1;
    if (fStartI > frontierMaxSize)
        fStartI = uint64(1);
    end
    
    % expand the state i
    childI = i + childrenI;
    
    % keep unlabeled children
    childI = childI(cc(childI) == 0);
    
    % label them with the connected component index
    cc(childI) = ccI;
    
    % append them to the frontier
    for cI = 1:length(childI)
        frontier(fNextI) = childI(cI);
        fNextI = fNextI + 1;
        if (fNextI > frontierMaxSize)
            fNextI = uint64(1);
        end
    end
end

end
