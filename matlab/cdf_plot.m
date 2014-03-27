function [ cdf ] = cdf_plot( err, opts )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
steps = size(err,2);
cdf = zeros(1, ceil(max(err))*10);
err_point = [0:.01:ceil(max(err))];

for i = 1:length(err_point)
    cdf(i) = sum(err <= err_point(i)) / steps;
end

plot(err_point, cdf, opts)
end

