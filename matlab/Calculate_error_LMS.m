mean_errors = zeros(1,21);
proc_error_on_dist = zeros(1,21);
for i=1:21
    sum = 0;
    for j=1:101
        sum = sum + least_mean_squares((i)/100);
    end
    mean_errors(i) = sum/100;
    proc_error_on_dist(i) = i;
end

plot(proc_error_on_dist,mean_errors)
title('Mean error on position resulting from inaccurate distance measurement');
xlabel('Procentual error on distance measurement');
ylabel('Procentual error on calculated position');
grid on;
set(axh,'GridLineStyle','-');
