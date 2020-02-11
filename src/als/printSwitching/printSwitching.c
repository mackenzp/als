/**CFile****************************************************************

#==============================================================================#
#                                 printSwitching                               #
#                                                                              #
#                                 Ghasem Pasandi                               #
#      SPORT Lab, University of Southern California, Los Angeles, CA 90089     #
#                           http://sportlab.usc.edu/                           #
#                                                                              #
#==============================================================================#

Explanation:
prints switching activities of nodes

***********************************************************************/

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "base/abc/abc.h"
#include "base/main/mainInt.h"
#include "proof/fraig/fraig.h"
#include "opt/fxu/fxu.h" 
#include "opt/cut/cut.h"
#include "map/fpga/fpga.h"
#include "map/if/if.h"
#include "opt/res/res.h"
#include "opt/lpk/lpk.h"
#include "aig/aig/aig.h"
#include "opt/dar/dar.h"
#include "map/mio/mio.h"
#include "map/mio/mioInt.h"


//==========================================================================================================================
////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

static int SPORT_printSwitchingCommand  ( Abc_Frame_t * pAbc, int argc, char ** argv );

//==========================================================================================================================

void printSwitching_Init( Abc_Frame_t * pAbc )
{
	Cmd_CommandAdd( pAbc, "Synthesis",    "printSwitching",    SPORT_printSwitchingCommand,       0 );

}
//==========================================================================================================================
void printSwitching_End( Abc_Frame_t * pAbc )
{


}
//==========================================================================================================================
float als_NtkMfsNodesSwitching( Abc_Ntk_t * pNtk )
{
    extern Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );
    extern Vec_Int_t * Saig_ManComputeSwitchProbs( Aig_Man_t * p, int nFrames, int nPref, int fProbOne );
    Vec_Int_t * vSwitching;
    float * pSwitching;
    Abc_Ntk_t * pNtkStr;
    Aig_Man_t * pAig;
    Aig_Obj_t * pObjAig;
    Abc_Obj_t * pObjAbc, * pObjAbc2;
    float Result = (float)0;
    float temp_s = (float)0;
    int i;

    FILE *fp = fopen("nodes_switching.txt","w");

    // strash the network
    pNtkStr = Abc_NtkStrash( pNtk, 0, 1, 0 );
    Abc_NtkForEachObj( pNtk, pObjAbc, i )
        if ( Abc_ObjRegular((Abc_Obj_t *)pObjAbc->pTemp)->Type == ABC_FUNC_NONE || (!Abc_ObjIsCi(pObjAbc) && !Abc_ObjIsNode(pObjAbc)) )
            pObjAbc->pTemp = NULL;
    // map network into an AIG
    pAig = Abc_NtkToDar( pNtkStr, 0, (int)(Abc_NtkLatchNum(pNtk) > 0) );
    vSwitching = Saig_ManComputeSwitchProbs( pAig, 48, 16, 0 );
    pSwitching = (float *)vSwitching->pArray;
    Abc_NtkForEachNode( pNtk, pObjAbc, i )
    {
        if ( (pObjAbc2 = Abc_ObjRegular((Abc_Obj_t *)pObjAbc->pTemp)) && (pObjAig = Aig_Regular((Aig_Obj_t *)pObjAbc2->pTemp)) )
        {
            
            temp_s = Abc_ObjFanoutNum(pObjAbc) * pSwitching[pObjAig->Id];
            fprintf(fp,"%s %f\n",Abc_ObjName(pObjAbc), temp_s);
            Result += temp_s;
//            Abc_ObjPrint( stdout, pObjAbc );
//            printf( "%d = %.2f\n", i, Abc_ObjFanoutNum(pObjAbc) * pSwitching[pObjAig->Id] );
        }
    }
    fclose(fp);
    Vec_IntFree( vSwitching );
    Aig_ManStop( pAig );
    Abc_NtkDelete( pNtkStr );
    return Result;
}
//==================================================================================================
int SPORT_printSwitchingCommand ( Abc_Frame_t * pAbc, int argc, char ** argv )
{// pAbc is current network
    //FILE * pOut, *pErr;
    Abc_Ntk_t *pNtk;
    int c;
    //acquire current network information
    pNtk = Abc_FrameReadNtk(pAbc);
    //pOut = Abc_FrameReadOut(pAbc);
    //pErr = Abc_FrameReadErr(pAbc);
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h': 
                goto usage;
                break;
        }
    }

	
    float total_switching = als_NtkMfsNodesSwitching(pNtk);
    printf("Total switching : %f\n", total_switching);

    //pNtk->ntkFunc = ABC_FUNC_MAP;
    //pNtk->ntkType = ABC_NTK_LOGIC;

    return 0;

usage:
    Abc_Print( -2, "usage: printSwitching\n" );
    Abc_Print( -2, "\t prints switching activities of nodes in a file\n");
    return 1;
}


