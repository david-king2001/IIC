#include "definitions.h"
#include <string.h>

//Global structures and variables
I80IT8951DevInfo gstI80DevInfo;
TByte* gpFrameBuf;
// TByte FrameBuf[59520];
TDWord gulImgBufAddr; //IT8951 Image buffer address

void delay() {
    TWord i, j;
    for (i = 0; i < 10000; i++)
        for (j = 0; j < 10; j++);
}

void delay_nop() {
    TWord i;
    for (i = 0; i < 50; i++);
}

void LCDWaitForReady() {
    while (DISPLAY_RDY_Get() == 0);

}

void SPI_Send(TWord* data, TDWord size) {
    while (SPI2_IsBusy());
    SPI2_Write((uint8_t*) data, size * 2);
    while (SPI2_IsBusy());
}

void SPI_Read(TWord* data, TDWord size) {
    while (SPI2_IsBusy());
    SPI2_Read((uint8_t*) data, size * 2);
    while (SPI2_IsBusy());
}

//-----------------------------------------------------------------
//Host controller function 2   Write command code to host data Bus
//-----------------------------------------------------------------

void LCDWriteCmdCode(TWord usCmdCode) {
    //Set Preamble for Write Command
    TWord wPreamble = 0x6000;

    wPreamble = MY_WORD_SWAP(wPreamble);


    LCDWaitForReady();
    SS_DISPLAY_Clear();
    SPI_Send(&wPreamble, 1);

    usCmdCode = MY_WORD_SWAP(usCmdCode);
    LCDWaitForReady();

    SPI_Send(&usCmdCode, 1);

    SS_DISPLAY_Set();
}

//-----------------------------------------------------------
//Host controller function 3    Write Data to host data Bus
//-----------------------------------------------------------

void LCDWriteData(TWord usData)//ok
{
    TWord wPreamble = 0x0000;
    wPreamble = MY_WORD_SWAP(wPreamble);


    LCDWaitForReady();
    SS_DISPLAY_Clear();

    SPI_Send(&wPreamble, 1);


    LCDWaitForReady();

    usData = MY_WORD_SWAP(usData);

    SPI_Send(&usData, 1);

    SS_DISPLAY_Set();

}

void LCDWriteNData(TWord* pwBuf, TDWord ulSizeWordCnt) {
    TWord wPreamble = 0x0000;

    wPreamble = MY_WORD_SWAP(wPreamble);

    LCDWaitForReady();
    SS_DISPLAY_Clear();


    SPI_Send(&wPreamble, 1);

    for (TDWord i = 0; i < ulSizeWordCnt; i++) {
        pwBuf[i] = MY_WORD_SWAP(pwBuf[i]);
    }


    LCDWaitForReady();
    //Send Data

    SPI_Send(pwBuf, ulSizeWordCnt);

    SS_DISPLAY_Set();


}

//-----------------------------------------------------------
//Host controller function 4   Read Data from host data Bus
//-----------------------------------------------------------

TWord LCDReadData() {
    TWord wRData;
    TWord dummy;
    //set type and direction
    TWord wPreamble = 0x1000;
    wPreamble = MY_WORD_SWAP(wPreamble);

    LCDWaitForReady();

    SS_DISPLAY_Clear();

    SPI_Send(&wPreamble, 1);


    LCDWaitForReady();
    SPI_Read(&dummy, 1); //Dummy


    LCDWaitForReady();

    //Read Data
    SPI_Read(&wRData, 1);

    SS_DISPLAY_Set();

    wRData = MY_WORD_SWAP(wRData);


    return wRData;
}

//-------------------------------------------------------------------
//  Read Burst N words Data
//-------------------------------------------------------------------

void LCDReadNData(TWord* pwBuf, TDWord ulSizeWordCnt) {
    //set type and direction
    TWord wPreamble = 0x1000;
    TWord dummy;
    wPreamble = MY_WORD_SWAP(wPreamble);

    LCDWaitForReady();
    SS_DISPLAY_Clear();

    //Send Preamble before reading data
    SPI_Send(&wPreamble, 1);


    LCDWaitForReady();
    SPI_Read(&dummy, 1); //Dummy

    LCDWaitForReady();


    SPI_Read(pwBuf, ulSizeWordCnt);

    SS_DISPLAY_Set();

    for (TDWord i = 0; i < ulSizeWordCnt; i++) {
        pwBuf[i] = MY_WORD_SWAP(pwBuf[i]);
    }

}


