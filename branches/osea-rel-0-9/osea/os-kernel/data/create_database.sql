CREATE USER asplfact PASSWORD 'changeme' ;

CREATE GROUP asplfact ;

ALTER GROUP asplfact ADD USER asplfact ;

CREATE DATABASE asplfact WITH ENCODING = 'UTF-8';

