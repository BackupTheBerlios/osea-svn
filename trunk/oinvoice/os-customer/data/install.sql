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
CREATE USER os_customer PASSWORD 'changeme';
CREATE GROUP os_customer; 
ALTER GROUP os_customer ADD USER os_customer;
ALTER GROUP os_customer ADD USER asplfact;

-- Entities definition
CREATE TABLE os_customer_enum_values (
	id                   SERIAL PRIMARY KEY, 
	enum_type            VARCHAR(250),
	enum_order_in_type   INT,
	enum_value           VARCHAR(250),
	CHECK (enum_type IN ('Legal ID Type',
			     'Address Type',
			     'Phone Type',
                             'Web Data Type'))
);

CREATE TABLE os_customer_address (
	id                   SERIAL PRIMARY KEY, 
	enum_values_id       INT NOT NULL ,
	customer_id          INT NOT NULL ,
	address              VARCHAR(250),
	city                 VARCHAR(250),
	state                VARCHAR(250),
	zip_code             VARCHAR(250),
	country              VARCHAR(250)
);

CREATE TABLE os_customer_telephone (
	id                   SERIAL PRIMARY KEY, 
	enum_values_id       INT NOT NULL ,
	customer_id          INT NOT NULL ,
	phone                VARCHAR(250),
	description          VARCHAR(250)
);

CREATE TABLE os_customer_inet_data (
	id                   SERIAL PRIMARY KEY, 
	enum_values_id       INT NOT NULL ,
	customer_id          INT NOT NULL ,
	inet_data            VARCHAR(250)
);

CREATE TABLE os_customer_contact (
	id                   SERIAL PRIMARY KEY, 
	customer_id          INT NOT NULL ,
	name                 VARCHAR(250),
	position             VARCHAR(250)
);

CREATE TABLE os_customer_customer (
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
ALTER TABLE os_customer_enum_values OWNER TO os_customer;
ALTER TABLE os_customer_enum_values_id_seq OWNER TO os_customer;
ALTER TABLE os_customer_address OWNER TO os_customer;
ALTER TABLE os_customer_address_id_seq OWNER TO os_customer;
ALTER TABLE os_customer_telephone OWNER TO os_customer;
ALTER TABLE os_customer_telephone_id_seq OWNER TO os_customer;
ALTER TABLE os_customer_inet_data OWNER TO os_customer;
ALTER TABLE os_customer_inet_data_id_seq OWNER TO os_customer;
ALTER TABLE os_customer_contact OWNER TO os_customer;
ALTER TABLE os_customer_contact_id_seq OWNER TO os_customer;
ALTER TABLE os_customer_customer OWNER TO os_customer;
ALTER TABLE os_customer_customer_id_seq OWNER TO os_customer;
REVOKE ALL ON os_customer_enum_values,
              os_customer_enum_values_id_seq,
              os_customer_address,
              os_customer_address_id_seq,
              os_customer_telephone,
              os_customer_telephone_id_seq,
              os_customer_inet_data,
              os_customer_inet_data_id_seq,
              os_customer_contact,
              os_customer_contact_id_seq,
              os_customer_customer,
              os_customer_customer_id_seq FROM PUBLIC;
GRANT ALL ON os_customer_enum_values,
             os_customer_enum_values_id_seq,
             os_customer_address,
             os_customer_address_id_seq,
             os_customer_telephone,
             os_customer_telephone_id_seq,
             os_customer_inet_data,
             os_customer_inet_data_id_seq,
             os_customer_contact,
             os_customer_contact_id_seq,
             os_customer_customer,
             os_customer_customer_id_seq TO os_customer;
GRANT SELECT ON os_customer_enum_values,
                os_customer_enum_values_id_seq,
                os_customer_address,
                os_customer_address_id_seq,
                os_customer_telephone,
                os_customer_telephone_id_seq,
                os_customer_inet_data,
                os_customer_inet_data_id_seq,
                os_customer_contact,
                os_customer_contact_id_seq,
                os_customer_customer,
                os_customer_customer_id_seq TO GROUP os_customer;
COMMIT WORK;
