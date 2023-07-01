%% Visualizing
% Vadim Bulitko
% May 18, 2023

close all
clear
clc
format short g
r = rng('shuffle');
rngSeed = r.Seed;
npw = numParWorkers;

%% Parameters
%fn = '4x4-0.pzl';
%fn = '4x5-1.pzl';
%fn = '5x3-0.pzl';
%fn = '5x4-0.pzl';
%fn = '5x4-1.pzl';
%fn = '5x5-1.pzl';
%fn = '5x5-2.pzl';
fn = '6x4-2.pzl';

%% Load
cg = loadPZL2(fn);

% create the boolean grid
g = (cg == '#');


%% Visualize
visualizeGrid(g,false(size(g)),cg);

% save
[~,fns,~] = fileparts(fn);
exportgraphics(gcf,sprintf('../%s.pdf',fns));

