function visualizeGrid(grid, p, l, greyScale)
%% Visualize a grid and a protection mask
% Vadim Bulitko
% Jan 23, 2022

arguments
    grid (:,:) logical
    p (:,:) logical = false(size(grid))
    l (:,:) char = []
    greyScale (1,1) double = 0.3
end

%% Create a visualization matrix
m = double(grid)*2 + double(p);

%% Create the color map
cm = [1 1 1; 1 0.8 0.8; greyScale greyScale greyScale; greyScale+0.2 0.3 0.3];

% trim it to size
cm = cm(1:max(m,[],'all')+1,:);

%% Visualize
hold on
pcolor2([],[],m);
axis(gca,'ij');
axis(gca,'equal');
colormap(gca,cm);
colorbar off
hold off

%% Display letters if supplied
if (~isempty(l))
    % leave the letters only
    l(l == '#') = ' ';
    l(l == '.') = ' ';

    % display non spaces
    hold on
    for xI = 1:size(l,2)
        for yI = 1:size(l,1)
            if (l(yI,xI) ~= ' ')
                text(xI+0.25,yI+0.5,l(yI,xI),'Color','black');
            end
        end
    end
    hold off
end

end
