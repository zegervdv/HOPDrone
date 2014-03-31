close all;
    fileID = fopen('S11_UWB.txt');
    C = textscan(fileID,'%f32 %f32');
    fclose(fileID);
    
    fileID2 = fopen('Phase_UWB.txt');
    C2 = textscan(fileID2,'%f32 %f32');
    fclose(fileID2);
    
    fileID3 = fopen('gain_UWB.txt');
    C3 = textscan(fileID3,'%f32 %f32');
    fclose(fileID3);
    
    fileID4 = fopen('eff_UWB.txt');
    C4 = textscan(fileID4,'%f32 %f32');
    fclose(fileID4);
    
    figure(1)
    plot(C{1}./1e9,C{2});
    title('Reflectie coëfficient ADS-simulatie');
    xlabel('Frequentie [GHz]');
    ylabel('S11 [dB]');
    
    figure(2)
    plot(C2{1}./1e9,C2{2});
    title('Fase respons ADS-simulatie');
    xlabel('Frequentie [GHz]');
    ylabel('Fase [°]');
    
    figure(3)
    stem(C3{1}./1e9,C3{2});
    title('Gain ADS-simulatie');
    xlabel('Frequentie [GHz]');
    ylabel('Gain [dB]');
    hold on;
    plot([min(C3{1}./1e9) max(C3{1}./1e9)],[3 3],'r');
    
    
    figure(4)
    plot(C4{1}./1e9,C4{2});
    title('Efficiëntie ADS-simulatie');
    xlabel('Frequentie [GHz]');
    ylabel('Efficiëntie [%]');
    hold on;
    plot([3.1 3.1],[0 97.7],'r');
    plot([5.3 5.3],[0 50],'r');
    plot([min(C4{1}./1e9) 3.1],[97.7 97.7],'r'); 
    plot([min(C4{1}./1e9) 5.3],[50 50],'r'); 