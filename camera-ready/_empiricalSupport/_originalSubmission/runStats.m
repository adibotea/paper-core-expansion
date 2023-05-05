%% Compute stats on SoCS runs
% Vadim Bulitko
% May 5, 2023

close all
clear
clc
format short g
rng('shuffle');

%% Control parameters
% fnRange = {
%     'y-2013/results/mrmeGrids_y2013-60x932-14400x352-14113352.mat'
%     'y-2013/results/mrmeGrids_y2013-60x932-14400x352-14113464.mat'
%     'y-2013/results/mrmeGrids_y2013-60x932-14400x352-14113465.mat'
%     'y-2013/results/mrmeGrids_y2013-60x932-14400x352-14113466.mat'
%     };

% fnRange = {
%     'y-2021/results/mrmeGrids_feb3-60x38035-14400x224-13741779.mat'
%     'y-2021/results/mrmeGrids_feb3-60x38035-14400x224-13741780.mat'
%     'y-2021/results/mrmeGrids_feb3-60x38035-14400x224-13741781.mat'
%     'y-2021/results/mrmeGrids_feb3-60x38035-14400x224-13741783.mat'
%     };

fnRange = {
    'y-2023/results/mrmeGrids_y2023-60x446-14400x352-14179335.mat'
    'y-2023/results/mrmeGrids_y2023-60x446-14400x352-14179462.mat'
    'y-2023/results/mrmeGrids_y2023-60x446-14400x352-14179463.mat'
    'y-2023/results/mrmeGrids_y2023-60x446-14400x352-14179464.mat'
    };

%% Load each file and concatenate the champions
champion = [];

for fnI = 1:length(fnRange)
    fn = fnRange{fnI};
    l = load(fn,'champion');
    champion = [champion l.champion]; %#ok<AGROW>

    fprintf('%d | loaded %d champions from %s\n',fnI,length(l.champion),fn);
end

%% Print the stats
fprintf('\nA total of %d champions | ',length(champion));
cs = [champion.score];
fprintf('evolution scores:\n\t\t%d to %d\n\t\tmean %0.1f +/- %0.2f\n\t\tmedian %0.1f\n',min(cs),max(cs),mean(cs),std(cs),median(cs));
