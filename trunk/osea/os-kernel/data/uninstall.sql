-- BEGIN WORK;
-- First drop all relations
DROP TABLE kernel_user_have;
DROP TABLE kernel_group_have;
DROP TABLE kernel_depends;
DROP TABLE kernel_belongs;

-- Drop all tables
DROP TABLE Kernel_User;
DROP TABLE Kernel_Group;
DROP TABLE Kernel_Permission;
DROP TABLE Kernel_Server;
DROP TABLE Kernel_Session;
DROP TABLE Kernel_Registry;

-- Misc drop
DROP GROUP afkernel;
-- COMMIT WORK;