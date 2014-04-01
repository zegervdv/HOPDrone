% Compare original and new antennas

[origFreq, origS11, origS21, origS12, origS22] = VNA_parser_new('ref_meas2_dipole_board_pol0.cti');
[patchFreq, patchS11, patchS21, patchS12, patchS22] = VNA_parser_new('ref_meas_patch_pol0.cti');

close all
figure(1);
plot(origFreq, 20*log(abs(origS22)));
hold on;
plot(patchFreq, real(patchS22),'r');
legend('Original', 'Patch','Location','SouthEast');
plot([3,6],[-10,-10],'m');
ylim([-60,0]);