%% Visualize MRME results
% Vadim Bulitko
% Mar 9, 2023

close all
clear
clc
diary off
format short g
rng('shuffle');

%% Control parameters
fn = 'y-2013/results/mrmeGrids_y2013-60x932-14400x352-14113466.mat';
%fn = 'y-2021/results/mrmeGrids_feb3-60x38035-14400x224-13741779.mat';
%fn = 'y-2023/results/mrmeGrids_y2023-60x446-14400x352-14179462.mat';

[fnp,fns,~] = fileparts(fn);


%% Preliminaries
warning('off','parallel:gpu:device:DeviceDeprecated');
warning('off','MATLAB:graphics:axestoolbar:PrintWarning');

figMain = figure('Position',[40 40 1200 400]);
tiledlayout(1,3); 

%% Load the data
load(fn,'champion');

[maxScore,maxScoreI] = max([champion.score]);

fprintf('\nMax champion score %d | i = %d\n',maxScore,maxScoreI);

% load the puzzle
sg = loadPZL2(sprintf('%s/_runWombat/solution.pzl',fnp));

%% Visualize
% the overall champion
nexttile(1)
visualizeGrid(champion(maxScoreI).g, champion(maxScoreI).p,champion(maxScoreI).cg);
[~,fnShort,~] = fileparts(champion(maxScoreI).fn);
title(sprintf('%s',removeUnderscore(fnShort)));

% heat map
nexttile(2)
visualizeHeatMap(champion(maxScoreI).hm);
title(sprintf('MRME %d | MC %d\nWombat(%d,%s,%0.2f)',champion(maxScoreI).score,champion(maxScoreI).mcscore,...
    champion(maxScoreI).wombat.targetScore,sec2str(champion(maxScoreI).wombat.timeLimit),champion(maxScoreI).wombat.weight));

nexttile(3)
visualizeGrid(champion(maxScoreI).g,false(size(sg)),sg);

sgtitle(removeUnderscore(fn));

exportgraphics(figMain,sprintf('%s/_runWombat/%s-internal.pdf',fnp,fns));

close(figMain);

% figure for the paper
figMain = figure('Position',[40 40 900 400]);
tiledlayout(1,2,'TileSpacing','compact','Padding','compact');

nexttile(1)
visualizeGrid(champion(maxScoreI).g, champion(maxScoreI).p,champion(maxScoreI).cg);

nexttile(2)
visualizeGrid(champion(maxScoreI).g,false(size(sg)),sg);

exportgraphics(figMain,sprintf('%s/_runWombat/%s-paper.pdf',fnp,fns));

%% Extract the grid
%savePZL2(sprintf('%s/_runWombat/%s.pzl',fnp,fns),putInWallsFromGrid(champion(maxScoreI).cg,champion(maxScoreI).g));

