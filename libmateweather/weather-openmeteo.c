/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* weather-openmeteo.c - Weather provider using Open-Meteo API
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

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MATEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include "weather.h"
#include "weather-priv.h"

#include <json-glib/json-glib.h>

#define OPENMETEO_BASE_URL "https://api.open-meteo.com/v1/forecast"
#define OPENMETEO_CURRENT_PARAMS "temperature_2m,relative_humidity_2m,apparent_temperature,precipitation,weather_code,cloud_cover,wind_speed_10m,wind_direction_10m,visibility"
#define OPENMETEO_DAILY_PARAMS "weather_code,temperature_2m_max,temperature_2m_min,precipitation_sum,precipitation_probability_max,wind_speed_10m_max,wind_direction_10m_dominant"
#define OPENMETEO_FORECAST_DAYS 7

static void
wmo_to_sky_conditions (gint wmo_code, gint cloud_cover,
                       WeatherSky *sky, WeatherConditions *cond)
{
    cond->qualifier = QUALIFIER_NONE;
    cond->phenomenon = PHENOMENON_NONE;
    cond->significant = FALSE;

    switch (wmo_code) {
    case 0:
        *sky = SKY_CLEAR;
        break;
    case 1:
        *sky = (cloud_cover < 30) ? SKY_FEW : SKY_SCATTERED;
        break;
    case 2:
        *sky = SKY_SCATTERED;
        break;
    case 3:
        *sky = SKY_OVERCAST;
        break;
    case 45:
    case 48:
        *sky = SKY_CLEAR;
        cond->phenomenon = PHENOMENON_FOG;
        cond->qualifier = (wmo_code == 48) ? QUALIFIER_FREEZING : QUALIFIER_MODERATE;
        cond->significant = TRUE;
        break;
    case 51:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_DRIZZLE;
        cond->qualifier = QUALIFIER_LIGHT;
        cond->significant = TRUE;
        break;
    case 53:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_DRIZZLE;
        cond->qualifier = QUALIFIER_MODERATE;
        cond->significant = TRUE;
        break;
    case 55:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_DRIZZLE;
        cond->qualifier = QUALIFIER_HEAVY;
        cond->significant = TRUE;
        break;
    case 56:
    case 57:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_DRIZZLE;
        cond->qualifier = QUALIFIER_FREEZING;
        cond->significant = TRUE;
        break;
    case 61:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_RAIN;
        cond->qualifier = QUALIFIER_LIGHT;
        cond->significant = TRUE;
        break;
    case 63:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_RAIN;
        cond->qualifier = QUALIFIER_MODERATE;
        cond->significant = TRUE;
        break;
    case 65:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_RAIN;
        cond->qualifier = QUALIFIER_HEAVY;
        cond->significant = TRUE;
        break;
    case 66:
    case 67:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_RAIN;
        cond->qualifier = QUALIFIER_FREEZING;
        cond->significant = TRUE;
        break;
    case 71:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_SNOW;
        cond->qualifier = QUALIFIER_LIGHT;
        cond->significant = TRUE;
        break;
    case 73:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_SNOW;
        cond->qualifier = QUALIFIER_MODERATE;
        cond->significant = TRUE;
        break;
    case 75:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_SNOW;
        cond->qualifier = QUALIFIER_HEAVY;
        cond->significant = TRUE;
        break;
    case 77:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_SNOW_GRAINS;
        cond->qualifier = QUALIFIER_MODERATE;
        cond->significant = TRUE;
        break;
    case 80:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_RAIN;
        cond->qualifier = QUALIFIER_SHOWERS;
        cond->significant = TRUE;
        break;
    case 81:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_RAIN;
        cond->qualifier = QUALIFIER_SHOWERS;
        cond->significant = TRUE;
        break;
    case 82:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_RAIN;
        cond->qualifier = QUALIFIER_HEAVY;
        cond->significant = TRUE;
        break;
    case 85:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_SNOW;
        cond->qualifier = QUALIFIER_SHOWERS;
        cond->significant = TRUE;
        break;
    case 86:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_SNOW;
        cond->qualifier = QUALIFIER_HEAVY;
        cond->significant = TRUE;
        break;
    case 95:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_RAIN;
        cond->qualifier = QUALIFIER_THUNDERSTORM;
        cond->significant = TRUE;
        break;
    case 96:
    case 99:
        *sky = SKY_OVERCAST;
        cond->phenomenon = PHENOMENON_HAIL;
        cond->qualifier = QUALIFIER_THUNDERSTORM;
        cond->significant = TRUE;
        break;
    default:
        *sky = cloud_cover >= 80 ? SKY_OVERCAST
              : cloud_cover >= 50 ? SKY_BROKEN
              : cloud_cover >= 20 ? SKY_SCATTERED
              : SKY_FEW;
        break;
    }
}

