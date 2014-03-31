
    fileID = fopen('S11_UWB.txt');
    C = textscan(fileID,'%f32 %f32');
    fclose(fileID);
    
    fileID2 = fopen('Phase_UWB.txt');
    C2 = textscan(fileID2,'%f32 %f32');
    fclose(fileID2);
    
    figure(1)
    plot(C{1}./1e9,C{2});
    title('Reflectie coëfficient ADS-simulatie');
    xlabel('Frequency [GHz]');
    ylabel('S11 [dB]');
    
    figure(2)
    plot(C2{1}./1e9,C2{2});
    title('Fase respons ADS-simulatie');
    xlabel('Frequency [GHz]');
    ylabel('Fase [°]');