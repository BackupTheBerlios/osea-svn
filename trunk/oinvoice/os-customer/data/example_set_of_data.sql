-- Some customers for initial set of data. Ready for testing the application.

-- Customer #1
BEGIN WORK;
	-- Customer
	INSERT INTO af_customer_customer VALUES ( 1, 2, '52535762J', 'Marcos', 'Olmos Dominguez', NULL, 0, NULL);
	-- Address (normal address)
	INSERT INTO af_customer_address VALUES ( 1, 5, 1, 'Calle 13', 'Villasequilla', 'Toledo', '23645', 'Spain');
	-- Inet_data (mail: marcos@aspl.es)
	INSERT INTO af_customer_inet_data VALUES ( 1, 10, 1, 'marcos@aspl.es');
	-- There is no contact

	-- 2 phones: home and mobile
	INSERT INTO af_customer_telephone VALUES ( 1, 7, 1, '123456789', 'Home phone');
	INSERT INTO af_customer_telephone VALUES ( 2, 9, 1, '987654321', 'Mobile; call for 9 to 14');
COMMIT WORK;


-- Customer #2
BEGIN WORK;
	-- Customer
	INSERT INTO af_customer_customer VALUES ( 2, 3, 'B82827932', '', 'Advanced Software Production Line, S.L.', NULL, 0, NULL);
	-- Address (normal and deliver addresses)
	INSERT INTO af_customer_address VALUES ( 2, 5, 2, 'Aptdo. de Correos 228', 'Coslada', 'Madrid', '28820', 'Spain');
	INSERT INTO af_customer_address VALUES ( 3, 6, 2, 'Doctor Michavila, 14', 'Coslada', 'Madrid', '28820', 'Spain');
	-- Inet_data (web: http://www.aspl.es and mail: info@aspl.es)
	INSERT INTO af_customer_inet_data VALUES ( 2, 11, 2, 'http://www.aspl.es');
	INSERT INTO af_customer_inet_data VALUES ( 3, 10, 2, 'info@aspl.es');
	-- Contact
	INSERT INTO af_customer_contact VALUES ( 1, 2, 'Edward Brosnan', 'General Manager');
	-- Phone
	INSERT INTO af_customer_telephone VALUES ( 3, 7, 2, '916695532', 'Main phone');
COMMIT WORK;


-- Customer #3
BEGIN WORK;
	-- Customer
	INSERT INTO af_customer_customer VALUES ( 3, 4, 'X0912343X', 'Francis', 'Brosnan Blazquez', NULL, 0, NULL);
	-- Address (normal address)
	INSERT INTO af_customer_address VALUES ( 4, 5, 3, 'Arenal, 133', 'Madrid', 'Madrid', '28001', 'Spain');
	-- Inet_data (web: http://francis.mycooldomain.com)
	INSERT INTO af_customer_inet_data VALUES ( 4, 11, 3, 'http://francis.mycooldomain.com');
	-- There is no contact

	-- phone: mobile
	INSERT INTO af_customer_telephone VALUES ( 4, 9, 3, '607124638', NULL);
COMMIT WORK;



-- Customer #4
BEGIN WORK;
	-- Customer
	INSERT INTO af_customer_customer VALUES ( 4, 1, 'PASS-08912442', NULL, 'ManKrake', 'Jony Melavo', 0, NULL);
	-- Address (normal address)
	INSERT INTO af_customer_address VALUES ( 5, 6, 4, 'Deck 19', 'Portland', 'New York', '0012445134', 'EEUU');
	-- There is no inet_data

	-- There is no contact

	-- There is no phone

COMMIT WORK;
