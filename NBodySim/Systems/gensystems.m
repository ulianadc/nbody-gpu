clear,clc
wbh = waitbar(0, 'Generating systems...');
systemSizes = (2^13:2^13:2^17);
for i=systemSizes
    system = [ random('norm', 0, 1e3, [i 3]).*(1e5), ...
               random('norm', 0, 1e2, [i 3]), ...
               random('norm', 1e18, 1e15, [i 1]) ];
    fid = fopen(sprintf('system%06d.dat', i), 'w');
    fwrite(fid, system', 'float');
    fclose(fid);
    dlmwrite(sprintf('system%06d.txt', i), system, '\t');
    waitbar(i/max(systemSizes), wbh);
end
close(wbh);