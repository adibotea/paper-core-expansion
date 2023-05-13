function [xRange,meanY,stdY] = averageCurves(x,y,overlapOnly,resolutionFactor)
%% Averages several curves
% Vadim Bulitko
% May 8, 2022

arguments
    x (:,1) cell
    y (:,1) cell
    overlapOnly (1,1) logical = false
    resolutionFactor (1,1) double = 1
end

%% Preliminaries
numCurves = length(x);
assert(length(y) == numCurves,'x and y must have the same number of curves');

% Compute the global min and max of the x coordinate
minX = Inf;
maxX = -Inf;

for cI = 1:numCurves
    minX = min(minX,min(x{cI}));
    maxX = max(maxX,max(x{cI}));
end

%% Find the ranges and create data structures
assert(minX > 0,'only positive x-values are supported');

scaleX = 10^(floor(log10(minX))-resolutionFactor);
minX = round(minX/scaleX);
maxX = round(maxX/scaleX);
numX = maxX - minX + 1;

dataMatrix = NaN(numCurves,numX);

%% Put the data into the matrix
for cI = 1:numCurves
    dataMatrix(cI,:) = placeData(round(x{cI}/scaleX),y{cI},minX,maxX);
end


%% Create X range
xRange = scaleX*(minX:maxX);

%% Remove columns which do not have enough y values in them
if (overlapOnly)
    minDataPointsPerColumn = numCurves;
else
    minDataPointsPerColumn = 1;
end

haveEnoughDataMoves = sum(~isnan(dataMatrix),1) >= minDataPointsPerColumn;
dataMatrix = dataMatrix(:,haveEnoughDataMoves);
xRange = xRange(haveEnoughDataMoves);

%% Return the averages and variances
meanY = mean(dataMatrix,1,'omitnan');
stdY = std(dataMatrix,0,1,'omitnan');

end


function pd = placeData(x,y,minX,maxX)
%% A helper function

arguments
    x (1,:) double
    y (1,:) double
    minX (1,1) double
    maxX (1,1) double
end

numX = maxX - minX + 1;
pd = NaN(1,numX);

x = x-minX+1;

for i = 1:length(x)
    % fill in the previous y value
    if (i > 1)
        pd(x(i-1)+1:x(i)-1) = y(i-1);
    end

    % put in this value
    pd(x(i)) = y(i);
end

% fill to the end
%pd(x(end)+1:numX) = y(end);

end


