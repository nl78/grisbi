/* file gsb_file_others.c
 * used to save and load differents files under grisbi */
/*     Copyright (C)	2000-2005 C�dric Auger (cedric@grisbi.org) */
/* 			http://www.grisbi.org */

/*     This program is free software; you can redistribute it and/or modify */
/*     it under the terms of the GNU General Public License as published by */
/*     the Free Software Foundation; either version 2 of the License, or */
/*     (at your option) any later version. */

/*     This program is distributed in the hope that it will be useful, */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/*     GNU General Public License for more details. */

/*     You should have received a copy of the GNU General Public License */
/*     along with this program; if not, write to the Free Software */
/*     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/**
 * \file gsb_file_others.c
 * save and load differents files under grisbi
 * for now : export/import categories, budgetaries, reports
 */


#include "include.h"

/*START_INCLUDE*/
#include "gsb_file_others.h"
#include "categories_onglet.h"
#include "imputation_budgetaire.h"
#include "dialog.h"
#include "gsb_budget_data.h"
#include "gsb_category_data.h"
#include "gsb_file_load.h"
#include "gsb_file_save.h"
#include "navigation.h"
#include "utils_str.h"
#include "fichiers_gestion.h"
#include "structures.h"
#include "include.h"
/*END_INCLUDE*/

/*START_STATIC*/
static gboolean gsb_file_others_check_file ( gchar *file_content,
				      gint origin );
static gboolean gsb_file_others_load ( gchar *filename,
				gint origin );
static gulong gsb_file_others_save_general_part ( gulong iterator,
					   gulong *length_calculated,
					   gchar **file_content,
					   gchar *version );
static void gsb_file_others_start_element ( GMarkupParseContext *context,
				     const gchar *element_name,
				     const gchar **attribute_names,
				     const gchar **attribute_values,
				     GSList **import_list,
				     GError **error);
/*END_STATIC*/


/*START_EXTERN*/
extern GSList *liste_struct_categories;
extern GSList *liste_struct_etats;
extern GSList *liste_struct_imputation;
extern gint no_dernier_etat;
/*END_EXTERN*/


/** 
 * save the category file
 * we don't check anything here, all must be done before, here we just write
 * the file
 * use the same method as gsb_file_save_save_file
 *
 * \param filename the name of the file
 *
 * \return TRUE : ok, FALSE : problem
 * */
gboolean gsb_file_others_save_category ( gchar *filename )
{
    FILE *file;
    gulong iterator;
    gulong length_calculated;
    gchar *file_content;
    gulong length_part;

    if ( DEBUG )
	printf ( "gsb_file_others_save_category : %s\n",
		 filename );

    /* we begin to try to reserve enough memory to make the entire file
     * if not enough, we will make it growth later
     * the data below are about the memory to take for each part and for 1 of this part
     * with that i think we will allocate enough memory in one time but not too much */

    length_part = 500;
    
    length_calculated = length_part * g_slist_length ( liste_struct_categories );

    iterator = 0;
    file_content = malloc ( length_calculated );

    /* begin the file whit xml markup */
    
    iterator = gsb_file_save_append_part ( iterator,
					   &length_calculated,
					   &file_content,
					   g_strdup ("<?xml version=\"1.0\"?>\n<Grisbi_categ>\n"));

    iterator = gsb_file_others_save_general_part ( iterator,
						   &length_calculated,
						   &file_content,
						   VERSION_FICHIER_CATEG );

    iterator = gsb_file_save_category_part ( iterator,
					     &length_calculated,
					     &file_content );

    /* finish the file */

    iterator = gsb_file_save_append_part ( iterator,
					   &length_calculated,
					   &file_content,
					   g_strdup ("</Grisbi_categ>"));

    /* the file is in memory, we can save it */

    file = fopen ( filename,
			  "w" );

    if ( !file
	 ||
	 !fwrite ( file_content,
		   sizeof (gchar),
		   iterator,
		   file ))
    {
	dialogue_error ( g_strdup_printf ( _("Cannot save file '%s': %s"),
					   filename,
					   latin2utf8(strerror(errno)) ));
	free ( file_content);
	return ( FALSE );
    }
    
    fclose (file);
    free ( file_content);

    return ( TRUE );
}

/** 
 * save the budget file
 * we don't check anything here, all must be done before, here we just write
 * the file
 * use the same method as gsb_file_save_save_file
 *
 * \param filename the name of the file
 *
 * \return TRUE : ok, FALSE : problem
 * */
