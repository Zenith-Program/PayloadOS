function [CriticalTimes, CriticalAltitudes] = ModelGeneration(Gravity,Thrust,BurnOutTime,DrogueCoastSpeed,MainHeight,MainCoastSpeed)
%Generates 2 matrices (1 x 6) for the critical times and altitudes of the
%generated model
%Initialization of Variables
G = Gravity;
T = Thrust;
VT1 = DrogueCoastSpeed;
VT2 = MainCoastSpeed;
M = MainHeight;
%Launch to Burnout

%Model generation does not care about wait time, adjusting it to include a
%wait time will break all the formulas used.

%ONLY USE t = 0 TO INDICATE LAUNCH
TimeA = 0;
TimeB = BurnOutTime;
    %Height Function for interval A <= t <=B at time B
    Hab = @(t) ((t-TimeA)^2)*((T/2)+(G/2));
    HabA = Hab(TimeA);
    HabB = Hab(TimeB);
%Burnout to Apogee
TimeC = TimeB - ((TimeB*(T+G))/G);
    %Height Function for interval B < t <= C at Time C
    Hbc = @(t) (G/2)*((t + ((TimeB*(T+G))/G) - TimeB)^2) + (((TimeB^2)/2) * (T + G) + TimeB * (T + G) * (- ((TimeB*(T+G))/G)) + (G/2) * ((- ((TimeB*(T+G))/G))^2));
    HbcC = Hbc(TimeC);
%Apogee to Drogue Deployment Terminal Velocity Time
TimeD = TimeC + (VT1/G);
    %Height Function for interval C < t <= D at Time D
    Hcd = @(t) (G/2)*((t + ((TimeB*(T+G))/G) - TimeB)^2) + (((TimeB^2)/2) * (T + G) + TimeB * (T + G) * (- ((TimeB*(T+G))/G)) + (G/2) * ((- ((TimeB*(T+G))/G))^2));
    HcdD = Hcd(TimeD);
%Drogue Deployment Terminal Velocity Time to Main Deployment Velocity Time
TimeE = TimeD + ((M - HcdD)/VT1);
    %Height Function for  interval D < t <= E at Time E
    Hde = @(t) VT1 * (t - TimeD) + HcdD;
    HdeE = Hde(TimeE);
%Main Deployment Terminal Velocity Time to Landing
TimeF = TimeE - (HdeE/VT2);
    %Height Function for interval E < t <= F at Time F
    Hef = @(t) VT2 * (t - TimeE) + HdeE;
    HefF = Hef(TimeF);

%Critical Times of flight based on inital parameters
CriticalTimes = [TimeA, TimeB, TimeC, TimeD, TimeE, TimeF];

%Altitudes of Critical Times
CriticalAltitudes = [HabA,HabB,HbcC,HcdD,HdeE,HefF];

end