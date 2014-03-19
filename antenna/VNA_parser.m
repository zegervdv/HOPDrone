%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                 %
%               Script to parse the calibration output            %
%                   of the VNA into Matlab vectors                %
%                                                                 %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Author: Wouter Dullaert
% Created: 6 dec 2007
% Last Modified: 6 dec 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [Freq_list,S11,S21,S12,S22] = VNA_parser(filename)

% Load the file into memory for quick access
m = memmapfile(filename);

% Get the data as one long string
datastring = char(m.Data.');

% Find out which lines contain the descriptors of the data included
data_list = strfind(datastring,'DATA');
% Find out which lines contain the frequency range and samplepoints
seg_list = [strfind(datastring,'SEG'),strfind(datastring, 'VAR_LIST_BEGIN')];

% Find out where the datalists start and end
start_list = strfind(datastring,'BEGIN');
end_list = strfind(datastring,'END');

% Read the line containing the frequency range and samplepoints
temp = textscan(datastring(seg_list(1):seg_list(2)-1),'SEG %n %n %n');
start_freq = temp{1};
end_freq = temp{2};
num_freq = temp{3};


%LV: frequency list in GHz
Freq_list=10^-9*[start_freq:(end_freq-start_freq)/(num_freq -1):end_freq].';

% Initialize the returnvectors
S11 = zeros(num_freq,2);
S21 = zeros(num_freq,2);
S12 = zeros(num_freq,2);
S22 = zeros(num_freq,2);

% Read the first data list
% NOTE: I might make the assignment dynamic in the future (this would then become a loop
temp = textscan(datastring(start_list(2):end_list(2)-1),'%f,%f','Headerlines',1);
S11 = reshape([temp{:}],[],2);
S11 = S11(:,1)+j*S11(:,2);

% Read the second data list
temp = textscan(datastring(start_list(3):end_list(3)),'%f,%f','Headerlines',1);
S21 = reshape([temp{:}],[],2);
S21 = S21(:,1)+j*S21(:,2);

% Read the third data list
temp = textscan(datastring(start_list(4):end_list(4)),'%f,%f','Headerlines',1);
S12 = reshape([temp{:}],[],2);
S12 = S12(:,1)+j*S12(:,2);

% Read the fourth data list
temp = textscan(datastring(start_list(5):end_list(5)),'%f,%f','Headerlines',1);
S22 = reshape([temp{:}],[],2);
S22 = S22(:,1)+j*S22(:,2);