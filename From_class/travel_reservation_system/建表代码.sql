create database travel_reservation;

use travel_reservation;

create table flights(
flightNum char(6) comment '航班号',
price int comment '机票价格',
numSeats int comment '座位总数',
numAvail int comment '剩余可订座位数',
FromCity varchar(100) comment '出发地',
ArivCity varchar(100) comment '到达地',
primary key (flightNum)
) comment '航班信息';

create table hotels(
location varchar(100) comment '宾馆地址',
price int comment '入住价格',
numRooms int comment '房间总数',
numAvail int comment '剩余可订房间数',
primary key (location)
) comment '宾馆信息';

create table bus(
location varchar(100) comment '客车所属地',
price int comment '客车价格',
numBus int comment '客车总数',
numAvail int comment '剩余可订客车数',
primary key (location)
) comment '客车信息';

create table customers(
custName varchar(30) comment '客户姓名',
custID varchar(30) comment '客户ID',
custPW varchar(20) comment '密码',
primary key (custName)
) comment '客户信息';

create table reservations(
custName varchar(30) comment '客户姓名',
resvType int comment '预订类型',
resvContent varchar(100) comment '预订内容',
resvAvail bool comment '预订可用性',
resvKey char(30) comment '预订号',
resvNote varchar(100) comment '订单备注',
primary key (resvKey)
) comment '预订信息';

