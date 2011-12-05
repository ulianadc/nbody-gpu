close all
clear,clc

results = [
    4096 0.18 0.16
    8192 0.31 0.33
    12288 0.48 0.5
    16384 1.3 1.29
    20480 1.62 1.62
    24576 1.94 1.94
    28672 2.26 2.26
    32768 3.88 3.89
    36864 4.37 4.37
    40960 4.85 4.86
    45056 7.11 7.12
    49152 7.77 7.77
    53248 8.41 8.42
    57344 9.07 9.06
    61440 12.13 12.14
    65536 12.95 12.94
];

x = results(:,1)';
tMax = max(results(:,2:3), [], 2)';
tMin = min(results(:,2:3), [], 2)';
tMed = median(results(:,2:3), 2)';

eUpper = tMax - tMed;
eLower = tMed - tMin;

[fit S] = polyfit(x, sqrt(tMed), 1);

figure();
hold on;
errorbar(x, sqrt(tMed), sqrt(eUpper), sqrt(eLower), ...
         'LineStyle', 'none', 'Marker', '.', 'MarkerSize', 10, ...
         'Color', 'b');
plot(x, polyval(fit,x), 'r--');
grid on;

xlabel('System Size N');
ylabel('sqrt(Time per Iteration (s))');
legend('sqrt(Time per Iteration)', ...
       sprintf('%.3g*N+%0.2f', fit(1), fit(2)), ...
       'Location', 'SouthEast');
   
   
%%
close all;
clear, clc;

results = [
    8192 0.0817
    16384 0.0607
    32768 0.0433
    65536 0.0311
    131072 0.0233
    262144 0.0226
    524228 0.0180
    1048456 0.0151
    2096912 0.0130
    4193824 0.0127
    8387648 0.0139
    16775296 0.0135
];

sysSize = results(:,1);
sysMemSize = sysSize .* (7*4/2^20);
pcieOverItTime = results(:,2);

subplot(2,1,1)
loglog(sysSize, sysMemSize, 'r.', 'MarkerSize', 10);
grid on;
xlabel('System Size N');
ylabel('System Memory Size (MB)');

subplot(2,1,2)
semilogx(sysSize, pcieOverItTime, 'b.', 'MarkerSize', 10);
grid on;
xlabel('System Size N')
ylabel('PCIe Txfer Time / Total Iteration Time');