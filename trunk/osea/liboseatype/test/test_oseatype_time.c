#include <liboseatype/src/oseatype-time.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	OseaTypeObject *object;
	GError *error = NULL;

	g_type_init();
	g_thread_init(NULL);

	object = oseatype_time_new();

	if (! oseatype_time_set (OSEATYPE_TIME(object), "15:34:49", &error)) {
		printf ("Error: %d %s\n", error->code, error->message);
		exit (1);
	} 
	
	if (strcmp (oseatype_time_get (OSEATYPE_TIME(object), NULL), "15:34:49 +0100")) {
		printf ("Error: I have got '%s'\n", oseatype_time_get(OSEATYPE_TIME(object), NULL));
		exit (1);
	}
		




}