//-----------------------------------------------------------
//Host controller function 5 ¡V Write command to host data Bus with aruments
//-----------------------------------------------------------

void LCDSendCmdArg(TWord usCmdCode, TWord* pArg, TWord usNumArg) {
    TWord i;
    //Send Cmd code
    LCDWriteCmdCode(usCmdCode);
    //Send Data
    for (i = 0; i < usNumArg; i++) {
        LCDWriteData(pArg[i]);
    }
}

//-----------------------------------------------------------
//Host Cmd 1 ¡V SYS_RUN
//-----------------------------------------------------------

void IT8951SystemRun() {
    LCDWriteCmdCode(IT8951_TCON_SYS_RUN);
}

//-----------------------------------------------------------
//Host Cmd 2 - STANDBY
//-----------------------------------------------------------

void IT8951StandBy() {
    LCDWriteCmdCode(IT8951_TCON_STANDBY);
}

//-----------------------------------------------------------
//Host Cmd 3 - SLEEP
//-----------------------------------------------------------

void IT8951Sleep() {
    LCDWriteCmdCode(IT8951_TCON_SLEEP);
}

//-----------------------------------------------------------
//Host Cmd 4 - REG_RD
//-----------------------------------------------------------

TWord IT8951ReadReg(TWord usRegAddr) {
    TWord usData;
    //----------I80 Mode-------------
    //Send Cmd and Register Address

    LCDWriteCmdCode(IT8951_TCON_REG_RD);
    LCDWriteData(usRegAddr);
    //Read data from Host Data bus
    usData = LCDReadData();

    return usData;
}

//-----------------------------------------------------------
//Host Cmd 5 - REG_WR
//-----------------------------------------------------------

void IT8951WriteReg(TWord usRegAddr, TWord usValue) {
    //I80 Mode
    //Send Cmd , Register Address and Write Value
    LCDWriteCmdCode(IT8951_TCON_REG_WR);
    LCDWriteData(usRegAddr);
    LCDWriteData(usValue);
}

//-----------------------------------------------------------
//Host Cmd 6 - MEM_BST_RD_T
//-----------------------------------------------------------

void IT8951MemBurstReadTrigger(TDWord ulMemAddr, TDWord ulReadSize) {
    TWord usArg[4];
    //Setting Arguments for Memory Burst Read
    usArg[0] = (TWord) (ulMemAddr & 0x0000FFFF); //addr[15:0]
    usArg[1] = (TWord) ((ulMemAddr >> 16) & 0x0000FFFF); //addr[25:16]
    usArg[2] = (TWord) (ulReadSize & 0x0000FFFF); //Cnt[15:0]
    usArg[3] = (TWord) ((ulReadSize >> 16) & 0x0000FFFF); //Cnt[25:16]
    //Send Cmd and Arg
    LCDSendCmdArg(IT8951_TCON_MEM_BST_RD_T, usArg, 4);
}

//-----------------------------------------------------------
//Host Cmd 7 - MEM_BST_RD_S
//-----------------------------------------------------------

void IT8951MemBurstReadStart() {
    LCDWriteCmdCode(IT8951_TCON_MEM_BST_RD_S);
}

//-----------------------------------------------------------
//Host Cmd 8 - MEM_BST_WR
//-----------------------------------------------------------

void IT8951MemBurstWrite(TDWord ulMemAddr, TDWord ulWriteSize) {
    TWord usArg[4];
    //Setting Arguments for Memory Burst Write
    usArg[0] = (TWord) (ulMemAddr & 0x0000FFFF); //addr[15:0]
    usArg[1] = (TWord) ((ulMemAddr >> 16) & 0x0000FFFF); //addr[25:16]
    usArg[2] = (TWord) (ulWriteSize & 0x0000FFFF); //Cnt[15:0]
    usArg[3] = (TWord) ((ulWriteSize >> 16) & 0x0000FFFF); //Cnt[25:16]
    //Send Cmd and Arg
    LCDSendCmdArg(IT8951_TCON_MEM_BST_WR, usArg, 4);
}

