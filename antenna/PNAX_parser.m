% ========================================================================
%
% This function reads a .cti file (real,imag) and returns the data
% - freq in GHz
% - S11,S21,S12,S22 as complex number
%
%
% Freek Boeykens
% Date: 05/05/2010
% Last modified: 06/05/2010
% ========================================================================

function [freq,S11,S21,S12,S22] = PNAX_parser(inputFile)

input = fopen(inputFile);

line = fgets(input);

% ------------------------------------------------------------------------
% read number of points
% ------------------------------------------------------------------------
while line~=-1
    
    % read line
    line = fgets(input);
    
    % find the number of points
    check = strfind(line,'VAR Freq MAG');
    
    % stop this loop
    if check
        numPoints = str2double(line(14:end));
        break
    end
end

% ------------------------------------------------------------------------
% init
% ------------------------------------------------------------------------
freq = zeros(numPoints,1);
S11  = zeros(numPoints,1);
S21  = zeros(numPoints,1);
S12  = zeros(numPoints,1);
S22  = zeros(numPoints,1);

while line~=-1
   
    % read line
    line = fgets(input);
    
    % find start of frequency list
    check = strfind(line,'VAR_LIST_BEGIN');
    
    % stop this loop
    if check
        break
    end
    
end

% ------------------------------------------------------------------------
% read data
% ------------------------------------------------------------------------
% read frequency list
for i=1:numPoints
    line = fgets(input);
    freq(i) = 10^(-9)*str2double(line);
end



% read 2 dummy lines
fgets(input);
fgets(input);

% read S11
for i=1:numPoints
    line   = fgets(input);
    data   = str2num(line);
    S11(i) = data(1) + j*data(2);
end

% read 2 dummy lines
fgets(input);
fgets(input);

% read S21
for i=1:numPoints
    line   = fgets(input);
    data   = str2num(line);
    S21(i) = data(1) + j*data(2);
end

% read 2 dummy lines
fgets(input);
fgets(input);

% read S12
for i=1:numPoints
    line   = fgets(input);
    data   = str2num(line);
    S12(i) = data(1) + j*data(2);
end

% read 2 dummy lines
fgets(input);
fgets(input);

% read S22
for i=1:numPoints
    line   = fgets(input);
    data   = str2num(line);
    S22(i) = data(1) + j*data(2);
end

return