gboolean gsb_file_others_save_budget ( gchar *filename )
{
    FILE *file;
    gulong iterator;
    gulong length_calculated;
    gchar *file_content;
    gulong length_part;

    if ( DEBUG )
	printf ( "gsb_file_others_save_budget : %s\n",
		 filename );

    /* we begin to try to reserve enough memory to make the entire file
     * if not enough, we will make it growth later
     * the data below are about the memory to take for each part and for 1 of this part
     * with that i think we will allocate enough memory in one time but not too much */

    length_part = 500;
    
    length_calculated = length_part * g_slist_length ( liste_struct_imputation );

    iterator = 0;
    file_content = malloc ( length_calculated );

    /* begin the file whit xml markup */
    
    iterator = gsb_file_save_append_part ( iterator,
					   &length_calculated,
					   &file_content,
					   g_strdup ("<?xml version=\"1.0\"?>\n<Grisbi_budget>\n"));

    iterator = gsb_file_others_save_general_part ( iterator,
						   &length_calculated,
						   &file_content,
						   VERSION_FICHIER_IB );

    iterator = gsb_file_save_budgetary_part ( iterator,
					      &length_calculated,
					      &file_content );

    /* finish the file */

    iterator = gsb_file_save_append_part ( iterator,
					   &length_calculated,
					   &file_content,
					   g_strdup ("</Grisbi_budget>"));

    /* the file is in memory, we can save it */

    file = fopen ( filename,
			  "w" );

    if ( !file
	 ||
	 !fwrite ( file_content,
		   sizeof (gchar),
		   iterator,
		   file ))
    {
	dialogue_error ( g_strdup_printf ( _("Cannot save file '%s': %s"),
					   filename,
					   latin2utf8(strerror(errno)) ));
	free ( file_content);
	return ( FALSE );
    }
    
    fclose (file);
    free ( file_content);

    return ( TRUE );
}

/** 
 * save the report file
 * we don't check anything here, all must be done before, here we just write
 * the file
 * use the same method as gsb_file_save_save_file
 *
 * \param filename the name of the file
 *
 * \return TRUE : ok, FALSE : problem
 * */
gboolean gsb_file_others_save_report ( gchar *filename )
{
    FILE *file;
    gulong iterator;
    gulong length_calculated;
    gchar *file_content;
    gulong length_part;

    if ( DEBUG )
	printf ( "gsb_file_others_save_report : %s\n",
		 filename );

    /* we begin to try to reserve enough memory to make the entire file
     * if not enough, we will make it growth later
     * the data below are about the memory to take for each part and for 1 of this part
     * with that i think we will allocate enough memory in one time but not too much */

    length_part = 2500;
    
    length_calculated = length_part * g_slist_length ( liste_struct_etats );

    iterator = 0;
    file_content = malloc ( length_calculated );

    /* begin the file whit xml markup */
    
    iterator = gsb_file_save_append_part ( iterator,
					   &length_calculated,
					   &file_content,
					   g_strdup ("<?xml version=\"1.0\"?>\n<Grisbi_report>\n"));

    iterator = gsb_file_others_save_general_part ( iterator,
						   &length_calculated,
						   &file_content,
						   VERSION_FICHIER_ETAT );

    iterator = gsb_file_save_report_part ( iterator,
					   &length_calculated,
					   &file_content,
					   TRUE );

    /* finish the file */

    iterator = gsb_file_save_append_part ( iterator,
					   &length_calculated,
					   &file_content,
					   g_strdup ("</Grisbi_report>"));

    /* the file is in memory, we can save it */

    file = fopen ( filename,
			  "w" );

    if ( !file
	 ||
	 !fwrite ( file_content,
		   sizeof (gchar),
		   iterator,
		   file ))
    {
	dialogue_error ( g_strdup_printf ( _("Cannot save file '%s': %s"),
					   filename,
					   latin2utf8(strerror(errno)) ));
	free ( file_content);
	return ( FALSE );
    }
    
    fclose (file);
    free ( file_content);

    return ( TRUE );
}


/**
 * save the general part for the others files
 * for now, it's just the version of grisbi and the version of the file
 *
 * \param iterator the current iterator
 * \param length_calculated a pointer to the variable lengh_calculated
 * \param file_content a pointer to the variable file_content
 * \param version the version of the file (depends of categ, budget or report)
 *
 * \return the new iterator
 * */
