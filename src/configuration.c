/* ************************************************************************** */
/*                                                                            */
/*     Copyright (C)	2004 Joao F. (joaof@users.sf.net)	              */
/* 			http://www.grisbi.org		                      */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation; either version 2 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*                                                                            */
/* ************************************************************************** */

#include "include.h"

/*START_INCLUDE*/
#include "configuration.h"
#include "./xmlnames.h"
/*END_INCLUDE*/

/*START_STATIC*/
static ConfigFormat *get_config_format();
/*END_STATIC*/

/*START_EXTERN*/
/*END_EXTERN*/


static ConfigFormat  g_config_format;


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  Function : load_config_format                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void load_config_format(GKeyFile *config)
{
    gchar * section;

    section = g_key_file_get_string ( config,
				      "Display",
				      tagAFFICHAGE_FORMAT_DATE_LISTE_OPE,
				      NULL );

    if ( section )
    {
	strcpy ( g_config_format.format_date_liste_ope, section );
    }
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  Function : save_config_format                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void save_config_format(GKeyFile *config)
{
     g_key_file_set_string ( config,
			    "Display",
			    tagAFFICHAGE_FORMAT_DATE_LISTE_OPE,
			    g_config_format.format_date_liste_ope );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  Function : set_default_config_format                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void set_default_config_format()
{
	strncpy(g_config_format.format_date_liste_ope, 
			DEFAULT_DATE_FORMAT_LISTE_OP, sizeof(g_config_format.format_date_liste_ope)
	);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  Function : get_config_format                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
ConfigFormat *get_config_format()
{
	return &g_config_format;
}

