function [GeneratedData] = GeneratedDataConverger(AltitudeTimes1,AltitudeTimes2,Altimeter1,Altimeter2)
%Takes in the generated data and converges them into a single file
%Matrix Size Calculation
NumberOfRows = size(AltitudeTimes1);
%Creates the correctly sized matrix (Columns are 59 as to the what the
%program expects
GeneratedData = zeros(NumberOfRows(2),59);
%Array inversions
FlippedAltitudeTimes1 = AltitudeTimes1';
FlippedAltitudeTimes2 = AltitudeTimes2';
FlippedAltimeter1 = Altimeter1';
FlippedAltimeter2 = Altimeter2';
%Pasting inverted arrays into the single matrix
GeneratedData(:,3) = FlippedAltimeter1(:,1);
GeneratedData(:,4) = FlippedAltimeter2(:,1);
GeneratedData(:,1) = FlippedAltitudeTimes1(:,1);
GeneratedData(:,2) = 4;
GeneratedData(:,1) = GeneratedData(:,1) + 10000;
GeneratedData = round(GeneratedData,4);
end