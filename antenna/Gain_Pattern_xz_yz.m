%--------------------------------------------------------------------------
%  Script that calculates the Gain Patterns on XZ and YZ plane (for a fixed f)
%  (but it works as well if the measure is only at one of the two plane XZ or YZ)
%  of the antenna under measurement, starting from the received S21 pattern,
%  measured by us in the anechoic cheamber of INTEC.
%  It uses the Friis formula 
%
%  This is a new version, based on the measurement method with full 2 port 
%  calibration of the NWA in the anechoic chamber
%
% It assumes that the receiving AUT is placed in a way for which it holds:
% azimuth = theta
% roll = phi
% 
%
%
%  Luigi Vallozzi
%  Date: 06\12\2007
%  Last Modified: 19\02\2007
%--------------------------------------------------------------------------
clear;
close all

%---------------------------------BEGIN Insert Inputs-------------------------------%
% In this section there are the only things that the operator has to modify
%-------------------------------------------------------
%Set name of the meas. file from the positioning system
%-------------------------------------------------------
%meas_file_fold_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\Gps_antenna_new_meas_LV\planar_with_textiles\prot5';
%meas_file_fold_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa_jacket_bot-no_bot_meas_29_10_08\measurements_az_plane';
%meas_file_fold_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa_jack_back_port1_bot_meas_14_11_08\measurements';
%meas_file_fold_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa_jack_back_port1_bot_meas_14_11_08\measurements';
%meas_file_fold_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\Orban_meas_01_09\measurements';
%meas_file_fold_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa_jacket_front_bot-no_bot_meas_29_10_08\measurements_az_plane';
%meas_file_fold_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\Patrick_onbody_meas\measurements';
meas_file_fold_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\Measurements_bending_rad_pat_aram_stretch_09_11\measurements';

%meas_file_name_no_ext = 'gps5_pattern_xz_yz_text_meas';
%meas_file_name_no_ext = 'dppa_jacket_bot_port2';
%meas_file_name_no_ext = 'dppa_2_back_jack_port1_nobot';
%meas_file_name_no_ext = 'dppa_2_back_jack_port2_bot';
%meas_file_name_no_ext = 'gps_tuned';
%meas_file_name_no_ext = 'dppa_jacket_no_bot_port1';
%meas_file_name_no_ext = 'test_12_radpat';
meas_file_name_no_ext = 'flat_rad_pat_xz_yz_1_575';


%meas_file_fold_path = 'C:\Program Files\MATLAB71\work\GPS_prot_4_meas\open_space';
%meas_file_name_no_ext = 'GPS_luigi_4_os_exp';

% meas_file_fold_path = 'C:\Program Files\MATLAB71\work\GPS_prot_4_meas\textiles';
% meas_file_name_no_ext = 'GPS_luigi_4_tex_exp';

%meas_file_fold_path = 'C:\Program Files\MATLAB71\work\GPS_prot_4_meas\on_body';
%meas_file_name_no_ext = 'GPS_luigi_4_ob_exp';


