function [champion, progress] = mrmeRun(seedI,mcscore,seedG,evolution,wombat,features)
%% A single MRME run from a single seed
% Vadim Bulitko
% Feb 3, 2023

arguments
    seedI (1,1) double
    mcscore (1,1) double
    seedG (1,1) struct
    evolution (1,1) struct
    wombat (1,1) struct
    features (1,1) struct
end

%% Preliminaries
tttE = tic;
gridSize = size(seedG.g,1);

%% Initial population
% set up the seed grid
g(1).g = seedG.g;
g(1).cg = seedG.cg;
g(1).fn = seedG.fn;
g(1).score = extractScoreAdiSeedFN(seedG.fn);
g(1).hm = [];
g(1).p = seedG.p;
[g(1).f1, g(1).f2] = extractFeatures(g(1),features);

%% Create initial population and place it on the map ==================================================================================
for i = 1:features.numMaps
    [maps{i}, features] = createMap(features,i); %#ok<*AGROW>
end

% put the seed on all maps
for i = 1:features.numMaps
    [map, features] = getMap(maps,features,i);
    map = updateMap(map,g,features);
    maps{i} = map;
end

%% Evolve =============================================================================================================================
champion.score = NaN;
genStalled = 0;
genI = 0;
totalScoreComputations = 0;

while (true)
    genStalled = genStalled+1;

    % go through all maps to update the overall champion -------------------------------------------------------------------------
    for resolutionI = 1:features.numMaps
        % get the right map
        [map, features] = getMap(maps,features,resolutionI);

        % get the map's elite with the highest score
        [maxScore, maxScoreI] = max([map.score]);

        % update the champion if necessary
        if (isnan(champion.score) || maxScore > champion.score)
            champion = map(maxScoreI);
            champion.wombat = wombat;
            champion.mcscore = mcscore;

            fprintf('seed %2d | gen %4d (%s grids scored in %s) | maps{%d} | new champion: %s %d, %s %d, score %d\n',...
                seedI,genI,hrNumber(totalScoreComputations),sec2str(toc(tttE)),resolutionI,...
                features.f1.name,champion.f1,features.f2.name,champion.f2,...
                champion.score);

            genStalled = 0;
        end
    end

    % log population stats ----------------------------------------------------------------------------------------------------------
    allMapScore = [];
    for resolutionI = 1:features.numMaps
        % get the right map
        [map, features] = getMap(maps,features,resolutionI);

        % means
        progress.f1.mapMean(resolutionI,genI+1) = mean([map.f1],'all','omitnan');
        progress.f2.mapMean(resolutionI,genI+1) = mean([map.f2],'all','omitnan');
        progress.score.mapMean(resolutionI,genI+1) = mean([map.score],'all','omitnan');

        allMapScore = [allMapScore reshape([map.score],1,numel([map.score]))];

        % map stats
        progress.map.elites(resolutionI,genI+1) = nnz(~isnan([map.score]));
        progress.map.filled(resolutionI,genI+1) = progress.map.elites(resolutionI,genI+1)/numel(map);
    end

    progress.score.popmean(genI+1) = mean(allMapScore,'omitnan');

    % log champion stats
    progress.f1.champion(genI+1) = champion.f1;
    progress.f2.champion(genI+1) = champion.f2;
    progress.score.champion(genI+1) = champion.score;

    progress.scoreComputations(genI+1) = totalScoreComputations;
    progress.time(genI+1) = toc(tttE);

    % are we out of time?
    if (toc(tttE) >= evolution.maxTime)
        break
    end

    % make a list of random elites from all maps ------------------------------------------------------
    clear('g');
    cEliteI = 0;

    for resolutionI = 1:features.numMaps
        % get the right map
        [map, features] = getMap(maps,features,resolutionI);

        % select some elite indecies
        eliteI = find(~isnan([map.score]));
        mapI = eliteI(randi(length(eliteI),1,evolution.batchSize));

        % get the elite graphs and their protection masks for those indecies
        for i = 1:length(mapI)
            cEliteI = cEliteI + 1;
            g(cEliteI).g =  map(mapI(i)).g;
            g(cEliteI).cg =  map(mapI(i)).cg;
            g(cEliteI).p =  map(mapI(i)).p;
            g(cEliteI).fn =  map(mapI(i)).fn;
        end
    end

    % mutate the combined elites --------------------------------------------------------------------
    evolution.mu = max(1,1+evolution.muScaler*genStalled);
    offspring = false(gridSize,gridSize,length(g));

    clear('offspringCparent','offspringC','offspringPMparent','offspringFN');

    for i = 1:length(g)
        % create an offspring
        offspring(:,:,i) = mutateGrid_mex(g(i).g,g(i).p,evolution.mu,evolution.numViolations,...
            evolution.enforceMaxBC,evolution.ensureValidOffspring); 

        % we inherit the character grid from the parent and extend it with possible new walls
        offspringCparent(:,:,i) = g(i).cg;
        offspringC(:,:,i) = putInWallsFromGrid(offspringCparent(:,:,i),offspring(:,:,i));

        offspringPMparent(:,:,i) = g(i).p;
        offspringFN{i} = g(i).fn;
    end

    % prune duplicates from the offspring list ----------------------------------------------------------
    uniqueOffspring = true(1,size(offspring,3));

    for i = 2:size(offspring,3)
        % go through all earlier offspring
        for j = 1:i-1
            if (isequal(offspring(:,:,i),offspring(:,:,j)) && isequal(offspringC(:,:,i),offspringC(:,:,j)))
                % the binary grids of the two offspring are the same
                % also check their character grids
                % the character grids also match
                uniqueOffspring(i) = false;
                break
            end
        end
    end

    offspring = offspring(:,:,uniqueOffspring);
    offspringC = offspringC(:,:,uniqueOffspring);
    offspringCparent = offspringCparent(:,:,uniqueOffspring);
    offspringPMparent = offspringPMparent(:,:,uniqueOffspring);
    offspringFN = offspringFN(uniqueOffspring);

    % score offspring --------------------------------------------------------------------------------
    wombat.timeLimit = max(wombat.timeLimitMin, min(wombat.timeLimitMax, wombat.timeLimitMin+genStalled*wombat.timeLimitScaler));
    wombat.weight = max(wombat.weightMin, min(wombat.weightMax, wombat.weightMin+sqrt(genStalled)*wombat.weightScaler));

    [offspringScores, ~, offspringHM] = wombatScore(offspring,wombat.targetScore,wombat.timeLimit,wombat.jobID,wombat.weight,offspringC,false);

    totalScoreComputations = totalScoreComputations + nnz(offspringScores ~= -1);

    % compute their features and package them into a structure array -----------------------------------
    clear('g');
    for i = 1:length(offspringScores)
        g(i).g = offspring(:,:,i);

        % we are working with Adi's seeds. Keep inheriting the original character grids and protection masks
        g(i).cg = offspringCparent(:,:,i);
        g(i).p = offspringPMparent(:,:,i);

        g(i).score = offspringScores(i);
        g(i).hm = offspringHM(:,:,i);
        g(i).fn = offspringFN{i};

        [g(i).f1, g(i).f2] = extractFeatures(g(i),features);
    end

    % update all maps with the elites' children -----------------------------------------------------------------
    for resolutionI = 1:features.numMaps
        % get the right map
        [map, features] = getMap(maps,features,resolutionI);

        % update it
        [maps{resolutionI}, ~] = updateMap(map,g,features);
    end

    % next generation
    genI = genI + 1;
