% Compare original and new antennas

[origFreq, origS11] = VNA_parser_single('measurements/reflections/s11_ref.cti');
[patchFreq, patchS11] = VNA_parser_single('measurements/reflections/s11_patch_stairs_v1.cti');
[patch2Freq, patch2S11] = VNA_parser_single('measurements/reflections/s11_patch_stairs.cti');

fileID = fopen('simulations/S11_UWB.txt');
C = textscan(fileID,'%f32 %f32');
fclose(fileID);

fileID = fopen('simulations/patch_stairs_v2.txt');
C2 = textscan(fileID,'%f32 %f32');
fclose(fileID);

close all
figure(1);
subplot(2,1,1);
title('Version 1')
plot(origFreq, 10*log(abs(origS11)));
hold on;
plot(patchFreq, 10*log(abs(patchS11)),'r');
plot(C{1}*1e-9, C{2}, 'k');

legend('Reference', 'Patch','Simulation','Location','SouthEast');
grid on;
ylim([-50,0]);
xlim([3,6]);
xlabel('Frequency [GHz]');
ylabel('S11 Magnitude [dB]');

subplot(2,1,2);
title('Version 2')
plot(origFreq, 10*log(abs(origS11)));
hold on
plot(patch2Freq, 10*log(abs(patch2S11)),'r');
plot(C2{1}*1e-9, C2{2}, 'k');
legend('Reference', 'Patch','Simulation','Location','SouthEast');

grid on;
ylim([-50,0]);
xlim([3,6]);
xlabel('Frequency [GHz]');
ylabel('S11 Magnitude [dB]');

print('-depsc', 'antenna_comparison')
