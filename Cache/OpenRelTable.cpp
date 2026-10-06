#include "OpenRelTable.h"
#include <cstdlib>
#include <cstring>
#include<stdio.h>


OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable() {

    // Initialize relation cache and attribute cache
    for(int i = 0; i < MAX_OPEN; i++) {
        RelCacheTable::relCache[i] = nullptr;
        AttrCacheTable::attrCache[i] = nullptr;
        tableMetaInfo[i].free=true;
    }

    /************ Setting up Relation Cache entries ************/
    /**** Relation Catalog ****/
    RecBuffer relCatBlock(RELCAT_BLOCK); 
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    relCatBlock.getRecord(relCatRecord,RELCAT_SLOTNUM_FOR_RELCAT);

    //converts the raw Attribute[] representation into the more convenient:RelCatEntry
    RelCacheEntry relCacheEntry;
    RelCacheTable::recordToRelCatEntry(relCatRecord,&relCacheEntry.relCatEntry);
    relCacheEntry.dirty = false;
    relCacheEntry.recId.block = RELCAT_BLOCK;
    relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;
    relCacheEntry.searchIndex.block = -1;
    relCacheEntry.searchIndex.slot = -1;

 RelCacheTable::relCache[RELCAT_RELID]=(RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;


    /**** Attribute Catalog ****/
    //Read the catalog record of ATTRCAT from the Relation Catalog (RELCAT) 
    //and store that record in relCatRecord.
    relCatBlock.getRecord(relCatRecord,RELCAT_SLOTNUM_FOR_ATTRCAT);
    RelCacheEntry attrRelCacheEntry;
    RelCacheTable::recordToRelCatEntry(relCatRecord,&attrRelCacheEntry.relCatEntry);
    attrRelCacheEntry.dirty = false;
    attrRelCacheEntry.recId.block = RELCAT_BLOCK;
    attrRelCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;
    attrRelCacheEntry.searchIndex.block = -1;
    attrRelCacheEntry.searchIndex.slot = -1;

 RelCacheTable::relCache[ATTRCAT_RELID]=(RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[ATTRCAT_RELID]) =attrRelCacheEntry;

    /************ Setting up Attribute Cache entries ************/
    RecBuffer attrCatBlock(ATTRCAT_BLOCK);
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];

    /**** Attributes of Relation Catalog ****/
    AttrCacheEntry *head = nullptr;
    AttrCacheEntry *prev = nullptr;
    for(int i = 0; i < RELCAT_NO_ATTRS; i++) {
        attrCatBlock.getRecord(attrCatRecord, i);
        AttrCacheEntry *entry =(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&entry->attrCatEntry);
        entry->dirty = false;
        entry->recId.block = ATTRCAT_BLOCK;
        entry->recId.slot = i;
        entry->searchIndex.block = -1;
        entry->searchIndex.index = -1;
        entry->next = nullptr;

        if(head == nullptr) {
            head = entry;
        }
        else {
            prev->next = entry;
        }
        prev = entry;
    }
    
    AttrCacheTable::attrCache[RELCAT_RELID] = head;

    /**** Attributes of Attribute Catalog ****/

    head = nullptr;
    prev = nullptr;
    for(int i = RELCAT_NO_ATTRS;i < ATTRCAT_NO_ATTRS + RELCAT_NO_ATTRS;i++) {
        attrCatBlock.getRecord(attrCatRecord, i);
        AttrCacheEntry *entry =(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&entry->attrCatEntry);
        entry->dirty = false;
        entry->recId.block = ATTRCAT_BLOCK;
        entry->recId.slot = i;
        entry->searchIndex.block = -1;
        entry->searchIndex.index = -1;
        entry->next = nullptr;

        if(head == nullptr) {
            head = entry;
        }
        else {
            prev->next = entry;
        }
        prev = entry;
    }

    AttrCacheTable::attrCache[ATTRCAT_RELID] = head;


    tableMetaInfo[RELCAT_RELID].free=false;
    strcpy(OpenRelTable::tableMetaInfo[0].relName,"RELATIONCAT");
    tableMetaInfo[ATTRCAT_RELID].free=false;
    strcpy(OpenRelTable::tableMetaInfo[1].relName,"ATTRIBUTECAT");
    
    
} 