//-----------------------------------------------------------
//Host Cmd 9 - MEM_BST_END
//-----------------------------------------------------------

void IT8951MemBurstEnd(void) {
    LCDWriteCmdCode(IT8951_TCON_MEM_BST_END);
}

//-----------------------------------------------------------
//Example of Memory Burst Write
//-----------------------------------------------------------
// ****************************************************************************************
// Function name: IT8951MemBurstWriteProc( )
//
// Description:
//   IT8951 Burst Write procedure
//      
// Arguments:
//      TDWord ulMemAddr: IT8951 Memory Target Address
//      TDWord ulWriteSize: Write Size (Unit: Word)
//      TByte* pDestBuf - Buffer of Sent data
// Return Values:
//   NULL.
// Note:
//
// ****************************************************************************************

void IT8951MemBurstWriteProc(TDWord ulMemAddr, TDWord ulWriteSize, TWord* pSrcBuf) {

    TDWord i;

    //Send Burst Write Start Cmd and Args
    IT8951MemBurstWrite(ulMemAddr, ulWriteSize);

    //Burst Write Data
    for (i = 0; i < ulWriteSize; i++) {
        LCDWriteData(pSrcBuf[i]);
    }

    //Send Burst End Cmd
    IT8951MemBurstEnd();
}

// ****************************************************************************************
// Function name: IT8951MemBurstReadProc( )
//
// Description:
//   IT8951 Burst Read procedure
//      
// Arguments:
//      TDWord ulMemAddr: IT8951 Read Memory Address
//      TDWord ulReadSize: Read Size (Unit: Word)
//      TByte* pDestBuf - Buffer for storing Read data
// Return Values:
//   NULL.
// Note:
//
// ****************************************************************************************

void IT8951MemBurstReadProc(TDWord ulMemAddr, TDWord ulReadSize, TWord* pDestBuf) {

    //Send Burst Read Start Cmd and Args
    IT8951MemBurstReadTrigger(ulMemAddr, ulReadSize);

    //Burst Read Fire
    IT8951MemBurstReadStart();

    //Burst Read Request for SPI interface only
    for (int i = 0; i < ulReadSize; i++) {
        LCDReadData(pDestBuf[i]);
    }



    //Send Burst End Cmd
    IT8951MemBurstEnd(); //the same with IT8951MemBurstEnd()
}

//-----------------------------------------------------------
//Host Cmd 10 - LD_IMG
//-----------------------------------------------------------

void IT8951LoadImgStart(IT8951LdImgInfo* pstLdImgInfo) {
    TWord usArg;
    //Setting Argument for Load image start
    usArg = (pstLdImgInfo->usEndianType << 8)
            | (pstLdImgInfo->usPixelFormat << 4)
            | (pstLdImgInfo->usRotate);
    //Send Cmd
    LCDWriteCmdCode(IT8951_TCON_LD_IMG);
    //Send Arg
    LCDWriteData(usArg);
}

//-----------------------------------------------------------
//Host Cmd 11 - LD_IMG_AREA
//-----------------------------------------------------------

void IT8951LoadImgAreaStart(IT8951LdImgInfo* pstLdImgInfo, IT8951AreaImgInfo* pstAreaImgInfo) {
    TWord usArg[5];
    //Setting Argument for Load image start
    usArg[0] = (pstLdImgInfo->usEndianType << 8)
            | (pstLdImgInfo->usPixelFormat << 4)
            | (pstLdImgInfo->usRotate);
    usArg[1] = pstAreaImgInfo->usX;
    usArg[2] = pstAreaImgInfo->usY;
    usArg[3] = pstAreaImgInfo->usWidth;
    usArg[4] = pstAreaImgInfo->usHeight;
    //Send Cmd and Args
    LCDSendCmdArg(IT8951_TCON_LD_IMG_AREA, usArg, 5);
}

//-----------------------------------------------------------
//Host Cmd 12 - LD_IMG_END
//-----------------------------------------------------------

void IT8951LoadImgEnd(void) {
    LCDWriteCmdCode(IT8951_TCON_LD_IMG_END);
}

