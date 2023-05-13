function cg = putInWallsFromGrid(cg,g)
%% Adds walls from a binary grid into a character grid
% Jan 18, 2023

arguments
    cg (:,:) char
    g (:,:) logical
end

cg(g) = '#';

end
