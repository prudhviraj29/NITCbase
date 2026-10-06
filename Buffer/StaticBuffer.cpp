#include "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS]; 

// StaticBuffer::StaticBuffer()
// {
//     for(int i=0;i<BUFFER_CAPACITY;i++)
//     {
//         metainfo[i].free =true;
//         metainfo[i].dirty=false;
//         metainfo[i].timeStamp=-1;
//         metainfo[i].blockNum=-1;
//     }
// }

// StaticBuffer::~StaticBuffer(){
//       //unsigned char *bufferptr;
//         for(int i=0;i<BUFFER_CAPACITY;i++)
//         {
//             if( metainfo[i].free==false && metainfo[i].dirty==true)
//             {
//                 Disk::writeBlock(blocks[i],metainfo[i].blockNum);
//             }
//         }
// }

// declare the blockAllocMap array


StaticBuffer::StaticBuffer() {
  // copy blockAllocMap blocks from disk to buffer (using readblock() of disk)
  // blocks 0 to 3
  // unsigned char * Buff;
   for(int i=0;i<4;i++)
   {
    Disk::readBlock(blockAllocMap+i*BLOCK_SIZE,i);
   }
  /* initialise metainfo of all the buffer blocks with
     dirty:false, free:true, timestamp:-1 and blockNum:-1
     (you did this already)
  */
   for(int i=0;i<BUFFER_CAPACITY;i++)
    {
        metainfo[i].free =true;
        metainfo[i].dirty=false;
        metainfo[i].timeStamp=-1;
        metainfo[i].blockNum=-1;
    }
   
}

StaticBuffer::~StaticBuffer() {
  // copy blockAllocMap blocks from buffer to disk(using writeblock() of disk)
    for(int i=0;i<4;i++)
    {
        Disk::writeBlock(blockAllocMap+i*BLOCK_SIZE,i);
    }
   
  /*iterate through all the buffer blocks,
    write back blocks with metainfo as free:false,dirty:true
    (you did this already)
  */
    for(int i=0;i<BUFFER_CAPACITY;i++)
        {
            if( metainfo[i].free==false && metainfo[i].dirty==true)
            {
                Disk::writeBlock(blocks[i],metainfo[i].blockNum);
            }
        }
}


int StaticBuffer::getFreeBuffer(int blockNum){
    // Check if blockNum is valid (non zero and less than DISK_BLOCKS)
    // and return E_OUTOFBOUND if not valid.
    if(blockNum <0 || blockNum >= DISK_BLOCKS){
        return E_OUTOFBOUND;

    }

    // increase the timeStamp in metaInfo of all occupied buffers.
    for(int allocatedBuffer=0;allocatedBuffer<BUFFER_CAPACITY;allocatedBuffer++)
  {
    if(metainfo[allocatedBuffer].free==false)
    {
        metainfo[allocatedBuffer].timeStamp++;
    }
  }
    // let bufferNum be used to store the buffer number of the free/freed buffer.
    int bufferNum;
     bufferNum = -1;
    // iterate through metainfo and check if there is any buffer free

    // if a free buffer is available, set bufferNum = index of that free buffer.
    //int allocatedBuffer;
    for(int i=0;i<BUFFER_CAPACITY;i++)
    {
        if(metainfo[i].free)
        {
            bufferNum=i;
            break;
            
        }
        
    }
  
    if(bufferNum==-1){
    int max=0;
    for(int j=1;j<BUFFER_CAPACITY;j++)
    {
        if(metainfo[j].timeStamp>metainfo[max].timeStamp)
        {
            max=j;   
        }
        
    }
    bufferNum=max;

    if(metainfo[bufferNum].dirty==true)
     {
         Disk::writeBlock(blocks[bufferNum],metainfo[bufferNum].blockNum);
     }
}

    // if a free buffer is not available,
    //     find the buffer with the largest timestamp
    //     IF IT IS DIRTY, write back to the disk using Disk::writeBlock()
    //     set bufferNum = index of this buffer

    // update the metaInfo entry corresponding to bufferNum with
    // free:false, dirty:false, blockNum:the input block number, timeStamp:0.
    metainfo[bufferNum].free=false;
    metainfo[bufferNum].dirty=false;
    metainfo[bufferNum].blockNum=blockNum;
    metainfo[bufferNum].timeStamp=0;

    // return the bufferNum.
    return bufferNum;
}



int StaticBuffer::getBufferNum(int blockNum)
{
    if(blockNum < 0 || blockNum >= DISK_BLOCKS)
    {
        return E_OUTOFBOUND;
    }
    for(int i=0;i<BUFFER_CAPACITY;i++)
    {
        if(metainfo[i].blockNum == blockNum && !metainfo[i].free)
        {
            return i;
        }
      
    }
      return E_BLOCKNOTINBUFFER;
}

int StaticBuffer::setDirtyBit(int blockNum){
    // find the buffer index corresponding to the block using getBufferNum().
    int bufferIndex=getBufferNum(blockNum);
    if(bufferIndex==E_BLOCKNOTINBUFFER)
    {
        return E_BLOCKNOTINBUFFER;
    }

    // if block is not present in the buffer (bufferNum = E_BLOCKNOTINBUFFER)
    //     return E_BLOCKNOTINBUFFER

    // if blockNum is out of bound (bufferNum = E_OUTOFBOUND)
    //     return E_OUTOFBOUND
    if(blockNum < 0 || blockNum >= DISK_BLOCKS)
    {
        return E_OUTOFBOUND;
    }
    // else
    //     (the bufferNum is valid)
    //     set the dirty bit of that buffer to true in metainfo
    metainfo[bufferIndex].dirty=true;
     return SUCCESS;
}