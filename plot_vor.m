
data = readmatrix("seeds.txt");
voronoi(data(:,1), data(:, 2));
xlim([0,1]); ylim([0,1]);