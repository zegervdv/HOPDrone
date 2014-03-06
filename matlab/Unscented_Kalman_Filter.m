% HOP 2014
% Unscented Kalman Filter

% Constants
steps = 100; % Number of timesteps
Nx = 6;
kappa = 2;
alpha = 1e-3;
beta = 0;
Fs = 1e3;
deltaT = 1 / Fs;
var_u = 1 * eye(Nx/2); % Noise variance on prediction: 1m
std_meas = 5/1e2; % Standard deviation on measurement 5cm

% Anchor points
Na = 4;
a1 = [0,0,20];
a2 = [20,0,0];
a3 = [0,20,0];
a4 = [20,20,20];
a5 = [0,0,0];
a6 = [0,20,20];
a7 = [20,0,20];
a8 = [20,20,0];
anch = [a1; a2; a3; a4];

% Movement and measurements
pos = [1:steps; 1:steps; zeros(1,steps)] / 10;
estimate = zeros(Nx,steps);

z = zeros(steps, Na);
for i = 1:steps
    for j = 1:Na
        z(i,j) = norm(pos(:,i)' - anch(j,:));
    end
end

% Assume start at (0,0,0) at 0 velocity
prevX = zeros(Nx, 1);
X = zeros(Nx, 1);

var = 5 * eye(Nx);


% N = 6 => 13 sigma points
sigma = zeros(2*Nx + 1, Nx);

% Weights
Wm = ones(1, 2*Nx + 1);
Wc = ones(1, 2*Nx + 1);

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

Z = zeros(Na, 2*Nx + 1);

R = std_meas^2 * eye(Na);

for k = 1:steps
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
    Y = F * sigma';
    for i = 1:Na
       for j = 1:2*Nx+1
        Z(i,j) = norm(sigma(j,1:3) - anch(i,:));
       end
    end
    
    Z = Z + std_meas * randn(Na, 2*Nx+1);
    
    Ef = mkmin;
    Eh = Wm * Z';
    
    cov_fh = zeros(Nx, Na);
    for i = 1:2*Nx+1
        cov_fh = cov_fh + Wc(i) * (Y(:,i) - Ef) * (Z(:,i) - Eh')';
    end
    
    varh = zeros(Na,Na);
    for i = 1:2*Nx+1
        varh = varh + Wc(i) * (Z(:,i) - Eh')' * (Z(:,i) - Eh');
    end
    
    K = cov_fh / (varh + R);
    mu = Eh;
    mk = mkmin + K * (z(k,:) - mu)';
    Pk = Pkmin - K * (varh + R) * K';
    
    % Update var and prevX
    prevX = mk;
    estimate(:,k) = mk;
    var = Pk
end

plot(pos(1,:),pos(2,:))
hold on;
plot(estimate(1,:),estimate(2,:),'r')
