#include "stm32f10x.h"





/**********************FLASH¶ÁÐ´²Ù×÷***************************************************/
void Flash_ErasePage(u32 addr){
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(addr); 
    FLASH_Lock();    
}

void Flash_WriteHalfWord(u32 addr, unsigned short *data, unsigned short len){
			u8 count;
			FLASH_Unlock();
			FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
			for(count = 0; count<len; count++){
		        FLASH_ProgramHalfWord(addr+count*2,data[count]);	
			}
			FLASH_Lock();
}

unsigned short* Flash_ReadHalfWord(u32 addr){
		return (unsigned short*)addr;		
}


