

%-------------------------------------------------------------------------
% Script which permits the determination of polarization of the antenna 
% under test, from two measurement in anechoic chamber, with two orthogonal
% polarization of the Tx antenna, with the AUT in the Rx mode
%-------------------------------------------------------------------------

%-------------------------------------------------------------------------
%S21_x and S21_y = are arrays of 3 columns:
%        
%        the 1st column representing the list of the azimuth amgles 
%        of the positioning system from -180 to 180 with certain step
%      
%        the 2nd column the corresponding amplitude of received S21
%
%        the 3rd the phase of received S21
%
%        _x and _y respectively with _x or _y polarized Tx antenna
%
%S21_x and S21_y need to be imported from the results of the measurements
%in the anechoic chamber
%
%The script works for a 2D measurement
%
% NOTE: It gives wrong results only when alpha = 0 or 180 !!! --> Still to solve
% it
%
% Author: Luigi Vallozzi
% Last Modified: 26/06/09
%-------------------------------------------------------------------------
clear
close all

%--------------FILES AND FOLDER SETTINGS-----------------------

%meas_folder_main = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa_jacket_front_bot-no_bot_meas_29_10_08';
meas_folder_main = 'C:\Program Files\MATLAB71\work\Measurement_Scripts_New\dppa_jack_back_port1_bot_meas_14_11_08';

meas_subfolder = 'measurements_az_plane';

