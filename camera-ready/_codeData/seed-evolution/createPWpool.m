function createPWpool(numWorkers)
%% Creates a local pool of workers of needed size
% we can use evalc to avoid messages about pool starting and stopping
% Vadim Bulitko
% Jan 31, 2022

arguments
    numWorkers (1,1) double = NaN
end

%% Check on whether the pool exists
pool = gcp('nocreate');

%% See if need to create a new one
if (isempty(pool))
    % no pool at the moment
    if (~isnan(numWorkers))
        % specified number of workers
        parpool('local',numWorkers,'IdleTimeout',Inf);
    else
        % default number of workers
        parpool('local','IdleTimeout',Inf);
    end
else
    % a pool already exists
    if (~isnan(numWorkers) && pool.NumWorkers < numWorkers)
        % but wrong size (not enough workers)
        delete(pool);
        parpool('local',numWorkers,'IdleTimeout',Inf);
    end
end

end
