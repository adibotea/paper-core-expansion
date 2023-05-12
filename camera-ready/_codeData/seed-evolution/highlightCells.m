function highlightCells(n,ind)
%% Highlight cells
% Vadim Bulitko
% Jun 17, 2022

arguments
    n (1,2) double
    ind (1,:) double
end

% special case
if (isempty(ind))
    % no states to highlight
    return
end

% display
hold on

for i = ensureRowVector(ind)
    [y,x] = ind2sub([n n],i);
    rectangle('Position',[x+0.1,y+0.1,0.8,0.8],'EdgeColor',[0.8500 0.3250 0.0980],'LineWidth',1);
end

hold off

end
