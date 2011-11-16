clear,clc

for bodyCt=4096:4096:65536
    system = [ random('norm', 0, 1e3, [bodyCt 3]).*(1e5), ...
               random('norm', 0, 1e2, [bodyCt 3]), ...
               random('norm', 1e18, 1e15, [bodyCt 1]) ];
    save(sprintf('system%d.dat', bodyCt), 'system', '-ascii');
end