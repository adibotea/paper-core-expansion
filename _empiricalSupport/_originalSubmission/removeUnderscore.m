function newStr = removeUnderscore(strA)
% removeUnderscore
% replaces all underscores with '\_'

if (isempty(strA))
    newStr = '';
    return
end


for i = 1:size(strA,1)
    str = strA(i,:);
    
    up = strfind(str,'_');
    if (isempty(up))
        sc{i} = str; %#ok<AGROW>
    else
        sc{i} = [str(1:up-1) '\_' removeUnderscore(str(up+1:end))]; %#ok<AGROW>
    end
end

% convert to char array
newStr = char(sc);

end
