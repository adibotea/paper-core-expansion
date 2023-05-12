function mrmeGrids(jobID)
%% MRME for crossword grids
% Vadim Bulitko
% Feb 3, 2023

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
    jobID (1,:) char = 'nonCC';
end

%% Determine our mode of operation
if (isequal(jobID,'nonCC'))
    % called locally like a script
    close all
    clear
    clc
    format short g
    jobID = 'mrmeGrids';
    rng('shuffle');
else
    % we are called from an sbatch on Compute Canada
    % cannot use rng('shuffle') since the scheduler sometimes starts several jobs at exactly the same time
    rng(str2num(jobID)); %#ok<ST2NM>
end

npw = numParWorkers;

%% Control parameters
% seeds
%initialPop.seedFN = 'grids/seeds/individual/feb3.mat';  % year 2021
initialPop.seedFN = 'grids/seeds/individual/y2013.mat';
%initialPop.seedFN = 'grids/seeds/individual/y2023.mat';

[~,initialPop.seedFNshort,~] = fileparts(initialPop.seedFN);

% wombat parameters
wombat.targetScore = 0;          % 0 178

wombat.timeLimitScaler = 10;
wombat.timeLimitMin = 3;
wombat.timeLimitMax = 180;
wombat.timeLimit = wombat.timeLimitMin;

wombat.weightScaler = 0.02;
wombat.weightMin = 0.5;               % 0.64 0.7
wombat.weightMax = 0.7;               % 0.64 0.7
wombat.weight = wombat.weightMin;

wombat.jobID = jobID;

% evolution parameters
evolution.mu = 1;
evolution.muScaler = 1;
evolution.batchSize = 8*npw;
evolution.pmMode = 3;                                   % protective mask mode
evolution.enforceMaxBC = true;                          % make sure all offspring have 26 BC
%evolution.ensureValidOffspring = true;
evolution.ensureValidOffspring = false;
evolution.numViolations = 0;
evolution.maxTime = 4*3600;

% feature 1: maximum word-slot length
features.f1.min = 6;
features.f1.max = 13;
features.f1.numValues = 1+features.f1.max-features.f1.min;
features.f1.resolutionRange = [3 round(features.f1.numValues/2) features.f1.numValues];
features.f1.name = 'max wsl';

% feature 2: number of walls
features.f2.min = 0;
features.f2.max = 26;
features.f2.numValues = 1+features.f2.max-features.f2.min;
features.f2.resolutionRange = [3 round(features.f2.numValues/2) features.f2.numValues];
features.f2.name = 'nw';

assert(length(features.f1.resolutionRange) == length(features.f2.resolutionRange));
features.numMaps = length(features.f1.resolutionRange);

% MC parameters
mcs.wombat.jobID = jobID;
mcs.wombat.targetScore = 0;
mcs.wombat.timeLimit = 60;
mcs.wombat.weight = 0.64;
mcs.maxTime = max(mcs.wombat.timeLimit,60);

% The global timings
totalTargetTime = 2*24*3600;

%% Preliminaries
createPWpool(npw);

warning('off','parallel:gpu:device:DeviceDeprecated');
warning('off','MATLAB:graphics:axestoolbar:PrintWarning');

figMain = figure('Position',[40 40 1800 400]);
tiledlayout(1,4); %,'TileSpacing','compact','Padding','compact');

tttTotal = tic;

%% Load seed grids
adiSeeds = load(initialPop.seedFN,'g');
adiSeeds.numSeeds = length(adiSeeds.g);

fprintf('Loaded %s seeds from %s\n',hrNumber(adiSeeds.numSeeds),initialPop.seedFN);

%% Pre-select seeds
mcs.numRuns = adiSeeds.numSeeds;

fprintf('\tscoring all of them using MC\n');

% figure out the time split
mcs.totalTargetTime = mcs.maxTime*ceil(mcs.numRuns/npw);
evolution.totalTargetTime = totalTargetTime - mcs.totalTargetTime;
evolution.numRuns = npw*floor(evolution.totalTargetTime/evolution.maxTime);

% sanity checks
assert(mcs.totalTargetTime >= mcs.maxTime);
assert(evolution.totalTargetTime >= evolution.maxTime);
assert(mcs.numRuns > evolution.numRuns);

fprintf('\tMCS total time %s + evolution total time %s = %s\n',...
    sec2str(mcs.totalTargetTime),sec2str(evolution.totalTargetTime),sec2str(totalTargetTime));
fprintf('\tMCS runs %d (%s each) --> evolution runs %d (%s each)\n\n',mcs.numRuns,sec2str(mcs.maxTime),evolution.numRuns,sec2str(evolution.maxTime));

% run MC
mcscore = NaN(1,mcs.numRuns);
tttMC = tic;

