%sampleperiode
Ts;

%snelheid initieel resetten (op 0) en dan laten accumuleren
v_X; 
v_Y;
v_Z;
%posities volgens accelerometer -- initieel resetten (op x_bias,y_bias,z_bias) en dan laten accumuleren
Xa;
Ya;
Za;
%versnelling output accelerometer
a_X; 
a_Y; 
a_Z;

%huidige snelheid op basis van vorige snelheid en gemeten versnelling
v_X = v_X + Ts*a_X;
v_Y = v_Y + Ts*a_Y;
v_Z = v_Z + Ts*a_Z;
%huidige positie op basis van vorige positie en gemeten snelheid
Xa=Xa + v_X*Ts;
Ya=Ya + v_Y*Ts;
Za=Za + v_Z*Ts;

%output gyroscoop
yaw_rate;
pitch_rate;
roll_rate;
