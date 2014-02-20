% HOP 2014
% Least Mean Squares Method

% Deviation of measurements in percentage
error = 0.05;

% Anchors
a1 = [0,0,200];
a2 = [200,0,0];
a3 = [0,200,0];
a4 = [200,200,200];
a5 = [0,0,0];
a6 = [0,200,200];
a7 = [200,0,200];
a8 = [200,200,0];
a = [a1 a2 a3 a4 a5 a6 a7 a8];
N = floor(length(a) / 3);
a = reshape(a,3,N)';

% Location
pos = [100,200,100];
pos_M = ones(N,1) * pos;

% Measured distances
Dsq = sum((((a - pos_M).*(1 + rand(N,3)*error)).^2)')';

x_N = ones(N-1,1) * a(N,:);

A = (-2)*(x_N - a(1:(N-1),:));

B = Dsq(N,:) - Dsq(1:(N-1),:) - sum(x_N.^2')' + sum(a(1:(N-1),:).^2')';

x = A \ B