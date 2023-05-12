function score = extractScoreAdiSeedFN(fn)
%% Extracts score from a file name for Adi's seed grid
% Jan 18, 2023

arguments
    fn (1,:) char
end

% prepare the file name
[~,fn,~] = fileparts(fn);

% extract 'puzzle-%d'
pat = 'puzzle-' + digitsPattern;
sc = extract(fn,pat);

% get the number
score = sscanf(sc{1},'puzzle-%d');

assert(~isempty(score));

end

% 
% 
% 
% % original file name format
% score = sscanf(fn,'legal-puzzle-%d');
% 
% % alternative file name formats
% if (isempty(score))
%     score = sscanf(fn,'alive-0-puzzle-%d');
% end
% 
% if (isempty(score))
%     score = sscanf(fn,'puzzle-%d');
% end
% 
% if (isempty(score))
%     score = sscanf(fn,'alive-0-legal-puzzle-%d');
% end
% 
% 
% end
