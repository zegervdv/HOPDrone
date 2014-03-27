% HOP 2014
% Unscented Kalman Filter
function [estimate] = Unscented_Kalman_Filter(pos, std_meas, F, G, Nx, Fs, INC_ACC)
close all;

% Constants
steps = size(pos,2); % Number of timesteps
alpha = 1e-3;
beta = 2;
kappa = alpha^2*Nx - Nx;
var_u = 25e-2 * eye(Nx/3); % Noise variance on prediction: 1cm
deltaT = 1/Fs;

% Anchor points
Na = 4;
a1 = [0,0,2];
a2 = [2,0,0];
a3 = [0,2,0];
a4 = [2,2,2];
a5 = [0,0,0];
a6 = [0,2,2];
a7 = [2,0,2];
a8 = [2,2,0];
anch = [a1; a2; a3; a4] * 10;

if INC_ACC == 1
% Add 3D accelerometer measurements
Na = Na + 3;
end 

var = eye(Nx);

% N = 6 => 13 sigma points
sigma = zeros(2*Nx + 1, Nx);

% Weights
Wm = ones(1, 2*Nx + 1);
Wc = ones(1, 2*Nx + 1);

Wm = 1/(2*(Nx + kappa)) * Wm;
Wc = 1/(2*(Nx + kappa)) * Wc;

Wm(1) = kappa/(Nx + kappa);
Wc(1) = kappa/(Nx + kappa) + (1 - alpha^2 + beta);

Z = zeros(Na, 2*Nx + 1);

R = std_meas^2 * eye(Na); % Wrong, last 3 need different variance, because accelerometer noise

% Assume start at (0,0,0) at 0 velocity
prevX = zeros(Nx, 1);

% Movement and measurements
estimate = zeros(Nx,steps);

z = zeros(steps, Na);
if INC_ACC == 1
for i = 1:steps
    for j = 1:Na-3
        z(i,j) = norm(pos(1:3,i)' - anch(j,:)) + std_meas*randn(1);
    end
    for j = Na-2:Na
        % TODO: Add measurement noise and quantise
        z(i,j) = pos(j+2,i) + 2*std_meas*randn(1);
    end
end
else
for i = 1:steps
    for j = 1:Na
        z(i,j) = norm(pos(1:3,i)' - anch(j,:)) + std_meas*randn(1);
    end
end    
    
end

plot(pos(1,:),pos(2,:))

for k = 1:steps
    % Prediction
    mkmin = F * prevX;
    Pkmin = F * var * F' + G * var_u * G';

    % Calculate sigmapoints
    prevSigma = sigma;
    sigma(1, :) = mkmin';
    root = sqrt(Nx + kappa) * cholcov(Pkmin);
    for i = 2:Nx+1
       sigma(i, :) = mkmin' + root(i-1, :);
       sigma(Nx + i, :) = mkmin' - root(i-1, :);
    end

    % Measurement update
    Y = sigma';
if INC_ACC == 1
    for i=1:2*Nx+1
      for j=1:Na-3
        Z(j,i) = norm(sigma(i,1:3) - anch(j,:));
      end
      for j=Na-2:Na
        % Use Sigmapoints as estimates for acceleration
        %Z(j,i) = sigma(i,j+2);
        
        % TODO: Replace 4 by variable number of amchor points
        Z(j,i)= 0.5*(sigma(i,j-4) - prevSigma(i,j-4))*deltaT^2;
      end
    end
else
    for i=1:2*Nx+1
      for j=1:Na
        Z(j,i) = norm(sigma(i,1:3) - anch(j,:));
      end
    end
end
    Ef = mkmin;
    Eh = Wm * Z';

    cov_fh = zeros(Nx, Na);
    for i = 1:2*Nx+1
        cov_fh = cov_fh + Wc(i) * (Y(:,i) - Ef) * (Z(:,i) - Eh')';
    end

    varh = zeros(Na,Na);
    for i = 1:2*Nx+1
        varh = varh + Wc(i) * (Z(:,i) - Eh') * (Z(:,i) - Eh')';
    end

    K = cov_fh / (varh + R);
    mu = Eh;
    mk = mkmin + K * (z(k,:) - mu)';
    Pk = Pkmin - K * (varh + R) * K';

    % Update var and prevX
    prevX = mk;
    estimate(:,k) = mk;
    var = Pk;
    var = (var + var') / 2;
end
end


