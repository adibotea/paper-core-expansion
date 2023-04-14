function str = sec2str(numSeconds)
%% Converts a number of seconds into a human-readable string
% Vadim Bulitko
% Jan 29, 2022

arguments 
    numSeconds (1,1) double
end

%% Preliminaries
secMinute = 60;
secHour = 60*secMinute;
secDay = 24*secHour;
secWeek = 7*secDay;
secMonth = (365/12)*secDay;

%% Convert
if (numSeconds < secMinute)
    str = sprintf('%0.1f s',numSeconds);
elseif (numSeconds < secHour)
    str = sprintf('%0.1f m',numSeconds/secMinute);
elseif (numSeconds < secDay)
    str = sprintf('%0.1f h',numSeconds/secHour);
elseif (numSeconds < secWeek)
    str = sprintf('%0.1f d',numSeconds/secDay);
elseif (numSeconds < secMonth)
    str = sprintf('%0.1f w',numSeconds/secWeek);
else
    str = sprintf('%0.1f mo',numSeconds/secMonth);
end

return