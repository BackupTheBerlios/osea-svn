BEGIN WORK;

CREATE USER aftax PASSWORD 'changeme' ;

CREATE GROUP aftax ;

ALTER GROUP asplfact ADD USER aftax ;
ALTER GROUP aftax ADD USER aftax ;

CREATE TABLE Tax_VAT_Customer_Type (
	id 		SERIAL PRIMARY KEY,
	reference 	VARCHAR(10) NOT NULL UNIQUE,
	name 		VARCHAR(100) NOT NULL,
	description 	TEXT
);

CREATE TABLE Tax_VAT_Item_Type (
	id 		SERIAL PRIMARY KEY,
	reference 	VARCHAR(10) NOT NULL UNIQUE,
	name 		VARCHAR(100) NOT NULL,
	description 	TEXT
);

CREATE TABLE Tax_VAT_Values (
	id 			SERIAL PRIMARY KEY,
	customer_type_id 	INT,
	item_type_id 		INT,
	value 			DECIMAL(5,2),

	FOREIGN KEY (customer_type_id) REFERENCES Tax_VAT_Customer_Type (id)
	ON UPDATE CASCADE ON DELETE CASCADE,
	FOREIGN KEY (item_type_id) REFERENCES Tax_VAT_Item_Type (id)
	ON UPDATE CASCADE ON DELETE CASCADE
	
);

ALTER TABLE Tax_VAT_Customer_Type OWNER TO aftax;
ALTER TABLE Tax_VAT_Item_Type OWNER TO aftax;
ALTER TABLE Tax_VAT_Values OWNER TO aftax;
ALTER TABLE Tax_VAT_Customer_Type_id_seq OWNER TO aftax;
ALTER TABLE Tax_VAT_Item_Type_id_seq OWNER TO aftax;
ALTER TABLE Tax_VAT_Values_id_seq OWNER TO aftax;


REVOKE ALL ON Tax_VAT_Customer_Type, Tax_VAT_Item_Type, Tax_VAT_Values, 
	      Tax_VAT_Customer_Type_id_seq, Tax_VAT_Item_Type_id_seq,
	      Tax_VAT_Values_id_seq FROM PUBLIC;

GRANT ALL ON Tax_VAT_Customer_Type, Tax_VAT_Item_Type, Tax_VAT_Values, 
	      Tax_VAT_Customer_Type_id_seq, Tax_VAT_Item_Type_id_seq,
	      Tax_VAT_Values_id_seq TO aftax;

GRANT SELECT ON Tax_VAT_Customer_Type, Tax_VAT_Item_Type, Tax_VAT_Values, 
	      Tax_VAT_Customer_Type_id_seq, Tax_VAT_Item_Type_id_seq,
	      Tax_VAT_Values_id_seq TO GROUP aftax;

COMMIT WORK;
