function nbc = maxNumBC(gridSize)
%% Returns the maximum number of black cells for a given grid size
% Vadim Bulitko
% Jan 8, 2023

arguments
    gridSize (1,1) double = 13
end

nbc = ceil(0.15*gridSize*gridSize);

end
