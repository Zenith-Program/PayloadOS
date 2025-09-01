function [AltitudeTimes1,AltitudeTimes2,Altitudes1,Altitudes2] = AltitudeDataGeneration(AltimeterPolling1,AltimeterPolling2,CriticalTimes,G,T,VT1,VT2,Accuracy1,Accuracy2,WaitTime)
%Output of this function will result in 4 separate matrices (1 x n) that
%records altitudes and times

% Variable Initialization
%5 distinct phases
%Launch to Burnout
StageAB = CriticalTimes(1:2);
%Burnout to Apogee
StageBC = CriticalTimes(2:3);
%Apogee to Effective Drogue Deployment (Falling at new terminal velocity
%from drogue parachute
StageCD = CriticalTimes(3:4);
%Effective Drogue Deployment to Effective Main Deployment
StageDE = CriticalTimes(4:5);
%Effective Main Deployment to landing
StageEF = CriticalTimes(5:6);
%Implementation of Wait Time inorder to expand the data matrix
%accordingly
Timing1 = -1 * WaitTime;
Timing2 = -1 * WaitTime;
%Time variables received from Model Generation in order to use the
%functions properly
TimeA = CriticalTimes(1);
TimeB = CriticalTimes(2);
TimeD = CriticalTimes(4);
TimeE = CriticalTimes(5);
TimeF = CriticalTimes(6);
%All Functions and Intermediates (DO NOT TOUCH)
Hab = @(t) ((t-TimeA)^2)*((T/2)+(G/2));
Hbc = @(t) (G/2)*((t + ((TimeB*(T+G))/G) - TimeB)^2) + (((TimeB^2)/2) * (T + G) + TimeB * (T + G) * (- ((TimeB*(T+G))/G)) + (G/2) * ((- ((TimeB*(T+G))/G))^2));
Hcd = @(t) (G/2)*((t + ((TimeB*(T+G))/G) - TimeB)^2) + (((TimeB^2)/2) * (T + G) + TimeB * (T + G) * (- ((TimeB*(T+G))/G)) + (G/2) * ((- ((TimeB*(T+G))/G))^2));
HcdD = Hcd(TimeD);
Hde = @(t) VT1 * (t - TimeD) + HcdD;
HdeE = Hde(TimeE);
Hef = @(t) VT2 * (t - TimeE) + HdeE;
%Initialization of Increments for both altimeters
i = 1;
j = 1;
%Initialization of Altitudes and Times for both altimeters
Altitudes1 = 0;
Altitudes2 = 0;
AltitudeTimes1 = 0;
AltitudeTimes2 = 0;
%While loop break conditions for both altimeters (Either it landed or an
%error has occured)
State1 = 0;
State2 = 0;
%Used for random noise functions (CHANGING THIS VALUE WILL MESS UP THE NOISE!!!)
Precision1 = 1;
Precision2 = 1;
%While loop nested with two separate if statements outputing altitudes for
%every interval of time progressing
while State1 ~= -1
    if Timing1 < 0
        AltitudeTimes1(i) = Timing1;
        Altitudes1(i) = 0;
    elseif Timing1 > TimeF
        %Altitude is 0
        AltitudeTimes1(i) = Timing1;
        Altitudes1(i) = 0;
        State1 = -1;
    elseif Timing1 >= StageAB(1) && Timing1 <= StageAB(2)
        Altitudes1(i) = Hab(Timing1);
    elseif Timing1 > StageBC(1) && Timing1 <= StageBC(2)
        Altitudes1(i) = Hbc(Timing1);
    elseif Timing1 > StageCD(1) && Timing1 <= StageCD(2)
        Altitudes1(i) = Hcd(Timing1);
    elseif Timing1 > StageDE(1) && Timing1 <= StageDE(2)
        Altitudes1(i) = Hde(Timing1);
    elseif Timing1 > StageEF(1) && Timing1 <= StageEF(2)
        Altitudes1(i) = Hef(Timing1);
    else
        disp('Error')
        State1 = -1;
    end
    if State1 ~= -1
        AltitudeTimes1(i) = Timing1;
        RNG1 = -1 + (1- (-1)).*rand(Precision1,1);
        %RNG1 = (2 * rand) - 1;
        Noise1 = RNG1 * Accuracy1;
        Altitudes1(i) = Altitudes1(i) + Noise1;
        Timing1 = Timing1 + 1 / AltimeterPolling1;
        i = i + 1;
    end
end
while State2 ~= -1
    if Timing2 < 0
        AltitudeTimes2(j) = Timing1;
        Altitudes2(j) = 0;
    elseif Timing2 > TimeF
        %Altitude is 0
        AltitudeTimes2(j) = Timing2;
        Altitudes2(j) = 0; 
        State2 = -1;
    elseif Timing2 >= StageAB(1) && Timing2 <= StageAB(2)
        Altitudes2(j) = Hab(Timing2);
    elseif Timing2 > StageBC(1) && Timing2 <= StageBC(2)
        Altitudes2(j) = Hbc(Timing2);
    elseif Timing2 > StageCD(1) && Timing2 <= StageCD(2)
        Altitudes2(j) = Hcd(Timing2);
    elseif Timing2 > StageDE(1) && Timing2 <= StageDE(2)
        Altitudes2(j) = Hde(Timing2);
    elseif Timing2 > StageEF(1) && Timing2 <= StageEF(2)
        Altitudes2(j) = Hef(Timing2);
    else
        disp('Error')
        State2 = -1;
    end
    if State2 ~= -1
        AltitudeTimes2(j) = Timing2;
        RNG2 = -1 + (1- (-1)).*rand(Precision2,1);
        %RNG2 = (2 * rand) - 1;
        Noise2 = RNG2 * Accuracy2;
        Altitudes2(j) = Altitudes2(j) + Noise2;
        Timing2 = Timing2 + 1 / AltimeterPolling2;
        j = j + 1;
    end
end
end


    
