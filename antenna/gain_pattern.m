%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Author: ir. Wouter Dullaert
% Created: 4 feb 2009
% Last Modified: 14 feb 2012 By Sam
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Description: Calculates the powergain, rhcp/lhcp, horizontal/vertical
% gain and efficiency of an antenna measurement performed by the Midas
% system. Regardless of the amount of angles or frequency points.
%
% Input
%  meas_file: the file name of the MIDAS output file
%  vna_file: the file name of the VNA citifile
%  d: the distance between the 2 antennas in meter
%
% Output
%  results.gain = the antenna gain in dB;
%  results.gain_right = the antenna right handed polarization gain in dB;
%  results.gain_left = the antenna left handed polarization gain in dB;
%  results.gain_hor = the antenna horizontal polarization gain in dB;
%  results.gain_ver = the antenna vertical polarization gain in dB;
%  results.efficiency = the antenna efficiency
%  results.azimuth = the azimuth angles at which the pattern in sampled in degrees
%  results.roll = the roll angles at which the pattern in sampled in degrees
%  results.frequency = the frequency points at which the pattern in sampled in GHz
%  results.phase = the antenna phase in degrees
% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function results = gain_pattern(meas_file,vna_file,vna_file_90deg,d)

% Do a rough check of the validity of the input
if nargin ~= 4
  error('MeasurementToolbox:IncorrectInputError','Incorrect input arguments.\nUsage: gain_pattern(meas_file,vna_file,d)');
end

% Load calibration data
[Freq_list,S11,S21,S12,S22]= VNA_parser(vna_file);
[Freq_list_90deg,S11_90deg,S21_90deg,S12_90deg,S22_90deg]= VNA_parser(vna_file_90deg);

% Load the measurement data
[S21_0pol] = parser(meas_file,'Polarization',0);
[S21_90pol,azimuth,roll,frequency] = parser(meas_file,'Polarization',90);
S21_0pol_dB = 20*log10(abs(S21_0pol));
S21_90pol_dB = 20*log10(abs(S21_90pol));

% Convert the calibration data to dB and phase
S11_dB = 20*log10(abs(S11));
S21_dB = 20*log10(abs(S21));
S12_dB = 20*log10(abs(S12));
S22_dB = 20*log10(abs(S22));
S21_90deg_dB = 20*log10(abs(S21_90deg));

% Get all the measured frequency points
frequency_unique = unique(frequency);

% Lightspeed
c=2.99792458*10^8;

% Interpolate the calibration data to the measured frequencies
S11_interp_dB = interp1(Freq_list,S11_dB,frequency_unique);
S21_interp_dB = interp1(Freq_list,S21_dB,frequency_unique);
S12_interp_dB = interp1(Freq_list,S12_dB,frequency_unique);
S22_interp_dB = interp1(Freq_list,S22_dB,frequency_unique);
S21_90deg_interp_dB = interp1(Freq_list,S21_90deg_dB,frequency_unique);

% Initialise the result vectors
PL = zeros(size(S21_interp_dB));
Mr_dB = PL;
Mt_dB = PL;
gain_horn = PL;

% Loop to calculate the correction at each frequency
for k=1:length(frequency_unique)
    % Current frequency
    f=frequency_unique(k);
    % Current wavelength
    lambda=c/(f*10^9);

    % Pathloss at current frequency
    PL(k) = 20*log10((4*pi*d)/lambda);

    % Mismatch at AUT (receiving end)
    S22 = 10^(S22_interp_dB(k)/20);
    Mr_dB(k) = 10*log10(1-(abs(S22))^2);


    % Mismatch at horn (sending end)
    S11 = 10^(S11_interp_dB(k)/20);
    Mt_dB(k) = 10*log10(1-(abs(S11))^2); 
    
    % Horn gain
    gain_horn(k) = get_gain_horn_data(f);
end

% Correction factor for all frequencies
if(length(roll)==size(S21_0pol,2))
    Correction = S21_interp_dB + PL - Mt_dB - Mr_dB - gain_horn - S21_0pol_dB(find(azimuth==0),find(roll==0)).';
    Correction_90deg = S21_90deg_interp_dB + PL - Mt_dB - Mr_dB - gain_horn - S21_90pol_dB(find(azimuth==0),find(roll==0)).';
