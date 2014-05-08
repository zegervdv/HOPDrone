
%E1_uncovered=gain_vs_frequency('E1_uncovered_pol0.cti','E1_uncovered_pol90.cti',4.25)
E1_uncovered=gain_vs_frequency('E:\ARMOURS\on_body\E1_ref_pol0_a.cti','E:\ARMOURS\on_body\E1_ref_pol90_a.cti',4.1)

figure1=figure(1)

plot(E1_uncovered.frequency,E1_uncovered.gain,'-b','Linewidth',2)

h=gca(figure1)
set(h,'Fontsize',16)
set(h,'XLim',[1.1 1.7])
set(h,'YLim',[-18 9])
set(h,'XTick',[1.1 1.2 1.3 1.4 1.54 1.61 1.7])
set(h,'YTick',[-18 -15 -12 -9 -6 -3 0 3 6 9])
set(h,'Position',[0.13 0.11 0.775 0.815])

grid on
xlabel('Frequency [GHz]','Fontsize',16)
ylabel('G [dBi]','Fontsize',16)

xlimit=(1.54-1.1)/(1.7-1.1);
ylimit=(18+3)/(18+9);

height=(9-3)/(18+9);
width=(1.61-1.54)/(1.7-1.1);

 annotation(figure1,'rectangle',...
      'Position',[0.13+xlimit*0.775,0.11+ylimit*0.815,width*0.775,height*0.815],...
      'FaceAlpha',0.5,...
      'FaceColor',[0.3 0.3 0.3]);
  
  