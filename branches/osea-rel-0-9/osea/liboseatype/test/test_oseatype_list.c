#include <liboseatype/src/oseatype-list.h>
#include <liboseatype/src/oseatype-compound-object.h>
#include <liboseatype/src/oseatype-int.h>
#include <liboseatype/src/oseatype-char.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>

void abort_execution (GError *err, gint exit_value) 
{
	if (err) {
		fprintf (stderr, "Error: %s\n", err->message);
		g_error_free (err);
	}
	exit (exit_value);
}

void insert_integer (OseaTypeList *list, gint int_value, gint return_value_1, gint return_value_2) {
	OseaTypeInt * integer;
	GError * err = NULL;

	integer = OSEATYPE_INT(oseatype_int_new());
	if (! oseatype_int_set (integer, int_value, &err))
		abort_execution (err, return_value_1);
	
	if (! oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list), 
					       OSEATYPE_OBJECT(integer), 
					       GINT_TO_POINTER(-1), &err))
		abort_execution (err, return_value_2);
	
	
}

gint value_next_element (OseaTypeList *list)
{
	OseaTypeInt * integer;
	GError *err = NULL;

	integer = OSEATYPE_INT (oseatype_compound_object_get_next_element (OSEATYPE_COMPOUND_OBJECT(list), &err));
	if (err)
		abort_execution (err, 99);

	return oseatype_int_get (integer, NULL);

}

int main(int argc, char **argv)
{
	OseaTypeList * list = NULL;
	OseaTypeInt * integer = NULL;
	OseaTypeChar * character = NULL;
	GError * err = NULL;


	g_type_init();
	g_thread_init(NULL);

	
	// First, insert, finish and then extract data from a list;


	// We insert 5 elements to an empty list

	list = OSEATYPE_LIST(oseatype_list_new());

	insert_integer (list, 10, 1, 2);
	insert_integer (list, 20, 3, 4);
	insert_integer (list, 30, 5, 6);
	insert_integer (list, 40, 7, 8);
	insert_integer (list, 50, 9, 10);

	// finish the list

	if (! oseatype_compound_object_finish (OSEATYPE_COMPOUND_OBJECT(list), &err))
		abort_execution (err, 11);

	// try to insert a new element into an already finished list

	integer = OSEATYPE_INT(oseatype_int_new());
	if (! oseatype_int_set (integer, 60, &err))
		abort_execution (err, 12);
	if (oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list), 
					     OSEATYPE_OBJECT(integer), 
					     GINT_TO_POINTER(-1), &err))
		abort_execution (err, 13);
	g_error_free (err);
	g_object_unref (OSEATYPE_OBJECT(integer));
	integer = NULL;

	err = NULL;

	// now start to extract the elements


	if (value_next_element (list) != 10) return 14;
	if (value_next_element (list) != 20) return 15;
	if (value_next_element (list) != 30) return 16;
	if (value_next_element (list) != 40) return 17;
	if (value_next_element (list) != 50) return 18;

	integer = OSEATYPE_INT (oseatype_compound_object_get_next_element (OSEATYPE_COMPOUND_OBJECT(list), &err));
	if (! err)
		return 19;
	else {
		g_error_free (err);
		err = NULL;
	}

	g_object_unref (G_OBJECT(list));
	list = NULL;






	// Second, create a length and type defined list


	integer = OSEATYPE_INT(oseatype_int_new());
	list = OSEATYPE_LIST(oseatype_list_new_defined (3, G_OBJECT_TYPE_NAME(integer)));

	character = OSEATYPE_CHAR (oseatype_char_new());
	if (! oseatype_char_set (character, 'a', &err))
		abort_execution (err, 20);
	if (oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list),
					       OSEATYPE_OBJECT(character),
					       GINT_TO_POINTER(-1), &err)) {
		return 20;
	}
	g_object_unref (G_OBJECT(character));
	

	g_error_free (err);
	err = NULL;

	if (! oseatype_int_set (integer, 10, &err))
		abort_execution (err, 21);
	if (! oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list), 
					       OSEATYPE_OBJECT(integer), 
					       GINT_TO_POINTER(-1), &err)) 
		abort_execution (err, 21);
	 



	integer = OSEATYPE_INT(oseatype_int_new());
	if (! oseatype_int_set (integer, 30, &err))
		abort_execution (err, 22);
	if (! oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list), 
					     OSEATYPE_OBJECT(integer), 
					     GINT_TO_POINTER(2), &err))
		abort_execution (err, 23);

	integer = OSEATYPE_INT(oseatype_int_new());
	if (! oseatype_int_set (integer, 20, &err))
		abort_execution (err, 24);
	if (! oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list), 
					     OSEATYPE_OBJECT(integer), 
					     GINT_TO_POINTER(1), &err))
		abort_execution (err, 25);

	if (oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list), 
					     OSEATYPE_OBJECT(integer), 
					     GINT_TO_POINTER(1), &err)) {
		return 26;
	}
		
	g_error_free (err);
	err = NULL;

	if (oseatype_compound_object_finish (OSEATYPE_COMPOUND_OBJECT(list), &err))
		abort_execution (err, 27);

	g_error_free (err);
	err = NULL;

	
	if (value_next_element (list) != 10) return 28;
	if (value_next_element (list) != 20) return 29;
	if (value_next_element (list) != 30) return 30;
	
	g_object_unref (G_OBJECT(list));
	list = NULL;





	// Third, we are going to write and starting reading before finishing


	integer = OSEATYPE_INT(oseatype_int_new());
	list = OSEATYPE_LIST(oseatype_list_new_defined (-1, G_OBJECT_TYPE_NAME(integer)));

	if (! oseatype_int_set (integer, 10, &err))
		abort_execution (err, 31);
	if (! oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list), 
					       OSEATYPE_OBJECT(integer), 
					       GINT_TO_POINTER(-1), &err)) {
		abort_execution (err, 31);
	}

	if (value_next_element (list) != 10) return 32;

	integer = OSEATYPE_INT(oseatype_int_new());
	if (! oseatype_int_set (integer, 20, &err))
		abort_execution (err, 33);
	if (! oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list), 
					     OSEATYPE_OBJECT(integer), 
					     GINT_TO_POINTER(-1), &err))
		abort_execution (err, 33);

	if (value_next_element (list) != 20) return 34;

	integer = OSEATYPE_INT(oseatype_int_new());
	if (! oseatype_int_set (integer, 30, &err))
		abort_execution (err, 35);
	if (! oseatype_compound_object_insert (OSEATYPE_COMPOUND_OBJECT(list), 
					     OSEATYPE_OBJECT(integer), 
					     GINT_TO_POINTER(-1), &err))
		abort_execution (err, 35);

	if (value_next_element (list) != 30) return 36;

	if (! oseatype_compound_object_finish (OSEATYPE_COMPOUND_OBJECT(list), &err))
		abort_execution (err, 37);

	integer = OSEATYPE_INT (oseatype_compound_object_get_next_element (OSEATYPE_COMPOUND_OBJECT(list), &err));
	if (! err)
	        return 38;
		
	g_error_free (err);
	err = NULL;

	g_object_unref (G_OBJECT(list));
	list = NULL;

	if (argc > 1)
		sleep (60);


	return 0;

}