else
    Correction = S21_interp_dB + PL - Mt_dB - Mr_dB - gain_horn - S21_0pol_dB(find(roll==0),find(azimuth==0)).';
    Correction_90deg = S21_90deg_interp_dB + PL - Mt_dB - Mr_dB - gain_horn - S21_90pol_dB(find(roll==0),find(azimuth==0)).';
end

% Initialise the gain vectors
gain_0pol = zeros(size(S21_0pol));
gain_90pol = gain_0pol;

for k=1:length(frequency_unique)
    % Current frequency
    f=frequency_unique(k);
    
    % Indices where samples for the current frequency are located
    indices = find(frequency == f);
    
    % Correct the measurement
    gain_0pol(indices,:) = S21_0pol_dB(indices,:) + Correction(k);
    gain_90pol(indices,:) = S21_90pol_dB(indices,:) + Correction_90deg(k);
end

% Gain is the linear sum of both components, expressed in dB
gain = 10*log10(10.^(gain_0pol/10) + 10.^(gain_90pol/10));
results.gain = gain;

% %%%TEST%%%%%%%%
% % calibration error
% cal_err = interp1(Freq_list,S21,f) - S21_0pol(find(roll==0),find(azimuth==0))';
% cal_err_90deg = interp1(Freq_list,S21_90deg,f) - S21_90pol(find(roll==0),find(azimuth==0))';
% for k=1:length(frequency_unique)
%     
%     f=frequency_unique(k);
%     
%     %Indices where samples for the current frequency are located
%     indices = find(frequency == f);
%     
%     S21_0pol(indices,:) = S21_0pol(indices,:) + cal_err(k);
%     S21_90pol(indices,:) = S21_90pol(indices,:) + cal_err_90deg(k);
% end
% %%%TEST%END%%%%

%% Stuff for circularly polarised radiation patterns
% Calculate the left and right E fields
E_left_real = 0.5*(real(S21_0pol)+imag(S21_90pol));
E_left_imag = 0.5*(imag(S21_0pol)-real(S21_90pol));
E_right_real = 0.5*(real(S21_0pol)-imag(S21_90pol));
E_right_imag = 0.5*(imag(S21_0pol)+real(S21_90pol));

% Get the amplitude ratio of the two
mag_E_left = sqrt(abs(E_left_real).^2+abs(E_left_imag).^2);
mag_E_right = sqrt(abs(E_right_real).^2+abs(E_right_imag).^2);
pol_ratio_c = (mag_E_right./mag_E_left).^2;

% The gains have the same ratio as the amplitudes
gain_right = pol_ratio_c.*10.^(gain./10)./(1+pol_ratio_c);
gain_left = 10.^(gain./10)-gain_right;
AR = (abs(mag_E_left)+abs(mag_E_right))./(abs(mag_E_left)-abs(mag_E_right));
results.axial_ratio = 20.*log10(abs(AR));

% Convert to dB
results.gain_right = 10*log10(gain_right);
results.gain_left = 10*log10(gain_left);


%% Stuff for horizontal and vertical radiation patterns
% Get the amplitude ratio of the two
mag_E_hor = sqrt(abs(real(S21_0pol)).^2+abs(imag(S21_0pol)).^2);
mag_E_ver = sqrt(abs(real(S21_90pol)).^2+abs(imag(S21_90pol)).^2);
pol_ratio = (mag_E_hor./mag_E_ver).^2;

% The gains have the same ratio as the amplitudes
gain_hor = pol_ratio.*10.^(gain./10)./(1+pol_ratio);
gain_ver = 10.^(gain./10)-gain_hor;

% Convert to dB
results.gain_hor = 10*log10(gain_hor);
results.gain_ver = 10*log10(gain_ver);


%% Efficiency
% Convert the gain to linear
gain = 10.^(gain./10);

% Initialise the output
eff = zeros(size(frequency_unique));

