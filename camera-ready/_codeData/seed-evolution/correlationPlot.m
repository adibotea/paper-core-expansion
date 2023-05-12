function [rho, rrho, rmseOut] = correlationPlot(x,y,titleStr,diagonal,leg,ylog,anyPlot)
%% A correlation plot
% Vadim Bulitko
% Feb 12, 2022

arguments
    x (1,:) double
    y (1,:) double
    titleStr (1,:) char = ''
    diagonal (1,1) logical = false
    leg (1,1) logical = false
    ylog (1,1) logical = false
    anyPlot (1,1) logical = true
end

%% Preliminaries
x = ensureColumnVector(x);
y = ensureColumnVector(y);

assert(length(x) == length(y));

% sort the data (needed for trend lines in log scale)
[x,p] = sort(x,'ascend');
y = y(p);

% are the values integer?
intX = isequal(round(x),x);
intY = isequal(round(y),y);

%% Correlation: Pearson
rho = corr(x,y,'Type','Pearson');

%% Correlation: Spearman rank
rrho = corr(x,y,'Type','Spearman');

%% Fit a line
linF = fit(x,y,'poly1');
linY = ensureColumnVector(x*linF.p1 + linF.p2);

%% Plot
if (anyPlot)
    % if the values are integer then perturb them randomly by 1% of the range for plotting
    if (intX)
        plotX = x + (rand(size(x))-0.5)*((max(x) - min(x))/50);
    else
        plotX = x;
    end

    if (intY)
        plotY = y + (rand(size(y))-0.5)*((max(y) - min(y))/50);
    else
        plotY = y;
    end

    scatter(plotX,plotY,'.');
    hold(gca,'on')
    plot(x,linY,'-');

    box on
    grid on

    % plot a 45-degree diagonal if desired
    if (diagonal)
        axis(gca,'tight');
        xl = xlim;
        yl = ylim;
        xyl = [min(xl(1),yl(1)),max(xl(2),yl(2))];
        xlim(xyl);
        ylim(xyl);
        plot(xyl,xyl,'k-');
        axis(gca,'equal');
        axis(gca,'tight');
        axis(gca,'square');
    else
        axis(gca,'tight');
    end

    % log scale?
    if (ylog)
        set(gca,'YScale','log');
    end

    hold(gca,'off')

    % legend if requested
    if (leg)
        legend('data',sprintf('y = %s x + %s',latexEN(linF.p1),latexEN(linF.p2)),'Location','best');
    end

    if (~isempty(titleStr))
        title(sprintf('%s\n%s | r %0.2f | \\rho %0.2f | RMSE %0.2f',...
            titleStr,hrNumber(length(x)),rho,rrho,rmse(x,y)));
    else
        title(sprintf('%s | r %0.2f | \\rho %0.2f | RMSE %0.2f',...
            hrNumber(length(x)),rho,rrho,rmse(x,y)));
    end
end

rmseOut = rmse(x,y);

end
