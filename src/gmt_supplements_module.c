/* $Id$
 *
 * Copyright (c) 2012-2015
 * by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis, and F. Wobbe
 * See LICENSE.TXT file for copying and redistribution conditions.
 */

/* gmt_supplements_module.c populates the external array of GMT supplements
 * module parameters such as name, group and purpose strings.
 * This file also contains the following convenience function to
 * display all module purposes:
 *
 *   void gmt_supplements_module_show_all (struct GMTAPI_CTRL *API);
 *
 * Developers of external APIs for accessing GMT modules may use this
 * function to retrieve option keys needed for module arg processing:
 *
 *   char * gmt_supplements_module_info (void *API, const char *module);
 *
 * DO NOT edit this file directly! Regenerate thee file by running
 * 	gmt_make_module_src.sh supplements
 */
#include "gmt.h"
#include "gmt_supplements_module.h"
#include <string.h>

/* Sorted array with information for all GMT supplements modules */

/* name, library, and purpose for each module */
struct Gmt_moduleinfo {
	const char *name;             /* Program name */
	const char *component;        /* Component (core, supplement, custom) */
	const char *purpose;          /* Program purpose */
	const char *keys;             /* Program option info for external APIs */
};

struct Gmt_moduleinfo g_supplements_module[] = {
	{"grd2sph", "core", "Compute spherical harmonic coefficients from global grid", "<GI,>DO"},
	{"gshhg", "gshhg", "Extract data tables from binary GSHHS or WDBII data files", ">DO"},
	{"img2grd", "img", "Extract a subset from an img file in Mercator or Geographic format", "<GI,GGO"},
	{"pscoupe", "meca", "Plot cross-sections of focal mechanisms", "<DI,>XO"},
	{"psmeca", "meca", "Plot focal mechanisms on maps", "<DI,>XO"},
	{"pspolar", "meca", "Plot polarities on the inferior focal half-sphere on maps", "<DI,>XO"},
	{"psvelo", "meca", "Plot velocity vectors, crosses, and wedges on maps", "<DI,>XO"},
	{"mgd77convert", "mgd77", "Convert MGD77 data to other file formats", ""},
	{"mgd77info", "mgd77", "Extract information about MGD77 files", ">TO"},
	{"mgd77list", "mgd77", "Extract data from MGD77 files", ">DO"},
	{"mgd77magref", "mgd77", "Evaluate the IGRF or CM4 magnetic field models", "<DI,>DO"},
	{"mgd77manage", "mgd77", "Manage the content of MGD77+ files", ""},
	{"mgd77path", "mgd77", "Return paths to MGD77 cruises and directories", ">TO"},
	{"mgd77sniffer", "mgd77", "Along-track quality control of MGD77 cruises", ""},
	{"mgd77track", "mgd77", "Plot track-line map of MGD77 cruises", ">XO"},
	{"dimfilter", "misc", "Directional filtering of grids in the space domain", "<GI,GGO"},
	{"gmtflexure", "potential", "Compute flexural deformation of 2-D loads, forces, and bending moments", "<DI,QDi,>DO"},
	{"gmtgravmag3d", "potential", "Compute the gravity/magnetic anomaly of a body by the method of Okabe", "<DI,FDi,GGO"},
	{"gravfft", "potential", "Compute gravitational attraction of 3-D surfaces and a little more (ATTENTION z positive up)", "<GI,GGO"},
	{"grdflexure", "potential", "Compute flexural deformation of 3-D surfaces for various rheologies", "<GI,GGO"},
	{"grdgravmag3d", "potential", "Computes the gravity effect of one (or two) grids by the method of Okabe", "<GI,FDi,GGO"},
	{"grdredpol", "potential", "Compute the Continuous Reduction To the Pole, AKA differential RTP", "<GI,EGi,GGO"},
	{"grdseamount", "potential", "Compute synthetic seamount (Gaussian, parabolic, cone or disc, circular or elliptical) bathymetry", "<DI,GGO"},
	{"talwani2d", "potential", "Compute free-air, geoid or vertical gravity gradients anomalies over 2-D bodies", "<DI,NDi,>DO"},
	{"talwani3d", "potential", "Compute free-air, geoid or vertical gravity gradients anomalies over 3-D bodies", "<DI,NDi,ZGi,GGo,>DO"},
	{"pssegyz", "segy", "Plot a SEGY file on a map in 3-D", ">XO"},
	{"pssegy", "segy", "Plot a SEGY file on a map", ">XO"},
	{"segy2grd", "segy", "Converting SEGY data to a GMT grid", "GGO"},
	{"backtracker", "spotter", "Generate forward and backward flowlines and hotspot tracks", "<DI,>DO,EDI,FDi"},
	{"grdpmodeler", "spotter", "Evaluate a plate model on a geographic grid", "<GI,ETI,FDi,GGo,>DG"},
	{"grdrotater", "spotter", "Finite rotation reconstruction of geographic grid", "<GI,ETI,FDi,GGO"},
	{"grdspotter", "spotter", "Create CVA image from a gravity or topography grid", "<GI,AGi,ETI,DGo,LGo,GGO"},
	{"hotspotter", "spotter", "Create CVA image from seamount locations", "<DI,ETI,GGO"},
	{"originator", "spotter", "Associate seamounts with nearest hotspot point sources", "<DI,ETI,FDi,>DO"},
	{"rotconverter", "spotter", "Manipulate total reconstruction and stage rotations", ">DO"},
	{"x2sys_binlist", "x2sys", "Create bin index listing from track data files", ">TO"},
	{"x2sys_cross", "x2sys", "Calculate crossovers between track data files", "ATi,>DO"},
	{"x2sys_datalist", "x2sys", "Extract content of track data files", "LTi,ITi,>DO"},
	{"x2sys_get", "x2sys", "Get track listing from track index database", ">TO"},
	{"x2sys_init", "x2sys", "Initialize a new x2sys track database", ""},
	{"x2sys_list", "x2sys", "Extract subset from crossover data base", "LTi,ITi,>DO"},
	{"x2sys_merge", "x2sys", "Merge an updated COEs table (smaller) into the main table (bigger)", ">TO"},
	{"x2sys_put", "x2sys", "Update track index database from track bin file", ""},
	{"x2sys_report", "x2sys", "Report statistics from crossover data base", "LTi,ITi,>TO"},
	{"x2sys_solve", "x2sys", "Determine least-squares systematic correction from crossovers", ">TO"},
	{NULL, NULL, NULL, NULL} /* last element == NULL detects end of array */
};

