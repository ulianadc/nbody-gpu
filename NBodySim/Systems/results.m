close all
clear,clc

results = [
    4096 0.003358 0.002032 0.003013 0.002547
    8192 0.005172 0.005163 0.005344 0.005384
    12288 0.010266 0.009788 0.009619 0.009869
    16384 0.016835 0.016281 0.016462 0.016814
    20480 0.025372 0.025354 0.023137 0.025278
    24576 0.03236 0.034914 0.035003 0.036742
    28672 0.044196 0.047613 0.047235 0.047476
    32768 0.057178 0.060812 0.052675 0.06084
    36864 0.065478 0.077242 0.077059 0.079111
    40960 0.023237 0.080845 0.096408 0.097391
    45056 0.037521 0.114156 0.116028 0.115544
    49152 0.054109 0.136388 0.13637 0.141616
    53248 0.087944 0.143953 0.137666 0.136964
    57344 0.093065 0.160905 0.159617 0.152607
    61440 0.114907 0.226943 0.001995 0.250935
    65536 0.211067 0.293001 0.204634 0.240854
];

x = results(:,1)';
tMax = max(results(:,2:5), [], 2)';
tMin = min(results(:,2:5), [], 2)';
tMed = median(results(:,2:5), 2)';

eUpper = tMax - tMed;
eLower = tMed - tMin;

errorbar(x,sqrt(tMed),sqrt(eUpper),sqrt(eLower),'LineStyle','none','Marker','o')
xlabel('Body Count N')
ylabel('sqrtIteration Time (s))')
title('sqrtIteration Time) vs. System Size')
grid on

hold on
[fit S] = polyfit(x,sqrt(tMed),1);
plot(x,polyval(fit,x),'Color','red')