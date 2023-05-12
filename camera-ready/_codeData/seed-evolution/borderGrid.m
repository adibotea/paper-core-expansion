function pp = borderGrid(p,fill)
%% Border a puzzle with black cells or open cells
% Vadim Bulitko
% Jan 23, 2022

% arguments
%     p (:,:) logical
%     fill (1,1) logical
% end

d = 1;

if (fill)
    % add the border of black cells
    pp = true(size(p,1)+2*d,size(p,2)+2*d);
else
    % add a border of empty cells
    pp = false(size(p,1)+2*d,size(p,2)+2*d);
end

pp(1+d:end-d,1+d:end-d) = p;

end
