function v = ensureColumnVector(v)
%% Ensures that we have a column vector
% Nov 4, 2020
% Vadim Bulitko

h = size(v,1);
w = size(v,2);

if (h == 1 && w > 1)
    % we have a row, transpose it
    v = v';
end

end
