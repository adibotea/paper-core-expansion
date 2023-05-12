function d = wordLengthDistribution(p,maxWordLength)
%% Compute a distribution of lengths of word slots
% p is assumed not bordered
% Vadim Bulitko
% May 7, 2021

% arguments
%     p (:,:) logical
%     maxWordLength (:,:) double
% end

%% Preliminaries
n = int64(size(p,1));
dTemp = zeros(1,2*numel(p),'int64');
dI = int64(0);

maxWL = int64(maxWordLength);

%% Scan row by row
for y = 1:n
    % start another row
    c = int64(0);
    for x = 1:n

        % Do we have a black cell at (x,y)?
        if (p(y,x))
            % Is this an end of a slot?
            if (c > int64(0))
                % yes, record the counter
                dI = dI + 1;
                dTemp(dI) = c;

                if (c > maxWL)
                    % found an excessively long slot: premature termination
                    d = dTemp(1:dI);
                    return
                end

                % reset the counter
                c = int64(0);
            end
        else
            % no, increase the counter
            c = c + 1;
        end
    end

    % Reached the end of the row: Is this an end of a slot?
    if (c > int64(0))
        % yes, record the counter
        dI = dI + 1;
        dTemp(dI) = c;

        if (c > maxWL)
            % found an excessively long slot: premature termination
            d = dTemp(1:dI);
            return
        end
    end
end

%% Scan column by column
for x = 1:n
    % start another column
    c = int64(0);
    for y = 1:n

        % Do we have a black cell at (x,y)?
        if (p(y,x))
            % Is this an end of a slot?
            if (c > int64(0))
                % yes, record the counter
                dI = dI + 1;
                dTemp(dI) = c;

                if (c > maxWL)
                    % found an excessively long slot: premature termination
                    d = dTemp(1:dI);
                    return
                end

                % reset the counter
                c = int64(0);
            end
        else
            % no, increase the counter
            c = c + 1;
        end
    end

    % Reached the end of a column: Is this an end of a slot?
    if (c > int64(0))
        % yes, record the counter
        dI = dI + 1;
        dTemp(dI) = c;

        if (c > maxWL)
            % found an excessively long slot: premature termination
            d = dTemp(1:dI);
            return
        end
    end
end

%% Trim the distribution
d = dTemp(1:dI);

end
