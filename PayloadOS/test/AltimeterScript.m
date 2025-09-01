clear all
close all

%%Input information 
%Gravititational Acceleration (m/s^2)
Gravity = -9.81;
%Thrust of Motor (m/s^2
Thrust = 48.265;
%Time after launch until burnout (s)
BurnOutTime = 3;
%Velocity of Drogue Coasting Phase (m/s)
DrogueCoastSpeed = -27.7368;
%Height at which Main will deploy (m)
MainCoastHeight = 175.26;
%Velocity of Main Coast Phase (m/s)
MainCoastSpeed = -6.096;
%Clock speed of both altimeters (1/s or Hz)
AltimeterPolling1 = 8;
AltimeterPolling2 = 8;
%Accuracy of Both Altimeters (Distance error from true reading) (m)
Accuracy1 = 1;
Accuracy2 = 1;
%Time Until Launch (s)
WaitTime = 10;
%Call function for Model Generation to Predetermine critical time and
%altitude values
[CriticalTimes,CritcalAltitudes] = ModelGeneration(Gravity,Thrust,BurnOutTime,DrogueCoastSpeed,MainCoastHeight,MainCoastSpeed);
%Call function for Altitude Data Generation for Pseudo-Sensor data from
%both altimeters
[AltitudeTimes1,AltitudeTimes2,Altimeter1,Altimeter2] = AltitudeDataGeneration(AltimeterPolling1,AltimeterPolling2,CriticalTimes,Gravity,Thrust,DrogueCoastSpeed,MainCoastSpeed,Accuracy1,Accuracy2,WaitTime);
%Conversions (s -> ms | m -> ft)
AltitudeTimes1 = AltitudeTimes1 .* 1000;
AltitudeTimes2 = AltitudeTimes2 .* 1000;
Altimeter1 = Altimeter1 .* 3.28084;
Altimeter2 = Altimeter2 .* 3.28084;
GeneratedData = GeneratedDataConverger(AltitudeTimes1,AltitudeTimes2,Altimeter1,Altimeter2);
%Plot of Altimeter 'Experienced' Altitudes from generated data versus time
figure(1)
plot(AltitudeTimes1,Altimeter1,AltitudeTimes2,Altimeter2)
grid on
title('Generated Altimeter Sensor Data')
xlabel('Time (ms)')
ylabel('Altitude (ft)')
legend('Altimeter 1','Altimeter 2')
%File Generation into a text file
writematrix(GeneratedData,'GeneratedAltimeterData.txt','Delimiter',' ');

