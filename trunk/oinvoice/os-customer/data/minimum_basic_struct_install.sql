-- Some af_customer_enum_values:

BEGIN WORK;

-- For Legal ID type:
INSERT INTO af_customer_enum_values VALUES ( 1, 'Legal ID Type', 0, 'Passport');
INSERT INTO af_customer_enum_values VALUES ( 2, 'Legal ID Type', 1, 'NIF');
INSERT INTO af_customer_enum_values VALUES ( 3, 'Legal ID Type', 2, 'CIF');
INSERT INTO af_customer_enum_values VALUES ( 4, 'Legal ID Type', 3, 'Resident Card');

-- For Addres type:
INSERT INTO af_customer_enum_values VALUES ( 5, 'Address Type', 0, 'Normal');
INSERT INTO af_customer_enum_values VALUES ( 6, 'Address Type', 1, 'Deliver');

-- For Phone type:
INSERT INTO af_customer_enum_values VALUES ( 7, 'Phone Type', 0, 'Normal');
INSERT INTO af_customer_enum_values VALUES ( 8, 'Phone Type', 1, 'Fax');
INSERT INTO af_customer_enum_values VALUES ( 9, 'Phone Type', 2, 'Mobile');

-- For Web Data type:
INSERT INTO af_customer_enum_values VALUES (10, 'Web Data Type', 0, 'email');
INSERT INTO af_customer_enum_values VALUES (11, 'Web Data Type', 1, 'web');

COMMIT WORK;