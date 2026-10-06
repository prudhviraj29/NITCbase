//  RecBuffer relCatBuffer(RELCAT_BLOCK);

//     HeadInfo relCatHeader;
//     relCatBuffer.getHeader(&relCatHeader);
    
//     printf("Relation entries : %d\n", relCatHeader.numEntries);
    
//     int totalAttrEntries = 0;
//     int currentBlock = ATTRCAT_BLOCK;
    
//     while (currentBlock != -1) {
    
//         RecBuffer attrCatBuffer(currentBlock);
    
//         HeadInfo attrCatHeader;
//         attrCatBuffer.getHeader(&attrCatHeader);
    
//         totalAttrEntries += attrCatHeader.numEntries;
    
//         currentBlock = attrCatHeader.rblock;
//     }
    
//     printf("Attribute entries: %d\n\n", totalAttrEntries);

//     for (int i = 0; i < relCatHeader.numEntries; i++) {

//         Attribute relCatRecord[RELCAT_NO_ATTRS];

//         relCatBuffer.getRecord(relCatRecord, i);

//         printf("Relation: %s\n",
//                relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

//        int currentBlock = ATTRCAT_BLOCK;

//         while (currentBlock != -1) {
        
//             RecBuffer attrCatBuffer(currentBlock);
        
//             HeadInfo attrCatHeader;
//             attrCatBuffer.getHeader(&attrCatHeader);
        
//             for (int j = 0; j < attrCatHeader.numEntries; j++) {
        
//                 Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
        
//                 attrCatBuffer.getRecord(attrCatRecord, j);
        
//                 if (strcmp(relCatRecord[RELCAT_REL_NAME_INDEX].sVal,
//                            attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal) == 0) {
        
//                     const char *type =
//                         (attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER)
//                             ? "NUM"
//                             : "STR";
        
//                     printf("  %s : %s\n",
//                            attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,
//                            type);
//                 }
//             }
        
//             currentBlock = attrCatHeader.rblock;
//         }
//         printf("\n");
//     } return 0;
