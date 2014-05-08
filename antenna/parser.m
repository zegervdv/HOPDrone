%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Author: Wouter Dullaert
% Created: 27 nov 2007
% Last Modified: 11 may 2010
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Description: Parses the output from the MIDAS system into Matlab
% matrices
%
% The script is constructed in such a way that the reference 
% scenario requires no additional input option. The further you
% move away from the reference scenario, the more additional
% options you will need to give
%
% It is set up in such a way that it will exit, or at least 
% generate a warning if your options are malformed
%
% The names used in this script are those of the MIDAS position
% system. This keeps it as general as possible (The script
% cannot possibly know which axes the user will have)
%  
% Input
%  varargin: see the dedicated parser page on gitorious
% 
% Output
%  data: The F vector as a complex number
%  azimuth: The azimuth angles at which the data is sampled in degrees
%  roll: The roll angles at which the data is sampled in degrees
%  frequency: The frequency points at which the data is sampled in GHz
%  polarization: The polarization angles at which the data is sampled in degrees
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [data,azimuth,roll,frequency, polarization] = parser(varargin);

% Check if a filename is supplied
if size(varargin,2)<1
    error('Please input a filename!');
end

% Read in the filename
filename = varargin{1};
% Initialize possible options
selected_frequency = NaN;
selected_polarization = NaN;
selected_roll = NaN;

% Check if the options are specified in a valid way
if mod(size(varargin,2),2)==0
    error('Error:InvalidOption', 'Invalid option!\n usage: parser(filename,option_name,option_value,...)');
% Run over the list of option and parse them
elseif size(varargin,2)>=3
    for k = 2:2:size(varargin,2)-1
        test = varargin{k};
        % Set the selected frequency
        if strcmp(test,'Frequency')
            if ~isnan(selected_frequency)
                % If the frequency has been set already
                % generate a warning and return (malformed options)
                error('You can only specify a frequency once');
            end
            selected_frequency = varargin{k+1};
        % Set the selected polarization
        elseif strcmp(test,'Polarization')
            if ~isnan(selected_polarization)
                % If the polarization has been set already
                % generate a warning and return (malformed options)
                error('You can only specify a polarization once');
            end
            selected_polarization = varargin{k+1};
        % Set the selected roll
        elseif strcmp(test,'Roll')
            if ~isnan(selected_roll)
                % If the roll has been set already
                % generate a warning and return (malformed options)
                error('You can only specify the role once');
            end
            selected_roll = varargin{k+1};
        else
            % An invalid option string was found
            error('Option should be Frequency, Polarization or Roll');
        end
    end
end


% Load the data into memory
m = memmapfile(filename);