OpenRelTable::~OpenRelTable() {

   for (int i = 2; i < MAX_OPEN; ++i) {
    if (!tableMetaInfo[i].free) {
      OpenRelTable::closeRel(i); // we will implement this function later
    }
  }

    /**** Closing the catalog relations in the relation cache ****/

    //releasing the relation cache entry of the attribute catalog
    /* RelCatEntry of the ATTRCAT_RELID-th RelCacheEntry has been modified */

    if (RelCacheTable::relCache[ATTRCAT_RELID]->dirty) {

        /* Get the Relation Catalog entry from RelCacheTable::relCache
        Then convert it to a record using RelCacheTable::relCatEntryToRecord(). */
         Attribute record[ATTRCAT_NO_ATTRS];
    RelCacheTable::relCatEntryToRecord(&RelCacheTable::relCache[ATTRCAT_RELID]->relCatEntry,record);
        // declaring an object of RecBuffer class to write back to the buffer
        RecId recId = RelCacheTable::relCache[ATTRCAT_RELID]->recId;
        RecBuffer relCatBlock(recId.block);

        // Write back to the buffer using relCatBlock.setRecord() with recId.slot
        
        relCatBlock.setRecord(record,recId.slot);;
    }
    // free the memory dynamically allocated to this RelCacheEntry
      free(RelCacheTable::relCache[ATTRCAT_RELID]);
      RelCacheTable::relCache[ATTRCAT_RELID]=nullptr;

    //releasing the relation cache entry of the relation catalog
    /* RelCatEntry of the RELCAT_RELID-th RelCacheEntry has been modified */
    if(RelCacheTable::relCache[RELCAT_RELID]->dirty) {

        /* Get the Relation Catalog entry from RelCacheTable::relCache
        Then convert it to a record using RelCacheTable::relCatEntryToRecord(). */
        Attribute record[RELCAT_NO_ATTRS];
        RelCacheTable::relCatEntryToRecord(&RelCacheTable::relCache[RELCAT_RELID]->relCatEntry,record);
        RecId recId = RelCacheTable::relCache[RELCAT_RELID]->recId;
       
    
        // declaring an object of RecBuffer class to write back to the buffer
        RecBuffer relCatBlock(recId.block);

        // Write back to the buffer using relCatBlock.setRecord() with recId.slot
         relCatBlock.setRecord(record,recId.slot);
    }
    // free the memory dynamically allocated for this RelCacheEntry
      free(RelCacheTable::relCache[RELCAT_RELID]);
      RelCacheTable::relCache[RELCAT_RELID]=nullptr;
    // free the memory allocated for the attribute cache entries of the
    // relation catalog and the attribute catalog

    for (int i = 0; i <2; i++) {
    if(RelCacheTable::relCache[i] != nullptr)
    {
        free(RelCacheTable::relCache[i]);
        RelCacheTable::relCache[i] = nullptr;
    }
  }
  for (int i = 0; i <2; i++) {
    if(AttrCacheTable::attrCache[i] != nullptr)
    {
        AttrCacheEntry *curr=AttrCacheTable::attrCache[i];
        while(curr!=nullptr)
        {
            AttrCacheEntry *temp=curr;
            curr=curr->next;
            free(temp);
        }
        AttrCacheTable::attrCache[i] = nullptr;
    }
  }
  
}




/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/
int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

  /* traverse through the tableMetaInfo array,
    find the entry in the Open Relation Table corresponding to relName.*/
    for(int i=0;i<MAX_OPEN;i++)
    {
        if(strcmp(tableMetaInfo[i].relName,relName)==0 && !tableMetaInfo[i].free)
        {
            return i;
        }
      
    }

  // if found return the relation id, else indicate that the relation do not
  // have an entry in the Open Relation Table.
   return E_RELNOTOPEN;
}



