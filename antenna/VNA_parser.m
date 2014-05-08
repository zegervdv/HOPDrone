%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Author: Wouter Dullaert
% Created: 6 dec 2007
% Last Modified: 11 may 2010
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Description: Script to parse the output of the VNA 
% into Matlab vectors
%  
% Input
%  filename: path to the citifile that needs to be parsed
% 
% Output
%  Freq_list: the frequencies at which the results are sampled in GHz
%  S11,S21,S12,S22: the S-parameters as complex numbers
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [Freq_list,S11,S21,S12,S22] = VNA_parser(filename)

% Do a rough check of the validity of the input
if nargin ~= 1
  error('MeasurementToolbox:IncorrectInputError','Incorrect input arguments.\nUsage: VNA_parser(filename)\nOutput: [Freq_list,S11,S21,S12,S22]');
end

% Load the file into memory for quick access
m = memmapfile(filename);

% Get the data as one long string
datastring = char(m.Data.');

% Find out which lines contain the descriptors of the data included
data_list = strfind(datastring,'DATA');

% Find out which lines contain the frequency range and samplepoints
seg_list = strfind(datastring,'VAR_LIST_BEGIN');
seg_list_end = strfind(datastring,'VAR_LIST_END');

% Find out where the datalists start and end
start_list = strfind(datastring,'BEGIN');
end_list = strfind(datastring,'END');

% Find out which format is used in the cti filename (we assume all fields use the same format)
temp = textscan(datastring(data_list(2):data_list(3)-1),'%s');
format = reshape([temp{:}],[],3);
format = format{3};

% Read the frequency list
temp = textscan(datastring(seg_list:seg_list_end-1),'%n','Headerlines',1);
Freq_list = 10^-9*temp{1};
num_freq = length(Freq_list);

% Initialize the returnvectors
S11 = zeros(num_freq,2);
S21 = zeros(num_freq,2);
S12 = zeros(num_freq,2);
S22 = zeros(num_freq,2);

% Read the first data list
% NOTE: I might make the assignment dynamic in the future (this would then become a loop)
temp = textscan(datastring(start_list(2):end_list(2)-1),'%f,%f','Headerlines',1);
S11 = reshape([temp{:}],[],2);

% Read the second data list
temp = textscan(datastring(start_list(3):end_list(3)),'%f,%f','Headerlines',1);
S21 = reshape([temp{:}],[],2);

% Read the third data list
temp = textscan(datastring(start_list(4):end_list(4)),'%f,%f','Headerlines',1);
S12 = reshape([temp{:}],[],2);

% Read the fourth data list
temp = textscan(datastring(start_list(5):end_list(5)),'%f,%f','Headerlines',1);
S22 = reshape([temp{:}],[],2);

% Convert the output to a complex number
switch format
  case 'RI'
    S11 = S11(:,1)+j*S11(:,2);
    S21 = S21(:,1)+j*S21(:,2);
    S12 = S12(:,1)+j*S12(:,2);
    S22 = S22(:,1)+j*S22(:,2);
  case 'DBANGLE'
    S11 = 10.^(S11(:,1)/20).*exp(j*S11(:,2)*pi/180);
    S21 = 10.^(S21(:,1)/20).*exp(j*S21(:,2)*pi/180);
    S12 = 10.^(S12(:,1)/20).*exp(j*S12(:,2)*pi/180);
    S22 = 10.^(S22(:,1)/20).*exp(j*S22(:,2)*pi/180);
end