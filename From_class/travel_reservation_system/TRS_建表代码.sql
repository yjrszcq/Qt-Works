CREATE DATABASE IF NOT EXISTS travel_reservation;
USE travel_reservation;

CREATE TABLE IF NOT EXISTS customers (
  custName varchar(30) NOT NULL COMMENT '客户姓名',
  custID varchar(30) DEFAULT NULL COMMENT '客户ID',
  custPW varchar(20) DEFAULT NULL COMMENT '密码',
  PRIMARY KEY (custName)
) COMMENT='客户信息';

CREATE TABLE IF NOT EXISTS flights (
  flightNum char(6) NOT NULL COMMENT '航班号',
  price int DEFAULT NULL COMMENT '机票价格',
  numSeats int DEFAULT NULL COMMENT '座位总数',
  numAvail int DEFAULT NULL COMMENT '剩余可订座位数',
  FromCity varchar(100) DEFAULT NULL COMMENT '出发地',
  ArivCity varchar(100) DEFAULT NULL COMMENT '到达地',
  PRIMARY KEY (flightNum)
) COMMENT='航班信息';

CREATE TABLE IF NOT EXISTS hotels (
  location varchar(100) NOT NULL COMMENT '宾馆地址',
  price int DEFAULT NULL COMMENT '入住价格',
  numRooms int DEFAULT NULL COMMENT '房间总数',
  numAvail int DEFAULT NULL COMMENT '剩余可订房间数',
  PRIMARY KEY (location)
) COMMENT='宾馆信息';

CREATE TABLE IF NOT EXISTS bus (
  location varchar(100) NOT NULL COMMENT '客车所属地',
  price int DEFAULT NULL COMMENT '客车价格',
  numBus int DEFAULT NULL COMMENT '客车总数',
  numAvail int DEFAULT NULL COMMENT '剩余可订客车数',
  PRIMARY KEY (location)
) COMMENT='客车信息';

CREATE TABLE IF NOT EXISTS reservations (
  resvKey CHAR(30) NOT NULL COMMENT '预订号',
  custName VARCHAR(30) DEFAULT NULL COMMENT '客户姓名',
  resvType INT DEFAULT NULL COMMENT '预订类型 (1: 航班, 2: 宾馆, 3: 客车)',
  resvAvail TINYINT(1) DEFAULT NULL COMMENT '预订可用性',
  resvNote VARCHAR(100) DEFAULT NULL COMMENT '订单备注',
  PRIMARY KEY (resvKey),
  KEY custName (custName),
  CONSTRAINT reservations_ibfk_1 FOREIGN KEY (custName) REFERENCES customers (custName)
) COMMENT='预订信息';

CREATE TABLE IF NOT EXISTS flight_reservations (
  resvKey CHAR(30) NOT NULL,
  flightNum CHAR(6) NOT NULL,
  PRIMARY KEY (resvKey),
  FOREIGN KEY (resvKey) REFERENCES reservations (resvKey),
  FOREIGN KEY (flightNum) REFERENCES flights (flightNum)
) COMMENT='航班预订信息';

CREATE TABLE IF NOT EXISTS hotel_reservations (
  resvKey CHAR(30) NOT NULL,
  location VARCHAR(100) NOT NULL,
  PRIMARY KEY (resvKey),
  FOREIGN KEY (resvKey) REFERENCES reservations (resvKey),
  FOREIGN KEY (location) REFERENCES hotels (location)
) COMMENT='宾馆预订信息';

CREATE TABLE IF NOT EXISTS bus_reservations (
  resvKey CHAR(30) NOT NULL,
  location VARCHAR(100) NOT NULL,
  PRIMARY KEY (resvKey),
  FOREIGN KEY (resvKey) REFERENCES reservations (resvKey),
  FOREIGN KEY (location) REFERENCES bus (location)
) COMMENT='客车预订信息';
