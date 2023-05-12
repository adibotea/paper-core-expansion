function [wlc, wsl] = wordLengthCounts(g)
%% Computes word-slot length counts
% Vadim Bulitko
% Feb 9, 2022

arguments
    g (:,:) logical
end

%% Get the list of word slot lengths
wsl = double(wordLengthDistribution_mex(g,13));

%% Build the counts
wlc = zeros(1,13);
for i = 1:length(wsl)
    % get the next length from the array
    l = wsl(i);
    assert(l >= 1 && l <= 13);

    % increase the count
    wlc(l) = wlc(l)+1;
end

end
