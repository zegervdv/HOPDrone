%--------------------------------------------------------------------------
%  Script that permits the determination of:
%  1) 3D Gain Pattern (for a fixed f, organized as a Matrix) 
%     of the Antenna under measurement (Rx), starting from the received 
%     power patterns, measured by us in the anechoic chamber.
%
%  2) Radiation Efficiency of the Antenna.
%
%  It uses the Friis formula 
%
%  This is new version for the new measurement method in anechoic chamber,
%  using the "Full 2 Port Calibration" on the NWA.
%
%  This version works fine when the azimuth goes from 0 :1: 180
%                                   roll goes from 0 :1: 359
%
%  Note: this script works correctly when the measurement of the 3D pattern
%  has been done at 1 frequency (for ex. 2.45 GHz)
%  Note2: respect to the version Gain_Pattern_3D.m, the calculation of the 
%  efficiency has been changed!
%
%  Luigi Vallozzi
%  Date: 04\12\2007
%  Last Modified: 14\04\2008
%
%--------------------------------------------------------------------------
clear;
close all;

%---------------------------------BEGIN Insert Inputs-------------------------------%
%-------------------------------------------------------
%Set name of the meas. file from the positioning system
%-------------------------------------------------------

meas_file_folder = 'D:\metingen\Galileo\sofant_10';

meas_file_name = 'sofant_1575_ascii.txt';