gulong gsb_file_others_save_general_part ( gulong iterator,
					   gulong *length_calculated,
					   gchar **file_content,
					   gchar *version )
{
    gchar *new_string;

    /* save the general informations */

    new_string = g_strdup_printf ( "\t<General\n"
				   "\t\tFile_version=\"%s\"\n"
				   "\t\tGrisbi_version=\"%s\" />\n",
				   version,
				   VERSION );

    /* append the new string to the file content
     * and return the new iterator */

    return gsb_file_save_append_part ( iterator,
				       length_calculated,
				       file_content,
				       new_string );
}


/**
 * load a category file
 *
 * \param filename
 *
 * \return TRUE ok
 * */
gboolean gsb_file_others_load_category ( gchar *filename )
{
    return gsb_file_others_load ( filename,
				  0 );
}


/**
 * load a budgetary file
 *
 * \param filename
 *
 * \return TRUE ok
 * */
gboolean gsb_file_others_load_budget ( gchar *filename )
{
    return gsb_file_others_load ( filename,
				  1 );
}


/**
 * load a report file
 *
 * \param filename
 *
 * \return TRUE ok
 * */
gboolean gsb_file_others_load_report ( gchar *filename )
{
    return gsb_file_others_load ( filename,
				  2 );
}



/**
 * called to load the category/budget/report file given in param
 *
 * \filename the filename to load with full path
 *
 * \return TRUE if ok
 * */
gboolean gsb_file_others_load ( gchar *filename,
				gint origin )
{
    gchar *file_content;
    GSList *import_list = NULL;

    if ( DEBUG )
	printf ( "gsb_file_others_load %s\n", 
		 filename );

    /* general check */
    
    if ( !g_file_test ( filename,
			G_FILE_TEST_EXISTS ))
    {
	dialogue_error (g_strdup_printf (_("Cannot open file '%s': %s"),
					 filename,
					 latin2utf8 (strerror(errno))));
	remove_file_from_last_opened_files_list (filename);
	return FALSE;
    }

    /* check here if it's not a regular file */

    if ( !g_file_test ( filename,
			G_FILE_TEST_IS_REGULAR ))
    {
	dialogue_error ( g_strdup_printf ( _("%s doesn't seem to be a regular file,\nplease check it and try again."),
					   filename ));
	remove_file_from_last_opened_files_list (filename);
	return ( FALSE );
    }

    /* load the file */

    if ( g_file_get_contents ( filename,
			       &file_content,
			       NULL,
			       NULL ))
    {
	GMarkupParser *markup_parser = g_malloc0 (sizeof (GMarkupParser));
	GMarkupParseContext *context;

	/* check if it's a good file */

	if ( !gsb_file_others_check_file ( file_content,
					   origin ))
	{
	    free (file_content);
	    return FALSE;
	}

	/* for now, we load only after 0.6 files */
	/* fill the GMarkupParser for a new xml structure */

	markup_parser -> start_element = (void *) gsb_file_others_start_element;
	markup_parser -> error = (void *) gsb_file_load_error;

	context = g_markup_parse_context_new ( markup_parser,
					       0,
					       &import_list,
					       NULL );

	g_markup_parse_context_parse ( context,
				       file_content,
				       strlen (file_content),
				       NULL );

	/* now, import_list contains the list of categories/budget or report */

	switch ( origin )
	{
	    struct struct_etat *report;

	    case 0:
		/* comes for category */

		gsb_category_data_merge_category_list ( import_list );
		remplit_arbre_categ ();
		creation_liste_categ_combofix ();
		break;

	    case 1:
		/* comes for budget */

		gsb_budget_data_merge_budget_list ( import_list );
		remplit_arbre_imputation ();
		creation_liste_imputation_combofix ();
		break;

	    case 2:
		/* comes for report,
		 * as we cannot have the same things between differents grisbi files,
		 * we cannot export/import currencies, financial years, accounts names,
		 * categories, budgetaries and parties
		 * so we erase them here because perhaps they doesn't exist and show
		 * a warning : the user has to do it by himself (untill a druid to help him ?) */

		report = import_list -> data;

		if (report)
		{
		    /* set the new number */
		    report -> no_etat = ++no_dernier_etat;

		    /* set the currencies */
		    report -> devise_de_calcul_general = 1;
		    report -> devise_de_calcul_categ = 1;
		    report -> devise_de_calcul_ib = 1;
		    report -> devise_de_calcul_tiers = 1;
		    report -> choix_devise_montant = 1;

		    /* erase the financials years */
		    report -> no_exercices = NULL;
		    /* erase the accounts */
		    report -> no_comptes = NULL;
		    /* erase the transferts accounts */
		    report -> no_comptes_virements = NULL;
		    /* erase the categories */
		    report -> no_categ = NULL;
		    /* erase the budget */
		    report -> no_ib = NULL;
		    /* erase the parties */
		    report -> no_tiers = NULL;
		    /* erase the kinds of payment */
		    report -> noms_modes_paiement = NULL;

		    /* append it to the list */
		    liste_struct_etats = g_slist_append ( liste_struct_etats, 
							  report );
		    gsb_gui_navigation_add_report ( report );

		    /* inform the user of that */
		    dialogue_hint ( _("Some things in a report cannot be imported :\nThe selected lists of financial years, accounts, transfer accounts, categories, budgetaries, parties and kind of payments.\nSo that lists have been erased while the import.\nThe currencies have been set too on the first currency of this grisbi file.\nYou should check and modify that in the property box of that account."),
				    _("Importing a report"));
		}
		break;
	}

	g_markup_parse_context_free (context);
	g_free (markup_parser);
	g_free (file_content);
    }
    else
    {
	dialogue_error (g_strdup_printf (_("Cannot open file '%s': %s"),
					 filename,
					 latin2utf8 (strerror(errno))));
	remove_file_from_last_opened_files_list (filename);
	return FALSE;
    }

    return TRUE;
}

