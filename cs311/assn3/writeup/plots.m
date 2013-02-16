%MATLAB script file
% Plots values for CS 311 uniqify program

%=================[ 10 sort processes ]======================%
x =      [984  688  23168 3256 592  552  384  8    136  5064];
y_sys =  [0.04 0.04 0.22  0.05 0.04 0.04 0.04 0.03 0.03 0.06];
y_real = [0.3  0.2  7.54  1.22 0.2  0.19 0.12 0.01 0.03 1.11];

%Fit to polynomial
P1 = polyfit(x,y_real,1);
y_real_fit = polyval(P1,x);
P2 = polyfit(x,y_sys,1);
y_sys_fit = polyval(P2,x);

figure;
plot(x,y_real_fit,x,y_sys_fit)
title('Uniqify using 10 sort processes');
xlabel('Filesize (KB)');
ylabel('Time required (s)');
legend('Real time', 'System time');
%============================================================%

%=================[ 50 sort processes ]======================%
x =      [984  688  23168 3256 592  552  384  8    136  5064];
y_sys =  [0.19 0.17 0.3   0.19 0.18 0.18 0.17 0.16 0.18 0.21];
y_real = [0.33 0.22 7.58  1.26 0.22 0.21 0.14 0.03 0.04 1.16];

%Fit to polynomial
P1 = polyfit(x,y_real,1);
y_real_fit = polyval(P1,x);
P2 = polyfit(x,y_sys,1);
y_sys_fit = polyval(P2,x);

figure(2);
plot(x,y_real_fit,x,y_sys_fit)
title('Uniqify using 50 sort processes');
xlabel('Filesize (KB)');
ylabel('Time required (s)');
legend('Real time', 'System time');
figure(2)
%============================================================%

%=================[ 100 sort processes ]======================%
x =     [984  688  23168 3256 592  552  384  8    136 5064];
y_sys = [0.36 0.36 0.47  0.37 0.36 0.36 0.35 0.35 0.34 .38];
y_real =[0.36 0.26 7.67  1.3  0.25 0.24 0.17 0.06 0.08 1.2];

%Fit to polynomial
P1 = polyfit(x,y_real,1);
y_real_fit = polyval(P1,x);
P2 = polyfit(x,y_sys,1);
y_sys_fit = polyval(P2,x);

figure(3);
plot(x,y_real_fit,x,y_sys_fit)
title('Uniqify using 100 sort processes');
xlabel('Filesize (KB)');
ylabel('Time required (s)');
legend('Real time', 'System time');
figure(3)
%============================================================%
