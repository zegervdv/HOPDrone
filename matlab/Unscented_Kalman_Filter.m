% HOP 2014
% Unscented Kalman Filter

% Constants
Nx = 6;
kappa = 2;
alpha = 1e-3;
beta = 0;
Fs = 1e3;
deltaT = 1 / Fs;

var_u = 1 * eye(Nx/2); % Noise variance on prediction: 1m

% Assume start at (0,0,0) at 0 velocity
prevX = zeros(Nx, 1);
X = zeros(Nx, 1);

var = 5 * eye(Nx);

% N = 6 => 13 sigma points
sigma = zeros(2*Nx + 1, Nx);

% Weights
Wm = ones(1, Nx + 1);
Wc = ones(1, Nx + 1);

Wm = 1/(2*(Nx + kappa)) * Wm;
Wc = 1/(2*(Nx + kappa)) * Wc;

Wm(1) = kappa/(Nx + kappa);
Wc(1) = kappa/(Nx + kappa) + (1 - alpha^2 + beta);

% Assume function F to be linear
F = eye(Nx);
for i = 1:Nx/2
    F(i, Nx/2 + i) = deltaT;
end

Gup = eye(Nx/2) * deltaT^2/2;
Glo = eye(Nx/2) * deltaT;
G = [Gup; Glo];


for k = 1:100
    % Calculate sigmapoints
    sigma(1, :) = prevX';
    root = sqrt(Nx + kappa) * chol(var, 'lower');
    for i = 2:Nx+1
       sigma(i, :) = prevX' + root(i-1, :);
       sigma(Nx + i, :) = prevX' - root(i-1, :);
    end
    
    % Prediction
    mkmin = F * prevX;
    Pkmin = F * var * F' + G * var_u * G';
    
    % Measurement update
    Y = F * sigma'
end