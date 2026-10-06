#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include<iostream>
#include<cstring>
using namespace std;

int main(int argc, char *argv[]) {
  /* Initialize the Run Copy of Disk */
  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;
  // create objects for the relation catalog and attribute catalog
  //main
 //stage e1
  //Disk disk_run;

  // create objects for the relation catalog and attribute catalog
  /* RecBuffer relCatBuffer(RELCAT_BLOCK);

  HeadInfo relCatHeader;

  // load the headers of both the blocks into relCatHeader and attrCatHeader.
  // (we will implement these functions later)
  relCatBuffer.getHeader(&relCatHeader);

  for (int i=0;i<relCatHeader.numEntries;i++) {

    RecBuffer attrCatBuffer(ATTRCAT_BLOCK);
    HeadInfo attrCatHeader;
    attrCatBuffer.getHeader(&attrCatHeader);

    Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog

    relCatBuffer.getRecord(relCatRecord, i);

    printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

    for (int j=0;j<attrCatHeader.numEntries;j++) {

      // declare attrCatRecord and load the attribute catalog entry into it
      Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
      attrCatBuffer.getRecord(attrCatRecord,j);

      if (strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)==0) {
        const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
        printf("  %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
      }

      if(j==attrCatHeader.numEntries-1){
        if(attrCatHeader.rblock==-1) break;
        j=-1;
        attrCatBuffer=RecBuffer(attrCatHeader.rblock);
        attrCatBuffer.getHeader(&attrCatHeader);
      }
    }
    printf("\n");
  }  */


    
   // stage 3 main exercise

    // RelCatEntry relCatEntry;
     
    //  int ret = RelCacheTable::getRelCatEntry(0, &relCatEntry);
     
    //  if(ret != SUCCESS) {
    //      printf("Error getting relation catalog entry\n");
    //      return ret;
    //  }
     
    //  for(int i=0;i<relCatEntry.numAttrs;i++){
    //  printf("Relation: %s\n", relCatEntry.relName);
     
    //  for(int j = 0; j < relCatEntry.numAttrs; j++) {
     
    //      AttrCatEntry attrCatEntry;
    //      ret = AttrCacheTable::getAttrCatEntry(0,j,&attrCatEntry);
    //      if(ret != SUCCESS) {
    //          printf("Error getting attribute catalog entry\n");
    //          return ret;
    //      }
    //      const char *attrType =
    //          (attrCatEntry.attrType == NUMBER) ? "NUM" : "STR";
     
    //      printf("  %s: %s\n",
    //             attrCatEntry.attrName,
    //             attrType);
    //  }
    //  }


   //main stage 3
    // for(int i = 0; i <= 1; i++) {

    //     RelCatEntry relCatEntry;

    //     RelCacheTable::getRelCatEntry(i,&relCatEntry);
    //     printf("Relation: %s\n",  relCatEntry.relName);
    //    for(int j = 0; j < relCatEntry.numAttrs; j++) {
    //         AttrCatEntry attrCatEntry;
    //         AttrCacheTable::getAttrCatEntry( i,  j, &attrCatEntry  );
    //    const char *attrType =
    //             (attrCatEntry.attrType == NUMBER)
    //                 ? "NUM"
    //                 : "STR";

    //         printf("  %s: %s\n",
    //                attrCatEntry.attrName,
    //                attrType);
    //     }
    //     printf("\n");
    // }
    

  return FrontendInterface::handleFrontend(argc, argv);
  }