int OpenRelTable::getFreeOpenRelTableEntry() {

  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/
    for(int i=0;i<MAX_OPEN;i++)
    {
        if(tableMetaInfo[i].free)
        {
            return i;
        }
        
    }
    return E_CACHEFULL;
  // if found return the relation id, else return E_CACHEFULL.
}

int OpenRelTable::openRel(char relName[ATTR_SIZE]) {
   int r=getRelId(relName);
  if(r!=E_RELNOTOPEN){
    // (checked using OpenRelTable::getRelId())
     return r;
    // return that relation id;
  }

  /* find a free slot in the Open Relation Table
     using OpenRelTable::getFreeOpenRelTableEntry(). */
    int relId=OpenRelTable::getFreeOpenRelTableEntry();

  if (relId== E_CACHEFULL){
    return E_CACHEFULL;
  }
  // let relId be used to store the free slot.
  
  /****** Setting up Relation Cache entry for the relation ******/

  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/

      Attribute attrVal;
      strcpy(attrVal.sVal,relName);
        char name[ATTR_SIZE];
        strcpy(name, RELCAT_ATTR_RELNAME);

     RelCacheTable::resetSearchIndex(RELCAT_RELID);

  // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.
  RecId relcatRecId;
  relcatRecId =BlockAccess::linearSearch(RELCAT_RELID,name,attrVal,EQ);
  if (relcatRecId.block == -1 || relcatRecId.slot==-1 ) {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
  }

  /* read the record entry corresponding to relcatRecId and create a relCacheEntry
      on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
      update the recId field of this Relation Cache entry to relcatRecId.
      use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
    NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  */
    RecBuffer relcatBlock(relcatRecId.block);
    Attribute relCatRecord[RELCAT_NO_ATTRS];


    relcatBlock.getRecord(relCatRecord,relcatRecId.slot);
    RelCacheEntry relCacheEntry;
    RelCacheTable::recordToRelCatEntry(relCatRecord,&relCacheEntry.relCatEntry);
    relCacheEntry.recId.block=relcatRecId.block;
    relCacheEntry.recId.slot=relcatRecId.slot;
    relCacheEntry.searchIndex.block=-1;
    relCacheEntry.searchIndex.slot=-1;
    relCacheEntry.dirty=false;

    RelCacheTable::relCache[relId]=(struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[relId])=relCacheEntry;


  /****** Setting up Attribute Cache entry for the relation ******/

  // let listHead be used to hold the head of the linked list of attrCache entries.
  AttrCacheEntry* listHead=nullptr;
  AttrCacheEntry* tail=nullptr;

  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/
  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
  while(1)
  {
      /* let attrcatRecId store a valid record id an entry of the relation, relName,
      in the Attribute Catalog.*/
      RecId attrcatRecId=BlockAccess::linearSearch(ATTRCAT_RELID,name,attrVal,EQ);
      if(attrcatRecId.block==-1 || attrcatRecId.slot==-1)
      {
        break;
      }

      RecBuffer attrCatBlock(attrcatRecId.block);
      attrCatBlock.getRecord(attrCatRecord,attrcatRecId.slot);
      AttrCacheEntry* attrCacheEntry=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
      AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&attrCacheEntry->attrCatEntry);
      attrCacheEntry->recId.block=attrcatRecId.block;
       attrCacheEntry->recId.slot=attrcatRecId.slot;
       attrCacheEntry->next=nullptr;
       if(listHead!=nullptr)
       {
            tail->next=attrCacheEntry;
       }
       else
       {
        listHead=attrCacheEntry;
       }
       tail=attrCacheEntry;
      /* read the record entry corresponding to attrcatRecId and create an
      Attribute Cache entry on it using RecBuffer::getRecord() and
      AttrCacheTable::recordToAttrCatEntry().
      update the recId field of this Attribute Cache entry to attrcatRecId.
      add the Attribute Cache entry to the linked list of listHead .*/
      // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
  }
  if(tail!=nullptr)
  {
    tail->next=nullptr;
    AttrCacheTable::attrCache[relId]=listHead;

    tableMetaInfo[relId].free=false;
    strcpy(tableMetaInfo[relId].relName,relName);
  }

  // set the relIdth entry of the AttrCacheTable to listHead.

  /****** Setting up metadata in the Open Relation Table for the relation******/

  // update the relIdth entry of the tableMetaInfo with free as false and
  // relName as the input.

  return relId;
}

