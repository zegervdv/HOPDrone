close all;
    fileID = fopen('simulations/S11_UWB.txt');
    C = textscan(fileID,'%f32 %f32');
    fclose(fileID);
    
    fileID2 = fopen('simulations/Phase_UWB.txt');
    C2 = textscan(fileID2,'%f32 %f32');
    fclose(fileID2);
    
    fileID3 = fopen('simulations/gain_UWB.txt');
    C3 = textscan(fileID3,'%f32 %f32');
    fclose(fileID3);
    
    fileID4 = fopen('simulations/eff_UWB.txt');
    C4 = textscan(fileID4,'%f32 %f32');
    fclose(fileID4);
    
    figure(1)
    plot(C{1}./1e9,C{2});
%     title('S11 Reflection Parameter ADS-simulation');
    xlabel('Frequency [GHz]');
    ylabel('S11 [dB]');
    hold on;
    plot([min(C{1}./1e9) max(C{1}./1e9)],[-10 -10],'r');
    print('-depsc','S11_ADS_sim.eps');
    
    figure(2)
    plot(C2{1}./1e9,C2{2});
%     title('Phase Respons ADS-simulation');
    xlabel('Frequency [GHz]');
    ylabel('Phase [°]'); 
    print('-depsc','fase_response_ADS_sim.eps');
    
    figure(3)
    stem(C3{1}./1e9,C3{2});
    title('Gain ADS-simulation');
    xlabel('Frequency [GHz]');
    ylabel('Gain [dB]');
    hold on;
    plot([min(C3{1}./1e9) max(C3{1}./1e9)],[3 3],'r');
    
    
    figure(4)
    plot(C4{1}./1e9,C4{2});
    title('Efficiency ADS-simulation');
    xlabel('Frequency [GHz]');
    ylabel('Efficiency [%]');
    hold on;
    plot([3.1 3.1],[0 97.7],'r');
    plot([5.3 5.3],[0 50],'r');
    plot([min(C4{1}./1e9) 3.1],[97.7 97.7],'r'); 
    plot([min(C4{1}./1e9) 5.3],[50 50],'r'); 
