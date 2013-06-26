/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* mateweather-prefs.c - Preference handling functions
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

#ifdef HAVE__NL_MEASUREMENT_MEASUREMENT
#include <langinfo.h>
#endif

#define MATEWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include "mateweather-prefs.h"
#include "weather-priv.h"

void
mateweather_prefs_load (MateWeatherPrefs *prefs, GSettings *settings)
{
    g_return_if_fail (prefs != NULL);
    g_return_if_fail (settings != NULL);

    if (prefs->location) {
	weather_location_free (prefs->location);
    }
    gchar *name, *code, *zone, *radar, *coordinates;
    name = g_settings_get_string (settings, "location4");
    code = g_settings_get_string (settings, "location1");
    zone = g_settings_get_string (settings, "location2");
    radar = g_settings_get_string (settings, "location3");
    coordinates = g_settings_get_string (settings, "coordinates");
    prefs->location = weather_location_new (name, code, zone, radar, coordinates,
					    NULL, NULL);

    g_free (name);
    g_free (code);
    g_free (zone);
    g_free (radar);
    g_free (coordinates);

    prefs->show_notifications =
    	g_settings_get_boolean (settings, "show-notifications");
    prefs->update_interval =
    	g_settings_get_int (settings, "auto-update-interval");
    prefs->update_interval = MAX (prefs->update_interval, 60);
    prefs->update_enabled =
    	g_settings_get_boolean (settings, "auto-update");
    prefs->detailed =
    	g_settings_get_boolean (settings, "enable-detailed-forecast");
    prefs->radar_enabled =
    	g_settings_get_boolean (settings, "enable-radar-map");
    prefs->use_custom_radar_url =
    	g_settings_get_boolean (settings, "use-custom-radar-url");

    if (prefs->radar) {
        g_free (prefs->radar);
        prefs->radar = NULL;
    }
    prefs->radar = g_settings_get_string (settings, "radar");
    
    prefs->temperature_unit = g_settings_get_enum (settings, GSETTINGS_TEMP_UNIT);
    prefs->speed_unit = g_settings_get_enum (settings, GSETTINGS_SPEED_UNIT);
    prefs->pressure_unit = g_settings_get_enum (settings, GSETTINGS_PRESSURE_UNIT);
    prefs->distance_unit = g_settings_get_enum (settings, GSETTINGS_DISTANCE_UNIT);

    return;
}

const char *
mateweather_prefs_get_temp_display_name (TempUnit temp)
{
    switch (temp) {
        case TEMP_UNIT_DEFAULT:
            return N_("Default");
        case TEMP_UNIT_KELVIN:
             /* translators: Kelvin */
            return N_("K");
        case TEMP_UNIT_CENTIGRADE:
            /* translators: Celsius */
            return N_("C");
        case TEMP_UNIT_FAHRENHEIT:
            /* translators: Fahrenheit */
            return N_("F");
	default:
    	    return N_("Invalid");
    }
}

const char *
mateweather_prefs_get_speed_display_name (SpeedUnit speed)
{
    switch (speed) {
        case SPEED_UNIT_DEFAULT:
            return N_("Default");
        case SPEED_UNIT_MS:
            /* translators: meters per second */
            return N_("m/s");
        case SPEED_UNIT_KPH:
            /* translators: kilometers per hour */
            return N_("km/h");
        case SPEED_UNIT_MPH:
            /* translators: miles per hour */
            return N_("mph");
        case SPEED_UNIT_KNOTS:
            /* translators: knots (speed unit) */
            return N_("knots");
        case SPEED_UNIT_BFT:
            /* translators: wind speed */
            return N_("Beaufort scale");
	default:
    	    return N_("Invalid");
    }
}

const char *
mateweather_prefs_get_pressure_display_name (PressureUnit pressure)
{
    switch (pressure) {
        case PRESSURE_UNIT_DEFAULT:
            return N_("Default");
        case PRESSURE_UNIT_KPA:
            /* translators: kilopascals */
            return N_("kPa");
        case PRESSURE_UNIT_HPA:
            /* translators: hectopascals */
            return N_("hPa");
        case PRESSURE_UNIT_MB:
            /* translators: millibars */
            return N_("mb");
        case PRESSURE_UNIT_MM_HG:
            /* translators: millimeters of mercury */
            return N_("mmHg");
        case PRESSURE_UNIT_INCH_HG:
            /* translators: inches of mercury */
            return N_("inHg");
        case PRESSURE_UNIT_ATM:
            /* translators: atmosphere */
            return N_("atm");
	default:
    	    return N_("Invalid");
    }
}

const char *
mateweather_prefs_get_distance_display_name (DistanceUnit distance)
{
    switch (distance) {
        case DISTANCE_UNIT_DEFAULT:
            return N_("Default");
        case DISTANCE_UNIT_METERS:
            /* translators: meters */
            return N_("m");
        case DISTANCE_UNIT_KM:
            /* translators: kilometers */
            return N_("km");
        case DISTANCE_UNIT_MILES:
            /* translators: miles */
            return N_("mi");
	default:
    	    return N_("Invalid");
    }
}
