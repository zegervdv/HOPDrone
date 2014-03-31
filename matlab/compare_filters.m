% HOP 2014
% Compare Least Mean Squares to Unscented Kalman Filter
close all

Nx = 6;
steps = 1000; % Number of timesteps
Nx = 6;
alpha = 1e-3;
beta = 2;
Fs = 5; % 1O Hz sampling
deltaT = 1 / Fs;
kappa = alpha^2*Nx - Nx;
var_u = 25e-2 * eye(Nx/2); % Noise variance on prediction: 1cm
std_meas = 10e-2; % Standard deviation on measurement 5cm

F = eye(Nx);
for i = 1:Nx/2
    F(i, Nx/2 + i) = deltaT;
end

Gup = eye(Nx/2) * deltaT^2/2;
Glo = eye(Nx/2) * deltaT;
G = [Gup; Glo];

% Assume start at (0,0,0) at 0 velocity
prevX = zeros(Nx, 1);

% Movement and measurements
estimate = zeros(Nx,steps);

pos = zeros(Nx,steps);
pos(:,1) = prevX;

for k = 2:steps
   pos(:,k) = F * pos(:,k-1) + G * randn(3,1);
end

lms = least_mean_squares(pos(1:3,:), 0.05);
ukf = Unscented_Kalman_Filter(pos, 0.05, F, G);

figure(1);
plot(pos(1,:), pos(2,:));
hold on
plot(lms(1,:), lms(2,:), 'r');
plot(ukf(1,:), ukf(2,:), 'k');
legend('Gevolgd', 'LMS', 'Kalman');
title('Pad');
xlabel('x coördinaat [m]');
ylabel('y coördinaat [m]');

figure(2);
err_lms = sum((lms - pos(1:3,:)).^2) / 3;
err_ukf = sum((ukf(1:3,:) - pos(1:3,:)).^2) / 3;

cdf_plot(err_lms, 'r');
hold on;
cdf_plot(err_ukf, 'k');
legend('LMS', 'Kalman');
title('Cumulatieve Distributiefunctie');
xlabel('Gemiddelde kwadratische fout [m^2]');
ylabel('Fractie [-]');