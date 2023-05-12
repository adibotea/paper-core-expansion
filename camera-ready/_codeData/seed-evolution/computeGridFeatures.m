function features = computeGridFeatures(grids)
%% computes features for grids
% Manuel Macedo Teran
% August 10th, 2022

arguments
    grids (:,:,:) logical
end

% Compute the number of wall components for every grid as well as the maximum slot length
nw = NaN(size(grids,3),1);
maxsl = NaN(size(grids,3),1);
for i = 1:size(grids,3)
    nw(i) = numWalls(grids(:,:,i));

    [~,wsl] = wordLengthCounts(grids(:,:,i));
    maxsl(i) = max(wsl);
end
features = [nw, maxsl];

end