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
fn = '../source/extcore-alive-0-puzzle-72-2975-1488--1--1.pzl';

%% Load
cg = loadPZL2(fn);

% create the boolean grid
g = (cg == '#');


%% Visualize
visualizeGrid(g,false(size(g)),cg)
