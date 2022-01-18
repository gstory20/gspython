/**********************************************************************
*
* MODULE: dirpath.c
*
* BRIEF DESCRIPTION:
*       Make sure a directory path ends in a /
*
* MODULE TYPE:
*       Source
*
* FULL DESCRIPTION:
*       This file contains the dirpath function which assures that a path
*       ends in a /
*
* FUNCTIONS DEFINED:
*       dirpath
*       {@functions_defined...@}
*
* GLOBAL DATA DEFINED:
*       {@global_data_defined...@}
*
* REVISION HISTORY:
*       12Mar93      G. Story        Original release
*       {@revision_history...@}
*
**********************************************************************/

/* Global Include Files */

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"

/* Subsystem Global Include Files */


/* Local Include Files */


/* Local Defines */


/* Global Data Referenced */


/* External Functions Referenced */


/**********************************************************************
*
* FUNCTION: dirpath
*
* DESCRIPTION:
*       ensure that a directory path ends in a /
*
* CALLING SEQUENCE:
*       void dirpath (path)
*
* INPUT PARAMETERS:
*       path                  Path to be examined
*
*
* OUTPUT PARAMETERS:
*       None
*
* RETURNED VALUE:
*       Nothing
*
* EXTERNAL FUNCTIONS CALLED:
*       {@external_functions_called...@}
*
* GLOBAL DATA MODIFIED:
*       {@global_data_modified...@}
*
* PSEUDO CODE:
*       {@detailed_pseudo_code@}
*
**********************************************************************/

void dirpath (char *path)

{
    if ((path[strlen (path) -1] != '/') && (strlen (path) > 0))
        strcat (path, "/");

}