// int OpenRelTable::closeRel(int relId) {
//   if (relId==RELCAT_RELID || relId==ATTRCAT_RELID)
//     return E_NOTPERMITTED;

//   if (relId<0 || relId>=MAX_OPEN)
//     return E_OUTOFBOUND;

//   if (tableMetaInfo[relId].free)
//     return E_RELNOTOPEN;

//   // free relation cache
//   if(RelCacheTable::relCache[relId])
//     free(RelCacheTable::relCache[relId]);
//     AttrCacheEntry* curr = AttrCacheTable::attrCache[relId];
//   while(curr)
//   {
//     AttrCacheEntry* temp = curr;
//     curr = curr->next;
//     free(temp);
//   }

//   RelCacheTable::relCache[relId] = nullptr;
//   AttrCacheTable::attrCache[relId] = nullptr;

//   tableMetaInfo[relId].free = true;

//   return SUCCESS;
// }


int OpenRelTable::closeRel(int relId) {
  // confirm that rel-id fits the following conditions
  //     2 <=relId < MAX_OPEN
  //     does not correspond to a free slot
  //  (you have done this already)
  if (relId==RELCAT_RELID || relId==ATTRCAT_RELID)
    return E_NOTPERMITTED;

  if (relId<0 || relId>=MAX_OPEN)
    return E_OUTOFBOUND;

  if (tableMetaInfo[relId].free)
    return E_RELNOTOPEN;

  /****** Releasing the Relation Cache entry of the relation ******/

  if (RelCacheTable::relCache[relId]->dirty==true)
  {

    /* Get the Relation Catalog entry from RelCacheTable::relCache
    Then convert it to a record using RelCacheTable::relCatEntryToRecord(). */
  //   if(RelCacheTable::relCache[relId])
  // {
  //   Attribute record[RELCAT_NO_ATTRS];
  //   if(RelCacheTable::relCache[relId]->dirty)
  //   {
  //     RelCacheTable::relCatEntryToRecord(&RelCacheTable::relCache[relId]->relCatEntry,record);
  //     RecId recId=RelCacheTable::relCache[relId]->recId;
  //     RecBuffer relCatBlock(recId.block);
  //     relCatBlock.setRecord(record,recId.slot);
  //   }
  //   free(RelCacheTable::relCache[relId]);
  // }

    if (RelCacheTable::relCache[relId]->dirty) {

    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(relId, &relCatEntry);

    Attribute relCatRecord[RELCAT_NO_ATTRS];
    RelCacheTable::relCatEntryToRecord(&relCatEntry, relCatRecord);

    RecId recId = RelCacheTable::relCache[relId]->recId;
    RecBuffer relCatBlock(recId.block);
    relCatBlock.setRecord(relCatRecord, recId.slot);
    }

  
    // Write back to the buffer using relCatBlock.setRecord() with recId.slot
  }
  free(RelCacheTable::relCache[relId]);
  RelCacheTable::relCache[relId] = nullptr;

  /****** Releasing the Attribute Cache entry of the relation ******/

  // free the memory allocated in the attribute caches which was
  // allocated in the OpenRelTable::openRel() function

  // (because we are not modifying the attribute cache at this stage,
  // write-back is not required. We will do it in subsequent
  // stages when it becomes needed)


  /****** Set the Open Relation Table entry of the relation as free ******/

  // update `metainfo` to set `relId` as a free slot
  AttrCacheEntry *entry = AttrCacheTable::attrCache[relId];
  while (entry != nullptr) {
    AttrCacheEntry *next = entry->next;
    free(entry);
    entry = next;
  }
  AttrCacheTable::attrCache[relId] = nullptr;

  /****** Set the Open Relation Table entry of the relation as free ******/

  tableMetaInfo[relId].free = true;

  return SUCCESS;

 
}