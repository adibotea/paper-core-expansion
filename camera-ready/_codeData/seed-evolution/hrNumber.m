function str = hrNumber(i)
%% Converts an integer into a better human-readable string

if (i < 10^3)
    str = sprintf('%0.0f',i);
elseif (i < 10^6)
    str = sprintf('%0.1fK',i/10^3);
elseif (i < 10^9)
    str = sprintf('%0.1fM',i/10^6);
else
    str = sprintf('%0.1fB',i/10^9);
end

return
