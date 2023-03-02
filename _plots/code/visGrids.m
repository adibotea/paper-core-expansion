%% Visualizing
% Vadim Bulitko
% Mar 1, 2023

close all
clear
clc
format short g
r = rng('shuffle');
rngSeed = r.Seed;
npw = numParWorkers;

%% Parameters
%fn = '../source/6x4-puzzle.pzl';
%fn = '../source/alive-0-puzzle-72-2975-1488--1-1.pzl';
%fn = '../source/alive-0-puzzle-72-2975-1488--1--1.pzl';
%fn = '../source/alive-0-puzzle-72-2975-1488--1-2.pzl';
%fn = '../source/extcore-alive-0-puzzle-72-2975-1488--1--1.pzl';
%fn = '../source/core-6x4-puzzle.pzl';

%% Load
cg = loadPZL2(fn);

% create the boolean grid
g = (cg == '#');


%% Visualize
visualizeGrid(g,false(size(g)),cg);

% save
[~,fns,~] = fileparts(fn);
exportgraphics(gcf,sprintf('../%s.pdf',fns));

