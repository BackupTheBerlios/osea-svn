--  af_customer: Uninstall SQL scheme script for the server-side component
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

-- First drop all relationships

-- Drop all tables
DROP TABLE af_customer_enum_values;
DROP SEQUENCE af_customer_enum_values_id_seq;
DROP TABLE af_customer_address;
DROP SEQUENCE af_customer_address_id_seq;
DROP TABLE af_customer_telephone;
DROP SEQUENCE af_customer_telephone_id_seq;
DROP TABLE af_customer_inet_data;
DROP SEQUENCE af_customer_inet_data_id_seq;
DROP TABLE af_customer_contact;
DROP SEQUENCE af_customer_contact_id_seq;
DROP TABLE af_customer_customer;
DROP SEQUENCE af_customer_customer_id_seq;

-- Misc drop
DROP GROUP af_customer;
DROP USER  af_customer;
