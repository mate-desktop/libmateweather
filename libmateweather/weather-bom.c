/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* weather-bom.c - Australian Bureau of Meteorology forecast source
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

#include <string.h>

#define MATEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include "weather-priv.h"
#include "weather.h"

static void bom_finish(GObject *source, GAsyncResult *result, gpointer data) {
  char *p, *rp;
  WeatherInfo *info = (WeatherInfo *)data;
  GError *error = NULL;
  GBytes *bytes;
  const char *response_body = NULL;
  gsize len = 0;

  g_return_if_fail(info != NULL);

  bytes =
      soup_session_send_and_read_finish(SOUP_SESSION(source), result, &error);

  if (error != NULL) {
    g_warning("Failed to get BOM forecast data: %s.\n", error->message);
    request_done(info, error);
    g_error_free(error);
    return;
  }

  response_body = g_bytes_get_data(bytes, &len);

  p = xstrnstr(response_body, len, "Forecast for the rest");
  if (p != NULL) {
    rp = xstrnstr(p, len - (p - response_body),
                  "The next routine forecast will be issued");
    if (rp == NULL)
      info->forecast = g_strndup(p, len - (p - response_body));
    else
      info->forecast = g_strndup(p, rp - p);
  }

  if (info->forecast == NULL)
    info->forecast = g_strndup(response_body, len);

  g_bytes_unref(bytes);
  g_print("%s\n", info->forecast);
  request_done(info, NULL);
}

void bom_start_open(WeatherInfo *info) {
  gchar *url;
  SoupMessage *msg;
  WeatherLocation *loc;

  loc = info->location;

  url = g_strdup_printf("http://www.bom.gov.au/fwo/%s.txt", loc->zone + 1);

  msg = soup_message_new("GET", url);
  soup_session_send_and_read_async(info->session, msg, G_PRIORITY_DEFAULT, NULL,
                                   bom_finish, info);
  g_free(url);

  info->requests_pending++;
}