void GetIT8951SystemInfo(void* pBuf) {
    TWord* pusWord = (TWord*) pBuf;


    LCDWriteCmdCode(USDEF_I80_CMD_GET_DEV_INFO);


    //Burst Read Request for SPI interface only
    LCDReadNData(pusWord, sizeof (I80IT8951DevInfo) / 2); //Polling HRDY for each words(2-bytes) if possible

}

//-----------------------------------------------------------
//Initial function 2 ¡V Set Image buffer base address
//-----------------------------------------------------------

void IT8951SetImgBufBaseAddr(TDWord ulImgBufAddr) {
    TWord usWordH = (TWord) ((ulImgBufAddr >> 16) & 0x0000FFFF);
    TWord usWordL = (TWord) (ulImgBufAddr & 0x0000FFFF);
    //Write LISAR Reg
    IT8951WriteReg(LISAR + 2, usWordH);
    IT8951WriteReg(LISAR, usWordL);
}

//-----------------------------------------------------
// 3.6. Display Functions
//-----------------------------------------------------

//-----------------------------------------------------------
//Display function 1 - Wait for LUT Engine Finish
//                     Polling Display Engine Ready by LUTNo
//-----------------------------------------------------------

void IT8951WaitForDisplayReady() {
    //Check IT8951 Register LUTAFSR => NonZero ¡V Busy, 0 - Free

    while (IT8951ReadReg(LUTAFSR));
}

//-----------------------------------------------------------
//Display function 2 ¡V Load Image Area process
//-----------------------------------------------------------

void IT8951HostAreaPackedPixelWrite(IT8951LdImgInfo* pstLdImgInfo, IT8951AreaImgInfo* pstAreaImgInfo) {
    TDWord j;
    //Source buffer address of Host
    TWord* pusFrameBuf = (TWord*) pstLdImgInfo->ulStartFBAddr;

    //Set Image buffer(IT8951) Base address
    IT8951SetImgBufBaseAddr(pstLdImgInfo->ulImgBufBaseAddr);
    //Send Load Image start Cmd
    IT8951LoadImgAreaStart(pstLdImgInfo, pstAreaImgInfo);



    //Host Write Data
    for (j = 0; j < pstAreaImgInfo->usHeight; j++) {

        LCDWriteNData(pusFrameBuf, pstAreaImgInfo->usWidth / 2);
        pusFrameBuf += pstAreaImgInfo->usWidth / 2;
    }

    //Send Load Img End Command

    IT8951LoadImgEnd();

}

//-----------------------------------------------------------
//Display functions 3 - Application for Display panel Area
//-----------------------------------------------------------

void IT8951DisplayArea(TWord usX, TWord usY, TWord usW, TWord usH, TWord usDpyMode) {
    //Send I80 Display Command (User defined command of IT8951)
    LCDWriteCmdCode(USDEF_I80_CMD_DPY_AREA); //0x0034
    //Write arguments
    LCDWriteData(usX);
    LCDWriteData(usY);
    LCDWriteData(usW);
    LCDWriteData(usH);
    LCDWriteData(usDpyMode);
}

//Display Area with bitmap on EPD
//-----------------------------------------------------------
// Display Function 4 - for Display Area for 1-bpp mode format
//   the bitmap(1bpp) mode will be enable when Display
//   and restore to Default setting (disable) after displaying finished
//-----------------------------------------------------------

void IT8951DisplayArea1bpp(TWord usX, TWord usY, TWord usW, TWord usH, TWord usDpyMode, TByte ucBGGrayVal, TByte ucFGGrayVal) {
    //Set Display mode to 1 bpp mode - Set 0x18001138 Bit[18](0x1800113A Bit[2])to 1
    IT8951WriteReg(UP1SR + 2, IT8951ReadReg(UP1SR + 2) | (1 << 2));

    //Set BitMap color table 0 and 1 , => Set Register[0x18001250]:
    //Bit[7:0]: ForeGround Color(G0~G15)  for 1
    //Bit[15:8]:Background Color(G0~G15)  for 0
    IT8951WriteReg(BGVR, (ucBGGrayVal << 8) | ucFGGrayVal);

    //Display
    IT8951DisplayArea(usX, usY, usW, usH, usDpyMode);
    IT8951WaitForDisplayReady();

    //Restore to normal mode
    IT8951WriteReg(UP1SR + 2, IT8951ReadReg(UP1SR + 2) & ~(1 << 2));
}

