function r = randr(range,height,width)
%% Generates a matrix of matrixSize of uniformly distributed random numbers (using rand) in the specified range

% Default parameters
switch (nargin)
    case 1
        matrixSize = [1 1];
    case 2
        matrixSize = height;
    case 3
        matrixSize = [height width];
end

% Generate and scale the output
r = range(1) + (range(2)-range(1)) .* rand(matrixSize);
end
