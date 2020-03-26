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
double FindCLNode(Abc_Obj_t * pNode);
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
float SPORT_AdditionalFanoutDelay(Abc_Obj_t * pNode){
    int i;
    //float R_gate = 1.0/(my_Gate->dArea);
    Abc_Obj_t * pFanout;
    Mio_Gate_t* Current_Gate, *Fanout_Gate;
    Mio_Pin_t * pPin, *cPin;
	
    float TotalFanout = 0.0;
    float AdditionalDelayDueToFanout = 0.0;
	
    Abc_ObjForEachFanout( pNode, pFanout, i ){
        if(Abc_ObjIsNode(pFanout)){
            Fanout_Gate = pFanout->pData;
            pPin = Mio_GateReadPins(Fanout_Gate);
            TotalFanout += pPin->dLoadInput;	
        }
    }
	
    Current_Gate = pNode->pData;
    cPin = Mio_GateReadPins(Current_Gate);
    AdditionalDelayDueToFanout = (cPin->dDelayFanoutRise)*TotalFanout;
    return (AdditionalDelayDueToFanout);
}
//==========================================================================================================================
double als_Find_CL_Node(Abc_Obj_t * pNode){
    //finding total output capacitance of pNode
    int i;
    Abc_Obj_t * pFanout;
    Mio_Gate_t *Fanout_Gate; // = (Mio_Gate_t *)ABC_ALLOC( Mio_Gate_t, 1);
    Mio_Pin_t * pPin;
	
    double TotalFanout = 0.0;
    //printf("here -123\n");
    Abc_ObjForEachFanout( pNode, pFanout, i ){
        if(Abc_ObjIsNode(pFanout)){
            Fanout_Gate = (Mio_Gate_t *)pFanout->pData;
            //printf("area : %f\n", Fanout_Gate->dArea);
            pPin = Mio_GateReadPins(Fanout_Gate);
            TotalFanout += pPin->dLoadInput;
            //printf("%f\n", pPin->dLoadInput);	
        }
    }
	
    return TotalFanout;
}
//==========================================================================================================================
void als_Find_Save_CL(Abc_Ntk_t* pNtk){
    Abc_Obj_t* pNode;
    int i;
    Abc_NtkForEachNode(pNtk, pNode, i){
        pNode->CL = als_Find_CL_Node(pNode);
    }
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
    int j;
    Abc_Obj_t* pFanout;    

    //calc CL before mapping is gone!
    als_Find_Save_CL(pNtk);

    FILE *fp = fopen("nodes_switching.txt","w");
    FILE *fp2 = fopen("nodes_switching_fanouts.txt","w");

    printf("here 1\n");
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
            
            //temp_s = Abc_ObjFanoutNum(pObjAbc) * pSwitching[pObjAig->Id];
            temp_s = pObjAbc->CL * pSwitching[pObjAig->Id];
            fprintf(fp,"%s %f\n",Abc_ObjName(pObjAbc), temp_s);
            Result += temp_s;
            //printing fanouts:
            //for indexing in python code:
            fprintf(fp2, "%s ", Abc_ObjName(pObjAbc));
            Abc_ObjForEachFanout(pObjAbc, pFanout, j){
                if(Abc_ObjIsNode(pFanout))
                    fprintf(fp2,"%s ",Abc_ObjName(pFanout));
            }
            fprintf(fp2,"\n");
//            Abc_ObjPrint( stdout, pObjAbc );
//            printf( "%d = %.2f\n", i, Abc_ObjFanoutNum(pObjAbc) * pSwitching[pObjAig->Id] );
        }
    }
    fclose(fp);
    fclose(fp2);
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

    printf("here 0\n");	
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


