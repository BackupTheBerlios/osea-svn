#include <liboseatype/src/oseatype-date.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	OseaTypeObject *object;
	GError *error = NULL;

	g_type_init();
	g_thread_init(NULL);

	object = oseatype_date_new();

	if (! oseatype_date_set (OSEATYPE_DATE(object), "2004-08-06", &error)) {
		printf ("Error\n");
		exit (1);
	} 
	
	if (strcmp (oseatype_date_get (OSEATYPE_DATE(object), NULL), "2004-08-06")) {
		printf ("Error\n");
		exit (1);
	}
		
	if (! oseatype_date_set (OSEATYPE_DATE(object), "2005-13-06", &error)) {
		printf ("Error: %s\n", error->message);
		exit (1);
	} 




}