parfor sI = 1:mcs.numRuns
    mcscore(sI) = csemc(sI,adiSeeds.g(sI),mcs,mcs.wombat) %#ok<PFBNS>
end

timeMC = toc(tttMC);

% select the top-MC-score seeds
[mcscore,sortI] = sort(mcscore,'descend');
adiSeeds.g = adiSeeds.g(sortI);

minMCscoreOld = min(mcscore);
maxMCscoreOld = max(mcscore);
meanMCscoreOld = mean(mcscore);

mcscore = mcscore(1:evolution.numRuns);
adiSeeds.g = adiSeeds.g(1:evolution.numRuns);

fprintf('\nEvaluated %d seeds with MC | MC c.s. [%d,%d] %0.1f\n',mcs.numRuns,minMCscoreOld,maxMCscoreOld,meanMCscoreOld);
fprintf('\tselected top %d of them for MRME | MC c.s. [%d,%d] %0.1f\n',evolution.numRuns,...
    min(mcscore),max(mcscore),mean(mcscore));
fprintf('\t%s\n\n',sec2str(timeMC));

%% Evolve each seed in parallel
tttMRME = tic;

parfor sI = 1:evolution.numRuns
    [champion(sI), progress(sI)] = mrmeRun(sI,mcscore(sI),adiSeeds.g(sI),evolution,wombat,features) %#ok<PFBNS>
end

timeMRME = toc(tttMRME);

[maxScore,maxScoreI] = max([champion.score]);

fprintf('\nMax champion score %d | %s\n\n',maxScore,sec2str(timeMRME));

%% Save the champions and the progress
save(sprintf('data/mrmeGrids_%s-%dx%d-%dx%d-%s.mat',...
    initialPop.seedFNshort,mcs.maxTime,mcs.numRuns,evolution.maxTime,evolution.numRuns,jobID),...
    'progress','champion');

%% Visualize
% the overall champion
nexttile(1)
visualizeGrid(champion(maxScoreI).g, champion(maxScoreI).p,champion(maxScoreI).cg);
[~,fnShort,~] = fileparts(champion(maxScoreI).fn);
title(sprintf('%s',removeUnderscore(fnShort)));

nexttile(2)
visualizeHeatMap(champion(maxScoreI).hm);
title(sprintf('MRME %d | MC %d\nWombat(%d,%s,%0.2f)',champion(maxScoreI).score,champion(maxScoreI).mcscore,...
    champion(maxScoreI).wombat.targetScore,sec2str(champion(maxScoreI).wombat.timeLimit),champion(maxScoreI).wombat.weight));

% the synthesis curves
totalSynthesisTime = 0;
cm = lines(1);
nexttile(3)

for sI = 1:evolution.numRuns
    % filter out timestamps which are too low
    runtime = progress(sI).time;
    runtimeM = runtime > 1;

    % record the data for later averaging
    xTime{sI} = runtime(runtimeM); %#ok<*AGROW>
    yScore{sI} = progress(sI).score.champion(runtimeM);

    fprintf('seed %2d | %d data points | %s | %d\n',...
        sI,length(xTime{sI}),sec2str(max(xTime{sI})),champion(sI).score);

    totalSynthesisTime = totalSynthesisTime + max(xTime{sI});

    plot(xTime{sI},yScore{sI},'-','Color',[cm(1,:) 0.3]);
    hold on
end

[x,y,~] = averageCurves(xTime,yScore,true,1);
plot(x,y,'-','Color',cm(1,:),'LineWidth',1.5);

xlabel('time (sec)');
ylabel('champion score');
box on
grid on
set(gca,'XScale','log');
hold off

nexttile(4)
plotScoreHist([champion.score]);
xlabel('champion score');
ylabel('number of seeds');
title(sprintf('%0.1f +/- %0.2f',mean([champion.score]),std([champion.score])));

sgtitle(sprintf('%s | %d x %s --> %d x %s | %s + %s = %s',...
   removeUnderscore(initialPop.seedFNshort),...
   mcs.numRuns,sec2str(mcs.maxTime),...
   evolution.numRuns,sec2str(evolution.maxTime),...
   sec2str(timeMC),sec2str(timeMRME),sec2str(toc(tttTotal))));

% save the plots
exportgraphics(figMain,sprintf('plots/mrmeGrids_%s-%dx%d-%dx%d-%s.pdf',...
    initialPop.seedFNshort,mcs.maxTime,mcs.numRuns,evolution.maxTime,evolution.numRuns,jobID));

exportgraphics(figMain,sprintf('plots/mrmeGrids_%s-%dx%d-%dx%d-%s.pdf',...
    initialPop.seedFNshort,mcs.maxTime,mcs.numRuns,evolution.maxTime,evolution.numRuns,jobID));

end