%do not change this:
meas_folder = [meas_folder_main,'\',meas_subfolder];

%file_name (without extension .txt)
%meas_file_name = 'dppa_jacket_no_bot_port2';
meas_file_name = 'dppa_2_back_jack_port2_nobot'

meas_file = [meas_folder,'\',meas_file_name,'.txt'];
%---------------------------------------------------------------

%------------------------------
% Input data aquisition
%------------------------------
%azimuth index of maximum gain
load([meas_folder_main,'\processed_meas_and_plots\',meas_file_name, '_gain_XZ_Gm_LW']);
ind_max = I;

%--------------------------------
% Set output folder path and name
%--------------------------------
output_folder = [meas_folder_main,'\processed_meas_and_plots'];




[S21_pol_0,azimuth,roll,frequency, polarization_0] = parser(meas_file, 'Polarization', 0);
[S21_pol_90,azimuth,roll,frequency, polarization_90] = parser(meas_file, 'Polarization', 90);

%build_vect_ellipt_pol;

S21_y(:,1) = azimuth;
S21_y(:,2) = (abs(S21_pol_0)).';
S21_y(:,3) = (angle(S21_pol_0)*180/pi).';

S21_x(:,1) = azimuth;
S21_x(:,2) = (abs(S21_pol_90)).';
S21_x(:,3) = (angle(S21_pol_90)*180/pi).';


S21_x_mod = S21_x(:,2);
S21_y_mod = S21_y(:,2);

S21_x_re = S21_x_mod.*cos(S21_x(:,3)*pi/180);
S21_x_im = S21_x_mod.*sin(S21_x(:,3)*pi/180);
S21_y_re = S21_y_mod.*cos(S21_y(:,3)*pi/180);
S21_y_im = S21_y_mod.*sin(S21_y(:,3)*pi/180);

tan_2phi=2*(S21_x_re.*S21_x_im + S21_y_re.*S21_y_im)./(S21_x_re.^2 - S21_x_im.^2 + S21_y_re.^2 - S21_y_im.^2);

phi = atan(tan_2phi)/2*180/pi;

phi = phi+90;

phi_x=S21_x(:,3) - phi;
phi_y=S21_y(:,3) - phi;

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



%direction of maximum gain, and read alpha, tau and AR
alpha_at_max = alpha(ind_max);
tau_at_max = tau(ind_max);
AR_LP_at_max = AR_LP(ind_max);

%look for the broadside direction, and read alpha, tau and AR
ind_az_0 = find(azimuth(:)==0);

alpha_bs = alpha(ind_az_0);
tau_bs = tau(ind_az_0);
AR_LP_bs = AR_LP(ind_az_0);

%look for the back scattering direction, and read alpha tau and AR
ind_az_180 = find(azimuth(:)==180);

alpha_back = alpha(ind_az_180);
tau_back = tau(ind_az_180);
AR_LP_back = AR_LP(ind_az_180);


%correction of alpha(list), tau(list), alpha_bs, and tau_bs, alpha_back and tau_back, alpha_at_max
%and tau_at_max
alpha = -alpha
for ind=1:length(alpha)
    if alpha(ind)<0
        alpha(ind)=alpha(ind)+180;
    end
    tau(ind) = 1/tau(ind);
end


alpha_bs = -alpha_bs;
if alpha_bs<0
    alpha_bs=alpha_bs+180;
end
tau_bs = 1/tau_bs;


alpha_back = -alpha_back;
if alpha_back<0
    alpha_back=alpha_back+180;
end
tau_back = 1/tau_back;


alpha_at_max = -alpha_at_max;
if alpha_at_max<0
    alpha_at_max=alpha_at_max+180;
end
tau_at_max = 1/tau_at_max;



%correction to apply to have alpha always referred to the major axis
%and thus tau always <1
for ind=1:length(tau)
    if abs(tau(ind)) > 1
        alpha(ind) = alpha(ind)+90;
        tau(ind) = 1/tau(ind);
    end
end
    

if abs(tau_bs) > 1
    alpha_bs = alpha_bs+90;
    tau_bs = 1/tau_bs;
end

if abs(tau_back) > 1
    alpha_back = alpha_back+90;
    tau_back = 1/tau_back;
end

if abs(tau_at_max) > 1
    alpha_at_max = alpha_at_max+90;
    tau_at_max = 1/tau_at_max;
end

% display('Parameters polarization at dir. of maximum:')
% display(['alpha = ', num2str(alpha_at_max)])
% display(['tau = ', num2str(tau_at_max)])
% display(['AR_LP = ', num2str(AR_LP_at_max), ' dB'])

display('Parameters polarization at broadside:')
display(['alpha = ', num2str(alpha_bs)])
display(['tau = ', num2str(tau_bs)])
display(['AR_LP = ', num2str(AR_LP_bs), ' dB'])
% 
% display('')
% display('Parameters polarization at backscattering:')
% display(['alpha = ', num2str(alpha_back)])
% display(['tau = ', num2str(tau_back)])
% display(['AR_LP = ', num2str(AR_LP_back), ' dB'])




%draw the polarization ellipse for broadside(normalized with respect to the major axis)
angles = 0:2*pi/500:2*pi;
Fx = cos(alpha_bs*pi/180)*cos(angles) + tau_bs*sin(alpha_bs*pi/180)*sin(angles);
Fy = sin(alpha_bs*pi/180)*cos(angles) - tau_bs*cos(alpha_bs*pi/180)*sin(angles);
 %normalization
if abs(tau_bs)>=1
    Fx = Fx/abs(tau_bs);
    Fy = Fy/abs(tau_bs);
end

figure;
plot(Fx, Fy)
title('polarization ellipse, broadside')
% if tau_bs<=1
    xlim([-1,1])
    ylim([-1,1])
% else
%     xlim([-tau_bs,tau_bs])
%     ylim([-tau_bs,tau_bs])
% end
legend(['alpha = ',num2str(alpha_bs), ', tau = ', num2str(tau_bs)])



%draw the polarization ellipse for backscattering(normalized with respect to the major axis)
angles = 0:2*pi/500:2*pi;
Fx = cos(alpha_back*pi/180)*cos(angles) + tau_back*sin(alpha_back*pi/180)*sin(angles);
Fy = sin(alpha_back*pi/180)*cos(angles) - tau_back*cos(alpha_back*pi/180)*sin(angles);
 %normalization
if abs(tau_back)>=1
    Fx = Fx/abs(tau_back);
    Fy = Fy/abs(tau_back);
end

figure;
plot(Fx, Fy)
title('polarization ellipse, backscattering')
% if tau_bs<=1
    xlim([-1,1])
    ylim([-1,1])
% else
%     xlim([-tau_bs,tau_bs])
%     ylim([-tau_bs,tau_bs])
% end
legend(['alpha = ',num2str(alpha_back), ', tau = ', num2str(tau_back)])




 
%draw the polarization ellipse for dir of max(normalized with respect to the major axis)
angles = 0:2*pi/500:2*pi;
Fx = cos(alpha_at_max*pi/180)*cos(angles) + tau_at_max*sin(alpha_at_max*pi/180)*sin(angles);
Fy = sin(alpha_at_max*pi/180)*cos(angles) - tau_at_max*cos(alpha_at_max*pi/180)*sin(angles);
 %normalization
if abs(tau_at_max)>=1
    Fx = Fx/abs(tau_at_max);
    Fy = Fy/abs(tau_at_max);
end

figure;
plot(Fx, Fy)
title('polarization ellipse, at direction of max.')
% if tau_bs<=1
    xlim([-1,1])
    ylim([-1,1])
% else
%     xlim([-tau_bs,tau_bs])
%     ylim([-tau_bs,tau_bs])
% end
legend(['alpha = ',num2str(alpha_at_max), ', tau = ', num2str(tau_at_max)])


%save patterns alpha, tau and AR_LP, and corresponding angles
save([output_folder,'\',meas_file_name,'_pol_param'], 'alpha', 'tau', 'AR_LP', 'azimuth');

