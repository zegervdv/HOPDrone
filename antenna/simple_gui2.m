function simple_gui2
close all;
%  Initialize and hide the GUI as it is being constructed.
f = figure('Visible','off','Position',[50,50,1200,700]);
% Construct the components.
% make string for menu
% results_evaluated=evalin('base','results_evaluated');
% optimresults=evalin('base','optimresults');
% nPoints = length(results_evaluated);
% c=cell(1,nPoints);
% for l=1:nPoints
% c{l}=int2str(l);
% end
% hpopup = uicontrol('Style','popupmenu',...
%            'String',c,...
%            'Position',[700,500,100,25],'Callback',{@popup_menu_Callback});

%make axes handles for plots
resultFreq=0;
results=0;
PathName='C:\';
uniqueFreq=0;
selectedFreqIndex=1;
selectedRollIndex=1;
selectedHorn=1.1;
nFreq=0;
roll=0;
%make export buttons for plotted data
ha1ExT = uicontrol('Style','push','String','Export tab delimited',...
    'Position',[315,410,100,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha1ExTCallback},'HorizontalAlignment','left');
ha2ExT = uicontrol('Style','push','String','Export tab delimited',...
    'Position',[615,410,100,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha2ExTCallback},'HorizontalAlignment','left');
ha3ExT = uicontrol('Style','push','String','Export tab delimited',...
    'Position',[915,410,100,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha3ExTCallback},'HorizontalAlignment','left');
ha4ExT = uicontrol('Style','push','String','Export tab delimited',...
    'Position',[315,60,100,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha4ExTCallback},'HorizontalAlignment','left');
ha5ExT = uicontrol('Style','push','String','Export tab delimited',...
    'Position',[615,60,100,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha5ExTCallback},'HorizontalAlignment','left');
ha6ExT = uicontrol('Style','push','String','Export all in .mat',...
    'Position',[15,60,100,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha6ExTCallback},'HorizontalAlignment','left');
ha1ExF = uicontrol('Style','push','String','Open in new window',...
    'Position',[450,410,120,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha1ExFCallback},'HorizontalAlignment','left');
ha2ExF = uicontrol('Style','push','String','Open in new window',...
    'Position',[750,410,120,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha2ExFCallback},'HorizontalAlignment','left');
ha3ExF = uicontrol('Style','push','String','Open in new window',...
    'Position',[1050,410,120,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha3ExFCallback},'HorizontalAlignment','left');
ha4ExF = uicontrol('Style','push','String','Open in new window',...
    'Position',[450,65,120,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha4ExFCallback},'HorizontalAlignment','left');
ha5ExF = uicontrol('Style','push','String','Open in new window',...
    'Position',[750,65,120,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha5ExFCallback},'HorizontalAlignment','left');
ha4ExPolar = uicontrol('Style','push','String','Open polar plot',...
    'Position',[450,50,120,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha4ExPolarCallback},'HorizontalAlignment','left');
ha5ExPolar = uicontrol('Style','push','String','Open polar plot',...
    'Position',[750,50,120,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha5ExPolarCallback},'HorizontalAlignment','left');
ha6Ex3D = uicontrol('Style','push','String','Open 3D plot',...
    'Position',[1050,50,120,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@ha6Ex3DCallBack},'HorizontalAlignment','left');


