function g  = mutateGrid2(g,p)
%% Mutate a grid by adding up to maximum extra black cells to open and non-protected areas
% Vadim Bulitko
% Jan 16, 2023

arguments
    g (:,:) logical
    p (:,:) logical
end

%% Preliminaries
maxNBC = maxNumBC(size(g,1));

%% Open and unprotected cells
availableI = find(~p & ~g);

%% Figure out how many black cells to add
numBCtoAdd = min(length(availableI),maxNBC-nnz(g));

%% Add them
g(availableI(randperm(length(availableI),numBCtoAdd))) = true;

end
