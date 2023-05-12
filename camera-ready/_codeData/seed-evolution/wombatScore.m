function [score, perGridTime, heatmap] = wombatScore(gridB,targetScore,timeLimit,jobID,weight,gridC,parallel)
%% In parallel runs command-line Wombat to evaluate a set of grids
% score = -1 if Wombat does not produce an output or the grid is invalid
% Vadim Bulitko
% Jan 19, 2022

arguments
    gridB (:,:,:) logical
    targetScore (1,1) double
    timeLimit (1,1) double
    jobID (1,:) char
    weight (1,1) double
    gridC (:,:,:) char = []
    parallel (:,:) logical = true
end

%% Preliminaries
numGrids = size(gridB,3);
score = -1*ones(1,numGrids);
heatmap = NaN(size(gridB,1),size(gridB,2),numGrids);
perGridTime = NaN(1,numGrids);

% run Wombat on valid grids
%for i = 1:numGrids

if (parallel)
    nw = npw;
else
    nw = 1;
end

parfor (i = 1:numGrids, nw)
    perGridTTT = tic;

    % make sure the grid is valid
    if (isValidGrid_mex(gridB(:,:,i)))
        % get the task ID for parallel invocations
        ct = getCurrentTask();
        if (~isempty(ct))
            taskID = ct.ID;
        else
            taskID = 0;  % in case we want to change parfor to for for testing
        end

        % write out the grid
        puzzleFN = sprintf('wombat/%s-%d.pzl',jobID,taskID);
        if (isempty(gridC))
            % no character grids provided, use the binary grid only
            savePZL2(puzzleFN,puzzleBin2Char(gridB(:,:,i)));
        else
            % use the actual character grid provided
            savePZL2(puzzleFN,gridC(:,:,i));
        end

        % delete a pre-existing output if any
        scoreFN = sprintf('wombat/%s-%d.score',jobID,taskID);
        if (exist(scoreFN,'file'))
            delete(scoreFN);
        end

        heatmapFN = sprintf('wombat/%s-%d.hm',jobID,taskID);
        if (exist(heatmapFN,'file'))
            delete(heatmapFN);
        end

        % form the command line
        wombatCMD =...
            sprintf('wombat/wombat -v 0 -ntw3456 20 20 20 20 -mnw 100 -s 1 -i %s -mts %d -t %d -start_slot -1 -search 5 -heur 13 -weight %0.2f -sf %s -hf %s',...
            puzzleFN,targetScore,timeLimit,weight,scoreFN,heatmapFN);

        %     wombatCMD =...
        %         sprintf('wombat/wombat -v 0 -ntw3456 20 20 20 20 -mnw 100 -s 1 -i %s -mts %d -t %d -start_slot -1 -search 5 -heur 13 -weight %0.2f -sf %s',...
        %         puzzleFN,targetScore,timeLimit,weight,scoreFN);

        % invoke Wombat
        [~,~] = system(wombatCMD);

        % load the score if it exists
        if (exist(scoreFN,'file'))
            score(i) = floor(load(scoreFN,'-ascii'));
        end

        % load the heat map if it exists
        if (exist(heatmapFN,'file'))
            heatmap(:,:,i) = readHeatMap(heatmapFN);
        end

        % fprintf('\t%d | Wombat score %d | %s\n',i,score(i),sec2str(perGridTime(i)));
    end

    perGridTime(i) = toc(perGridTTT);
end

end
