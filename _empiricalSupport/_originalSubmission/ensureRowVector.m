function v = ensureRowVector(v)
%% Ensures that we have a row vector
% Jan 23, 2022
% Vadim Bulitko

h = size(v,1);
w = size(v,2);

if (w == 1 && h > 1)
    % we have a column, transpose it
    v = v';
end

end
