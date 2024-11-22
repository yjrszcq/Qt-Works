pixsize is 3;
origin is (200,200);
scale is (100,100);
color is (230,230,0);
for t from 0 to 2*pi step pi/10000 draw(cos(t)*cos(t)*cos(t),sin(t)*sin(t)*sin(t)); --星型线
pixsize is 5;
origin is (350,400);
scale is (10,10);
color is (255,0,0);
rot is pi;
for t from 0 to 2*pi step pi/10000 draw(16*sin(t)*sin(t)*sin(t),13*cos(t)-5*cos(2*t)-2*cos(3*t)-cos(4*t)); --桃心线