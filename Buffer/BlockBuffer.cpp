#include "BlockBuffer.h"
#include "StaticBuffer.h"
#include "../Disk_Class/Disk.h"
#include <cstring>
#include <iostream>

// Constructor
BlockBuffer::BlockBuffer(int blockNum) {
    this->blockNum = blockNum;
}

BlockBuffer::BlockBuffer(char blockType){
    // allocate a block on the disk and a buffer in memory to hold the new block of
    // given type using getFreeBlock function and get the return error codes if any.
     int type;
    if(blockType=='R')
    {
      type=REC;
    }
    else if(blockType=='I')
    {
      type=IND_INTERNAL;
    }
    else if(blockType=='L')
    {
      type=IND_LEAF;
    }
    else
    {
      type=UNUSED_BLK;
    }
    int ret=getFreeBlock(type);
    this->blockNum=ret;      
    // set the blockNum field of the object to that of the allocated block
    // number if the method returned a valid block number,
    // otherwise set the error code returned as the block number.
     if(ret<0 || ret>=DISK_BLOCKS)
            return;
    // (The caller must check if the constructor allocatted block successfully
    // by checking the value of block number field.)
}

RecBuffer::RecBuffer() : BlockBuffer('R'){}

// RecBuffer constructor
RecBuffer::RecBuffer(int blockNum): BlockBuffer::BlockBuffer(blockNum) {}

// Read block header
int BlockBuffer::getHeader(HeadInfo *head) {

   /* unsigned char buffer[BLOCK_SIZE];

    // Read block from disk
    Disk::readBlock(buffer, this->blockNum);*/
     unsigned char *bufferPtr;
    int ret = loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS)
    {
        return ret;
    }

    // Copy header fields
    memcpy(&head->blockType,  bufferPtr + 0, 4);
    memcpy(&head->pblock,     bufferPtr + 4, 4);
    memcpy(&head->lblock,     bufferPtr + 8, 4);
    memcpy(&head->rblock,     bufferPtr + 12, 4);
    memcpy(&head->numEntries, bufferPtr + 16, 4);
    memcpy(&head->numAttrs,   bufferPtr + 20, 4);
    memcpy(&head->numSlots,   bufferPtr + 24, 4);

    return SUCCESS;

   
}

// Read one record from a record block
int RecBuffer::getRecord(union Attribute *rec, int slotNum) {

    HeadInfo head;
    this->getHeader(&head);

    int attrCount = head.numAttrs;
    int slotCount = head.numSlots;
   

    // unsigned char buffer[BLOCK_SIZE];

    // Disk::readBlock(buffer, this->blockNum);
    unsigned char *bufferptr;
    int ret = loadBlockAndGetBufferPtr(&bufferptr);
    if(ret!=SUCCESS)
    {
        return ret;
    }

    int recordSize = attrCount * ATTR_SIZE;

    unsigned char *recordPtr =
        bufferptr + HEADER_SIZE + slotCount + slotNum * recordSize;

    memcpy(rec, recordPtr, recordSize);

    return SUCCESS;
}

/* NOTE: This function will NOT check if the block has been initialised as a
   record or an index block. It will copy whatever content is there in that
   disk block to the buffer.
   Also ensure that all the methods accessing and updating the block's data
   should call the loadBlockAndGetBufferPtr() function before the access or
   update is done. This is because the block might not be present in the
   buffer due to LRU buffer replacement. So, it will need to be bought back
   to the buffer before any operations can be done.
 */
int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char ** buffPtr) {
    /* check whether the block is already present in the buffer
       using StaticBuffer.getBufferNum() */
    int bufferNum = StaticBuffer::getBufferNum(this->blockNum);
     if(bufferNum == E_OUTOFBOUND)
        {
            return E_OUTOFBOUND;
        }
    // if present (!=E_BLOCKNOTINBUFFER),
        // set the timestamp of the corresponding buffer to 0 and increment the
        // timestamps of all other occupied buffers in BufferMetaInfo.
    if(bufferNum!=E_BLOCKNOTINBUFFER)
    {
        for(int i=0;i<BUFFER_CAPACITY;i++)
        {
            if(StaticBuffer::metainfo[i].free!=true)
            {
                StaticBuffer::metainfo[i].timeStamp++;
            }
        }
        StaticBuffer::metainfo[bufferNum].timeStamp=0;
        *buffPtr = StaticBuffer::blocks[bufferNum];
        return SUCCESS;
    }
    // else
        // get a free buffer using StaticBuffer.getFreeBuffer()
     int ret= StaticBuffer::getFreeBuffer(this->blockNum);
     if(ret==E_OUTOFBOUND)
    {
        return E_OUTOFBOUND;
    }
        // if the call returns E_OUTOFBOUND, return E_OUTOFBOUND here as
        // the blockNum is invalid

        // Read the block into the free buffer using readBlock()
    Disk::readBlock(StaticBuffer::blocks[ret],this->blockNum);
    // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr
    *buffPtr=StaticBuffer::blocks[ret];
     return SUCCESS;
}


