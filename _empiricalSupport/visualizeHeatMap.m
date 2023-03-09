function visualizeHeatMap(hm)
%% Visualizes a heat map from Wombat
% Vadim Bulitko
% Oct 31, 2022

arguments
    hm (:,:) double
end

%% Special case
if (isempty(hm))
    return
end

%% Visualize the grid
visualizeGrid(hm == -2);
hold on

% create a color map
fa = 0.65;  % transparency

cm = lines(3);
cm(:,4) = fa;


%% Visualize the 0-point letters
ii = ensureRowVector(find(hm == 0));
if (~isempty(ii))
    for i = ii
        [y,x] = ind2sub(size(hm),i);
        rectangle('Position',[x+0.025 y+0.025 0.95 0.95],'FaceColor',cm(1,:));
    end
end

%% Visualize the 1-point letters
ii = ensureRowVector(find(hm == 1));
if (~isempty(ii))
    for i = ii
        [y,x] = ind2sub(size(hm),i);
        rectangle('Position',[x+0.025 y+0.025 0.95 0.95],'FaceColor',cm(3,:));
    end
end

%% Visualize the 2-point letters
ii = ensureRowVector(find(hm == 2));
if (~isempty(ii))
    for i = ii
        [y,x] = ind2sub(size(hm),i);
        rectangle('Position',[x+0.025 y+0.025 0.95 0.95],'FaceColor',cm(2,:));
    end
end

% pcolor2([],[],hm);
% colormap(gca,...
%     [0.3 0.3 0.3                % grey: black cell (-2)
%     1 1 1                       % white: no letter (-1)
%     0 0.4470 0.7410             % blue: letter, 0 points (0)
%     0.9290 0.6940 0.1250        % yellow: letter, 1 point (1)
%     0.8500 0.3250 0.0980]);     % orange: letter, 2 points (2)
% cb = colorbar;
% cb.Ticks = [-2 -1 0 1 2];
% cb.TickLabels = {'black cell','no letter','0 pts','1 pt','2 pts'};

axis square equal ij
hold off

end
