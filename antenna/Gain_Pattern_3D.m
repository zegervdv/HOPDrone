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
%  Note: this script works correctly when the measurement of the 3D pattern
%  has been done at 1 frequency (for ex. 2.45 GHz)
%
%  Luigi Vallozzi
%  Date: 20\11\2008
%  Last Modified: 20\11\2008
%
%--------------------------------------------------------------------------
clear;
close all;

%---------------------------------BEGIN Insert Inputs-------------------------------%
%-------------------------------------------------------
%Set name of the meas. file from the positioning system
%-------------------------------------------------------
%meas_file_name = 'RRA1_full.txt'; %example that works!!
%meas_file_name = 'foam125_2_3Dazimuth.txt';

%meas_file_folder = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa2_3D_measurement\measurements';
%meas_file_name = 'pattern_3D_dppa_2_port2_osExp';

meas_file_folder = '.';
meas_file_name = 'uwb_dipole_3D_1G';


meas_file_path = [meas_file_folder,'/',meas_file_name,'.txt'];

%-------------------------------------------------------
%Set name of the 2port calibration. file from NWA 
%-------------------------------------------------------
%cal_file_name = 'RI_RRA1.txt'; %example that works!!
%cal_file_name = 'DD_MLS.txt';

%cal_file_folder = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa2_3D_measurement\ref_measurements';
%cal_file_name = 'ref_meas_port2';

cal_file_folder = '.';
cal_file_name = 'ref_meas2_dipole_board_pol0';

cal_file_path = [cal_file_folder,'/',cal_file_name,'.txt'];

%-------------------------------------------------------
% Set Out Folder
%-------------------------------------------------------

%out_folder = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa2_3D_measurement\processed_data_and_plots';

out_folder = '.';



%-----------------------------------
%Set distance between antennas [m]
%-----------------------------------
d=4.355;
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

% FREQ = [2 2.1 2.2 2.3 2.4 2.5 2.6 2.7 2.8 2.9 3];
% GAIN_horn = [15.62 15.92 16.21 16.50 16.78 17.04 17.26 17.30 17.56 17.80 18.03];   %INPUT

% The frequency list
FREQ11 = 1.1:0.01:1.7;
FREQ17 = 1.7:0.01:2.6;
FREQ26 = 2.6:0.01:4;
FREQ39 = 3.94:0.02:5.86;
FREQ58 = 5.8:0.02:8.2;
FREQ82 = 8.2:0.05:12.5;
FREQ124 = 12.4:0.1:18;

%select the right frequency range
FREQ = FREQ26;

