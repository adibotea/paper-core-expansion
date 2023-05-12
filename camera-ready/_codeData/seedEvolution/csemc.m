function scoreE = csemc(seedI,seedG,mcs,wombat,verbose)
%% Estimating completion score of a seed via MC sampling
% Vadim Bulitko
% Feb 9, 2023

%Copyright 2023 Vadim Bulitko
%
%   Licensed under the Apache License, Version 2.0 (the "License");
%   you may not use this file except in compliance with the License.
%   You may obtain a copy of the License at
%
%       http://www.apache.org/licenses/LICENSE-2.0
%
%   Unless required by applicable law or agreed to in writing, software
%   distributed under the License is distributed on an "AS IS" BASIS,
%   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%   See the License for the specific language governing permissions and
%   limitations under the License.

arguments
    seedI (1,1) double
    seedG (1,1) struct
    mcs (1,1) struct
    wombat (1,1) struct
    verbose (1,1) logical = true
end

% score mutations until we run out of time
mI = 0;
tttMC = tic;

while (toc(tttMC) < mcs.maxTime)

    % create a valid completion
    while (true)
        % mutate the grid by adding a random number of black cells up to the maximum
        mg = mutateGrid2(seedG.g,seedG.p);

        % make sure we do produce a legal and novel grid
        if (isValidGrid_mex(mg))
            break
        end
    end

    % put the new walls into the character grid
    gridC = seedG.cg;
    gridC(mg) = '#';

    % score the mutation
    mI = mI + 1;
    [score(mI), ~, ~] = wombatScore(mg,wombat.targetScore,wombat.timeLimit,wombat.jobID,wombat.weight,wombat.year,gridC,false); %#ok<AGROW> 
end

% create the estimate
scoreE = max(score);

if (verbose)
    fprintf('seed %5d | Wombat(%d,%s,%0.2f) x %4d | MC estimate %3d\n',...
        seedI,wombat.targetScore,sec2str(wombat.timeLimit),wombat.weight,mI,scoreE);
end

end
