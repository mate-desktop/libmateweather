/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* weather-wx.c - Weather server functions (WX Radar)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define MATEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include "weather.h"
#include "weather-priv.h"

static void
wx_finish (GObject *source, GAsyncResult *result, gpointer data)
{
    WeatherInfo *info = (WeatherInfo *)data;
    GdkPixbufAnimation *animation;
    GError *error = NULL;

    g_return_if_fail (info != NULL);

    animation = gdk_pixbuf_animation_new_from_stream_finish (result, &error);

    if (error != NULL) {
        g_warning ("Failed to get radar map image: %s.\n", error->message);
        request_done (info, error);
        g_error_free (error);
        return;
    }
    if (animation != NULL) {
        if (info->radar)
            g_object_unref (info->radar);
        info->radar = animation;
        g_object_ref (info->radar);
    }

    request_done (info, NULL);
}

static void
wx_got_chunk (GObject *source, GAsyncResult *result, gpointer data)
{
    WeatherInfo *info = (WeatherInfo *)data;
    GError *error = NULL;
    GInputStream *istream;

    g_return_if_fail (info != NULL);

    istream = soup_session_send_finish (SOUP_SESSION (source), result, &error);

    if (error != NULL) {
        g_warning ("Failed to get radar map image: %s.\n", error->message);
        g_error_free (error);
        request_done (info, error);
        return;
    }

    gdk_pixbuf_animation_new_from_stream_async (istream, NULL, wx_finish, data);
}

/* Get radar map and into newly allocated pixmap */
void
wx_start_open (WeatherInfo *info)
{
    gchar *url;
    SoupMessage *msg;
    WeatherLocation *loc;

    g_return_if_fail (info != NULL);
    info->radar = NULL;
    loc = info->location;
    g_return_if_fail (loc != NULL);

    if (info->radar_url)
	url = g_strdup (info->radar_url);
    else {
	if (loc->radar[0] == '-')
	    return;
	url = g_strdup_printf ("http://image.weather.com/web/radar/us_%s_closeradar_medium_usen.jpg", loc->radar);
    }

    msg = soup_message_new ("GET", url);
    if (!msg) {
	g_warning ("Invalid radar URL: %s\n", url);
	g_free (url);
	return;
    }

    soup_session_send_async (info->session, msg, G_PRIORITY_DEFAULT, NULL,
                             wx_got_chunk, info);
    g_free (url);

    info->requests_pending++;
}