/**
 * check if the file given in param is a good file with the origin
 *
 * \param file_content the file loaded
 * \param origin what we want (0: category, 1:budget, 2:report)
 *
 * \return TRUE ok, FALSE else
 * */
gboolean gsb_file_others_check_file ( gchar *file_content,
				      gint origin )
{
    if ( !file_content
	 ||
	 strlen (file_content) < 37 )
    {
	dialogue_error ( _("This is not a grisbi file, loading canceled..."));
	return FALSE;
    }

    switch ( origin )
    {
	case 0:
	    if ( !strncmp ( file_content,
			    "<?xml version=\"1.0\"?>\n<Grisbi_categ>",
			    36 ))
		return TRUE;
	    dialogue_error ( _("This is not a category file, loading canceled..."));
	    break;

	case 1:
	    if ( !strncmp ( file_content,
			    "<?xml version=\"1.0\"?>\n<Grisbi_budget>",
			    37 ))
		return TRUE;
	    dialogue_error ( _("This is not a budget file, loading canceled..."));

	    break;

	case 2:
	    if ( !strncmp ( file_content,
			    "<?xml version=\"1.0\"?>\n<Grisbi_report>",
			    37 ))
		return TRUE;
	    dialogue_error ( _("This is not a report file, loading canceled..."));
	    break;
    }

    return FALSE;
}

void gsb_file_others_start_element ( GMarkupParseContext *context,
				     const gchar *element_name,
				     const gchar **attribute_names,
				     const gchar **attribute_values,
				     GSList **import_list,
				     GError **error)
{
    if ( !strcmp ( element_name,
		   "Category" ))
    {
	gsb_file_load_category ( attribute_names,
				 attribute_values,
				 import_list );
	return;
    }

    if ( !strcmp ( element_name,
		   "Sub_category" ))
    {
	gsb_file_load_sub_category ( attribute_names,
				     attribute_values,
				     import_list );
	return;
    }

    if ( !strcmp ( element_name,
		   "Budgetary" ))
    {
	gsb_file_load_budgetary ( attribute_names,
				  attribute_values,
				  import_list );
	return;
    }

    if ( !strcmp ( element_name,
		   "Sub_budgetary" ))
    {
	gsb_file_load_sub_budgetary ( attribute_names,
				      attribute_values,
				      import_list );
	return;
    }

    if ( !strcmp ( element_name,
		   "Report" ))
    {
	gsb_file_load_report ( attribute_names,
			       attribute_values,
			       import_list );
	return;
    }

    if ( !strcmp ( element_name,
		   "Text_comparison" ))
    {
	gsb_file_load_text_comparison ( attribute_names,
					attribute_values,
					import_list );
	return;
    }

    if ( !strcmp ( element_name,
		   "Amount_comparison" ))
    {
	gsb_file_load_amount_comparison ( attribute_names,
					  attribute_values,
					  import_list );
	return;
    }
}


