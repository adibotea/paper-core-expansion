function p = heatmapToProtectmask(hm,mode)
%% Computes protected regions from a heat map
% Vadim Bulitko
% Jan 8, 2023

arguments
    hm (:,:) double
    mode (1,1) double = 3
end

n = size(hm,1);
assert(n == size(hm,2));

% default: all cells from thematic words
p = (hm >= 1);

switch (mode)
    case 0
        % no protection
        p = false(size(hm));
    case 1
        % all open cells with heat-map value of 1 or higher
    case 2
        % all open cells with heat-map value of 2
        p = (hm >= 2);
    case 3
        % all thematic words and walls defining them

        % second, mark black cells adjacent to them
        % scan columns
        for x = 1:n
            for y = 1:n
                % do we have a wall at position (y,x)?
                if (hm(y,x) == -2)
                    % thematic word (of length > 1) above
                    if (y >= 3 && p(y-1,x) && p(y-2,x))
                        p(y,x) = true;
                    end

                    % thematic word (of length > 1) below
                    if (y <= n-2 && p(y+1,x) && p(y+2,x))
                        p(y,x) = true;
                    end

                    % thematic word (of length > 1) to the left
                    if (x >= 3 && p(y,x-1) && p(y,x-2))
                        p(y,x) = true;
                    end

                    % thematic word (of length > 1) to the right
                    if (x <= n-2 && p(y,x+1) && p(y,x+2))
                        p(y,x) = true;
                    end
                end
            end
        end
end

end
