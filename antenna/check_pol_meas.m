%QCN19 data
qcnData=dlmread('C:\Users\Arnaut\Documents\qcn19_amp_phase_unbal.txt');
%E6 antenna
file_pol0='E:\ARMOURS\E6_ref_pol0.cti';
file_pol90='E:\ARMOURS\E6_ref_pol90.cti';
[Freq_list,S11_0,S21_0,S12_0,S22_0]= VNA_parser(file_pol0);
[Freq_list,S11_90,S21_90,S12_90,S22_90]= VNA_parser(file_pol90);
e6_pol0=[S11_0,S21_0,S12_0,S22_0];
e6_pol90=[S11_90,S21_90,S12_90,S22_90];
resultFreq = Axial_Ratio(file_pol0,file_pol90,4.19,1.1,0);
close all
phaseDiff=90-(phase(e6_pol0(:,2))-phase(e6_pol90(:,2)))*180/pi;
magDiff=20*log10(abs(e6_pol0(:,2)))-20*log10(abs(e6_pol90(:,2)));
figure;
plot(Freq_list,magDiff,Freq_list,phaseDiff,resultFreq.freq,resultFreq.AR_dB,qcnData(:,1)/1000,qcnData(:,2),qcnData(:,1)/1000,90-qcnData(:,3));
%E1 antenna
file_pol0='E:\ARMOURS\E1_ref_pol0.cti';
file_pol90='E:\ARMOURS\E1_ref_pol90.cti';
[Freq_list,S11_0,S21_0,S12_0,S22_0]= VNA_parser(file_pol0);
[Freq_list,S11_90,S21_90,S12_90,S22_90]= VNA_parser(file_pol90);
e6_pol0=[S11_0,S21_0,S12_0,S22_0];
e6_pol90=[S11_90,S21_90,S12_90,S22_90];
resultFreq = Axial_Ratio(file_pol0,file_pol90,4.19,1.1,0);
phaseDiff=90-(phase(e6_pol0(:,2))-phase(e6_pol90(:,2)))*180/pi;
magDiff=20*log10(abs(e6_pol0(:,2)))-20*log10(abs(e6_pol90(:,2)));
figure;
plot(Freq_list,magDiff,Freq_list,phaseDiff,resultFreq.freq,resultFreq.AR_dB,qcnData(:,1)/1000,qcnData(:,2),qcnData(:,1)/1000,90-qcnData(:,3));