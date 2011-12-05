clear,clc;
close all;

runs = [
		8192 100 1 38.849998;
		8192 100 2 39.160000;
		8192 100 3 39.150002;
		8192 100 4 39.150002;
		8192 100 5 39.150002;
		8192 100 6 39.160000;
		8192 100 7 39.150002;
		8192 100 8 39.160000;
		8192 100 9 39.130001;
		8192 100 10 39.160000;
		8192 100 11 39.130001;
		8192 100 12 39.139999;
		8192 100 13 39.139999;
		8192 100 14 39.150002;
		8192 100 15 39.150002;
		8192 100 16 39.160000;
		8192 100 17 39.139999;
		8192 100 18 39.160000;
		8192 100 19 39.160000;
		8192 100 20 39.130001;
	];

wbp = runs(:,3);
tpi = runs(:,4)./runs(:,2);
bw  = (runs(:,1)*4*7)./(wbp.*tpi)./2^20;

figure

subplot(2,1,1)
plot(wbp, tpi, '.', 'MarkerSize', 10)
grid on
xlim([0 21])
ylim([0 0.5])
xlabel('Write-Back Period (# iterations)')
ylabel('Time per Iteration (s)')

subplot(2,1,2)
plot(wbp, bw, 'r.', 'MarkerSize', 10)
grid on
xlim([0 21])
xlabel('Write-Back Period (# iterations)')
ylabel('PCIe Bus Bandwidth (MB/s)')



%%
[ax,h1,h2] = plotyy(wbp, tpi, wbp, bw, 'plot');
xlabel(ax(1), 'Write-Back Period (# iterations)');
ylabel(ax(1), 'Time per Iteration (s)');
ylabel(ax(2), 'PCIe Bus Bandwidth (MB/s)');
xlim(ax(1), [0 max(wbp)+1]);
xlim(ax(2), [0 max(wbp)+1]);
ylim(ax(1), [0 0.45]);
grid on

gcf

% set(get(AX(1), 'xlabel', 'Write-Back Period (# iterations)')
ylabel('Time per Iteration (s)')