meas_file_path = [meas_file_folder,'\',meas_file_name];

%-------------------------------------------------------
%Set name of the 2port calibration. file from NWA 
%-------------------------------------------------------

%cal_file_folder = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa2_3D_measurement\ref_measurements';
cal_file_folder = 'D:\metingen\Galileo\sofant_10';


cal_file_name = 'sofant_ref10_0.cti';

cal_file_path = [cal_file_folder,'\',cal_file_name];

%--------------------------------------------------------
%Set folder of the output
%--------------------------------------------------------

%out_folder = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa2_3D_measurement\processed_data_and_plots';
out_folder = 'D:\metingen\Galileo\sofant_10';


%-----------------------------------
%Set distance between antennas [m]
%-----------------------------------
d=4.16;
%---------------------------------END Insert Inputs-------------------------------%





%---------------------------------BEGIN Aquisition Inputs-------------------------------%
%--------------------------                                               
%Parse output orbit system 
%--------------------------
[S21_pol_0,azimuth,roll,frequency, polarization_0] = parser(meas_file_path, 'Polarization', 0);

[S21_pol_90,azimuth,roll,frequency, polarization_90] = parser(meas_file_path, 'Polarization', 90);

%note: 
% data_pol_0 and data_pol_90  are  matrices phipoints x thetapoints, each element of
%which is a complex number representing the   S21    measured by the
%positioning system, in linear scale


%we re-obtain the matrices phipoints x thetapoints of the modulus of S21:
S21_dB_pol_0 = 20*log10(abs(S21_pol_0));

S21_dB_pol_90 = 20*log10(abs(S21_pol_90));

%..and of the phases
S21_ph_pol_0 = angle(S21_pol_0);

S21_ph_pol_90 = angle(S21_pol_90);



%--------------------------                                               
%Data from the 2port cal.
%--------------------------

[Freq_list,S11_cal_RI,S21_cal_RI,S12_cal_RI,S22_cal_RI] = VNA_parser_new(cal_file_path);

S11_cal_dB = 20*log10(abs(S11_cal_RI)); 
S22_cal_dB = 20*log10(abs(S22_cal_RI));
S21_cal_dB = 20*log10(abs(S21_cal_RI));



%--------------------------                                               
%Frequency
%--------------------------
f = frequency(1); %[GHz]       

%------------------------------------------
% Selection of the element of S11_cal_dB,
% S22_cal_dB, S21_cal_dB correspondent to 
% the frequency f  (for 2.45 is the element 181)
%------------------------------------------

ind_f = find(Freq_list(:) == f);

S11_cal_dB_f = S11_cal_dB(ind_f);  
S22_cal_dB_f = S22_cal_dB(ind_f);
S21_cal_dB_f = S21_cal_dB(ind_f);



%-----------------------------------------------------------------------
%Setting max, min phi and theta angle and number of points measured, in
%DEGREE, according to the measured data
%-----------------------------------------------------------------------
%If Roll=phi,  Azimuth=theta

minphi=roll(1);
maxphi=roll(end);
step_phi = roll(2)-roll(1);

mintheta=azimuth(1);
maxtheta=azimuth(end);
step_theta = azimuth(2)-azimuth(1);

%------------------------------------------------------------------
%Set Gain (dB) of the transmitting antenna at frquency f 
%------------------------------------------------------------------
%this should come from the data's from the tables of the horns' gain --> we
%should put all the datas of all the horns, and find an automated method to
% look for the right horn and the gain at the frequency f

% %horn 2 - 3 GHz
% FREQ = [2 2.1 2.2 2.3 2.4 2.5 2.6 2.7 2.8 2.9 3];
% GAIN_horn = [15.62 15.92 16.21 16.50 16.78 17.04 17.26 17.30 17.56 17.80 18.03];   %INPUT

%horn 2.6 - 3.95 GHz
% FREQ = [2.9 3];
% GAIN_horn = [17.8  18.3];

%horn 1.7-2.6 GHz
% FREQ = [1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 2.5 2.6];
% GAIN_horn = [14.5 14.95  15.3  15.6  15.9  16.2  16.5  16.8  17.25];

%horn 1.1 - 1.7 GHz
% FREQ = [1.2  1.25  1.3];
% GAIN_horn = [14.97  15.2  15.48];
FREQ = 1.1:0.01:1.7;
GAIN_horn = [14.36 14.43 14.49 14.55 14.62 14.67 14.73 14.79 14.85 14.9 14.96 15.01 15.06 15.11 15.17 15.22 15.27 15.32 15.37 15.41 15.46 15.51 15.56 15.60 15.65 15.7 15.74 15.79 15.84 15.88 15.93 15.97 16.02 16.06 16.11 16.15 16.2 16.24 16.28 16.33 16.37 16.41 16.45 16.49 16.53 16.57 16.61 16.65 16.69 16.72 16.76 16.8 16.83 16.86 16.89 16.92 16.95 16.98 17.00 17.03 17.05];

gain_horn_f = interp1(FREQ,GAIN_horn,f);





%---------------------------------END Aquisition Inputs------------------------------%





%---------------------------------
% Calculation of useful parameters
%---------------------------------

phipoints=(maxphi-minphi)/step_phi+1;     %360 (cols)
thetapoints=(maxtheta-mintheta)/step_theta+1;    %361 (rows)

%----------------------------------
%Speed of light in free space [m/s]
%----------------------------------
c=2.99792458*10^8;

%Wavelenght
lambda=c/(f*10^9); %[m]

%Path Loss at frequency f (in dB)
PL= 20*log10((4*pi*d)/lambda);


%Calculation of the mismatch factor of the antenna under test (receiving)
%at frequency f
S22 = 10^(S22_cal_dB_f/20);
Mr_dB = 10*log10(1-(abs(S22))^2); 


%Calculation of the mismatch factor of the Transmitting Horn at frequency f
% in dB
S11 = 10^(S11_cal_dB_f/20);
Mt_dB = 10*log10(1-(abs(S11))^2); 
 


%-----------------------------------------------
% Calculation of the corrections(at frequency f),
% for the 2 polarizations 0 and 90,
% to add to the measured S21 to obtain the gain
%-----------------------------------------------
ind_az_0 = find(azimuth(:)==0);  %column index coresponding to the position az=0
ind_roll_0 = find(roll(:)==0);   %row index roll=0
ind_roll_90 = find(roll(:)==90); %row index roll=90
gain_cal_f = S21_cal_dB_f - Mr_dB - Mt_dB + PL - gain_horn_f;

%correction to apply
Corr = gain_cal_f - S21_dB_pol_0(ind_roll_0, ind_az_0);   



%--------------------------------------------------------------------------
%calculation of the gain (3D pattern) of the antenna under test (receiving),
%multiplied for the 2 polarization mismatch for polarization 0 and 90 deg
%
%These two are the gain pattern (that is only one) respectively 
%multiplied for the polarization mismatch factors for 0 deg. and 90 deg.
%--------------------------------------------------------------------------

Gr_pol0 =  S21_dB_pol_0 + Corr;

Gr_pol90 = S21_dB_pol_90 + Corr;

%We pass in linear scale:
Gr_pol0_lin = 10.^(Gr_pol0/10);

Gr_pol90_lin = 10.^(Gr_pol90/10);

%The gain (pattern) is the sum of the last two, in linear scale
%(it can be demonstrated)
Gr_lin = Gr_pol0_lin + Gr_pol90_lin;

%Gain (pattern) in dB
Gr=10*log10(Gr_lin);




%-------------------------
% Efficiency calculation
%-------------------------

%Now we organize the elements of Gr_lin in a matrix (Gr_mat) of dimensions 
%phipoints x thetapoints 
%Rows: gain patterns with variable theta (and fixed phi)
%Column: gain patterns with variable phi (and fixed theta)

%transposition (because it is thetapoints x phipoints  and we want  phipoints x thetapoints)
Gr_lin = Gr_lin.';


Gr_mat=reshape(Gr_lin,phipoints,thetapoints);

%We create also two vectors containing the corresponding span of the phi
%and theta angle to apply to the previous matrix;

phi_vector=[minphi:step_phi:maxphi];

theta_vector=[mintheta:step_theta:maxtheta];


%----------------------------------------------------------
%Calculation of the radiation efficiency:
%
%We must integrate the Gain along theta and phi,
%(see eq 1.8 on pag.10 of Antenna and Propagation course)
%with theta and phi expressed in radians
%
%----------------------------------------------------------

L = 180;

I_0=0;

for j = 1:phipoints-1
    t = sum(sin(2*pi/360*(0:step_theta:L-1)).*Gr_mat(j,1:180));
    I_0 = I_0+t;
end

%Radiation Efficiency
Rad_Eff = I_0* pi/(360)^2;
display(['radiation efficiency =',num2str(Rad_Eff)]);
save([out_folder,'\','Rad_eff_',meas_file_name], 'Rad_Eff', '-ascii');
if(length(azimuth))==size(Gr,1)
    %azimuth changes along with row number
    sin_theta=repmat(sin(azimuth(1:end-1)/180*pi)',1,length(roll)-1);
    eff = pi/360/360*sum(sum(sin_theta.*Gr(1:end-1,1:end-1)));
end

%plot of the gain patterns on the XZ and YZ planes
figure
%polar(azimuth.'*pi/180, Gr(ind_roll_0,:))
plot(azimuth, Gr(ind_roll_0,:))
grid on
title('Gain Pattern in dB on the \phi=0 plane')


ind_roll_90 = find(roll(:)==90);

figure
%polar(azimuth.'*pi/180, Gr(ind_roll_90,:))
plot(azimuth, Gr(ind_roll_90,:))
grid on
title('Gain Pattern in dB on \phi=90 plane')




			
