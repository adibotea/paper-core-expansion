function c = puzzleBin2Char(p)
%% Converts a binary BCC to char format
% Vadim Bulitko
% Apr 21, 2021

arguments
    p (:,:) logical
end

c = char(zeros(size(p)));
c(p) = '#';
c(~p) = '.';

end
