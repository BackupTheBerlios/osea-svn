-- See ../doc/af_kernel_er_diagram.dia for more information.
-- Object described on that diagram are represented here following
-- the next rules:
--
-- 1) Entities are represented with capitalized words so:
--     Entity: "GROUP" generates "Kernel_Group" table.
--
-- 2) Relationship: are represented with lower case words so:
---    Relationship: "group have" generates "kernel_group_have" table.
---

BEGIN WORK;

CREATE GROUP afkernel ;

ALTER GROUP afkernel ADD USER asplfact ;


-- Entities
CREATE TABLE Kernel_User (
	id              SERIAL PRIMARY KEY,
	nick            VARCHAR(16)    UNIQUE NOT NULL,
	passwd          CHAR(32)       NOT NULL,
	description     VARCHAR(100)   
);

CREATE TABLE Kernel_Group (
	id              SERIAL PRIMARY KEY,
	name            VARCHAR(32)    UNIQUE NOT NULL,
	description     VARCHAR(100)
);

CREATE TABLE Kernel_Server (
	id              SERIAL PRIMARY KEY,
	name            VARCHAR(32)    UNIQUE NOT NULL,
	version         INT            NOT NULL,
	description     VARCHAR(100)
);

CREATE TABLE Kernel_Permission (
	id              SERIAL PRIMARY KEY,
	name            VARCHAR(64)    UNIQUE NOT NULL,
	description     VARCHAR(100),
	server_id       INT NOT NULL,
	FOREIGN KEY (server_id) REFERENCES kernel_server (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE       
);
	
CREATE TABLE Kernel_Session (
	id 		VARCHAR(100)   PRIMARY KEY,
	user_id 	INT	       NOT NULL,
	stime 		TIMESTAMP      NOT NULL,
	expire          BOOL           NOT NULL
);

CREATE TABLE Kernel_Registry (
	name		VARCHAR(25) PRIMARY KEY,
	host		VARCHAR(100) NOT NULL,
	port		INT NOT NULL,
	stime		TIMESTAMP NOT NULL
);



-- Relationships
CREATE TABLE kernel_user_have (
	user_id         INT NOT NULL,
	permission_id   INT NOT NULL,
	PRIMARY KEY (permission_id, user_id),
	FOREIGN KEY (user_id) REFERENCES kernel_user (id) 
	ON UPDATE CASCADE
	ON DELETE CASCADE,
	FOREIGN KEY (permission_id) REFERENCES kernel_permission (id) 
	ON UPDATE CASCADE
	ON DELETE CASCADE
);

CREATE TABLE kernel_group_have (
	group_id        INT NOT NULL,
	permission_id   INT NOT NULL,
	PRIMARY KEY (permission_id, group_id),
	FOREIGN KEY (group_id) REFERENCES kernel_group (id) 
	ON UPDATE CASCADE
	ON DELETE CASCADE,
	FOREIGN KEY (permission_id) REFERENCES kernel_permission (id) 
	ON UPDATE CASCADE
	ON DELETE CASCADE
);

CREATE TABLE kernel_depends (
	id_permission   INT NOT NULL,
	id_depends      INT NOT NULL,
	PRIMARY KEY (id_permission, id_depends),
	FOREIGN KEY (id_permission) REFERENCES kernel_permission (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE,
	FOREIGN KEY (id_depends) REFERENCES kernel_permission (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE
);

CREATE TABLE kernel_belongs (
	id_user  	INT NOT NULL,
	id_group        INT NOT NULL,
	PRIMARY KEY (id_user, id_group),
	FOREIGN KEY (id_user) REFERENCES kernel_user (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE,
	FOREIGN KEY (id_group) REFERENCES kernel_group (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE
);


ALTER TABLE Kernel_User OWNER TO asplfact;
ALTER TABLE Kernel_Group OWNER TO asplfact;
ALTER TABLE Kernel_Permission OWNER TO asplfact;

ALTER TABLE Kernel_Session OWNER TO asplfact;
ALTER TABLE Kernel_Registry OWNER TO asplfact;

ALTER TABLE kernel_user_have OWNER TO asplfact;
ALTER TABLE kernel_group_have OWNER TO asplfact;
ALTER TABLE kernel_depends OWNER TO asplfact;
ALTER TABLE kernel_server OWNER TO asplfact;
ALTER TABLE kernel_belongs OWNER TO asplfact;

ALTER TABLE Kernel_User_id_seq OWNER TO asplfact;
ALTER TABLE Kernel_Group_id_seq OWNER TO asplfact;
ALTER TABLE Kernel_Permission_id_seq OWNER TO asplfact;
ALTER TABLE kernel_server_id_seq OWNER TO asplfact;


REVOKE ALL ON Kernel_User, 
	      Kernel_Group,
	      Kernel_Permission,	      
	      Kernel_Session,	      
	      Kernel_Registry,
	      kernel_user_have,
	      kernel_group_have,
	      kernel_depends,
	      Kernel_User_id_seq, 
	      Kernel_Group_id_seq,
	      Kernel_Permission_id_seq,
	      kernel_server_id_seq
FROM PUBLIC;

GRANT ALL ON  Kernel_User, 
	      Kernel_Group,
	      Kernel_Permission,
	      Kernel_Session,	      
	      Kernel_Registry,
	      kernel_user_have,
	      kernel_group_have,
	      kernel_depends, 
	      Kernel_User_id_seq, 
	      Kernel_Group_id_seq,
	      Kernel_Permission_id_seq,
	      kernel_server_id_seq
TO asplfact;

GRANT ALL ON  Kernel_User, 
	      Kernel_Group,
	      Kernel_Permission,
	      Kernel_Session,	      
	      Kernel_Registry,
	      kernel_user_have,
	      kernel_group_have,
	      kernel_depends,
	      Kernel_User_id_seq, 
	      Kernel_Group_id_seq,
	      Kernel_Permission_id_seq,
              kernel_server_id_seq
TO GROUP afkernel;


COMMIT WORK;
