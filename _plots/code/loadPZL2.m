function g = loadPZL2(gFN)
%% Loads the other PZL format given the full file name
% Vadim Bulitko
% Apr 20, 2021

arguments
    gFN (1,:) char
end

g = [];

%% open file
fid = fopen(gFN,'r');

%% load if the file exists
if (fid ~= -1)
    %  read the puzzle size
    opts = detectImportOptions(gFN,'FileType','text');
    opts.DataLines = [1 1];
    tmp = readcell(gFN,opts);
    tmp2 = tmp(:,1);
    gridSize = tmp2{1};

    % read the right part of the file
    opts = detectImportOptions(gFN,'FileType','text');
    opts.DataLines = [6 6+gridSize-1];
    opts.Whitespace = '';
    opts.EmptyLineRule = 'read';
    opts.Delimiter = {};
    tmp = readcell(gFN,opts);
    tmp2 = tmp(:,1);

    % convert it to a matrix
    numLines = size(tmp2,1);
    g = char(zeros(numLines,numLines));

    for i = 1:numLines
        tmp3 = cell2mat(tmp2(i));
        g(i,:) = tmp3(1:2:numLines*2);
    end

    % replace walls and empty spaces
    g(g == ' ') = '.';
    g(g == '@') = '#';

    fclose(fid);
end

end


% 13
% 13
% 1
% 0
% 0
%           @ M I O C A R D .
%         @ K A N G O R O O .
%       @     G E O   @     .
%   @       @   L @     @   .
%     @         @           .
% @           @             .
%           @       @     @ .
%         @       @         .
%       @               @   .
%     @               @     .
%   @             @         .
%         @     @           .
%             @             .
% 2
% 1
% them-dic-19.txt
% 1
% 1
% 0
% 6-dictionary.txt
% 1
% 1