% The known gain of the standard gain horn
GAIN_horn11 = [14.36 14.43 14.49 14.55 14.62 14.67 14.73 14.79 14.85 14.9 14.96 15.01 15.06 15.11 15.17 15.22 15.27 15.32 15.37 15.41 15.46 15.51 15.56 15.60 15.65 15.7 15.74 15.79 15.84 15.88 15.93 15.97 16.02 16.06 16.11 16.15 16.2 16.24 16.28 16.33 16.37 16.41 16.45 16.49 16.53 16.57 16.61 16.65 16.69 16.72 16.76 16.8 16.83 16.86 16.89 16.92 16.95 16.98 17.00 17.03 17.05];   %INPUT
GAIN_horn17 = [14.48 14.53 14.58 14.62 14.67 14.71 14.75 14.8 14.84 14.88 14.92 14.96 15 15.03 15.07 15.11 15.14 15.18 15.22 15.25 15.29 15.32 15.39 15.38 15.42 15.45 15.49 15.52 15.55 15.58 15.62 15.65 15.68 15.71 15.74 15.77 15.8 15.83 15.89 15.89 15.92 15.95 15.98 16.01 16.04 16.07 16.1 16.13 16.15 16.18 16.21 16.24 16.27 16.3 16.33 16.36 16.39 16.42 16.44 16.47 16.5 16.53 16.56 16.59 16.61 16.64 16.68 16.7 16.73 16.75 16.78 16.81 16.83 16.86 16.89 16.91 16.94 16.96 16.99 17.01 17.04 17.06 17.09 17.11 17.13 17.16 17.18 17.2 17.22 17.24 17.26];
GAIN_horn26 = [17 17.03 17.07 17.1 17.13 17.15 17.18 17.21 17.24 17.27 17.3 17.32 17.35 17.38 17.41 17.43 17.46 17.48 17.51 17.54 17.56 17.59 17.61 17.64 17.66 17.68 17.71 17.73 17.76 17.78 17.8 17.83 17.85 17.87 17.89 17.92 17.94 17.96 17.98 18.01 18.03 18.05 18.07 18.09 18.11 18.13 18.15 18.17 18.2 18.22 18.24 18.26 18.28 18.3 18.32 18.34 18.36 18.38 18.40 18.42 18.43 18.45 18.47 18.49 18.51 18.53 18.55 18.57 18.59 18.61 18.62 18.64 18.66 18.68 18.70 18.72 18.74 18.75 18.77 18.79 18.81 18.83 18.84 18.86 18.88 18.90 18.91 18.93 18.95 18.97 18.98 19.00 19.02 19.03 19.05 19.07 19.08 19.10 19.12 19.13 19.15 19.17 19.18 19.20 19.22 19.23 19.25 19.26 19.28 19.3 19.31 19.33 19.34 19.36 19.37 19.39 19.40 19.42 19.43 19.45 19.46 19.47 19.49 19.50 19.52 19.53 19.54 19.56 19.57 19.59 19.60 19.61 19.62 19.63 19.64 19.66 19.67 19.68 19.69 19.70 19.71];
GAIN_horn39 = [17.08 17.12 17.16 17.20 17.23 17.27 17.31 17.35 17.39 17.42 17.46 17.50 17.53 17.57 17.60 17.64 17.67 17.71 17.74 17.77 17.81 17.84 17.87 17.90 17.94 17.97 18.00 18.03 18.06 18.09 18.12 18.15 18.18 18.21 18.24 18.26 18.29 18.32 18.35 18.37 18.40 18.43 18.45 18.48 18.50 18.53 18.56 18.58 18.61 18.63 18.65 18.68 18.70 18.73 18.75 18.77 18.79 18.82 18.84 18.86 18.88 18.90 18.93 18.95 18.97 18.99 19.01 19.03 19.05 19.07 19.09 19.11 19.13 19.15 19.17 19.19 19.20 19.22 19.24 19.26 19.28 19.29 19.31 19.33 19.35 19.36 19.38 19.40 19.41 19.43 19.44 19.46 19.48 19.49 19.51 19.52 19.54];
GAIN_horn58 = [21.66 21.68 21.69 21.71 21.72 21.74 21.76 21.77 21.79 21.80 21.82 21.83 21.85 21.86 21.88 21.89 21.91 21.92 21.94 21.96 21.97 21.98 21.99 22.01 22.02 22.03 22.04 22.07 22.08 22.09 22.11 22.12 22.13 22.15 22.16 22.17 22.19 22.20 22.21 22.22 22.24 22.25 22.26 22.27 22.29 22.30 22.31 22.32 22.33 22.35 22.36 22.37 22.38 22.39 22.40 22.41 22.42 22.43 22.44 22.45 22.46 22.47 22.48 22.49 22.50 22.51 22.52 22.53 22.54 22.55 22.56 22.57 22.58 22.58 22.59 22.60 22.61 22.62 22.63 22.63 22.64 22.65 22.65 22.66 22.67 22.67 22.68 22.69 22.69 22.70 22.70 22.71 22.72 22.72 22.73 22.73 22.74 22.74 22.75 22.75 22.75 22.76 22.76 22.77 22.77 22.77 22.78 22.78 22.78 22.79 22.79 22.79 22.79 22.79 22.80 22.80 22.80 22.80 22.80 22.80 22.81];
GAIN_horn82 = [21.37 21.41 21.45 21.49 21.53 21.57 21.60 21.64 21.67 21.70 21.73 21.77 21.80 21.82 21.85 21.88 21.91 21.93 21.96 21.98 22.01 22.03 22.05 22.07 22.09 22.11 22.13 22.15 22.17 22.19 22.21 22.23 22.24 22.26 22.28 22.29 22.31 22.33 22.34 22.36 22.37 22.39 22.40 22.42 22.43 22.45 22.46 22.47 22.49 22.50 22.51 22.53 22.54 22.55 22.57 22.58 22.60 22.61 22.62 22.63 22.64 22.65 22.66 22.67 22.68 22.69 22.70 22.72 22.73 22.74 22.75 22.76 22.77 22.78 22.79 22.79 22.80 22.81 22.82 22.82 22.83 22.83 22.84 22.84 22.85 22.85 22.85];
GAIN_horn124 = [23.48 23.52 23.57 23.61 23.66 23.70 23.74 23.79 23.83 23.87 23.91 23.95 23.99 24.02 24.06 24.10 24.13 24.17 24.20 24.24 24.27 24.30 24.34 24.37 24.40 24.43 24.46 24.49 24.51 24.54 24.57 24.59 24.62 24.64 24.67 24.69 24.71 24.73 24.76 24.78 24.79 24.81 24.83 24.85 24.87 24.88 24.90 24.91 24.92 24.94 24.95 24.96 24.97 24.98 24.99 24.99 25.00];

%select the right horn gains
GAIN_horn = GAIN_horn26;

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
ind_roll_180 = find(roll(:)==180); %row index roll=180
ind_roll_270 = find(roll(:)==270);

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
%Gr_lin = Gr_lin.';


%Gr_mat=reshape(Gr_lin,phipoints,thetapoints);
Gr_mat = Gr_lin;

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

L = (thetapoints-1);
M = (phipoints-1);

I_0=0;

for j = 1:phipoints
    t = sum(abs(sin(2*pi/360*theta_vector(1:L)))'.*Gr_mat(j,1:L)');
    I_0 = I_0+t;
end

