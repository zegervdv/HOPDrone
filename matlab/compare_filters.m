% HOP 2014
% Compare Least Mean Squares to Unscented Kalman Filter
close all

% Include accelerometer data

INC_ACC = 0;

% Constants

steps = 1000; % Number of timesteps
Nx = 9;
alpha = 1e-3;
beta = 2;
Fs = 5; % 5 Hz sampling
deltaT = 1 / Fs;
kappa = alpha^2*Nx - Nx;
std_meas = 10e-2; % Standard deviation on measurement 5cm

F_vector = [1 0 0 deltaT 0 0 deltaT^2/2 0 0];
F = zeros(Nx,Nx);
for i=1:Nx
    F(i,i:Nx) = F_vector(1:Nx-i+1);
end

Gup = eye(Nx/3) * deltaT^2/2;
Glo = eye(Nx/3) * deltaT;
G = [Gup; Glo; eye(3)];

% Assume start at (0,0,0) at 0 velocity
prevX = zeros(Nx, 1);

% Movement and measurements

pos = zeros(Nx,steps);
pos(:,1) = prevX;

for k = 2:steps
   pos(:,k) = F * pos(:,k-1) + G * 1e-3 * randn(3,1);
end

lms = least_mean_squares(pos(1:3,:), 0.05);
ukf = Unscented_Kalman_Filter(pos, 0.05, F, G, Nx,Fs, 0);
ukf_acc = Unscented_Kalman_Filter(pos, 0.05, F, G, Nx, Fs, 1);

figure(1);
plot(pos(1,:), pos(2,:));
hold on
plot(lms(1,:), lms(2,:), 'r');
plot(ukf(1,:), ukf(2,:), 'k');
plot(ukf_acc(1,:), ukf_acc(2,:), 'm');

speed = sqrt(pos(4,:).^2 + pos(5,:).^2 + pos(6,:).^2);
max(speed)
mean(speed)

figure(2);
err_lms = sum((lms - pos(1:3,:)).^2) / 3;
err_ukf = sum((ukf(1:3,:) - pos(1:3,:)).^2) / 3;
err_ukf_acc = sum((ukf_acc(1:3,:) - pos(1:3,:)).^2) / 3;


cdf_plot(err_lms, 'r');
hold on;
cdf_plot(err_ukf, 'k');
cdf_plot(err_ukf_acc, 'm');