static gdouble
json_node_get_double (JsonNode *node)
{
    if (!node || json_node_get_value_type (node) != G_TYPE_DOUBLE)
        return -1.0;
    return json_node_get_double (node);
}

static gint
json_node_get_int (JsonNode *node)
{
    if (!node)
        return -1;
    if (json_node_get_value_type (node) == G_TYPE_INT64)
        return (gint) json_node_get_int64 (node);
    if (json_node_get_value_type (node) == G_TYPE_DOUBLE)
        return (gint) json_node_get_double (node);
    return -1;
}

static const gchar *
json_node_get_string (JsonNode *node)
{
    if (!node || json_node_get_value_type (node) != G_TYPE_STRING)
        return NULL;
    return json_node_get_string (node);
}

static time_t
parse_iso8601_time (const gchar *str)
{
    GDateTime *dt;
    time_t t;

    if (!str)
        return 0;

    dt = g_date_time_new_from_iso8601 (str, NULL);
    if (!dt)
        return 0;

    t = g_date_time_to_unix (dt);
    g_date_time_unref (dt);
    return t;
}

static void
openmeteo_finish (GObject *source, GAsyncResult *result, gpointer data)
{
    WeatherInfo *info = (WeatherInfo *)data;
    GError *error = NULL;
    GBytes *bytes;
    const gchar *response_body;
    gsize len;
    JsonParser *parser;
    JsonNode *root;
    JsonObject *root_obj, *current_obj, *daily_obj;

    g_return_if_fail (info != NULL);

    bytes = soup_session_send_and_read_finish (SOUP_SESSION (source),
                                               result, &error);

    if (error != NULL) {
        switch (error->code) {
        case SOUP_SESSION_ERROR_PARSING:
        case SOUP_SESSION_ERROR_ENCODING:
        case SOUP_SESSION_ERROR_TOO_MANY_REDIRECTS:
            info->network_error = TRUE;
            break;
        default:
            break;
        }
        g_warning ("Failed to get Open-Meteo data: %s.\n", error->message);
        request_done (info, error);
        g_error_free (error);
        return;
    }

    response_body = g_bytes_get_data (bytes, &len);

    parser = json_parser_new ();
    if (!json_parser_load_from_data (parser, response_body, (gssize) len, &error)) {
        g_warning ("Failed to parse Open-Meteo JSON: %s.\n", error->message);
        g_error_free (error);
        g_object_unref (parser);
        g_bytes_unref (bytes);
        info->valid = FALSE;
        request_done (info, NULL);
        return;
    }

    root = json_parser_get_root (parser);
    if (!root || json_node_get_node_type (root) != JSON_NODE_OBJECT) {
        g_warning ("Open-Meteo response root is not an object.\n");
        g_object_unref (parser);
        g_bytes_unref (bytes);
        info->valid = FALSE;
        request_done (info, NULL);
        return;
    }

    root_obj = json_node_get_object (root);

    /* "current" is optional — may be absent if API doesn't return it */
    current_obj = NULL;
    {
        JsonNode *cn = json_object_get_member (root_obj, "current");
        if (cn && json_node_get_node_type (cn) == JSON_NODE_OBJECT)
            current_obj = json_node_get_object (cn);
    }

    daily_obj = NULL;
    {
        JsonNode *dn = json_object_get_member (root_obj, "daily");
        if (dn && json_node_get_node_type (dn) == JSON_NODE_OBJECT)
            daily_obj = json_node_get_object (dn);
    }

    if (!current_obj) {
        g_warning ("Open-Meteo response missing 'current' object.\n");
        g_object_unref (parser);
        g_bytes_unref (bytes);
        info->valid = FALSE;
        request_done (info, NULL);
        return;
    }

    /* Parse current conditions */
    do {
        JsonNode *node;
        gdouble dval;
        gint ival;

        /* Temperature: °C -> °F */
        node = json_object_get_member (current_obj, "temperature_2m");
        dval = json_node_get_double (node);
        if (dval > -100.0)
            info->temp = TEMP_C_TO_F (dval);

        /* Apparent temperature: °C -> °F */
        node = json_object_get_member (current_obj, "apparent_temperature");
        dval = json_node_get_double (node);
        if (dval > -100.0)
            info->dew = TEMP_C_TO_F (dval);

        /* Relative humidity */
        node = json_object_get_member (current_obj, "relative_humidity_2m");
        ival = json_node_get_int (node);
        if (ival >= 0) {
            /* Derive dew point from temp and humidity */
            if (info->temp > -500.0) {
                gdouble t = TEMP_F_TO_C (info->temp);
                gdouble rh = (gdouble) ival;
                gdouble gamma = log (rh / 100.0) + (17.27 * t) / (237.7 + t);
                gdouble dew_c = (237.7 * gamma) / (17.27 - gamma);
                info->dew = TEMP_C_TO_F (dew_c);
            }
        }

        /* Weather code + cloud cover -> sky + conditions */
        node = json_object_get_member (current_obj, "weather_code");
        ival = json_node_get_int (node);

        gint cloud_cover = 0;
        node = json_object_get_member (current_obj, "cloud_cover");
        if (node)
            cloud_cover = json_node_get_int (node);
        if (cloud_cover < 0) cloud_cover = 0;

        wmo_to_sky_conditions (ival, cloud_cover, &info->sky, &info->cond);

        /* Wind speed: km/h -> knots */
        node = json_object_get_member (current_obj, "wind_speed_10m");
        dval = json_node_get_double (node);
        if (dval >= 0.0)
            info->windspeed = WINDSPEED_KMH_TO_KNOTS (dval);

        /* Wind direction: degrees -> compass point */
        node = json_object_get_member (current_obj, "wind_direction_10m");
        ival = json_node_get_int (node);
        if (ival >= 0) {
            gint dir = ival;
            if ((349 <= dir) || (dir <= 11))
                info->wind = WIND_N;
            else if ((12 <= dir) && (dir <= 33))
                info->wind = WIND_NNE;
            else if ((34 <= dir) && (dir <= 56))
                info->wind = WIND_NE;
            else if ((57 <= dir) && (dir <= 78))
                info->wind = WIND_ENE;
            else if ((79 <= dir) && (dir <= 101))
                info->wind = WIND_E;
            else if ((102 <= dir) && (dir <= 123))
                info->wind = WIND_ESE;
            else if ((124 <= dir) && (dir <= 146))
                info->wind = WIND_SE;
            else if ((147 <= dir) && (dir <= 168))
                info->wind = WIND_SSE;
            else if ((169 <= dir) && (dir <= 191))
                info->wind = WIND_S;
            else if ((192 <= dir) && (dir <= 213))
                info->wind = WIND_SSW;
            else if ((214 <= dir) && (dir <= 236))
                info->wind = WIND_SW;
            else if ((237 <= dir) && (dir <= 258))
                info->wind = WIND_WSW;
            else if ((259 <= dir) && (dir <= 281))
                info->wind = WIND_W;
            else if ((282 <= dir) && (dir <= 303))
                info->wind = WIND_WNW;
            else if ((304 <= dir) && (dir <= 326))
                info->wind = WIND_NW;
            else if ((327 <= dir) && (dir <= 348))
                info->wind = WIND_NNW;
        }

        /* Visibility: meters -> miles */
        node = json_object_get_member (current_obj, "visibility");
        ival = json_node_get_int (node);
        if (ival > 0)
            info->visibility = (WeatherVisibility) ival / 1609.344;

        /* Update time */
        node = json_object_get_member (current_obj, "time");
        {
            const gchar *timestr = json_node_get_string (node);
            if (timestr)
                info->update = parse_iso8601_time (timestr);
        }
    } while (0);

    /* Parse daily forecast */
    if (daily_obj) {
        JsonArray *times, *wcodes, *tmax, *tmin, *wspeed, *wdir;
        gint i, count;

        times = json_object_get_array_member (daily_obj, "time");
        wcodes = json_object_get_array_member (daily_obj, "weather_code");
        tmax = json_object_get_array_member (daily_obj, "temperature_2m_max");
        tmin = json_object_get_array_member (daily_obj, "temperature_2m_min");
        wspeed = json_object_get_array_member (daily_obj, "wind_speed_10m_max");
        wdir = json_object_get_array_member (daily_obj, "wind_direction_10m_dominant");

        if (times && wcodes && tmax && tmin) {
            count = json_array_get_length (times);
            if (count > OPENMETEO_FORECAST_DAYS)
                count = OPENMETEO_FORECAST_DAYS;

            for (i = 0; i < count; i++) {
                WeatherInfo *day_info;
                JsonNode *day_node;
                gint wmo;

                day_info = weather_info_clone (info);
                day_info->forecast_type = FORECAST_LIST;
                day_info->requests_pending = 0;
                day_info->session = NULL;
                day_info->forecast = NULL;
                day_info->forecast_list = NULL;
                day_info->radar = NULL;
                day_info->radar_url = NULL;
                day_info->finish_cb = NULL;
                day_info->cb_data = NULL;
                day_info->sunriseValid = FALSE;
                day_info->sunsetValid = FALSE;
                day_info->moonValid = FALSE;

                /* Date */
                day_node = json_array_get_element (times, i);
                if (day_node) {
                    const gchar *dstr = json_node_get_string (day_node);
                    if (dstr)
                        day_info->update = parse_iso8601_time (dstr);
                }

                /* Weather code */
                day_node = json_array_get_element (wcodes, i);
                wmo = json_node_get_int (day_node);

                /* Temperatures: °C -> °F */
                day_node = json_array_get_element (tmax, i);
                if (day_node) {
                    gdouble v = json_node_get_double (day_node);
                    if (v > -100.0)
                        day_info->temp_max = TEMP_C_TO_F (v);
                }

                day_node = json_array_get_element (tmin, i);
                if (day_node) {
                    gdouble v = json_node_get_double (day_node);
                    if (v > -100.0)
                        day_info->temp_min = TEMP_C_TO_F (v);
                }

                day_info->tempMinMaxValid = (day_info->temp_max > -500.0 && day_info->temp_min > -500.0);

                /* Wind */
                day_node = json_array_get_element (wspeed, i);
                if (day_node) {
                    gdouble v = json_node_get_double (day_node);
                    if (v >= 0.0)
                        day_info->windspeed = WINDSPEED_KMH_TO_KNOTS (v);
                }

                day_node = json_array_get_element (wdir, i);
                if (day_node)
                    day_info->wind = (WeatherWindDirection) json_node_get_int (day_node);

                /* Sky + conditions from WMO code */
                wmo_to_sky_conditions (wmo, 0, &day_info->sky, &day_info->cond);
                day_info->valid = TRUE;

                info->forecast_list = g_slist_append (info->forecast_list, day_info);
            }
        }
    }

    info->valid = TRUE;

    g_object_unref (parser);
    g_bytes_unref (bytes);

    request_done (info, NULL);
}

