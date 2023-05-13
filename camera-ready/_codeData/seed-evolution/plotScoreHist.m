function plotScoreHist(score, ylog, maxScore, minScore)
%% Plots a histogram of grid scores
% Vadim Bulitko
% Feb 20, 2022

arguments
    score (1,:) double
    ylog (1,1) logical = false;
    maxScore (1,1) double = max(score);
    minScore (1,1) double = min(score);
end

%% Plot
if (ylog)
    score = score + 1;
    maxScore = maxScore + 1;
end

edges = minScore:maxScore+1;
x = minScore:maxScore;
[y,~] = histcounts(score,edges);
bar(x,y,1,'LineStyle','none','facealpha',0.5);
xlabel('Score');
ylabel('Grids');
box on
grid on

if (ylog)
    set(gca,'YScale','log');
end

end