//-------------------------------------------------------------------------------------------------------------
// 	Command - 0x0037 for Display Base addr by User 
//  TDWord ulDpyBufAddr - Host programmer need to indicate the Image buffer address of IT8951
//                                         In current case, there is only one image buffer in IT8951 so far.
//                                         So Please set the Image buffer address you got  in initial stage.
//                                         (gulImgBufAddr by Get device information 0x0302 command)
//
//-------------------------------------------------------------------------------------------------------------

void IT8951DisplayAreaBuf(TWord usX, TWord usY, TWord usW, TWord usH, TWord usDpyMode, TDWord ulDpyBufAddr) {
    //Send I80 Display Command (User defined command of IT8951)
    LCDWriteCmdCode(USDEF_I80_CMD_DPY_BUF_AREA); //0x0037

    //Write arguments
    LCDWriteData(usX);
    LCDWriteData(usY);
    LCDWriteData(usW);
    LCDWriteData(usH);
    LCDWriteData(usDpyMode);
    LCDWriteData((TWord) ulDpyBufAddr); //Display Buffer Base address[15:0]
    LCDWriteData((TWord) (ulDpyBufAddr >> 16)); //Display Buffer Base address[26:16]
}

//-----------------------------------------------------------
//Test function 1 ¡VSoftware Initial flow for testing
//-----------------------------------------------------------

void HostInit() {
    //Get Device Info
    GetIT8951SystemInfo(&gstI80DevInfo);

    //Host Frame Buffer allocation
    gpFrameBuf = (TByte*) malloc(gstI80DevInfo.usPanelW * gstI80DevInfo.usPanelH);
    //Get Image Buffer Address of IT8951
    gulImgBufAddr = gstI80DevInfo.usImgBufAddrL | (gstI80DevInfo.usImgBufAddrH << 16);

    //Set to Enable I80 Packed mode
    IT8951WriteReg(I80CPCR, 0x0001);
}

void IT8951DisplayExample() {

    IT8951LdImgInfo stLdImgInfo;
    IT8951AreaImgInfo stAreaImgInfo;
    //Host Initial
    HostInit(); //Test Function 1
    //Prepare image
    //Write pixel 0xF0(White) to Frame Buffer

    //memset(gpFrameBuf, 0xF0, (gstI80DevInfo.usPanelW * gstI80DevInfo.usPanelH)/8);

    //Check TCon is free ? Wait TCon Ready (optional)
    IT8951WaitForDisplayReady();
    //--------------------------------------------------------------------------------------------
    //      initial display - Display white only
    //--------------------------------------------------------------------------------------------
    //Load Image and Display
    //Setting Load image information
    stLdImgInfo.ulStartFBAddr = (TDWord) gpFrameBuf;
    stLdImgInfo.usEndianType = IT8951_LDIMG_B_ENDIAN;
    stLdImgInfo.usPixelFormat = IT8951_8BPP;
    stLdImgInfo.usRotate = IT8951_ROTATE_0;
    stLdImgInfo.ulImgBufBaseAddr = gulImgBufAddr;
    //Set Load Area
    stAreaImgInfo.usX = 0;
    stAreaImgInfo.usY = 0;
    stAreaImgInfo.usWidth = gstI80DevInfo.usPanelW;
    stAreaImgInfo.usHeight = gstI80DevInfo.usPanelH;

    //Load Image from Host to IT8951 Image Buffer
    IT8951HostAreaPackedPixelWrite(&stLdImgInfo, &stAreaImgInfo); //Display function 2


    //Display Area ¡V (x,y,w,h) with mode 0 for initial White to clear Panel
    IT8951DisplayArea(0, 0, gstI80DevInfo.usPanelW, gstI80DevInfo.usPanelH, 0);

}