int RecBuffer::setRecord(union Attribute *rec, int slotNum) {
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS)
    {
        return ret;
    }
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    /* get the header of the block using the getHeader() function */
    HeadInfo head;
    this->getHeader(&head);
    // get number of attributes in the block.
   // get the number of slots in the block.

    int attrCount=head.numAttrs;
    int slotCount=head.numSlots;
    // if input slotNum is not in the permitted range return E_OUTOFBOUND.
    if(slotNum<0 || slotNum>=slotCount)
    {
        return E_OUTOFBOUND;
    }
    /* offset bufferPtr to point to the beginning of the record at required
       slot. the block contains the header, the slotmap, followed by all
       the records. so, for example,
       record at slot x will be at bufferPtr + HEADER_SIZE + (x*recordSize)
       copy the record from `rec` to buffer using memcpy
       (hint: a record will be of size ATTR_SIZE * numAttrs)
    */
     int recordSize= attrCount*16;
    unsigned char *recordPtr = bufferPtr+32+slotCount+slotNum*recordSize;
    
     memcpy(recordPtr,rec,recordSize);
     StaticBuffer::setDirtyBit(this->blockNum);
     return SUCCESS;
    // update dirty bit using setDirtyBit()

    /* (the above function call should not fail since the block is already
       in buffer and the blockNum is valid. If the call does fail, there
       exists some other issue in the code) */

    // return SUCCESS
}


int RecBuffer::getSlotMap(unsigned char *slotMap){
    unsigned char * bufferPtr;
    int ret = loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS)
    {
        return ret;
    }

    struct HeadInfo head;
    this->getHeader(&head);

    int slotCount = head.numSlots;
    unsigned char *slotMapInBuffer= bufferPtr + HEADER_SIZE;

     //slotMap = slotMapInBuffer;   
     memcpy(slotMap,slotMapInBuffer,head.numSlots);
   
    return SUCCESS;

}

int compareAttrs(union Attribute attr1,union Attribute attr2,int attrType)
{
    double diff;

    if(attrType==STRING)    diff=strcmp(attr1.sVal,attr2.sVal);
    else                    diff=attr1.nVal-attr2.nVal;
    
    if(diff>0) return 1;
    if(diff<0) return -1;
     return 0;
}

int BlockBuffer::setHeader(struct HeadInfo *head){

    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using
    // loadBlockAndGetBufferPtr(&bufferPtr).
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.
    if(ret!=SUCCESS)
    {
        return ret;
    }
    // cast bufferPtr to type HeadInfo*
    struct HeadInfo *bufferHeader = (struct HeadInfo *)bufferPtr;

    // copy the fields of the HeadInfo pointed to by head (except reserved) to
    // the header of the block (pointed to by bufferHeader)
    //(hint: bufferHeader->numSlots = head->numSlots )
    bufferHeader->blockType=head->blockType;
    bufferHeader->lblock=head->lblock;
    bufferHeader->numAttrs=head->numAttrs;
    bufferHeader->numEntries=head->numEntries;
    bufferHeader->numSlots=head->numSlots;
    bufferHeader->pblock=head->pblock;
    bufferHeader->rblock=head->rblock;
    // update dirty bit by calling StaticBuffer::setDirtyBit()
    // if setDirtyBit() failed, return the error code
    ret=StaticBuffer::setDirtyBit(this->blockNum);
    if(ret!=SUCCESS)
    {
        return ret;
    }
     return SUCCESS;
}