end

end

%%%%%%%%%%%%%%%%%%%%%%%%%%% Aux functions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [f1I,f2I] = mapCoordinates(g,features)
%% Computes map coordinates given a grid
% Jan 3, 2023

arguments
    g (1,1) struct
    features (1,1) struct
end

[~,f1I] = min(abs(features.f1.range - g.f1));
[~,f2I] = min(abs(features.f2.range - g.f2));

end


function features = putInFeatureValueRanges(features,resolutionI)
%% Given a resolution index put in feature value ranges
% Jan 3, 2023

arguments
    features (1,1) struct
    resolutionI (1,1) double
end

%% Compute the feature value ranges
features.f1.range = linspace(features.f1.min,features.f1.max,features.f1.resolutionRange(resolutionI));
features.f2.range = linspace(features.f2.min,features.f2.max,features.f2.resolutionRange(resolutionI));

end


function [map, features] = createMap(features,resolutionI)
%% Pre-allocates a map
% Jan 3, 2023

arguments
    features (1,1) struct
    resolutionI (1,1) double
end

%% Compute the feature value ranges
features = putInFeatureValueRanges(features,resolutionI);

%% Create the map
for f1I = 1:length(features.f1.range)
    for f2I = 1:length(features.f2.range)
        map(f1I,f2I).score = NaN;
    end
end

end

function [map, numUpdatedElites] = updateMap(map,g,features)
%% Updates map from an array of grids
% Jan 3, 2023

arguments
    map (:,:,:,:) struct
    g (1,:) struct
    features (1,1) struct
end

% random permutation for tie breaking
%g = g(randperm(length(g)));

numUpdatedElites = 0;

for gI = 1:length(g)
    % get the coordinates
    [f1I,f2I] = mapCoordinates(g(gI),features);

    % is this the new elite?
    if (isnan(map(f1I,f2I).score) || g(gI).score > map(f1I,f2I).score)
        % update the elite
        map(f1I,f2I).g = g(gI).g;
        map(f1I,f2I).cg = g(gI).cg;
        map(f1I,f2I).hm = g(gI).hm;
        map(f1I,f2I).p = g(gI).p;
        map(f1I,f2I).fn = g(gI).fn;
        map(f1I,f2I).score = g(gI).score;
        map(f1I,f2I).f1 = g(gI).f1;
        map(f1I,f2I).f2 = g(gI).f2;

        % count updates
        numUpdatedElites = numUpdatedElites + 1;
    end
end

end

function [map, features] = getMap(maps,features,resolutionI)
%% Extracts a map and prepares its features
% Jan 3, 2023

arguments
    maps (1,:) cell
    features (1,1) struct
    resolutionI (1,1) double
end

map = maps{resolutionI};
features = putInFeatureValueRanges(features,resolutionI);

end

function cg = putInWallsFromGrid(cg,g)
%% Adds walls from a binary grid into a character grid
% Jan 18, 2023

arguments
    cg (:,:) char
    g (:,:) logical
end

cg(g) = '#';

end



function [f1, f2] = extractFeatures(g,features)
%% Extracts features
% Jan 19, 2023

arguments
    g (1,1) struct
    features (1,1) struct %#ok<INUSA>
end

% maximum word-slot length
f1 = max(double(wordLengthDistribution_mex(g.g,13)));

% number of walls
f2 = double(numWalls(g.g));

end
