function mg  = mutateGrid(g,p,mu,numViolationsAllowed,enforceMaxBC,ensureValid)
%% Mutate a grid
% Vadim Bulitko
% Jan 6, 2023

arguments
    g (:,:) logical
    p (:,:) logical
    mu (1,1) double
    numViolationsAllowed (1,1) double = mu
    enforceMaxBC (1,1) logical = true
    ensureValid (1,1) logical = false
end

%% Preliminaries
%numMutations = ceil(0.1+abs(exprnd(mu)));
numMutations = ceil(0.1+abs(exprnd(1)));

gridSize = size(g,1);
maxNBC = maxNumBC(gridSize);

if (numViolationsAllowed == -1)
    numViolationsAllowed = mu;
end

np = ~p;

%% Repeat sequences of mutations until we have one that does not violate the protected regions
while (true)
    mg = g;

    % execute a sequence of mutations
    for i = 1:numMutations
        switch (randi(9))
            case {1,2,3}
                % add black cells
                maxNumBCtoAdd = maxNBC - nnz(mg);
                if (maxNumBCtoAdd > 0)
                    if (numViolationsAllowed > 0)
                        l = find(~mg);
                    else
                        % make sure the spaces are not protected if we are not allowed any violations
                        l = find(~mg & np);
                    end
                    %mg(l(randperm(length(l),randi(maxNumBCtoAdd)))) = true;
                    mg(l(randperm(length(l),maxNumBCtoAdd))) = true;
                end

            case {4,5,6}
                % remove some black cells
                if (nnz(mg) > 0)
                    if (numViolationsAllowed > 0)
                        l = find(mg);
                    else
                        % make sure they are not protected if we are not allowed any violations
                        l = find(mg & np);
                    end
                    ll = length(l);
                    mg(l(randperm(ll,randi(ll)))) = false;
                end

            otherwise
                % shift a single black cell (multiple times)
                if (nnz(mg) > 0)
                    mg = shiftBC(mg,np,mu,numViolationsAllowed);
                end
        end
    end

    % see if we are asked to check for the maximum number of BC
    if (enforceMaxBC && nnz(mg) ~= maxNBC)
        % we don't have maxNBC, try again
        continue
    end

    %     % did we actually make any changes?
    %     if (isequal(mg,g))
    %         % no, try again
    %         continue
    %     end

    % check for validity if asked
    if (ensureValid && ~isValidGrid(mg))
        continue;
    end

    % check for violations of the protected region
    violations = p & xor(g,mg);

    if (nnz(violations) <= numViolationsAllowed)
        % the number of violations is allowed
        return
    end
end

end


function g = shiftBC(g,np,mu,numViolationsAllowed)
%% Shift a single black cell around

arguments
    g (:,:) logical
    np (:,:) logical
    mu (1,1) double %#ok<INUSA>
    numViolationsAllowed (1,1) double
end

% pick a random black cell
if (numViolationsAllowed > 0)
    bcI = find(g);
else
    % make sure it is not protected if we are not allowed any violations
    bcI = find(g & np);
end

i = bcI(randi(length(bcI)));

% shift it multiple times
%numShifts = ceil(0.1+abs(exprnd(mu)));
numShifts = ceil(0.1+abs(exprnd(1)));

iOriginal = i;
for s = 1:numShifts
    i = shiftBlackCell(g,i);
end

% move the cell
g(iOriginal) = false;
g(i) = true;

end


function newI = shiftBlackCell(g,i)
%% Shifts the given black cell within the puzzle

arguments
    g (:,:) logical
    i (1,1) double
end

% get the coordinates
[y,x] = ind2sub(size(g),i);

% shift +/- 1 in each direction
yN = y + randi(3) - 2;
xN = x + randi(3) - 2;

% clip to the puzzle dimensions
yN = max(1,(min(size(g,1),yN)));
xN = max(1,(min(size(g,2),xN)));

% get the new index
newI = sub2ind(size(g),yN,xN);

end
