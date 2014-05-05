% HOP 2014
% Least Mean Squares Method
function[estimate] = least_mean_squares(pos, error)
% Deviation of measurements in percentage
% error = 0.05;
steps = size(pos,2);

% Anchors
a1 = [0,0,2];
a2 = [2,0,0];
a3 = [0,2,0];
a4 = [2,2,2];
a5 = [0,0,0];
a6 = [0,2,2];
a7 = [2,0,2];
a8 = [2,2,0];
a = [a1 a2 a3 a4] * 10;
Na = floor(length(a) / 3);
a = reshape(a,3,Na)'

% Location
estimate = zeros(3,steps);

for k = 1:steps
    pos_M = ones(Na,1) * pos(:,k)';
    % Measured distances
    Dsq = (sqrt(sum((a - pos_M).^2,2)) + error*randn(Na,1)).^2
    x_N = ones(Na-1,1) * a(Na,:);
    
    A = (-2)*(x_N - a(1:(Na-1),:));
    
    B = Dsq(Na,:) - Dsq(1:(Na-1),:) - sum(x_N.^2,2) + sum(a(1:(Na-1),:).^2,2)
    
    x = A \ B;
    estimate(:,k) = x';
end
end

