function savePZL2(gFN,p,genericDictFN)
%% Saves a puzzle into pzl format #2
% Vadim Bulitko
% Apr 20, 2021

arguments
    gFN (1,:) char
    p (:,:) char
    genericDictFN (1,:) char = 'dictionary.txt';
end

%% Preliminaries
height = size(p,1);
width = size(p,1);

%% open file
fid = fopen(gFN,'w');

%% replace walls and empty spaces
p(p == '.') = ' ';
p(p == '#') = '@';

%% output the header
fprintf(fid,'%d\n%d\n',height,width);
fprintf(fid,'1\n0\n0\n');

%% output the puzzle itself
for y = 1:height
    for x = 1:width
        fprintf(fid,'%c ',p(y,x));
    end
    fprintf(fid,'\n');
end

%% output the footer
fprintf(fid,'2\n');
fprintf(fid,'1\n');
fprintf(fid,'them-dic-23.txt\n');
fprintf(fid,'1\n');
fprintf(fid,'1\n');
fprintf(fid,'0\n');
fprintf(fid,'%s\n',genericDictFN);
fprintf(fid,'1\n');
fprintf(fid,'1\n');

%% close the file
fclose(fid);
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
