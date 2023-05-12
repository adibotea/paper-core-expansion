function [in, i] = inPopulation(grid,grids)
%% Checks if a given grid is already in the population
% Vadim Bulitko
% Jan 7, 2022

arguments
    grid (:,:) logical
    grids (:,:,:) logical
end

for i = 1:size(grids,3)
    if (isequal(grid,grids(:,:,i)))
        in = true;
        return
    end
end

in = false;
i = NaN;

end