%make file input blocks
hRef0.De  = uicontrol('Style','text','String','0 degree reference measurement file:',...
           'Position',[15,675,200,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
hRef0.InText  = uicontrol('Style','edit','Position',[15,660,200,15],...
    'HorizontalAlignment','left');
hRef0.InBrowse  = uicontrol('Style','push','String','...',...
    'Position',[215,660,15,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@hRefInBrowseCallback},'HorizontalAlignment','left');
hRef90.De  = uicontrol('Style','text','String','90 degree reference measurement file:',...
           'Position',[15,645,200,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
hRef90.InText  = uicontrol('Style','edit','Position',[15,630,200,15],...
    'HorizontalAlignment','left');
hRef90.InBrowse  = uicontrol('Style','push','String','...',...
    'Position',[215,630,15,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@hRef90InBrowseCallback},'HorizontalAlignment','left');
hMidas.De  = uicontrol('Style','text','String','Midas ASCII file:',...
           'Position',[15,615,200,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
hMidas.InText  = uicontrol('Style','edit','Position',[15,600,200,15],...
    'HorizontalAlignment','left');
hMidas.InBrowse  = uicontrol('Style','push','String','...',...
    'Position',[215,600,15,15],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@hMidasInBrowseCallback},'HorizontalAlignment','left');
hDis.De  = uicontrol('Style','text','String','Distance between AUT and horn:',...
           'Position',[15,585,200,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
hDis.InText  = uicontrol('Style','edit','Position',[15,570,200,15],...
    'HorizontalAlignment','left');
hRun=uicontrol('Style','push','String','Run',...
    'Position',[240,570,30,105],'BackgroundColor',[0.8 0.8 0.8],...
    'callback',{@hRunCallback},'HorizontalAlignment','left');

%make frequency selector block for radiation pattern data
c='freq';
hFreq.Select=uicontrol('Style','popupmenu','String',c,'Position',...
    [20,220,50,25],'Callback',{@hFreqSelectCallback});
hFreq.De=uicontrol('Style','text','String','Radiation pattern frequency:',...
    'Position',[20,250,150,15],'BackgroundColor',[0.8 0.8 0.8],...
    'HorizontalAlignment','left');
%make horn selector block
type={'1.1','1.7','2.6','3.9','5.8','8.2','12.4','UWB'};
hHorn.De  = uicontrol('Style','text','String','Select standard gain horn:',...
           'Position',[15,555,200,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
hHorn.Select=uicontrol('Style','popupmenu','String',type,'Position',...
    [15,540,50,15],'Callback',{@hHornSelectCallback});
%efficiency display
hEff.De  = uicontrol('Style','text','String','Antenna efficiency:',...
           'Position',[15,455,100,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
hEff.V  = uicontrol('Style','text','String','0',...
           'Position',[115,455,50,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
       
hEff2.De  = uicontrol('Style','text','String','Antenna efficiency:',...
           'Position',[15,425,100,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
hEff2.V  = uicontrol('Style','text','String','0',...
           'Position',[115,425,50,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
%figure titles
hTitle.s11Freq = uicontrol('Style','text','String','S11 vs. frequency',...
           'Position',[315,655,250,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','center');
hTitle.gainFreq = uicontrol('Style','text','String','Gain vs. frequency',...
           'Position',[615,655,250,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','center');
hTitle.arFreq = uicontrol('Style','text','String','Axial ratio vs. frequency',...
           'Position',[915,655,250,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','center');
hTitle.gain = uicontrol('Style','text','String','Gain vs. azimuth angle',...
           'Position',[315,355,250,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','center');
hTitle.ar = uicontrol('Style','text','String','Axial ratio vs. azimuth angle',...
           'Position',[615,355,250,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','center');
%slider to select roll angle
hSlider.gain = uicontrol('Style','slider','min',0,'max',10,'value',5,...
           'Position',[20,180,250,10],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','center','callback',{@hSlidergainCallback});
hSlider.gainDe1 = uicontrol('Style','text','String','Roll angle: ',...
           'Position',[20,200,50,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
hSlider.gainDe2 = uicontrol('Style','text','String','0',...
           'Position',[70,200,20,15],'BackgroundColor',[0.8 0.8 0.8],...
           'HorizontalAlignment','left');
%figure axes
ha1 = axes('Units','pixels','Position',[315,450,250,200]);
ha2 = axes('Units','pixels','Position',[615,450,250,200]);
ha3 = axes('Units','pixels','Position',[915,450,250,200]);
ha4 = axes('Units','pixels','Position',[315,150,250,200]);
ha5 = axes('Units','pixels','Position',[615,150,250,200]);
ha6 = axes('Units','pixels','Position',[915,150,250,200]);
% Assign the GUI a name to appear in the window title.
set(f,'Name','VNA/Midas Measurement Result Browser')
% Move the GUI to the center of the screen.
movegui(f,'center')
% Make the GUI visible.
set(f,'Visible','on');
%  Browse button callbacks
    function hRefInBrowseCallback(varargin)
        [FileName,PathName] = uigetfile('*.cti','Select the 0 degree reference measurement file',PathName);
        if FileName
            set(hRef0.InText,'String',fullfile(PathName,FileName));
        else
            clear PathName
        end
    end
    function hRef90InBrowseCallback(varargin)
        [FileName,PathName] = uigetfile('*.cti','Select the 90 degree reference measurement file',PathName);
        if FileName
            set(hRef90.InText,'String',fullfile(PathName,FileName));
        else
            clear PathName
        end
    end
    function hMidasInBrowseCallback(varargin)
        [FileName,PathName] = uigetfile('*.txt','Select the Midas ASCII file',PathName);
        if FileName
            set(hMidas.InText,'String',fullfile(PathName,FileName));
        else
            clear PathName
        end
    end
%run button callback
    function hRunCallback(varargin)
        clear results;
        selectedFreqIndex=1;
        %collect gui input data
        ref0=get(hRef0.InText,'string');
        ref90=get(hRef90.InText,'string');
        midas=get(hMidas.InText,'String');
        d=str2num(get(hDis.InText,'string'));
        %calculate results vs frequency
        resultFreq = Axial_Ratio(ref0,ref90,d,selectedHorn,0);
        %calculate radiation pattern
        if strcmp(selectedHorn,'uwb')
            results=gain_pattern_uwbhorn(midas,ref0,ref90,d);
        else
            results=gain_pattern(midas,ref0,ref90,d);
            %results=gain_pattern_1(midas,ref0,d);
        end
        set(hEff.V,'String',num2str(results.efficiency(selectedFreqIndex)));
%         set(hEff2.V,'String',num2str(results.effi(selectedFreqIndex)));
%         Simple_gui2 did not function with this line, so I put it in
%         comment
        roll=unique(results.roll);
        rollLength=length(roll);
        set(hSlider.gain,'min',1);
        set(hSlider.gain,'max',rollLength);
        set(hSlider.gain,'value',1);
        if rollLength>1
            set(hSlider.gain,'sliderstep',[1/(rollLength-1) 1/(rollLength-1)]);
        end
        plot(ha1,resultFreq.freq,resultFreq.S22_pol_0,[resultFreq.freq(1) resultFreq.freq(end)],[-10 -10]);
        set(ha1,'xlim',[resultFreq.freq(1) resultFreq.freq(end)],'xgrid',...
            'on','ygrid','on','xtick',[resultFreq.freq(1):0.1:resultFreq.freq(end)]);
        plot(ha2,resultFreq.freq,resultFreq.gain_tot_dB);
        set(ha2,'xlim',[resultFreq.freq(1) resultFreq.freq(end)],'xgrid',...
            'on','ygrid','on','xtick',[resultFreq.freq(1):0.1:resultFreq.freq(end)]);
        plot(ha3,resultFreq.freq,resultFreq.AR_dB,[resultFreq.freq(1) resultFreq.freq(end)],[3 3]);
        set(ha3,'xlim',[resultFreq.freq(1) resultFreq.freq(end)],'xgrid',...
            'on','ygrid','on','xtick',[resultFreq.freq(1):0.1:resultFreq.freq(end)]);
        plot(ha4,results.azimuth,results.gain(selectedFreqIndex*selectedRollIndex,:)');
        set(ha4,'xlim',[-180 180],'xgrid','on','ylim',[-10 10],'ygrid','on');
        plot(ha5,results.azimuth,results.axial_ratio(selectedFreqIndex*selectedRollIndex,:)');
        set(ha5,'xlim',[-180 180],'xgrid','on','ylim',[0 15],'ygrid','on');
        %determine unique frequency points for radiation pattern and put
        %them in frequency drop down box
        uniqueFreq=unique(results.frequency);
        nFreq=length(uniqueFreq);
        c=cell(1,nFreq);
        for l=1:nFreq
            c{l}=uniqueFreq(l);
        end
        set(hFreq.Select,'string',c);
        %%3d plotje van de gain
        %gain voor juiste frequentie selecteren
        r=results.gain(selectedFreqIndex:nFreq:end,:);
        %normaliseren
        minGain=min(min(r));
        r=r-minGain;
        %convert data to cartesian coordinates
        rollCart=pi/180*roll';
        azimuthCart=pi/180*results.azimuth';
        azimuthLength=length(azimuthCart);
        rollCartRep=repmat(rollCart,1,azimuthLength);
        azimuthCartRep=repmat(azimuthCart,rollLength,1);
        x=-r.*sin(azimuthCartRep).*sin(rollCartRep);
        y=r.*sin(azimuthCartRep).*cos(rollCartRep);
        z=r.*cos(azimuthCartRep);
        mesh(ha6,x,y,z,r);
    end
% roll angle slider callback 
    function hSlidergainCallback(varargin)
        slider=varargin{1};
        sliderpos=get(slider,'value');
        selectedRollIndex=sliderpos;
        set(hSlider.gainDe2,'string',num2str(roll(selectedRollIndex)));
        plot(ha4,results.azimuth,results.gain(selectedFreqIndex*selectedRollIndex,:)');
        set(ha4,'xlim',[-180 180],'xgrid','on','ylim',[-10 10],'ygrid','on');
        plot(ha5,results.azimuth,results.axial_ratio(selectedFreqIndex*selectedRollIndex,:)');
        set(ha5,'xlim',[-180 180],'xgrid','on','ylim',[0 15],'ygrid','on');
    end
%  export tab delimited button callback
    function ha1ExTCallback(varargin)
        outputFileName='S11vsFreq.txt';
        dlmwrite(fullfile(PathName,outputFileName),[resultFreq.freq resultFreq.S22_pol_0],'\t');
    end
    function ha2ExTCallback(varargin)
        outputFileName='gainvsFreq.txt';
        dlmwrite(fullfile(PathName,outputFileName),[resultFreq.freq resultFreq.gain_tot_dB],'\t');
    end
    function ha3ExTCallback(varargin)
        outputFileName='ARvsFreq.txt';
        dlmwrite(fullfile(PathName,outputFileName),[resultFreq.freq resultFreq.AR_dB],'\t');
    end
    function ha4ExTCallback(varargin)
        outputFileName='gainvsAzimuth.txt';
        dlmwrite(fullfile(PathName,outputFileName),[results.azimuth results.gain(selectedFreqIndex*selectedRollIndex,:)'],'\t');
    end
    function ha5ExTCallback(varargin)
        outputFileName='ARvsAzimuth.txt';
        dlmwrite(fullfile(PathName,outputFileName),[results.azimuth results.axial_ratio(selectedFreqIndex*selectedRollIndex,:)'],'\t');
    end
    function ha6ExTCallback(varargin)
        exportName='results.mat';
        exportResults.gain=results.gain;
        exportResults.axial_ratio=results.axial_ratio;
        exportResults.efficiency=results.efficiency;
        exportResults.azimuth=results.azimuth;
        exportResults.roll=results.roll;
        exportResults.frequency=results.frequency;
        save(fullfile(PathName,exportName),'exportResults');
    end
%open in new window button callback
    function ha1ExFCallback(varargin)
        figure;
        plot(resultFreq.freq,resultFreq.S22_pol_0,[resultFreq.freq(1) resultFreq.freq(end)],[-10 -10]);
        set(gca,'xlim',[resultFreq.freq(1) resultFreq.freq(end)],'xgrid',...
            'on','ygrid','on','xtick',[resultFreq.freq(1):0.1:resultFreq.freq(end)]);
    end
    function ha2ExFCallback(varargin)
        figure;
        plot(resultFreq.freq,resultFreq.gain_tot_dB);
        set(gca,'xlim',[resultFreq.freq(1) resultFreq.freq(end)],'xgrid',...
            'on','ygrid','on','xtick',[resultFreq.freq(1):0.1:resultFreq.freq(end)]);
    end
    function ha3ExFCallback(varargin)
        figure;
        plot(resultFreq.freq,resultFreq.AR_dB,[resultFreq.freq(1) resultFreq.freq(end)],[3 3]);
        set(gca,'xlim',[resultFreq.freq(1) resultFreq.freq(end)],'xgrid',...
            'on','ygrid','on','xtick',[resultFreq.freq(1):0.1:resultFreq.freq(end)]);
    end
    function ha4ExFCallback(varargin)
        figure;
        plot(results.azimuth,results.gain(selectedFreqIndex*selectedRollIndex,:)');
        set(gca,'xgrid','on','ygrid','on');
    end
    function ha5ExFCallback(varargin)
        figure;
        plot(results.azimuth,results.axial_ratio(selectedFreqIndex*selectedRollIndex,:)');
        set(gca,'xgrid','on','ygrid','on');
    end
%open polar plot callback
    function ha4ExPolarCallback(varargin)
        figure;
        Dirplot(results.azimuth,results.gain(selectedFreqIndex*selectedRollIndex,:)','-k',[25,-15,4]);
    end
    function ha5ExPolarCallback(varargin)
        figure;
        Dirplot(results.azimuth,results.axial_ratio(selectedFreqIndex*selectedRollIndex,:)','-k',[25,-15,4]);
    end
%open 3D plot callback
    function ha6Ex3DCallBack(varargin)
        figure;
        %%3d plotje van de gain
        %gain voor juiste frequentie selecteren
        r=results.gain(selectedFreqIndex:nFreq:end,:);
        rollLength=length(roll);
        %normaliseren
        minGain=min(min(r));
        r=r-minGain;
        %convert data to cartesian coordinates
        rollCart=pi/180*roll';
        azimuthCart=pi/180*results.azimuth';
        azimuthLength=length(azimuthCart);
        rollCartRep=repmat(rollCart,1,azimuthLength);
        azimuthCartRep=repmat(azimuthCart,rollLength,1);
        x=-r.*sin(azimuthCartRep).*sin(rollCartRep);
        y=r.*sin(azimuthCartRep).*cos(rollCartRep);
        z=r.*cos(azimuthCartRep);
        mesh(x,y,z,r);
    end
%frequency selector callback
    function hFreqSelectCallback(varargin)
        %get selected frequency
        dropdown=varargin{1};
        selectedFreqIndex=get(dropdown,'Value');
        plot(ha4,results.azimuth,results.gain(selectedFreqIndex*selectedRollIndex,:)');
        set(ha4,'xgrid','on','ygrid','on');
        plot(ha5,results.azimuth,results.axial_ratio(selectedFreqIndex*selectedRollIndex,:)');
        set(ha5,'xgrid','on','ygrid','on');
        set(hEff.V,'String',num2str(results.efficiency(selectedFreqIndex)));
        
        rollLength=length(roll);
        %%3d plotje van de gain
        %gain voor juiste frequentie selecteren
        r=results.gain(selectedFreqIndex:nFreq:end,:);
        %normaliseren
        minGain=min(min(r));
        r=r-minGain;
        %convert data to cartesian coordinates
        rollCart=pi/180*roll';
        azimuthCart=pi/180*results.azimuth';
        azimuthLength=length(azimuthCart);
        rollCartRep=repmat(rollCart,1,azimuthLength);
        azimuthCartRep=repmat(azimuthCart,rollLength,1);
        x=-r.*sin(azimuthCartRep).*sin(rollCartRep);
        y=r.*sin(azimuthCartRep).*cos(rollCartRep);
        z=r.*cos(azimuthCartRep);
        mesh(ha6,x,y,z,r);
        
    end
%Horn selector callback
    function hHornSelectCallback(varargin)
        dropdown=varargin{1};
        %get selected horn
        selectedHornIndex=get(dropdown,'Value');
        horn={1.1,1.7,2.6,3.9,5.8,8.2,12.4,'uwb'};
        selectedHorn=horn{selectedHornIndex};
    end
%  Pop-up menu callback. Read the pop-up menu Value property to
%  determine which item is currently displayed and make it the
%  current data. This callback automatically has access to 
%  current_data because this function is nested at a lower level.
%    function popup_menu_Callback(source,eventdata) 
%       Determine the selected data set.
%       str = get(source, 'String');
%       val = get(source,'Value');
%       Set current data to the selected data set.
%       current_freq=results_evaluated{val}.data.freq;
%       current_S11 =results_evaluated{val}.data.S11';
%       current_angles=results_evaluated{val}.data.angles;
%       current_AR=results_evaluated{val}.data.AR;
%       plot(ha1,current_freq,current_S11,[current_freq(1) current_freq(end)],[-10 -10]);
%       plot(ha2,current_angles,current_AR(1,:),[current_angles(151) current_angles(151) current_angles(211) current_angles(211)],[0 3 3 0]);
%       plot(ha3,current_angles,current_AR(2,:),[current_angles(151) current_angles(151) current_angles(211) current_angles(211)],[0 3 3 0]);
%       plot(ha4,current_angles,current_AR(3,:),[current_angles(151) current_angles(151) current_angles(211) current_angles(211)],[0 3 3 0]);
%       current_S11check=results_evaluated{val}.S11Check;
%       current_ARcheck=results_evaluated{val}.ARCheck;
%       if current_S11check==1
%             set(hled1,'BackgroundColor',[0 1 0]);
%       else
%             set(hled1,'BackgroundColor',[1 0 0]);
%       end
%       if current_ARcheck==1
%             set(hled2,'BackgroundColor',[0 1 0]);
%       else
%             set(hled2,'BackgroundColor',[1 0 0]);
%       end
%       current_antenna_dimensions=optimresults.x(val,:);
% set(hPatchSizeVal,'String',num2str(current_antenna_dimensions(1)));
% set(hSlotSizeVal,'String',[num2str(current_antenna_dimensions(2)) ' x ' num2str(current_antenna_dimensions(3))]);
% set(hStubLengthVal,'String',num2str(current_antenna_dimensions(4)));
%    end

end