int BlockBuffer::setBlockType(int blockType){

    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */
    
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.
    if(ret!=SUCCESS)
    {
        return ret;
    }

    // store the input block type in the first 4 bytes of the buffer.
    // (hint: cast bufferPtr to int32_t* and then assign it)
     *((int32_t *)bufferPtr) = blockType;
    
    // update the StaticBuffer::blockAllocMap entry corresponding to the
    // object's block number to `blockType`.
    StaticBuffer::blockAllocMap[this->blockNum]=blockType;
    // update dirty bit by calling StaticBuffer::setDirtyBit()
    // if setDirtyBit() failed
        // return the returned value from the call
    ret=StaticBuffer::setDirtyBit(this->blockNum);
    if(ret!=SUCCESS)
    {
        return ret;
    }
    return SUCCESS;
}

int BlockBuffer::getFreeBlock(int blockType){

    // iterate through the StaticBuffer::blockAllocMap and find the block number
    // of a free block in the disk.
    int freeblock=-1;
    for (int i = 0; i < DISK_BLOCKS; i++) {
    if (StaticBuffer::blockAllocMap[i] == UNUSED_BLK) 
    { 
        freeblock = i; 
        break; }
    }
    // if no block is free, return E_DISKFULL.
    if(freeblock==-1)
    {
        return E_DISKFULL;
    }
    // set the object's blockNum to the block number of the free block.
    this->blockNum=freeblock;
    // find a free buffer using StaticBuffer::getFreeBuffer() .
    int ret=StaticBuffer::getFreeBuffer(this->blockNum);
     if(ret<0) return ret;
    // initialize the header of the block passing a struct HeadInfo with values
    // pblock: -1, lblock: -1, rblock: -1, numEntries: 0, numAttrs: 0, numSlots: 0
    // to the setHeader() function.
   
      
    struct HeadInfo head;
     head.blockType  = blockType; 
    head.pblock = -1;
    head.lblock = -1;
    head.rblock = -1;
    head.numEntries = 0;
    head.numAttrs = 0;
    head.numSlots = 0;
    ret=setHeader(&head);
    if(ret!=SUCCESS) return ret;
    // update the block type of the block to the input block type using setBlockType().
    setBlockType(blockType);
    // return block number of the free block.
    return this->blockNum;
}


int RecBuffer::setSlotMap(unsigned char *slotMap) {
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block using
       loadBlockAndGetBufferPtr(&bufferPtr). */
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.
    if(ret!=SUCCESS)
    {
        return ret;
    }
    // get the header of the block using the getHeader() function
    HeadInfo head;
    getHeader(&head);
    int numSlots = /* the number of slots in the block */head.numSlots;

    // the slotmap starts at bufferPtr + HEADER_SIZE. Copy the contents of the
    // argument `slotMap` to the buffer replacing the existing slotmap.
    // Note that size of slotmap is `numSlots`
    memcpy(bufferPtr+HEADER_SIZE,slotMap,numSlots);
    // update dirty bit using StaticBuffer::setDirtyBit
    // if setDirtyBit failed, return the value returned by the call
    ret=StaticBuffer::setDirtyBit(this->blockNum);
    if (ret != SUCCESS) 
    {
         return ret;
    }
    return SUCCESS;
}

int BlockBuffer::getBlockNum(){

    //return corresponding block number.
    return this->blockNum;
}

void BlockBuffer::releaseBlock(){

    // if blockNum is INVALID_BLOCKNUM (-1), or it is invalidated already, do nothing
    if(this->blockNum==INVALID_BLOCKNUM)
    {
        return;
    }
    // else
        /* get the buffer number of the buffer assigned to the block
           using StaticBuffer::getBufferNum().
           (this function return E_BLOCKNOTINBUFFER if the block is not
           currently loaded in the buffer)
            */
       int ret= StaticBuffer::getBufferNum(this->blockNum);
       if(ret==E_BLOCKNOTINBUFFER)
       {
        StaticBuffer::metainfo[ret].free=true;
          StaticBuffer::blockAllocMap[this->blockNum]=UNUSED_BLK;
          this->blockNum=INVALID_BLOCKNUM;
          
       }
       return;
        // if the block is present in the buffer, free the buffer
        // by setting the free flag of its StaticBuffer::tableMetaInfo entry
        // to true.
        
        // free the block in disk by setting the data type of the entry
        // corresponding to the block number in StaticBuffer::blockAllocMap
        // to UNUSED_BLK.
        // set the object's blockNum to INVALID_BLOCK (-1)
}