% Convert the data to one long string
datastring = char(m.Data.');

% Find the start positions of each new section
start_positions = strfind(datastring,'File Name');

% If there are no valid sections: return gracefully
if length(start_positions)==0
    error('No valid output data found');
end

% Initialize all the vectors %

% Data in the table of a section
% Calculate the length of a table of data
start = strfind(datastring,'Azimuth');
start2 = strfind(datastring,'Roll');

% See if the list iterates over azimuth or roll and set flag accordingly
flag = 0;
if(start2(1)>start(1))
    flag = 1;
    start = start2;
end
eind = start(1)+strfind(datastring(start(1):start(2)),'		');
lengte = length(strfind(datastring(start(1):eind(1)-1),char([13 10])))-1;
% Check if a valid table length was aquired
if lengte<1
    % No EOL's found, try with different EOL encoding
    %display('No windows End of Line chars found, trying UNIX EOL...');
    lengte = length(strfind(datastring(start(1):eind(1)-1),char(10)))-1;
    % Check if a valid table length was aquired
    if lengte>1
        %display('Success (Hopefully)');
    else
        % Couldn't determine the length of the table, useless to continue
        display('No EOL found');
        error('Could not determine length of table');
    end
end

% Data in header of a section

% Check for a statistics table (which also contains the string 'filename'
% and influences the number of lists found
if isempty(strfind(datastring,'[Statistics Table]'))
    start_positions = [start_positions length(datastring)];
end
number_of_lists = length(start_positions)-1;

frequency = zeros(1,number_of_lists);
polarization = zeros(1,number_of_lists);
if flag
    roll = zeros(length(start_positions),lengte,3);
    azimuth = zeros(1,number_of_lists);
else
    azimuth = zeros(length(start_positions),lengte,3);
    roll = zeros(1,number_of_lists);
end

t=0;
% Run over all the tables (the last startposition is a statistics table)
for k=1:number_of_lists
    % Store the data for this section in a seperate variable
    datastring_temp = datastring(start_positions(k):start_positions(k+1)-1);

    % Find the start position of header variables and data table
    polarization_index = strfind(datastring_temp,'Polarizat.');
    frequency_index = strfind(datastring_temp,'Frequency');
    if flag
        azimuth_index = strfind(datastring_temp,'Azimuth');
        table_start_index = strfind(datastring_temp,'Roll  [deg]');
    else
        roll_index = strfind(datastring_temp,'Roll');
        table_start_index = strfind(datastring_temp,'Azimuth  [deg]');
    end
    table_end_index = table_start_index(1)+strfind(datastring_temp(table_start_index(1):end),'		')-2;

    % Extract the polarization value for this section
    temp = textscan(datastring_temp(polarization_index:end),'Polarizat. %f %*s\n',1,'Delimiter','\t');
    polarization(k) = temp{1};

    % Extract the frequency value for this section
    temp = textscan(datastring_temp(frequency_index:end),'Frequency %f %s\n',1,'Delimiter','\t');
    frequency(k) = temp{1};
    % Get the unit in which frequency is expressed
    testtemp = temp{2}{:};
    % If in MHz, convert to GHz
    if(testtemp(1)=='M') frequency(k) = frequency(k)/1e3; end

    if flag
        % Extract the roll value for this section
        temp = textscan(datastring_temp(azimuth_index:end),'%*s %f %*s\n',1,'Delimiter','\t');
        azimuth(k) = temp{1};
    else
        % Extract the roll value for this section
        temp = textscan(datastring_temp(roll_index:end),'%*s %f %*s\n',1,'Delimiter','\t');
        roll(k) = temp{1};
    end

    % Read the table for this section
    N = (table_end_index(1)-table_start_index(1))/3-2;
    temp = textscan(datastring_temp(table_start_index(1):table_end_index(1)),'%f %f %f','Delimiter','\t','HeaderLines',1);
    if flag
        roll(k,:,:) = reshape([temp{:}],[],3);
    else
        azimuth(k,:,:) = reshape([temp{:}],[],3);
    end
end

% Initialize the indexes to be returned
if isnan(selected_polarization)
    k=1:length(polarization);
% Select the specified polarizations
else
    k = find(polarization==selected_polarization);
    if isempty(k)
        % Generate a warning if the polarization was not found
        display('Warning: Polarization not found!');
    end
end

% Select the specified frequencies
if ~isnan(selected_frequency)
    l = find(frequency==selected_frequency);
    if isempty(l)
        % Generate a warning if the frequency was not found
        display('Warning: Frequency not found!');
    end
    k = intersect(l,k);
end

% Select the specified roll
if ~isnan(selected_roll)
    l = find(roll==selected_roll);
    if isempty(l)
        % Generate a warning if the roll was not found
        display('Warning: Roll not found!');
    end
    k = intersect(l,k);
end

% Convert the data from amplitude/phase to an imaginary number
if flag
    data = 10.^(roll(k,:,2)/20).*exp(j*roll(k,:,3)/180*pi);
else
    data = 10.^(azimuth(k,:,2)/20).*exp(j*azimuth(k,:,3)/180*pi);
end
% Set the list of frequencies
frequency = frequency(k);
% Set the list of polarizations
polarization = polarization(k);

if flag
    % Set the list of azimuth values
    azimuth = azimuth(k);
else
    % Set the list of roll values
    roll = roll(k);
end

if flag
    % Set the list of roll values
    roll = roll(1,:,1).';

    % Make sure the vertical dimension is the longest
    if size(roll,1)<size(roll,2)
        roll = roll.';
    end
else
    % Set the list of azimuth values
    azimuth = azimuth(1,:,1);

    % Make sure the vertical dimension is the longest
    if size(azimuth,1)<size(azimuth,2)
        azimuth = azimuth.';
    end
end