for k=1:length(frequency_unique)
    f = frequency_unique(k);
    indices = find(frequency==f);
    
    % Processing depends on the orientation of the vectors
    if length(roll)==size(S21_0pol,2)
        % Select the applicable gains
        Gr = gain(indices,:);
        
        % Construct the sin_theta vector over 180 angles, repmatted over
        % 360
        if (abs(min(azimuth))+abs(max(azimuth))>180)
            anglediff = roll(2)-roll(1);
            angles = 0:anglediff:179;
            repmat_length = length(0:(azimuth(indices(2))-azimuth(indices(1))):359);
            sin_theta = repmat(sin(angles/180*pi),repmat_length,1);
        else
            anglediff = azimuth(indices(2))-azimuth(indices(1));
            angles = 0:anglediff:179;
            repmat_length = length(0:(roll(2)-roll(1)):359);
            sin_theta = repmat(sin(angles/180*pi).',1,repmat_length);
        end

    else
        % Select the applicable gains
        Gr = gain(indices,:);

        % Construct the sin_theta vector over 180 angles, repmatted over
        % 360
        if (abs(min(azimuth))+abs(max(azimuth))>180)
            anglediff = roll(indices(2))-roll(indices(1));
            angles = 0:anglediff:179;
            repmat_length = length(0:(azimuth(2)-azimuth(1)):359);
            sin_theta = repmat(sin(angles/180*pi).',1,repmat_length);
        else
            anglediff = azimuth(2)-azimuth(1);
            angles = 0:anglediff:179;
            repmat_length = length(0:(roll(indices(2))-roll(indices(1))):359);
            sin_theta = repmat(sin(angles/180*pi),repmat_length,1);
        end
    end
    
    % Calculate the efficiency by integrating the gain
    eff(k) = pi*2*pi*sum(sum(sin_theta.*Gr(1:size(sin_theta,1),1:size(sin_theta,2))))/size(sin_theta,1)/size(sin_theta,2)/4/pi; 
end

%efficiency check
%consider data measured for azimuth -180 --> 180 / roll 0 --> 180, this
%means theta from -180 to 180 and phi from 0 to 180
%we need theta from 0 to 180 and phi from -180 to 180
%so we rearrange the gain data

%azimuth step
azi_step=azimuth(2)-azimuth(1);
roll_step=roll(2)-roll(1);
gain_new_order=[gain(1:(180/roll_step)+1,(180/azi_step)+1:-1:1); gain(2:(180/roll_step)+1,(180/azi_step)+1:(360/azi_step)+1)];
sin_theta=sin((0:azi_step:180)/180*pi);
sin_theta_r=repmat(sin_theta,360/azi_step,1);
effi=azi_step*roll_step*pi/360/360*sum(sum(gain_new_order(1:end-1,1:end-1).*sin_theta_r(1:end,1:end-1)));

% efficiency calculation rewrite
% azimuth = theta, roll = phi
% formula: pi/360^2 * sum_theta(sum_phi(sin(theta)*G(theta,phi)))
% first attempt: case with only one frequency
% check how gain is ordered in matrix
% if(length(azimuth))==size(Gr,1)
%     %azimuth changes along with row number
%     sin_theta=repmat(sin(azimuth(1:end-1)/180*pi)',1,length(roll)-1);
%     eff = pi/360/360*sum(sum(sin_theta(1:end,1:end).*Gr(1:end-1,1:end-1)));
% else
%     sin_theta=repmat(sin(azimuth(1:end-1)/180*pi)',length(roll)-1,1);
%     eff = pi/360/360*sum(sum(sin_theta(1:end,1:end).*Gr(1:end-1,1:end-1)));
% end
%let's try an integration with the roll angle as theta, result should be
%invariant, same sphere is integrated
% sin_theta=repmat(sin(roll(1:end-1))',length(azimuth)-1,1);
% eff = pi/360/360*sum(sum(sin_theta.*Gr(1:end-1,1:end-1)));
% Write the remaining results in the outputstruct
results.efficiency = eff;
results.effi=effi;
results.azimuth = azimuth;
results.roll = roll;
results.frequency = frequency;
results.phase = angle(S21);
results.ar = pol_ratio_c;
results.E_left_real = E_left_real;
results.E_left_imag = E_left_imag; 
results.E_right_real = E_right_real;
results.E_right_imag = E_right_imag;
results.E_left_mag = abs(E_left_real+1j*E_left_imag);
results.E_left_phase = angle(E_left_real+1j*E_left_imag); 
results.E_right_mag = abs(E_right_real + 1j*E_right_imag);
results.E_right_phase = angle(E_right_real + 1j*E_right_imag);


end


%% Helper functions
% Helper function to easily retrieve the gain of the used SGH
function gain = get_gain_horn_data(freq)

% The frequency list
FREQ11 = 1.1:0.01:1.7;
FREQ17 = 1.7:0.01:2.6;
FREQ26 = 2.6:0.01:4;
FREQ39 = 3.94:0.02:5.86;
FREQ58 = 5.8:0.02:8.2;
FREQ82 = 8.2:0.05:12.5;
FREQ124 = 12.4:0.1:18;
% The known gain of the standard gain horn
GAIN_horn11 = [14.36 14.43 14.49 14.55 14.62 14.67 14.73 14.79 14.85 14.9 14.96 15.01 15.06 15.11 15.17 15.22 15.27 15.32 15.37 15.41 15.46 15.51 15.56 15.60 15.65 15.7 15.74 15.79 15.84 15.88 15.93 15.97 16.02 16.06 16.11 16.15 16.2 16.24 16.28 16.33 16.37 16.41 16.45 16.49 16.53 16.57 16.61 16.65 16.69 16.72 16.76 16.8 16.83 16.86 16.89 16.92 16.95 16.98 17.00 17.03 17.05];   %INPUT
GAIN_horn17 = [14.48 14.53 14.58 14.62 14.67 14.71 14.75 14.8 14.84 14.88 14.92 14.96 15 15.03 15.07 15.11 15.14 15.18 15.22 15.25 15.29 15.32 15.39 15.38 15.42 15.45 15.49 15.52 15.55 15.58 15.62 15.65 15.68 15.71 15.74 15.77 15.8 15.83 15.89 15.89 15.92 15.95 15.98 16.01 16.04 16.07 16.1 16.13 16.15 16.18 16.21 16.24 16.27 16.3 16.33 16.36 16.39 16.42 16.44 16.47 16.5 16.53 16.56 16.59 16.61 16.64 16.68 16.7 16.73 16.75 16.78 16.81 16.83 16.86 16.89 16.91 16.94 16.96 16.99 17.01 17.04 17.06 17.09 17.11 17.13 17.16 17.18 17.2 17.22 17.24 17.26];
GAIN_horn26 = [17 17.03 17.07 17.1 17.13 17.15 17.18 17.21 17.24 17.27 17.3 17.32 17.35 17.38 17.41 17.43 17.46 17.48 17.51 17.54 17.56 17.59 17.61 17.64 17.66 17.68 17.71 17.73 17.76 17.78 17.8 17.83 17.85 17.87 17.89 17.92 17.94 17.96 17.98 18.01 18.03 18.05 18.07 18.09 18.11 18.13 18.15 18.17 18.2 18.22 18.24 18.26 18.28 18.3 18.32 18.34 18.36 18.38 18.40 18.42 18.43 18.45 18.47 18.49 18.51 18.53 18.55 18.57 18.59 18.61 18.62 18.64 18.66 18.68 18.70 18.72 18.74 18.75 18.77 18.79 18.81 18.83 18.84 18.86 18.88 18.90 18.91 18.93 18.95 18.97 18.98 19.00 19.02 19.03 19.05 19.07 19.08 19.10 19.12 19.13 19.15 19.17 19.18 19.20 19.22 19.23 19.25 19.26 19.28 19.3 19.31 19.33 19.34 19.36 19.37 19.39 19.40 19.42 19.43 19.45 19.46 19.47 19.49 19.50 19.52 19.53 19.54 19.56 19.57 19.59 19.60 19.61 19.62 19.63 19.64 19.66 19.67 19.68 19.69 19.70 19.71];
GAIN_horn39 = [17.08 17.12 17.16 17.20 17.23 17.27 17.31 17.35 17.39 17.42 17.46 17.50 17.53 17.57 17.60 17.64 17.67 17.71 17.74 17.77 17.81 17.84 17.87 17.90 17.94 17.97 18.00 18.03 18.06 18.09 18.12 18.15 18.18 18.21 18.24 18.26 18.29 18.32 18.35 18.37 18.40 18.43 18.45 18.48 18.50 18.53 18.56 18.58 18.61 18.63 18.65 18.68 18.70 18.73 18.75 18.77 18.79 18.82 18.84 18.86 18.88 18.90 18.93 18.95 18.97 18.99 19.01 19.03 19.05 19.07 19.09 19.11 19.13 19.15 19.17 19.19 19.20 19.22 19.24 19.26 19.28 19.29 19.31 19.33 19.35 19.36 19.38 19.40 19.41 19.43 19.44 19.46 19.48 19.49 19.51 19.52 19.54];
GAIN_horn58 = [21.66 21.68 21.69 21.71 21.72 21.74 21.76 21.77 21.79 21.80 21.82 21.83 21.85 21.86 21.88 21.89 21.91 21.92 21.94 21.96 21.97 21.98 21.99 22.01 22.02 22.03 22.04 22.07 22.08 22.09 22.11 22.12 22.13 22.15 22.16 22.17 22.19 22.20 22.21 22.22 22.24 22.25 22.26 22.27 22.29 22.30 22.31 22.32 22.33 22.35 22.36 22.37 22.38 22.39 22.40 22.41 22.42 22.43 22.44 22.45 22.46 22.47 22.48 22.49 22.50 22.51 22.52 22.53 22.54 22.55 22.56 22.57 22.58 22.58 22.59 22.60 22.61 22.62 22.63 22.63 22.64 22.65 22.65 22.66 22.67 22.67 22.68 22.69 22.69 22.70 22.70 22.71 22.72 22.72 22.73 22.73 22.74 22.74 22.75 22.75 22.75 22.76 22.76 22.77 22.77 22.77 22.78 22.78 22.78 22.79 22.79 22.79 22.79 22.79 22.80 22.80 22.80 22.80 22.80 22.80 22.81];
GAIN_horn82 = [21.37 21.41 21.45 21.49 21.53 21.57 21.60 21.64 21.67 21.70 21.73 21.77 21.80 21.82 21.85 21.88 21.91 21.93 21.96 21.98 22.01 22.03 22.05 22.07 22.09 22.11 22.13 22.15 22.17 22.19 22.21 22.23 22.24 22.26 22.28 22.29 22.31 22.33 22.34 22.36 22.37 22.39 22.40 22.42 22.43 22.45 22.46 22.47 22.49 22.50 22.51 22.53 22.54 22.55 22.57 22.58 22.60 22.61 22.62 22.63 22.64 22.65 22.66 22.67 22.68 22.69 22.70 22.72 22.73 22.74 22.75 22.76 22.77 22.78 22.79 22.79 22.80 22.81 22.82 22.82 22.83 22.83 22.84 22.84 22.85 22.85 22.85];
GAIN_horn124 = [23.48 23.52 23.57 23.61 23.66 23.70 23.74 23.79 23.83 23.87 23.91 23.95 23.99 24.02 24.06 24.10 24.13 24.17 24.20 24.24 24.27 24.30 24.34 24.37 24.40 24.43 24.46 24.49 24.51 24.54 24.57 24.59 24.62 24.64 24.67 24.69 24.71 24.73 24.76 24.78 24.79 24.81 24.83 24.85 24.87 24.88 24.90 24.91 24.92 24.94 24.95 24.96 24.97 24.98 24.99 24.99 25.00];

% Select the correct lists
testvalue = floor(freq(1)*10);
if(testvalue<11)
	error('No GainHorn information availlable');
elseif(testvalue<17)
	FREQ = FREQ11;
	GAIN_horn = GAIN_horn11;
elseif(testvalue<26)
	FREQ = FREQ17;
	GAIN_horn = GAIN_horn17;
elseif(testvalue<39)
	FREQ = FREQ26;
	GAIN_horn = GAIN_horn26;
elseif(testvalue<58)
	FREQ = FREQ39;
	GAIN_horn = GAIN_horn39;
elseif(testvalue<82)
	FREQ = FREQ58;
	GAIN_horn = GAIN_horn58;
elseif(testvalue<124)
	FREQ = FREQ82;
	GAIN_horn = GAIN_horn82;
elseif(testvalue<180)
	FREQ = FREQ124;
	GAIN_horn = GAIN_horn124;
else
	error('No GainHorn information availlable');
end

% Return the gain of the horn at the requested frequency samples
gain = interp1(FREQ,GAIN_horn,freq);

end