void
openmeteo_start_open (WeatherInfo *info)
{
    WeatherLocation *loc;
    SoupMessage *msg;
    gchar *uri;
    gdouble lat_deg, lon_deg;

    g_return_if_fail (info != NULL);
    info->valid = info->network_error = FALSE;
    loc = info->location;
    if (loc == NULL) {
        g_warning ("WeatherInfo missing location");
        return;
    }

    if (!loc->latlon_valid) {
        g_warning ("Open-Meteo requires valid coordinates for location '%s'", loc->name ? loc->name : "(null)");
        info->network_error = TRUE;
        request_done (info, NULL);
        return;
    }

    /* Convert radians to degrees */
    lat_deg = loc->latitude * 180.0 / M_PI;
    lon_deg = loc->longitude * 180.0 / M_PI;

    uri = g_strdup_printf (
        "%s?latitude=%.6f&longitude=%.6f"
        "&current=%s"
        "&daily=%s"
        "&timezone=UTC"
        "&forecast_days=%d",
        OPENMETEO_BASE_URL,
        lat_deg, lon_deg,
        OPENMETEO_CURRENT_PARAMS,
        OPENMETEO_DAILY_PARAMS,
        OPENMETEO_FORECAST_DAYS);

    msg = soup_message_new ("GET", uri);
    g_free (uri);

    if (!msg) {
        g_warning ("Failed to create SoupMessage for Open-Meteo");
        return;
    }

    soup_session_send_and_read_async (info->session, msg, G_PRIORITY_DEFAULT,
                                      NULL, openmeteo_finish, info);
    info->requests_pending++;
}