void IT8951DisplayExample2() {
    IT8951LdImgInfo stLdImgInfo;
    IT8951AreaImgInfo stAreaImgInfo;
    HostInit(); //Test Function 1
    uint16_t width = gstI80DevInfo.usPanelW;
    uint16_t high = gstI80DevInfo.usPanelH / 16;

    //--------------------------------------------------------------------------------------------
    //      Regular display - Display Any Gray colors with Mode 2 or others
    //--------------------------------------------------------------------------------------------
    //Preparing buffer to All black (8 bpp image)
    //or you can create your image pattern here..
    memset(gpFrameBuf, 0x00, width * high * 1);
    memset(gpFrameBuf + width * high * 1, 0x11, width * high * 1);
    memset(gpFrameBuf + width * high * 2, 0x22, width * high * 1);
    memset(gpFrameBuf + width * high * 3, 0x33, width * high * 1);
    memset(gpFrameBuf + width * high * 4, 0x44, width * high * 1);
    memset(gpFrameBuf + width * high * 5, 0x55, width * high * 1);
    memset(gpFrameBuf + width * high * 6, 0x66, width * high * 1);
    memset(gpFrameBuf + width * high * 7, 0x77, width * high * 1);
    memset(gpFrameBuf + width * high * 8, 0x88, width * high * 1);
    memset(gpFrameBuf + width * high * 9, 0x99, width * high * 1);
    memset(gpFrameBuf + width * high * 10, 0xaa, width * high * 1);
    memset(gpFrameBuf + width * high * 11, 0xbb, width * high * 1);
    memset(gpFrameBuf + width * high * 12, 0xcc, width * high * 1);
    memset(gpFrameBuf + width * high * 13, 0xdd, width * high * 1);
    memset(gpFrameBuf + width * high * 14, 0xee, width * high * 1);
    memset(gpFrameBuf + width * high * 15, 0xff, gstI80DevInfo.usPanelW * gstI80DevInfo.usPanelH - width * high * 15);

    IT8951WaitForDisplayReady();

    //Setting Load image information
    stLdImgInfo.ulStartFBAddr = (TDWord) gpFrameBuf;
    stLdImgInfo.usEndianType = IT8951_LDIMG_L_ENDIAN;
    stLdImgInfo.usPixelFormat = IT8951_8BPP;
    stLdImgInfo.usRotate = IT8951_ROTATE_0;
    stLdImgInfo.ulImgBufBaseAddr = gulImgBufAddr;
    //Set Load Area
    stAreaImgInfo.usX = 0;
    stAreaImgInfo.usY = 0;
    stAreaImgInfo.usWidth = gstI80DevInfo.usPanelW;
    stAreaImgInfo.usHeight = gstI80DevInfo.usPanelH;

    //Load Image from Host to IT8951 Image Buffer
    IT8951HostAreaPackedPixelWrite(&stLdImgInfo, &stAreaImgInfo); //Display function 2
    //Display Area ¡V (x,y,w,h) with mode 2 for fast gray clear mode - depends on current waveform 
    IT8951DisplayArea(0, 0, gstI80DevInfo.usPanelW, gstI80DevInfo.usPanelH, 2);
}

void IT8951DisplayClear() {
    IT8951LdImgInfo stLdImgInfo;
    IT8951AreaImgInfo stAreaImgInfo;


    //--------------------------------------------------------------------------------------------
    //      Regular display - Display Any Gray colors with Mode 2 or others
    //--------------------------------------------------------------------------------------------
    //Preparing buffer to All black (8 bpp image)
    //or you can create your image pattern here..
    memset(gpFrameBuf, 0x00, gstI80DevInfo.usPanelW * gstI80DevInfo.usPanelH);
    HostInit(); //Test Function 1

    IT8951WaitForDisplayReady();

    //Setting Load image information
    stLdImgInfo.ulStartFBAddr = (TDWord) gpFrameBuf;
    stLdImgInfo.usEndianType = IT8951_LDIMG_B_ENDIAN;
    stLdImgInfo.usPixelFormat = IT8951_8BPP;
    stLdImgInfo.usRotate = IT8951_ROTATE_0;
    stLdImgInfo.ulImgBufBaseAddr = gulImgBufAddr;
    //Set Load Area
    stAreaImgInfo.usX = 0;
    stAreaImgInfo.usY = 0;
    stAreaImgInfo.usWidth = gstI80DevInfo.usPanelW;
    stAreaImgInfo.usHeight = gstI80DevInfo.usPanelH;

    //Load Image from Host to IT8951 Image Buffer
    IT8951HostAreaPackedPixelWrite(&stLdImgInfo, &stAreaImgInfo); //Display function 2

    //Display Area ¡V (x,y,w,h) with mode 2 for fast gray clear mode - depends on current waveform 
    IT8951DisplayArea(0, 0, gstI80DevInfo.usPanelW, gstI80DevInfo.usPanelH, 2);


    //--------------------------------------------------------------------------------------------
    //      initial display - Display white only
    //--------------------------------------------------------------------------------------------
    //Load Image and Display
    //Setting Load image information
    stLdImgInfo.ulStartFBAddr = (TDWord) gpFrameBuf;
    stLdImgInfo.usEndianType = IT8951_LDIMG_B_ENDIAN;
    stLdImgInfo.usPixelFormat = IT8951_8BPP;
    stLdImgInfo.usRotate = IT8951_ROTATE_0;
    stLdImgInfo.ulImgBufBaseAddr = gulImgBufAddr;
    //Set Load Area
    stAreaImgInfo.usX = 0;
    stAreaImgInfo.usY = 0;
    stAreaImgInfo.usWidth = gstI80DevInfo.usPanelW;
    stAreaImgInfo.usHeight = gstI80DevInfo.usPanelH;

    //Load Image from Host to IT8951 Image Buffer
    IT8951HostAreaPackedPixelWrite(&stLdImgInfo, &stAreaImgInfo); //Display function 2
    //Display Area ¡V (x,y,w,h) with mode 0 for initial White to clear Panel
    IT8951DisplayArea(0, 0, gstI80DevInfo.usPanelW, gstI80DevInfo.usPanelH, 0);

}

