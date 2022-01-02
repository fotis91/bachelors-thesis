/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

/* This is a simple application managing the hardware accelerator called "compress". It initializes the accelerator and feeds it with data*/

#include <stdio.h>
#include "xscugic.h"
#include "xparameters.h"
#include "xcompress.h"


XCompress compress;
XScuGic ScuGic;

volatile static int Runcompressinit = 0;
volatile static int ResultAvailcompress = 0;

int compress_init(XCompress *compressInitPtr);
int setup_interrupt();
void compress_isr(void *InstancePtr);
void compress_start(void *InstancePtr);


int main()
{
	 int status;
	 int character=47;
	 int code;
     int i=0;
     int ud=1;
     int tmpud;



     //Setup compress init
	   status = compress_init(&compress);
	   if(status != XST_SUCCESS){
	         print("compress init peripheral setup failed\n\r");
	         exit(-1);
	      }

	   status = setup_interrupt();
	      if(status != XST_SUCCESS){
	         print("Interrupt setup failed\n\r");
	         exit(-1);
	      }


	      for(i=0;i<15000;i++){

	    	  tmpud=ud;

	    	  if(character>=68)ud=0;
	    	  else if(character<=48)ud=1;
	    	  else ud=tmpud;

	    	  if(ud==1)character=character+1;
	    	  else character=character-1;


	    	  XCompress_Set_character_r(&compress,character);

	      if (XCompress_IsReady(&compress))
	            print("compress is ready.  Starting... ");
	         else {
	            print("!!! compress is not ready! Exiting...\n\r");
	            exit(-1);
	         }


	      if (0) { // use interrupt
	    	  compress_start(&compress);
	            while(!ResultAvailcompress)
	               ; // spin
	            code = XCompress_Get_character_r(&compress);
	            print("Interrupt received from HLS HW.\n\r");
	         } else { // Simple non-interrupt driven test
	        	 XCompress_Start(&compress);
	            do {
	            	code = XCompress_Get_character_r(&compress);
	            } while (!XCompress_IsReady(&compress));
	            print("Detected HLS peripheral complete. Result received.\n\r");
	         }



	   printf("code=%d\n",code);}
    return 0;
}



void compress_isr(void *InstancePtr){
	XCompress *pAccelerator = (XCompress *)InstancePtr;

   //Disable the global interrupt
	XCompress_InterruptGlobalDisable(pAccelerator);
   //Disable the local interrupt
	XCompress_InterruptDisable(pAccelerator,0xffffffff);

   // clear the local interrupt
	XCompress_InterruptClear(pAccelerator,1);

	ResultAvailcompress = 1;
   // restart the core if it should run again
   if(Runcompressinit){
	   compress_start(pAccelerator);
   }
}

int compress_init(XCompress *compressInitPtr)
{
	XCompress_Config *cfgPtr;
   int status;

   cfgPtr = XCompress_LookupConfig(XPAR_COMPRESS_0_DEVICE_ID);
   if (!cfgPtr) {
      print("ERROR: Lookup of acclerator configuration failed.\n\r");
      return XST_FAILURE;
   }
   status = XCompress_CfgInitialize(compressInitPtr, cfgPtr);
   if (status != XST_SUCCESS) {
      print("ERROR: Could not initialize accelerator.\n\r");
      return XST_FAILURE;
   }
   return status;
}

void compress_start(void *InstancePtr){
	XCompress *pAccelerator = (XCompress *)InstancePtr;
	XCompress_InterruptEnable(pAccelerator,1);
	XCompress_InterruptGlobalEnable(pAccelerator);
	XCompress_Start(pAccelerator);
}

int setup_interrupt()
{
   //This functions sets up the interrupt on the ARM
   int result;
   XScuGic_Config *pCfg = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
   if (pCfg == NULL){
      print("Interrupt Configuration Lookup Failed\n\r");
      return XST_FAILURE;
   }
   result = XScuGic_CfgInitialize(&ScuGic,pCfg,pCfg->CpuBaseAddress);
   if(result != XST_SUCCESS){
      return result;
   }
   // self test
   result = XScuGic_SelfTest(&ScuGic);
   if(result != XST_SUCCESS){
      return result;
   }
   // Initialize the exception handler
   Xil_ExceptionInit();
   // Register the exception handler
   //print("Register the exception handler\n\r");
   Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&ScuGic);
   //Enable the exception handler
   Xil_ExceptionEnable();
   // Connect the Adder ISR to the exception table
   //print("Connect the Adder ISR to the Exception handler table\n\r");
   result = XScuGic_Connect(&ScuGic,XPAR_FABRIC_COMPRESS_0_INTERRUPT_INTR,(Xil_InterruptHandler)compress_isr,&compress);
   if(result != XST_SUCCESS){
      return result;
   }
   //print("Enable the Adder ISR\n\r");
   XScuGic_Enable(&ScuGic,XPAR_FABRIC_COMPRESS_0_INTERRUPT_INTR);
   return XST_SUCCESS;
}
