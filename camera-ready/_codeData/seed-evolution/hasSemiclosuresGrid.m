function sc = hasSemiclosuresGrid(pp)
%% Check if a puzzle has semiclosures
% The puzzle is assumed to be bordered with true
% Vadim Bulitko
% Jan 23, 2022

% arguments
%     pp (:,:) logical
% end

%% Preliminaries
im = zeros(size(pp),'int64');
n = size(pp,1);

%% Create an influence map in both cardinal and diagonal directions
for y = 1:n
    for x = 1:n
        
        % Do we have a wall at (x,y)?
        if (~pp(y,x))
            % no, move on
            continue;
        end
        
        % Yes, add the cell's influence in diagonal directions
        if (y > 1 && x > 1)
            im(y-1,x-1) = im(y-1,x-1) + 1;
        end
        
        if (y > 1 && x < n)
            im(y-1,x+1) = im(y-1,x+1) + 1;
        end
        
        if (y < n && x > 1)
            im(y+1,x-1) = im(y+1,x-1) + 1;
        end
        
        if (y < n && x < n)
            im(y+1,x+1) = im(y+1,x+1) + 1;
        end
        
        % Also add the cell's influence in cardinal directions
        if (y < n)
            im(y+1,x) = im(y+1,x) + 1;
        end
        
        if (y > 1)
            im(y-1,x) = im(y-1,x) + 1;
        end
        
        if (x < n)
            im(y,x+1) = im(y,x+1) + 1;
        end
        
        if (x > 1)
            im(y,x-1) = im(y,x-1) + 1;
        end
    end
end

% No influence in black cells
im(pp) = 0;

% pp
% im2 = im
% im2(im2 <= 1) = 0;         % not enough influence
% im2(im2 > 1) = 1;         % enough influence
% im2(pp) = -1              % black cells

%% Make a list of bottlenecks
bII = find(im > 1);

%% Go through each and see if closing it makes the puzzle disconnected
for i = 1:length(bII)
    bI = bII(i);
    
    % close the bottleneck
    ppC = pp;
    ppC(bI) = true;
    
    % is it still connected?
    if (~isConnectedGrid(ppC,1))
        sc = true;
        return
    end
end

%% Passed all tests
sc = false;

end
