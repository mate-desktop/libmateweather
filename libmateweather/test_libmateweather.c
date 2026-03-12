/*
 * (c) 2017 Bastien Nocera <hadess@hadess.net>
 * (c) 2026 Victor Kareh <vkareh@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301  USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include <string.h>
#include <unistd.h>

#define MATEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include "mateweather-location.h"
#include "mateweather-timezone.h"

static GHashTable *validated_tzids;

static void
validate_timezone_id (const char *tzid)
{
    char *path;

    if (g_hash_table_contains (validated_tzids, tzid))
	return;

    g_hash_table_add (validated_tzids, g_strdup (tzid));

    path = g_build_filename (ZONEINFO_DIR, tzid, NULL);
    if (access (path, F_OK) != 0) {
	g_test_message ("Location has invalid timezone '%s'", tzid);
	g_test_fail ();
    }
    g_free (path);
}

static void
test_timezones_for_location (MateWeatherLocation *location)
{
    MateWeatherTimezone **tzs;
    MateWeatherLocation **children;
    guint i;

    tzs = mateweather_location_get_timezones (location);
    if (tzs) {
	for (i = 0; tzs[i] != NULL; i++) {
	    const char *tzid = mateweather_timezone_get_tzid (tzs[i]);
	    if (tzid)
		validate_timezone_id (tzid);
	}
	mateweather_location_free_timezones (location, tzs);
    }

    children = mateweather_location_get_children (location);
    for (i = 0; children[i] != NULL; i++)
	test_timezones_for_location (children[i]);
}

static void
test_timezones (void)
{
    MateWeatherLocation *world;

    validated_tzids = g_hash_table_new_full (g_str_hash, g_str_equal,
					    g_free, NULL);

    world = mateweather_location_new_world (FALSE);
    g_assert (world);

    test_timezones_for_location (world);

    mateweather_location_unref (world);
    g_hash_table_destroy (validated_tzids);
}

static void
log_handler (const char *log_domain, GLogLevelFlags log_level, const char *message, gpointer user_data)
{
	g_test_message ("%s", message);
}

int
main (int argc, char *argv[])
{
	setlocale (LC_ALL, "");

	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("https://github.com/mate-desktop/libmateweather/issues/");

	/* We need to handle log messages produced by g_message so they're interpreted correctly by the GTester framework */
	g_log_set_handler (NULL, G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, log_handler, NULL);

	g_test_add_func ("/weather/timezones", test_timezones);

	return g_test_run ();
}