void IT8951Load1bppImage(TByte* p1bppImgBuf, TWord usX, TWord usY, TWord usW, TWord usH) {
    IT8951LdImgInfo stLdImgInfo;
    IT8951AreaImgInfo stAreaImgInfo;
    //Setting Load image information
    stLdImgInfo.ulStartFBAddr = (TDWord) p1bppImgBuf;
    stLdImgInfo.usEndianType = IT8951_LDIMG_L_ENDIAN;
    stLdImgInfo.usPixelFormat = IT8951_8BPP; //we use 8bpp because IT8951 dose not support 1bpp mode for load image¡Aso we use Load 8bpp mode ,but the transfer size needs to be reduced to Size/8
    stLdImgInfo.usRotate = IT8951_ROTATE_0;
    stLdImgInfo.ulImgBufBaseAddr = gulImgBufAddr;
    //Set Load Area
    stAreaImgInfo.usX = usX / 8;
    stAreaImgInfo.usY = usY;
    stAreaImgInfo.usWidth = usW / 8; //1bpp, Chaning Transfer size setting to 1/8X of 8bpp mode 
    stAreaImgInfo.usHeight = usH;
    //Load Image from Host to IT8951 Image Buffer
    IT8951HostAreaPackedPixelWrite(&stLdImgInfo, &stAreaImgInfo); //Display function 2
}

void IT8951Display1bppExample() {
    IT8951AreaImgInfo stAreaImgInfo;

    //Host Initial
    HostInit(); //Test Function 1
    //Prepare image
    //Write pixel 0x00(Black) to Frame Buffer
    //or you can create your image pattern here..
    memset(gpFrameBuf, 0x00, (gstI80DevInfo.usPanelW * gstI80DevInfo.usPanelH) / 8); //Host Frame Buffer(Source)
    //Check TCon is free ? Wait TCon Ready (optional)
    IT8951WaitForDisplayReady();
    //Load Image and Display
    //Set Load Area
    stAreaImgInfo.usX = 0;
    stAreaImgInfo.usY = 0;
    stAreaImgInfo.usWidth = gstI80DevInfo.usPanelW;
    stAreaImgInfo.usHeight = gstI80DevInfo.usPanelH;
    //Load Image from Host to IT8951 Image Buffer
    IT8951Load1bppImage(gpFrameBuf, stAreaImgInfo.usX, stAreaImgInfo.usY, stAreaImgInfo.usWidth, stAreaImgInfo.usHeight); //Display function 4, Arg

    //Display Area - (x,y,w,h) with mode 2 for Gray Scale
    //e.g. if we want to set b0(Background color) for Black-0x00 , Set b1(Foreground) for White-0xFF
    IT8951DisplayArea1bpp(0, 0, gstI80DevInfo.usPanelW, gstI80DevInfo.usPanelH, 0, 0x00, 0xFF);
}