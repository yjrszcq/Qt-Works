---- 左大圆
color is (255,0,0); // red
origin is (250,250);
scale is (200,200);
rot is 0;
for T from 1/2*pi to 3/2*pi step pi/700 draw (cos(t), sin(t));

color is (0,0,0);
origin is (250,250);
scale is (200,200);
rot is 0;
for T from 3/2*pi to 5/2*pi step pi/700 draw (cos(t), sin(t));

---- 上小圆
color is (0,0,0);
origin is (250,150);
scale is (30,30);
rot is 0;
for t from 0 to 2*pi step pi/350 draw (cos(t), sin(t));

---- 上半弧
color is (0,0,0);
origin is (250,150);
scale is (100,100);
rot is pi/2;
for t from -pi to 0 step pi/500 draw (cos(t), sin(t));

---- 下小圆
color is (255,0,0);
origin is (250,350);
scale is (30,30);
rot is 0;
for t from 0 to 2*pi step pi/350 draw (cos(t), sin(t));

---- 下半弧
color is (255, 0,0 );
origin is (250,350);
scale is (100,100);
rot is 0;
for t from -pi/2 to pi/2 step pi/500 draw (cos(t), sin(t));