%Radiation Efficiency
Rad_Eff = I_0* pi/(360)^2;
display(['radiation efficiency =',num2str(Rad_Eff)]);
%save([out_folder,'\','Rad_eff_',meas_file_name,'.txt'], 'Rad_Eff', '-ascii');


%gain patterns on xz and yz planes [361 x 2]
%first column: azimuth from -180:-1:+180
%second column: Gains
% Gr_XZ = [[fliplr(-azimuth(2:end)'),azimuth']' , [fliplr(Gr(ind_roll_180,1:end-1)), Gr(ind_roll_0,:)]'];
% Gr_YZ = [[fliplr(-azimuth(2:end)'),azimuth']' , [fliplr(Gr(ind_roll_270,1:end-1)), Gr(ind_roll_90,:)]'];

Gr_XZ = [azimuth , Gr_mat(1,:).'];
Gr_YZ = [azimuth, Gr_mat(91,:).'];

%save([out_folder,'\',meas_file_name,'_gain_XZ.txt'], 'Gr_XZ', '-ascii');
%save([out_folder,'\',meas_file_name,'_gain_YZ.txt'], 'Gr_YZ', '-ascii');

%save([out_folder,'\',meas_file_name,'_gain_3D_pol_0.mat'], 'Gr_pol0', '-mat');
%save([out_folder,'\',meas_file_name,'_gain_3D_pol_90.mat'], 'Gr_pol90', '-mat');



%save([out_folder,'\',meas_file_name,'_gain_3D.mat'], 'Gr_mat', '-mat');
%save([out_folder,'\',meas_file_name,'_phi_angles.mat'], 'phi_vector', '-mat');
%save([out_folder,'\',meas_file_name,'_theta_angles.mat'], 'theta_vector', '-mat');




%plot of the gain patterns on the XZ and YZ planes
figure(1)
plot(Gr_XZ(:,1), Gr_XZ(:,2))
xlim([-180,180])
xlabel('\theta [deg.]')
ylabel('[dBi]')
grid on
title('Gain Pattern in the XZ plane')

figure(2)
plot(Gr_YZ(:,1), Gr_YZ(:,2))
xlim([-180,180])
xlabel('\theta [deg.]')
ylabel('[dBi]')
grid on
title('Gain Pattern in the YZ plane')

%saveas(figure(1),[out_folder,'\',meas_file_name,'_gain_xz.fig'])
%saveas(figure(2),[out_folder,'\',meas_file_name,'_gain_yz.fig'])

%phases of the gain components
Gain_phi_3D_patt_phase = S21_ph_pol_0; %[rad]
Gain_theta_3D_patt_phase = S21_ph_pol_90; %[rad]

%components of gain in Re and Im parts
Gain_phi_3D_re_im = Gr_pol0_lin.*exp(i.*S21_ph_pol_0);
Gain_theta_3D_re_im = Gr_pol90_lin.*exp(i.*S21_ph_pol_90);


%save matrixes gain components Re and Im
save([out_folder,'\Gain_phi_3D_re_im.mat'], 'Gain_phi_3D_re_im', '-mat');
save([out_folder,'\Gain_theta_3D_re_im.mat'], 'Gain_theta_3D_re_im', '-mat');



%polarization ellipse parameters
S21_y{1} = abs(S21_pol_0);
S21_y{2} = angle(S21_pol_0)*180/pi;

S21_x{1} = abs(S21_pol_90);
S21_x{2} = angle(S21_pol_90)*180/pi;

S21_x_mod = S21_x{1};
S21_y_mod = S21_y{1};

S21_x_re = S21_x_mod.*cos(S21_x{2}*pi/180);
S21_x_im = S21_x_mod.*sin(S21_x{2}*pi/180);
S21_y_re = S21_y_mod.*cos(S21_y{2}*pi/180);
S21_y_im = S21_y_mod.*sin(S21_y{2}*pi/180);

tan_2phi=2*(S21_x_re.*S21_x_im + S21_y_re.*S21_y_im)./(S21_x_re.^2 - S21_x_im.^2 + S21_y_re.^2 - S21_y_im.^2);

phi = atan(tan_2phi)/2*180/pi;

phi = phi;

phi_x=S21_x{2} - phi;
phi_y=S21_y{2} - phi;

Ax_r=S21_x_mod.*cos(phi_x*pi/180);
Ax_i=S21_x_mod.*sin(phi_x*pi/180);
Ay_r=S21_y_mod.*cos(phi_y*pi/180);
Ay_i=S21_y_mod.*sin(phi_y*pi/180);

%tan_alpha=Ay_r./Ax_r;
%tan_alpha_2=-Ax_i./Ay_i;%must be equal to tan_alpha
%alpha = atan(tan_alpha)*180/pi;
%alpha_2 = atan(tan_alpha_2)*180/pi;

alpha = atan2(Ay_r,Ax_r)*180/pi;
alpha_2 = atan2(-Ax_i,Ay_i)*180/pi;

tau=Ax_i./Ay_r;

tau_2=-Ay_i./Ax_r;

AR_LP=20*log10(abs((1+abs(tau))./(1-abs(tau))));

%save matrixes polarization parameters
save([out_folder,'\alpha_3D.mat'], 'alpha', '-mat');
save([out_folder,'\tau_3D.mat'], 'tau', '-mat');
save([out_folder,'\AR_LP_dB.mat'], 'AR_LP', '-mat');


