-- 画出中心对称的魔法阵

-- 画出基础圆环
origin is (400, 300);
scale is (150, 150);
color is (255, 255, 0);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

-- 画出内部圆环
origin is (400, 300);
scale is (100, 100);
color is (255, 128, 0);
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

-- 画出中心圆
origin is (400, 300);
scale is (30, 30);
color is (255, 0, 0);
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

-- 画出四个对称小圆
origin is (400, 150);
scale is (20, 20);
color is (0, 0, 255);
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 450);
scale is (20, 20);
color is (0, 0, 255);
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (250, 300);
scale is (20, 20);
color is (0, 0, 255);
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (550, 300);
scale is (20, 20);
color is (0, 0, 255);
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

-- 画出四个对称魔法线条
origin is (400, 300);
scale is (150, 5);
color is (128, 0, 128);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (5, 150);
color is (128, 0, 128);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (150, 5);
color is (128, 0, 128);
rot is pi/4;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (5, 150);
color is (128, 0, 128);
rot is pi/4;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

-- 画出八个对称魔法符号
origin is (400, 150);
scale is (20, 20);
color is (255, 0, 255);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t)*cos(t)*cos(t), sin(t)*sin(t)*sin(t));

origin is (400, 450);
scale is (20, 20);
color is (255, 0, 255);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t)*cos(t)*cos(t), sin(t)*sin(t)*sin(t));

origin is (250, 300);
scale is (20, 20);
color is (255, 0, 255);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t)*cos(t)*cos(t), sin(t)*sin(t)*sin(t));

origin is (550, 300);
scale is (20, 20);
color is (255, 0, 255);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t)*cos(t)*cos(t), sin(t)*sin(t)*sin(t));

origin is (350, 200);
scale is (20, 20);
color is (255, 0, 255);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t)*cos(t)*cos(t), sin(t)*sin(t)*sin(t));

origin is (450, 200);
scale is (20, 20);
color is (255, 0, 255);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t)*cos(t)*cos(t), sin(t)*sin(t)*sin(t));

origin is (350, 400);
scale is (20, 20);
color is (255, 0, 255);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t)*cos(t)*cos(t), sin(t)*sin(t)*sin(t));

origin is (450, 400);
scale is (20, 20);
color is (255, 0, 255);
rot is 0;
for t from 0 to 2*pi step pi/10000 draw(cos(t)*cos(t)*cos(t), sin(t)*sin(t)*sin(t));

-- 画出八个对称魔法线条
origin is (400, 300);
scale is (150, 5);
color is (128, 0, 128);
rot is pi/8;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (5, 150);
color is (128, 0, 128);
rot is pi/8;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (150, 5);
color is (128, 0, 128);
rot is 3*pi/8;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (5, 150);
color is (128, 0, 128);
rot is 3*pi/8;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (150, 5);
color is (128, 0, 128);
rot is 5*pi/8;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (5, 150);
color is (128, 0, 128);
rot is 5*pi/8;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (150, 5);
color is (128, 0, 128);
rot is 7*pi/8;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));

origin is (400, 300);
scale is (5, 150);
color is (128, 0, 128);
rot is 7*pi/8;
for t from 0 to 2*pi step pi/10000 draw(cos(t), sin(t));
