--  af-customer: SQL scheme for the server-side component
--  Copyright (C) 2002,2003 Advanced Software Production Line, S.L.

--  This program is free software; you can redistribute it and/or modify 
--  it under the terms of the GNU General Public License as published by 
--  the Free Software Foundation; either version 2 of the License, or 
--  (at your option) any later version. 
-- 
--  This program is distributed in the hope that it will be useful, 
--  but WITHOUT ANY WARRANTY; without even the implied warranty of 
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
--  GNU General Public License for more details. 
-- 
--  You should have received a copy of the GNU General Public License 
--  along with this program; if not, write to the Free Software 
--  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 

BEGIN WORK;

-- Create server user and group:
CREATE USER af_customer PASSWORD 'changeme';
CREATE GROUP af_customer; 
ALTER GROUP af_customer ADD USER af_customer;
ALTER GROUP af_customer ADD USER asplfact;

-- Entities definition
CREATE TABLE af_customer_enum_values (
	id                   SERIAL PRIMARY KEY, 
	enum_type            VARCHAR(250),
	enum_order_in_type   INT,
	enum_value           VARCHAR(250),
	CHECK (enum_type IN ('Legal ID Type',
			     'Address Type',
			     'Phone Type',
                             'Web Data Type'))
);

CREATE TABLE af_customer_address (
	id                   SERIAL PRIMARY KEY, 
	enum_values_id       INT NOT NULL ,
	customer_id          INT NOT NULL ,
	address              VARCHAR(250),
	city                 VARCHAR(250),
	state                VARCHAR(250),
	zip_code             VARCHAR(250),
	country              VARCHAR(250)
);

CREATE TABLE af_customer_telephone (
	id                   SERIAL PRIMARY KEY, 
	enum_values_id       INT NOT NULL ,
	customer_id          INT NOT NULL ,
	phone                VARCHAR(250),
	description          VARCHAR(250)
);

CREATE TABLE af_customer_inet_data (
	id                   SERIAL PRIMARY KEY, 
	enum_values_id       INT NOT NULL ,
	customer_id          INT NOT NULL ,
	inet_data            VARCHAR(250)
);

CREATE TABLE af_customer_contact (
	id                   SERIAL PRIMARY KEY, 
	customer_id          INT NOT NULL ,
	name                 VARCHAR(250),
	position             VARCHAR(250)
);

CREATE TABLE af_customer_customer (
	id                   SERIAL PRIMARY KEY, 
	enum_values_id       INT NOT NULL ,
	legal_id             VARCHAR(250),
	name                 VARCHAR(250),
	surname              VARCHAR(250),
	salesman             VARCHAR(250),
	vat_customer_type    INT,
	customer_ref         VARCHAR(250)
);

-- Relationships definition
ALTER TABLE af_customer_enum_values OWNER TO af_customer;
ALTER TABLE af_customer_enum_values_id_seq OWNER TO af_customer;
ALTER TABLE af_customer_address OWNER TO af_customer;
ALTER TABLE af_customer_address_id_seq OWNER TO af_customer;
ALTER TABLE af_customer_telephone OWNER TO af_customer;
ALTER TABLE af_customer_telephone_id_seq OWNER TO af_customer;
ALTER TABLE af_customer_inet_data OWNER TO af_customer;
ALTER TABLE af_customer_inet_data_id_seq OWNER TO af_customer;
ALTER TABLE af_customer_contact OWNER TO af_customer;
ALTER TABLE af_customer_contact_id_seq OWNER TO af_customer;
ALTER TABLE af_customer_customer OWNER TO af_customer;
ALTER TABLE af_customer_customer_id_seq OWNER TO af_customer;
REVOKE ALL ON af_customer_enum_values,
              af_customer_enum_values_id_seq,
              af_customer_address,
              af_customer_address_id_seq,
              af_customer_telephone,
              af_customer_telephone_id_seq,
              af_customer_inet_data,
              af_customer_inet_data_id_seq,
              af_customer_contact,
              af_customer_contact_id_seq,
              af_customer_customer,
              af_customer_customer_id_seq FROM PUBLIC;
GRANT ALL ON af_customer_enum_values,
             af_customer_enum_values_id_seq,
             af_customer_address,
             af_customer_address_id_seq,
             af_customer_telephone,
             af_customer_telephone_id_seq,
             af_customer_inet_data,
             af_customer_inet_data_id_seq,
             af_customer_contact,
             af_customer_contact_id_seq,
             af_customer_customer,
             af_customer_customer_id_seq TO af_customer;
GRANT SELECT ON af_customer_enum_values,
                af_customer_enum_values_id_seq,
                af_customer_address,
                af_customer_address_id_seq,
                af_customer_telephone,
                af_customer_telephone_id_seq,
                af_customer_inet_data,
                af_customer_inet_data_id_seq,
                af_customer_contact,
                af_customer_contact_id_seq,
                af_customer_customer,
                af_customer_customer_id_seq TO GROUP af_customer;
COMMIT WORK;
