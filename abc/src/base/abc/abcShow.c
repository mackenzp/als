/**CFile****************************************************************

  FileName    [abcShow.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Network and node package.]

  Synopsis    [Visualization procedures using DOT software and GSView.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: abcShow.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#ifdef WIN32
#include <process.h> 
#else
#include <unistd.h>
#endif


#include "abc.h"
#include <string.h>
#include "base/main/main.h"
#include "base/io/ioAbc.h"
#include "map/mio/mio.h"

#ifdef ABC_USE_CUDD
#include "bdd/extrab/extraBdd.h"
#endif

ABC_NAMESPACE_IMPL_START


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

extern void Abc_ShowFile( char * FileNameDot );
static void Abc_ShowGetFileName( char * pName, char * pBuffer );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

#ifdef ABC_USE_CUDD

/**Function*************************************************************

  Synopsis    [Visualizes BDD of the node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NodeShowBddOne( DdManager * dd, DdNode * bFunc )
{
    char * FileNameDot = "temp.dot";
    FILE * pFile;
    if ( (pFile = fopen( FileNameDot, "w" )) == NULL )
    {
        fprintf( stdout, "Cannot open the intermediate file \"%s\".\n", FileNameDot );
        return;
    }
    Cudd_DumpDot( dd, 1, (DdNode **)&bFunc, NULL, NULL, pFile );
    fclose( pFile );
    Abc_ShowFile( FileNameDot );
}

/**Function*************************************************************

  Synopsis    [Visualizes BDD of the node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NodeShowBdd( Abc_Obj_t * pNode, int fCompl )
{
    FILE * pFile;
    Vec_Ptr_t * vNamesIn;
    char FileNameDot[200];
    char * pNameOut;
    DdManager * dd = (DdManager *)pNode->pNtk->pManFunc;

    assert( Abc_NtkIsBddLogic(pNode->pNtk) );
    // create the file name
    Abc_ShowGetFileName( Abc_ObjName(pNode), FileNameDot );
    // check that the file can be opened
    if ( (pFile = fopen( FileNameDot, "w" )) == NULL )
    {
        fprintf( stdout, "Cannot open the intermediate file \"%s\".\n", FileNameDot );
        return;
    }

    // set the node names 
    vNamesIn = Abc_NodeGetFaninNames( pNode );
    pNameOut = Abc_ObjName(pNode);
    if ( fCompl )
        Cudd_DumpDot( dd, 1, (DdNode **)&pNode->pData, (char **)vNamesIn->pArray, &pNameOut, pFile );
    else
    {
        DdNode * bAdd = Cudd_BddToAdd( dd, (DdNode *)pNode->pData ); Cudd_Ref( bAdd );
        Cudd_DumpDot( dd, 1, (DdNode **)&bAdd, (char **)vNamesIn->pArray, &pNameOut, pFile );
        Cudd_RecursiveDeref( dd, bAdd );
    }
    Abc_NodeFreeNames( vNamesIn );
    Abc_NtkCleanCopy( pNode->pNtk );
    fclose( pFile );

    // visualize the file 
    Abc_ShowFile( FileNameDot );
}
void Abc_NtkShowBdd( Abc_Ntk_t * pNtk, int fCompl )
{
    char FileNameDot[200];
    char ** ppNamesIn, ** ppNamesOut;
    DdManager * dd; DdNode * bFunc;
    Vec_Ptr_t * vFuncsGlob;
    Abc_Obj_t * pObj; int i;
    FILE * pFile;

    assert( Abc_NtkIsStrash(pNtk) );
    dd = (DdManager *)Abc_NtkBuildGlobalBdds( pNtk, 10000000, 1, 1, 0, 0 );
    if ( dd == NULL )
    {
        printf( "Construction of global BDDs has failed.\n" );
        return;
    }
    //printf( "Shared BDD size = %6d nodes.\n", Cudd_ReadKeys(dd) - Cudd_ReadDead(dd) );

    // complement the global functions
    vFuncsGlob = Vec_PtrAlloc( Abc_NtkCoNum(pNtk) );
    Abc_NtkForEachCo( pNtk, pObj, i )
        Vec_PtrPush( vFuncsGlob, Abc_ObjGlobalBdd(pObj) );

    // create the file name
    Abc_ShowGetFileName( pNtk->pName, FileNameDot );
    // check that the file can be opened
    if ( (pFile = fopen( FileNameDot, "w" )) == NULL )
    {
        fprintf( stdout, "Cannot open the intermediate file \"%s\".\n", FileNameDot );
        return;
    }

    // set the node names 
    ppNamesIn = Abc_NtkCollectCioNames( pNtk, 0 );
    ppNamesOut = Abc_NtkCollectCioNames( pNtk, 1 );
    if ( fCompl )
        Cudd_DumpDot( dd, Abc_NtkCoNum(pNtk), (DdNode **)Vec_PtrArray(vFuncsGlob), ppNamesIn, ppNamesOut, pFile );
    else
    {
        DdNode ** pbAdds = ABC_ALLOC( DdNode *, Vec_PtrSize(vFuncsGlob) );
        Vec_PtrForEachEntry( DdNode *, vFuncsGlob, bFunc, i )
            { pbAdds[i] = Cudd_BddToAdd( dd, bFunc ); Cudd_Ref( pbAdds[i] ); }
        Cudd_DumpDot( dd, Abc_NtkCoNum(pNtk), pbAdds, ppNamesIn, ppNamesOut, pFile );
        Vec_PtrForEachEntry( DdNode *, vFuncsGlob, bFunc, i )
            Cudd_RecursiveDeref( dd, pbAdds[i] );
        ABC_FREE( pbAdds );
    }
    ABC_FREE( ppNamesIn );
    ABC_FREE( ppNamesOut );
    fclose( pFile );

    // cleanup
    Abc_NtkFreeGlobalBdds( pNtk, 0 );
    Vec_PtrForEachEntry( DdNode *, vFuncsGlob, bFunc, i )
        Cudd_RecursiveDeref( dd, bFunc );
    Vec_PtrFree( vFuncsGlob );
    Extra_StopManager( dd );
    Abc_NtkCleanCopy( pNtk );

    // visualize the file 
    Abc_ShowFile( FileNameDot );
}

#else
void Abc_NodeShowBdd( Abc_Obj_t * pNode, int fCompl ) {}
void Abc_NtkShowBdd( Abc_Ntk_t * pNtk, int fCompl ) {}
#endif

/**Function*************************************************************

  Synopsis    [Visualizes a reconvergence driven cut at the node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NodeShowCut( Abc_Obj_t * pNode, int nNodeSizeMax, int nConeSizeMax )
{
    FILE * pFile;
    char FileNameDot[200];
    Abc_ManCut_t * p;
    Vec_Ptr_t * vCutSmall;
    Vec_Ptr_t * vCutLarge;
    Vec_Ptr_t * vInside;
    Vec_Ptr_t * vNodesTfo;
    Abc_Obj_t * pTemp;
    int i;

    assert( Abc_NtkIsStrash(pNode->pNtk) );

    // start the cut computation manager
    p = Abc_NtkManCutStart( nNodeSizeMax, nConeSizeMax, 2, ABC_INFINITY );
    // get the recovergence driven cut
    vCutSmall = Abc_NodeFindCut( p, pNode, 1 );
    // get the containing cut
    vCutLarge = Abc_NtkManCutReadCutLarge( p );
    // get the array for the inside nodes
    vInside = Abc_NtkManCutReadVisited( p );
    // get the inside nodes of the containing cone
    Abc_NodeConeCollect( &pNode, 1, vCutLarge, vInside, 1 );

    // add the nodes in the TFO 
    vNodesTfo = Abc_NodeCollectTfoCands( p, pNode, vCutSmall, ABC_INFINITY );
    Vec_PtrForEachEntry( Abc_Obj_t *, vNodesTfo, pTemp, i )
        Vec_PtrPushUnique( vInside, pTemp );

    // create the file name
    Abc_ShowGetFileName( Abc_ObjName(pNode), FileNameDot );
    // check that the file can be opened
    if ( (pFile = fopen( FileNameDot, "w" )) == NULL )
    {
        fprintf( stdout, "Cannot open the intermediate file \"%s\".\n", FileNameDot );
        return;
    }
    // add the root node to the cone (for visualization)
    Vec_PtrPush( vCutSmall, pNode );
    // write the DOT file
    Io_WriteDotNtk( pNode->pNtk, vInside, vCutSmall, FileNameDot, 0, 0 );
    // stop the cut computation manager
    Abc_NtkManCutStop( p );

    // visualize the file 
    Abc_ShowFile( FileNameDot );
}

/**Function*************************************************************

  Synopsis    [Visualizes AIG with choices.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_LookupNodeNum(char* test){
    if(strcmp(test, "inv1") == 0){
        return 0;
    }
    else if(strcmp(test, "inv2") == 0){
        return 1;
    }
    else if(strcmp(test, "inv3") == 0){
        return 2;
    }
    else if(strcmp(test, "inv4") == 0){
        return 3;
    }
    else if(strcmp(test, "nand2") == 0){
        return 4;
    }
    else if(strcmp(test, "nand3") == 0){
        return 5;
    }
    else if(strcmp(test, "nand4") == 0){
        return 6;
    }
    else if(strcmp(test, "nor2") == 0){
        return 7;
    }
    else if(strcmp(test, "nor3") == 0){
        return 8;
    }
    else if(strcmp(test, "nor4") == 0){
        return 9;
    }
    else if(strcmp(test, "and2") == 0){
        return 10;
    }
    else if(strcmp(test, "or2") == 0){
        return 11;
    }
    else if(strcmp(test, "xor2a") == 0){
        return 12;
    }
    else if(strcmp(test, "xor2b") == 0){
        return 13;
    }
    else if(strcmp(test, "xnor2a") == 0){
        return 14;
    }
    else if(strcmp(test, "xnor2b") == 0){
        return 15;
    }
    else if(strcmp(test, "aoi21") == 0){
        return 16;
    }
    else if(strcmp(test, "aoi22") == 0){
        return 17;
    }
    else if(strcmp(test, "oai21") == 0){
        return 18;
    }
    else if(strcmp(test, "oai22") == 0){
        return 19;
    }
    else if(strcmp(test, "BUF1") == 0){
        return 20;
    }
    else if(strcmp(test, "DFF") == 0){
        return 21;
    }
    else if(strcmp(test, "zero") == 0){
        return 22;
    }
    else if(strcmp(test, "one") == 0){
        return 23;
    }
    return -1;
}

void Abc_NtkShow( Abc_Ntk_t * pNtk0, int fGateNames, int fSeq, int fUseReverse )
{
    FILE * pFile;
    Abc_Ntk_t * pNtk;
    Abc_Obj_t * pNode;
    Vec_Ptr_t * vNodes;
    int nBarBufs;
    char FileNameDot[200];
    int i;

    // inserted variables for node feature traversal - mp
    int k;
    Abc_Obj_t * pCurrent;
    //


    assert( Abc_NtkIsStrash(pNtk0) || Abc_NtkIsLogic(pNtk0) );
    if ( Abc_NtkIsStrash(pNtk0) && Abc_NtkGetChoiceNum(pNtk0) )
    {
        printf( "Temporarily visualization of AIGs with choice nodes is disabled.\n" );
        return;
    }
    // create the file name
    Abc_ShowGetFileName( pNtk0->pName, FileNameDot );
    // check that the file can be opened
    if ( (pFile = fopen( FileNameDot, "w" )) == NULL )
    {
        fprintf( stdout, "SNeh   Cannot open the intermediate file \"%s\".\n", FileNameDot );
        return;
    }
    fclose( pFile );


    // convert to logic SOP
    pNtk = Abc_NtkDup( pNtk0 );
    if ( Abc_NtkIsLogic(pNtk) && !Abc_NtkHasMapping(pNtk) )
        Abc_NtkToSop( pNtk, -1, ABC_INFINITY );

    // collect all nodes in the network
    vNodes = Vec_PtrAlloc( 100 );
    Abc_NtkForEachObj( pNtk, pNode, i )
        Vec_PtrPush( vNodes, pNode );
    

    int maxFanin = 0;
    int maxFanout = 0;
    int maxFanin_id = 0;
    int maxFanout_id = 0;
    int sumFanout = 0;
    int sumFanin = 0;
    int numNodes = 0;
    int numNodesless10 = 0;
    int faninNum = 0;
    int fanoutNum = 0;

    FILE *t = fopen("train_data.txt", "w");
    //FILE *fo = fopen("fanout_stats.txt", "a");
    //FILE *fi = fopen("fanin_stats.txt", "a");
    


    // print to console?
    int printToConsole = 0;
    int printTrainVec = 1;
    int printStats = 0;

    // trainSizeFanin is the set max size of fanin nodes
    int trainSizeFanin = 4;
    // trainSizeFanout is the set max size of fanout nodes
    int trainSizeFanout = 10;
    // keep track of fanin count to print 0s at end
    int countFanin = 0;
    // keep track of fanout count to print 0s at end
    int countFanout = 0;

    // testing Here!
    
    // Must run an initial traversal through the network,
    // because the content "pNtk->LevelMax" always returns 0.
    // Traverse the network to obtain the max levels feature for dynamic logic depth calc
    int maxLevel = 0;
    int currLevel = 0;
    Abc_NtkForEachObj(pNtk, pNode, i){
        if(pNode->Type == 7){
            currLevel = Abc_NtkLevel_rec(pNode);
            if (currLevel > maxLevel){
                maxLevel = currLevel;
            }
        }
    }

    // print general info about logic network
    if(printToConsole){
        Abc_Print(-2, "\n");
        Abc_Print(-2, "Number of Nodes: %d\n", i);
        Abc_Print(-2, "nObjs: %d\n", pNtk->nObjs);
        Abc_Print(-2, "Max Number of levels: %d\n\n", maxLevel);
        Abc_Print(-2, "Excluding Input and Output Nodes!\n");
    }

    //if (printTrainVec){
    //    Abc_Print(-2, "\nOrder of Training Data: Node of Interest, Fanin nodes (4), Fanout nodes (10)\n\n");
    //    Abc_Print(-2, "Data is being placed in train_data.txt\n\n");
    //}

// start traversal through network for feature extraction
    Abc_NtkForEachObj(pNtk, pNode, i){
      // reset counts
        countFanin = 0;
        countFanout = 0;
        faninNum = 0;
        fanoutNum = 0;
        // make sure that the node that is being evaluated is of Type 7 (filtering inputs/outputs/latch etc)
        if(pNode->Type == 7){
            numNodes++;

            if(printToConsole){
                Abc_Print(-2, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                Abc_Print(-2, "\n");
                Abc_Print(-2, "Node Id: %d\n", (pNode)->Id);
                Abc_Print(-2, "Node Type: %d\n", (pNode)->Type);
                Abc_Print(-2, "Max Level: %d\n", maxLevel);
                Abc_Print(-2, "Node Num Fanins: %d\n", Abc_ObjFaninNum(pNode));
                Abc_Print(-2, "Node Num Fanouts: %d\n", Abc_ObjFanoutNum(pNode));
            }
            if(printTrainVec){
                // print in this order
                // Node ID, Node Type, Node Phase, Node Num_Fanins, Node Num_Fanouts
                //Abc_Print(-2, "%d ", (pNode)->Id);
                //Abc_Print(-2, "%d ", (pNode)->Type);

                //Abc_Print(-2, "%d ", maxLevel);
                //Abc_Print(-2, "%d ", Abc_ObjFaninNum(pNode));
                //Abc_Print(-2, "%d ", Abc_ObjFanoutNum(pNode));    
                fprintf(t, "%d ", maxLevel);
                fprintf(t, "%d ", Abc_ObjFaninNum(pNode));
                fprintf(t, "%d ", Abc_ObjFanoutNum(pNode));
            }

            countFanin = (trainSizeFanin - Abc_ObjFaninNum(pNode))*6;
            countFanout = (trainSizeFanout - Abc_ObjFanoutNum(pNode))*6;
   
            if(printStats){
                if(Abc_ObjFanoutNum(pNode) <= 10){
                    numNodesless10++;
                }
                // write the fanout num to a file for python processing
                //fprintf(fo, "%d\n", Abc_ObjFanoutNum(pNode));
                //fprintf(fi, "%d\n", Abc_ObjFaninNum(pNode));
                // average and std calculations
                sumFanout = Abc_ObjFanoutNum(pNode)+sumFanout;
                sumFanin = Abc_ObjFaninNum(pNode)+sumFanin;
            }

            if(((Mio_Gate_t*)(pNode)->pData) != NULL){
                if(printToConsole){
                    Abc_Print(-2, "Node Gate: %d\n", Abc_LookupNodeNum(Mio_GateReadName((Mio_Gate_t*)(pNode)->pData)));
                    Abc_Print(-2, "Node Level : %d\n", Abc_NtkLevel_rec(pNode));
                }
                if(printTrainVec){
                    //Abc_Print(-2, "%s ", Mio_GateReadName((Mio_Gate_t*)(pNode)->pData)); // replace this with int
                    //Abc_Print(-2, "%d ", Abc_NtkLevel_rec(pNode));
                    //Abc_Print(-2, "%lf ", (double)Abc_NtkLevel_rec(pNode)/maxLevel);
                    fprintf(t, "%d ", Abc_LookupNodeNum(Mio_GateReadName((Mio_Gate_t*)(pNode)->pData))); // replace this with int
                    fprintf(t, "%d ", Abc_NtkLevel_rec(pNode));
                    fprintf(t, "%lf ", (double)Abc_NtkLevel_rec(pNode)/maxLevel);
                }
            }   

            
// traversing through fanin for all nodes
            if(Abc_ObjFaninNum(pNode) > 0){
                if(printToConsole){
                    Abc_Print(-2, "\nFanin Nodes of Node ID: %d ~~~~~~~~~ Fanin\n", (pCurrent)->Id);
                }
                Abc_ObjForEachFanin((pNode), pCurrent, k){
                    if(faninNum >= 4){
                        break;
                    }
                    faninNum++;
                    if (printToConsole){
                        Abc_Print(-2, "Node [%d] ID: %d\n", k, pCurrent->Id);  
                        Abc_Print(-2, "Node Type [%d]: %d\n", k, pCurrent->Type);
                        Abc_Print(-2, "Node Num Fanins [%d]: %d\n", k, Abc_ObjFaninNum(pCurrent));
                        Abc_Print(-2, "Node Num Fanouts [%d]: %d\n", k, Abc_ObjFanoutNum(pCurrent));
                    }
        
                    if(printTrainVec){
                        // print in this order
                        // Node ID, Node Type, Node Phase, Node Num_Fanins, Node Num_Fanouts
                        //Abc_Print(-2, "%d ", (pCurrent)->Id);
                        //Abc_Print(-2, "%d ", (pCurrent)->Type);
                        //Abc_Print(-2, "%d ", maxLevel);
                        //Abc_Print(-2, "%d ", Abc_ObjFaninNum(pCurrent));
                        //Abc_Print(-2, "%d ", Abc_ObjFanoutNum(pCurrent));
                        fprintf(t, "%d ", maxLevel);
                        fprintf(t, "%d ", Abc_ObjFaninNum(pCurrent));
                        fprintf(t, "%d ", Abc_ObjFanoutNum(pCurrent));
                    }
            
                    // print the gate, level, and logic depth
                    if(printToConsole){
                        Abc_Print(-2, "Node Gate: %s\n", "NA");
                        Abc_Print(-2, "Node Level : %d\n", 0);
                    }
                    if(pCurrent->Type == 7){
                        if(printTrainVec){
                            //Abc_Print(-2, "%s ", Mio_GateReadName((Mio_Gate_t*)(pCurrent)->pData)); // replace this with int
                            //Abc_Print(-2, "%d ", Abc_NtkLevel_rec(pCurrent));
                            //Abc_Print(-2, "%lf ", (double)Abc_NtkLevel_rec(pCurrent)/maxLevel);
                            fprintf(t, "%d ", Abc_LookupNodeNum(Mio_GateReadName((Mio_Gate_t*)(pCurrent)->pData))); // replace this with int
                            fprintf(t, "%d ", Abc_NtkLevel_rec(pCurrent));
                            fprintf(t, "%lf ", (double)Abc_NtkLevel_rec(pCurrent)/maxLevel);
                        }
                    }
                    else{
                        if(printTrainVec){
                            fprintf(t, "0 ");
                            fprintf(t, "0 ");
                            fprintf(t, "0 ");
                        }
                    }
                    
                    if(printToConsole){
                        Abc_Print(-2, "\n");
                    }
                }
                if (printTrainVec){
                    for (int n = 0; n < countFanin; n++){
                       fprintf(t, "0 ");
                    } 
                }

                if(Abc_ObjFaninNum(pCurrent) > maxFanin){
                    maxFanin = Abc_ObjFaninNum(pCurrent);
                    maxFanin_id = (pCurrent)->Id;
                }
            }
            
// traversing through fanout for all nodes
            
            if(Abc_ObjFanoutNum(pNode) > 0){
                if(printToConsole){
                    Abc_Print(-2, "\nFanout Nodes of Node ID: %d ~~~~~~~~~ Fanout\n", (pCurrent)->Id);
                }
                Abc_ObjForEachFanout((pNode), pCurrent, k){
                    // Only look at first 10 fanouts if more than 10.
                    // Testing shows that this will give 98% node coverage
                    if (fanoutNum >= 10){
                       break;
                    }
                    fanoutNum++;

                    if(printToConsole){
                        Abc_Print(-2, "Node [%d] ID: %d\n", k, pCurrent->Id);  
                        Abc_Print(-2, "Node Type [%d]: %d\n", k, pCurrent->Type);
                        Abc_Print(-2, "Node Num Fanins [%d]: %d\n", k, Abc_ObjFaninNum(pCurrent));
                        Abc_Print(-2, "Node Num Fanouts [%d]: %d\n", k, Abc_ObjFanoutNum(pCurrent));
                    }
        
                    if(printTrainVec){
                        // print in this order
                        // Node ID, Node Type, Node Phase, Node Num_Fanins, Node Num_Fanouts
                        //Abc_Print(-2, "%d ", (pCurrent)->Id);
                        //Abc_Print(-2, "%d ", (pCurrent)->Type);
                        //Abc_Print(-2, "%d ", Abc_ObjFaninNum(pCurrent));
                        //Abc_Print(-2, "%d ", Abc_ObjFanoutNum(pCurrent));
                        fprintf(t, "%d ", maxLevel);
                        fprintf(t, "%d ", Abc_ObjFaninNum(pCurrent));
                        fprintf(t, "%d ", Abc_ObjFanoutNum(pCurrent));
                    }

                    // print the gate, level, and logic depth
                    if(printToConsole){
                        Abc_Print(-2, "Node Gate: %s\n", "NA");
                        Abc_Print(-2, "Node Level : %d\n", 0);
                    }
                    if(pCurrent->Type == 7){
                        if(printTrainVec){
                            //Abc_Print(-2, "%s ", Mio_GateReadName((Mio_Gate_t*)(pCurrent)->pData)); // replace this with int
                            //Abc_Print(-2, "%d ", Abc_NtkLevel_rec(pCurrent));
                            //Abc_Print(-2, "%lf ", (double)Abc_NtkLevel_rec(pCurrent)/maxLevel);
                            fprintf(t, "%d ", Abc_LookupNodeNum(Mio_GateReadName((Mio_Gate_t*)(pCurrent)->pData))); // replace this with int
                            fprintf(t, "%d ", Abc_NtkLevel_rec(pCurrent));
                            fprintf(t, "%lf ", (double)Abc_NtkLevel_rec(pCurrent)/maxLevel);
                        }  
                    }
                    else{
                        if(printTrainVec){
                            fprintf(t, "0 ");
                            fprintf(t, "0 ");
                            fprintf(t, "0 ");
                        }
                    }
            
                    if(printToConsole){
                        Abc_Print(-2, "\n");
                    }
                }
          
                if(printTrainVec){
                    for (int n = 0; n < countFanout; n++){
                        fprintf(t, "0 ");
                    } 
                }
                if(Abc_ObjFanoutNum(pCurrent) > maxFanout){
                    maxFanout = Abc_ObjFanoutNum(pCurrent);
                    maxFanout_id = (pCurrent)->Id;
                }
            }
            
        
        
            if(printTrainVec){
                fprintf(t, "\n");
            }
        
            if(printToConsole){
                Abc_Print(-2, "\n");
                Abc_Print(-2, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            }
        }
    }

    // close the fan(out/in)_stats.txt files
    //fclose(fo);
    //fclose(fi);
    //fclose(t);
    
    if (printStats){
        Abc_Print(-2, "\n");
        Abc_Print(-2, "Max Number of levels: %d\n\n", maxLevel);
        Abc_Print(-2, "Sum Fanin -> %d\n", sumFanin);
        Abc_Print(-2, "Sum Fanout -> %d\n", sumFanout);
        Abc_Print(-2, "Num Nodes == 7 -> %d\n", numNodes);
        Abc_Print(-2, "Percent Node Fanout (<10): %lf\n", (double)numNodesless10/numNodes);
        Abc_Print(-2, "Average Fanin -> %lf\n", (double)sumFanin/numNodes);
        Abc_Print(-2, "Average Fanout -> %lf\n", (double)sumFanout/numNodes);
        Abc_Print(-2, "MaxFanin ID: %d -> numFanin: %d\n", maxFanin_id, maxFanin);
        Abc_Print(-2, "MaxFanout ID: %d -> numFanout: %d\n", maxFanout_id, maxFanout);
        Abc_Print(-2, "\n\n");
    }


    // write the DOT file
    nBarBufs = pNtk->nBarBufs;
    pNtk->nBarBufs = 0;
    if ( fSeq )
        Io_WriteDotSeq( pNtk, vNodes, NULL, FileNameDot, fGateNames, fUseReverse );
    else
        Io_WriteDotNtk( pNtk, vNodes, NULL, FileNameDot, fGateNames, fUseReverse );
    pNtk->nBarBufs = nBarBufs;
    Vec_PtrFree( vNodes );

    
    // visualize the file 
    //Abc_ShowFile( FileNameDot );
    
    Abc_NtkDelete( pNtk );
}


/**Function*************************************************************

  Synopsis    [Shows the given DOT file.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_ShowFile( char * FileNameDot )
{
    FILE * pFile;
    char * FileGeneric;
    char FileNamePs[200];
    char CommandDot[1000];
    char * pDotName;
    char * pDotNameWin = "dot.exe";
    char * pDotNameUnix = "dot";
    char * pGsNameWin = "gsview32.exe";
    char * pGsNameUnix = "gv";
    int RetValue;

    // get DOT names from the resource file
    if ( Abc_FrameReadFlag("dotwin") )
        pDotNameWin = Abc_FrameReadFlag("dotwin");
    if ( Abc_FrameReadFlag("dotunix") )
        pDotNameUnix = Abc_FrameReadFlag("dotunix");

#ifdef WIN32
    pDotName = pDotNameWin;
#else
    pDotName = pDotNameUnix;
#endif

    // check if the input DOT file is okay
    if ( (pFile = fopen( FileNameDot, "r" )) == NULL )
    {
        fprintf( stdout, "Cannot open the intermediate file \"%s\".\n", FileNameDot );
        return;
    }
    fclose( pFile );

    // create the PostScript file name
    FileGeneric = Extra_FileNameGeneric( FileNameDot );
    sprintf( FileNamePs,  "%s.ps",  FileGeneric ); 
    ABC_FREE( FileGeneric );

    // generate the PostScript file using DOT
    sprintf( CommandDot,  "%s -Tps -o %s %s", pDotName, FileNamePs, FileNameDot ); 
    RetValue = system( CommandDot );
    if ( RetValue == -1 )
    {
        fprintf( stdout, "Command \"%s\" did not succeed.\n", CommandDot );
        return;
    }
    // check that the input PostScript file is okay
    if ( (pFile = fopen( FileNamePs, "r" )) == NULL )
    {
        fprintf( stdout, "Cannot open intermediate file \"%s\".\n", FileNamePs );
        return;
    }
    fclose( pFile ); 


    // get GSVIEW names from the resource file
    if ( Abc_FrameReadFlag("gsviewwin") )
        pGsNameWin = Abc_FrameReadFlag("gsviewwin");
    if ( Abc_FrameReadFlag("gsviewunix") )
        pGsNameUnix = Abc_FrameReadFlag("gsviewunix");

    // spawn the viewer
#ifdef WIN32
    _unlink( FileNameDot );
    if ( _spawnl( _P_NOWAIT, pGsNameWin, pGsNameWin, FileNamePs, NULL ) == -1 )
        if ( _spawnl( _P_NOWAIT, "C:\\Program Files\\Ghostgum\\gsview\\gsview32.exe", 
            "C:\\Program Files\\Ghostgum\\gsview\\gsview32.exe", FileNamePs, NULL ) == -1 )
            if ( _spawnl( _P_NOWAIT, "C:\\Program Files\\Ghostgum\\gsview\\gsview64.exe", 
                "C:\\Program Files\\Ghostgum\\gsview\\gsview64.exe", FileNamePs, NULL ) == -1 )
            {
                fprintf( stdout, "Cannot find \"%s\".\n", pGsNameWin );
                return;
            }
#else
    {
        char CommandPs[1000];
        unlink( FileNameDot );
        sprintf( CommandPs,  "%s %s &", pGsNameUnix, FileNamePs ); 
        if ( system( CommandPs ) == -1 )
        {
            fprintf( stdout, "Cannot execute \"%s\".\n", CommandPs );
            return;
        }
    }
#endif
}

/**Function*************************************************************

  Synopsis    [Derives the DOT file name.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_ShowGetFileName( char * pName, char * pBuffer )
{
    char * pCur;
    // creat the file name
    sprintf( pBuffer, "%s.dot", pName );
    // get rid of not-alpha-numeric characters
    for ( pCur = pBuffer; *pCur; pCur++ )
        if ( !((*pCur >= '0' && *pCur <= '9') || (*pCur >= 'a' && *pCur <= 'z') || 
               (*pCur >= 'A' && *pCur <= 'Z') || (*pCur == '.')) )
            *pCur = '_';
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkWriteFlopDependency( Abc_Ntk_t * pNtk, char * pFileName )
{
    FILE * pFile;
    Vec_Ptr_t * vSupp;
    Abc_Obj_t * pObj, * pTemp;
    int i, k, Count;
    pFile = fopen( pFileName, "w" );
    if ( pFile == NULL )
    {
        printf( "Cannot open input file %s.\n", pFileName );
        return;
    }
    fprintf( pFile, "# Flop dependency for \"%s\" generated by ABC on %s\n", Abc_NtkName(pNtk), Extra_TimeStamp() );
    fprintf( pFile, "digraph G {\n" );
    fprintf( pFile, "  graph [splines=true overlap=false];\n" );
    fprintf( pFile, "  size = \"7.5,10\";\n" );
    fprintf( pFile, "  center = true;\n" );
//    fprintf( pFile, "  edge [len=3,dir=forward];\n" );
    fprintf( pFile, "  edge [dir=forward];\n" );
    Abc_NtkForEachLatchInput( pNtk, pObj, i )
    {
        Abc_ObjFanout0( Abc_ObjFanout0(pObj) )->iTemp = i;
        vSupp = Abc_NtkNodeSupport( pNtk, &pObj, 1 );
        Count = 0;
        Vec_PtrForEachEntry( Abc_Obj_t *, vSupp, pTemp, k )
            Count += Abc_ObjIsPi(pTemp);
        Vec_PtrFree( vSupp );
        fprintf( pFile, "  { rank = same; %d [label=\"%d(%d)\"]; }\n", i, i, Count );
    }
    Abc_NtkForEachLatchInput( pNtk, pObj, i )
    {
        vSupp = Abc_NtkNodeSupport( pNtk, &pObj, 1 );
        Count = 0;
        Vec_PtrForEachEntry( Abc_Obj_t *, vSupp, pTemp, k )
            if ( !Abc_ObjIsPi(pTemp) )
                fprintf( pFile, "  %4d -> %4d\n", pTemp->iTemp, i );
        Vec_PtrFree( vSupp );
    }
    fprintf( pFile, "}\n" );
    fclose( pFile );
}


/**Function*************************************************************

  Synopsis    [Visualizes AIG with choices.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkShowFlopDependency( Abc_Ntk_t * pNtk )
{
    FILE * pFile;
    char FileNameDot[200];
    assert( Abc_NtkIsStrash(pNtk) || Abc_NtkIsLogic(pNtk) );
    // create the file name
    Abc_ShowGetFileName( pNtk->pName, FileNameDot );
    // check that the file can be opened
    if ( (pFile = fopen( FileNameDot, "w" )) == NULL )
    {
        fprintf( stdout, "Cannot open the intermediate file \"%s\".\n", FileNameDot );
        return;
    }
    fclose( pFile );
    // write the DOT file
    Abc_NtkWriteFlopDependency( pNtk, FileNameDot );
    // visualize the file 
    Abc_ShowFile( FileNameDot );
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

