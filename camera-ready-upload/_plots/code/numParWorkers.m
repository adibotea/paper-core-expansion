function n = numParWorkers
%% Returns the number of parallel workers to use 
% It is the maximum of the logical cores and the physical cores available to MATLAB
% Vadim Bulitko
% Mar 3, 2022

%n = feature('numcores');

s = evalc('feature(''numcores'');');
p = 'MATLAB was assigned: ';
i = strfind(s,p) + length(p);

n = sscanf(s(i(1):end),'%d');

end