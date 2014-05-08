
%E1_uncovered=gain_vs_frequency('E1_uncovered_pol0.cti','E1_uncovered_pol90.cti',4.25)
%E6_uncovered=gain_vs_frequency('E6_ref_pol0.cti','E6_ref_pol90.cti',4.19)
rollSelector=19;
%rollSelector=19;

midas='E:\ARMOURS\E6_20082013_1_300GHz_ascii.txt'
ref0='E:\ARMOURS\E6_ref_pol0.cti'
ref90='E:\ARMOURS\E6_ref_pol90.cti'
d=4.19;
results=gain_pattern(midas,ref0,ref90,d);
figure1=figure(1)

%plot(E1_uncovered.frequency,E1_uncovered.gain,'-b','Linewidth',2)
%plot(E6_uncovered.frequency,E6_uncovered.S22_cal_dB_pol0,'-b','Linewidth',2)
plot(results.azimuth,results.gain(rollSelector,:)');
gainAtZero=results.gain(rollSelector,37);
fprintf('Gain at azimuth = 0: %f \r\n',gainAtZero);
gainAtZeroMin3=results.gain(rollSelector,37)-3;

h=gca(figure1)
set(h,'Fontsize',16)
set(h,'XLim',[-180 180])
set(h,'YLim',[-20 5])
set(h,'XTick',[-180 -90 -30 0 30 90 180])
set(h,'YTick',[-20 -15 -10 -5 0 5])
set(h,'Position',[0.13 0.11 0.775 0.815])

grid on
xlabel('Azimuth angle [°]','Fontsize',16)
ylabel('Gain [dBi]','Fontsize',16)
title('Passive E6 antenna gain in YZ-plane (1.300 GHz)')

xlimit=(-30--180)/(180--180);
ylimit=(20+gainAtZeroMin3)/(20+5);

height=(5-gainAtZeroMin3)/(20+5);
width=(30--30)/(180--180);

 annotation(figure1,'rectangle',...
      'Position',[0.13+xlimit*0.775,0.11+ylimit*0.815,width*0.775,height*0.815],...
      'FaceAlpha',0.5,...
      'FaceColor',[0.3 0.3 0.3]);
  
  