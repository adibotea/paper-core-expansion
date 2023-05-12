function hm = readHeatMap(heatmapFN)
%% Reads a heatmap from Wombat
% Returns an empty value if the file is unreadable
% each cell can be: -2 (wall), -1 (no letter assigned), 0-2 (a letter assigned)
% Vadim Bulitko
% Oct 24, 2022

arguments
    heatmapFN (1,:) char
end

%% Preliminaries
hm = [];

%% Load the file
fid = fopen(heatmapFN,'r');
if (fid ~= -1)
    % read the cell array
    ca = readcell(heatmapFN,'FileType','text');

    hm = -2*ones(size(ca));

    for y = 1:size(ca,1)
        for x = 1:size(ca,2)
            element = ca{y,x};
            if (~ischar(element))
                % 0, 1 or 2 points
                hm(y,x) = element;
            else
                if (element == '-')
                    % no letters --> no points
                    hm(y,x) = -1;
                end
            end
        end
    end

    fclose(fid);
end

end