meas_file_name = [meas_file_fold_path,'\',meas_file_name_no_ext,'.txt'];

%-------------------------------------------------------
%Set name of the 2port calibration. file from NWA 
%-------------------------------------------------------
%cal_file_name = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa_jacket_front_bot-no_bot_meas_29_10_08\ref_measurements\ref_no_bot_port1.txt';   %must be obtained by the NWA saving in the "DATA" format (and not "FORMATTED")
%cal_file_name = 'C:\Program Files\MATLAB71\work\GPS_prot_4_meas\open_space\GPS_luigi_4_os_pol0_data.txt';   %must be obtained by the NWA saving in the "DATA" format (and not "FORMATTED")
%cal_file_name = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\ref_meas_test_pol0.txt';   %must be obtained by the NWA saving in the "DATA" format (and not "FORMATTED")
%cal_file_name = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\Patrick_onbody_meas\ref_measurements\ref_meas_BL_DD_V.txt';   %must be obtained by the NWA saving in the "DATA" format (and not "FORMATTED")
cal_file_name = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\Measurements_bending_rad_pat_aram_stretch_09_11\ref_measurements\flat_pol_par.txt';   %must be obtained by the NWA saving in the "DATA" format (and not "FORMATTED")


%-------------------------------------------------------
%Set the output folder path 
%-------------------------------------------------------
%out_folder_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\Patrick_onbody_meas\processed_meas_and_plots';
out_folder_path = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\Measurements_bending_rad_pat_aram_stretch_09_11\proc_meas_and_plots';

%-----------------------------------
%Set distance between antennas [m]
%-----------------------------------
d=4.182;

%---------------------------------END Insert Inputs-------------------------------%





%---------------------------------BEGIN Aquisition Inputs-------------------------------%
%--------------------------                                               
%Parse output orbit system 
%--------------------------
[S21_pol_0,azimuth,roll,frequency, polarization_0] = parser(meas_file_name, 'Polarization', 0);

[S21_pol_90,azimuth,roll,frequency, polarization_90] = parser(meas_file_name, 'Polarization', 90);


% data_pol_0 and data_pol_90  are  matrices 2 x thetapoints, each element of
%which is a complex number representing the   S21    measured by the
%positioning system, in linear scale


%we re-obtain the matrices (2 x thetapoints) of the modulus of S21:
S21_dB_pol_0 = 20*log10(abs(S21_pol_0));

S21_dB_pol_90 = 20*log10(abs(S21_pol_90));

%..and of the phases
S21_ph_pol_0 = angle(S21_pol_0);

S21_ph_pol_90 = angle(S21_pol_90);


%--------------------------                                               
%Data from the 2port cal.
%--------------------------
  
[Freq_list,S11_cal_RI,S21_cal_RI,S12_cal_RI,S22_cal_RI] = VNA_parser_new(cal_file_name);


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

%if the exact frequency is present in the frequency list 
ind_f = find(abs(Freq_list(:)-f)<1e-6)
%ind_f = 231;

S11_cal_dB_f = S11_cal_dB(ind_f);   
S22_cal_dB_f = S22_cal_dB(ind_f);
S21_cal_dB_f = S21_cal_dB(ind_f);

%if the exact frequency is not present in the frequency list,
%we have to interpolate


%-----------------------------------------------------------------------
%Setting max, min phi and theta angle and number of points measured, in
%DEGREE, according to the measured data
%-----------------------------------------------------------------------
%If Roll=phi,  Azimuth=theta

minphi=roll(1);
maxphi=roll(end);

if length(roll)>1
 step_phi = roll(2)-roll(1);
else  %in the case there is only 1 value of roll, i.e. the measurement is only on XZ or YZ plane
 step_phi = 0;
end
    
mintheta=azimuth(1);
maxtheta=azimuth(end);
step_theta = azimuth(2)-azimuth(1);

%------------------------------------------------------------------
%Set Gain (dB) of the transmitting antenna at frquency f 
%------------------------------------------------------------------
%this should come from the data's from the tables of the horns' gain

%Horn for 2.45 GHz
%FREQ = [2 2.1 2.2 2.3 2.4 2.5 2.6 2.7 2.8 2.9 3];
%GAIN_horn = [15.62 15.92 16.21 16.50 16.78 17.04 17.26 17.30 17.56 17.80 18.03];   %INPUT

%Horn for 1.575 GHz
FREQ = [1.1 1.2 1.3 1.4 1.5 1.6 1.7];
GAIN_horn = [14.36 14.96 15.46 15.93 16.37 16.76 17.05];

gain_horn_f = interp1(FREQ,GAIN_horn,f);





%---------------------------------END Aquisition Inputs------------------------------%



%---------------------------------
% Calculation of useful parameters
%---------------------------------

if length(roll)>1 
    phipoints=(maxphi-minphi)/step_phi+1; %2 (rows)
else %in case there is only 1 value of roll, i.e. measure on only 1 plane (XZ or YZ)
    phipoints = 0;
end

thetapoints=(maxtheta-mintheta)/step_theta+1;    %361 (cols)


%----------------------------------
%Speed of light in free space [m/s]
%----------------------------------
c=2.99792458*10^8;

%Wavelength
lambda=c/(f*10^9);%in m


%Path Loss at frequency f
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
ind_roll_0 = find(roll(:)==0 & frequency(:) == f);   %row index roll=0
ind_roll_90 = find(roll(:)==90); %row index roll=90

gain_cal_f = S21_cal_dB_f - Mr_dB - Mt_dB + PL - gain_horn_f; %check again: maybe -Mr_dB and - Mt_dB must be not present

%correction to apply in the two cases of polarization
Corr = gain_cal_f - S21_dB_pol_90(ind_roll_0, ind_az_0);   



%calculation of the gain (pattern) of the antenna under test (receiving),for the 2
%polarization 0 and 90 deg
%these two are the gain pattern (that is only one) respectively multiplied
%for the polarization mismatch factors for 0 deg. and 90 deg.
Gr_pol0 = S21_dB_pol_0 + Corr;

Gr_pol90 = S21_dB_pol_90 + Corr;



%We pass in linear scale:
Gr_pol0_lin = 10.^(Gr_pol0/10);

Gr_pol90_lin = 10.^(Gr_pol90/10);

%The gain (pattern) is the sum of the last two, in linear scale (it can be demonstrated)
Gr_lin = Gr_pol0_lin + Gr_pol90_lin;

%Gain (pattern) in dB (is a matrix 2 x thetapoints)
Gr=10*log10(Gr_lin);


%plot of the gain patterns on the XZ and YZ planes

if ind_roll_0  %check if roll=0 data exists
figure
plot(azimuth, Gr(ind_roll_0,:))
title('Gain Pattern in dB on XZ plane')
G_XZ = [azimuth, Gr(ind_roll_0,:).'];
save([out_folder_path,'\',meas_file_name_no_ext,'_gain_XZ.txt'], 'G_XZ', '-ascii');
end

if ind_roll_90  %chek if roll=90 data exists
figure
plot(azimuth, Gr(ind_roll_90,:))
title('Gain Pattern in dB on YZ plane')
G_YZ = [azimuth, Gr(ind_roll_90,:).'];
save([out_folder_path,'\',meas_file_name_no_ext,'_gain_YZ.txt'], 'G_YZ', '-ascii');
end

saveas(figure(1),[out_folder_path,'\',meas_file_name_no_ext,'_gain_XZ.fig'])
saveas(figure(2),[out_folder_path,'\',meas_file_name_no_ext,'_gain_YZ.fig'])


%build S22(f) (antenna under test) (freq, |s11|dB) for plot or analysis purposes
S22_dB_f = [Freq_list,S22_cal_dB];
save([out_folder_path,'\',meas_file_name_no_ext,'_S11_dB_f.txt'], 'S22_dB_f', '-ascii');






			
			