/* Pretty print all GMT supplements module names and their purposes */
void gmt_supplements_module_show_all (void *V_API) {
	unsigned int module_id = 0;
	char message[256];
	GMT_Message (V_API, GMT_TIME_NONE, "\n===  GMT suppl: The official supplements to the Generic Mapping Tools  ===\n");
	while (g_supplements_module[module_id].name != NULL) {
		if (module_id == 0 || strcmp (g_supplements_module[module_id-1].component, g_supplements_module[module_id].component)) {
			/* Start of new supplemental group */
			sprintf (message, "\nModule name:     Purpose of %s module:\n", g_supplements_module[module_id].component);
			GMT_Message (V_API, GMT_TIME_NONE, message);
			GMT_Message (V_API, GMT_TIME_NONE, "----------------------------------------------------------------\n");
		}
		sprintf (message, "%-16s %s\n",
			g_supplements_module[module_id].name, g_supplements_module[module_id].purpose);
			GMT_Message (V_API, GMT_TIME_NONE, message);
		++module_id;
	}
}

/* Lookup module id by name, return option keys pointer (for external API developers) */
const char *gmt_supplements_module_info (void *API, char *candidate) {
	int module_id = 0;

	/* Match actual_name against g_module[module_id].name */
	while (g_supplements_module[module_id].name != NULL &&
	       strcmp (candidate, g_supplements_module[module_id].name))
		++module_id;

	/* Return Module keys or NULL */
	return (g_supplements_module[module_id].keys);
}
