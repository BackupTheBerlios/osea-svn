#include <liboseatype/src/oseatype-language.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	OseaTypeObject *object;
	GError *error = NULL;

	g_type_init();
	g_thread_init(NULL);

	object = oseatype_language_new();

	if (! oseatype_language_set (OSEATYPE_LANGUAGE(object), "ES", &error)) {
		printf ("Error\n");
		exit (1);
	} 
	
	if (g_ascii_strcasecmp (oseatype_language_get (OSEATYPE_LANGUAGE(object), NULL), "ES")) {
		printf ("Error\n");
		exit (1);
	}
		
	if (oseatype_language_set (OSEATYPE_LANGUAGE(object), "lq", &error)) {
		printf ("Error: 'lq' must not be a valid language value");
		exit (1);
	} 

	return 0;


}
