/*
 * stm32_dma.c -- STM32 DMA driver
 *
 * (C) Copyright 2016, Philipp Rossak <embed3d@gmail.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/stm32.h>
#include <asm/arch/dma.h>


int initialize_dma(enum dma_select dma){
	/* clear pending interrupts */
	switch (dma) {
		case DMA1:
			setbits_le32(&STM32_DMA1->lifcr,CLEAR_ALL_LIFCR);
			setbits_le32(&STM32_DMA1->hifcr,CLEAR_ALL_HIFCR);
		break;
		case DMA2:
			setbits_le32(&STM32_DMA2->lifcr,CLEAR_ALL_LIFCR);
			setbits_le32(&STM32_DMA2->hifcr,CLEAR_ALL_HIFCR);
		break;
		default:
		break;
	}
	debug("DMA %d is now initilized\n",dma+1);
	return 0;

}

int configure_dma(enum dma_function dma_function ,struct dma_config config){
	switch(dma_function) {
		case SDMMC1:
			debug("DMA target adress: %08x\n",config.target_adress);
			debug("DMA source adress: %08x\n",config.source_adress);
			debug("DMA length: %d\n",config.datalength);
			/* use of dma2 stram3 */
			debug("DMA CURRENT CONFIG: %08x\n",readl(&STM32_DMA2->s3cr));
			debug("DMA ir CONFIG: %08x\n",readl(&STM32_DMA2->lisr));
			debug("FIFO STATUS REG: %08x\n",readl(&STM32_DMA2->s3fcr));

			/* disable stream & wait for en == 0*/
			debug("DMA CURRENT CONFIG: %08x\n",readl(&STM32_DMA2->s3cr));
			writel(0, &STM32_DMA2->s3cr);
			debug("DMA CURRENT CONFIG: %08x\n",readl(&STM32_DMA2->s3cr));
			while(readl(&STM32_DMA2->s3cr) & 0x1)
				udelay(1);

			u32 val = 0;
			/* clear ir reg */
			setbits_le32(&STM32_DMA2->lifcr,( CTCIF3 | CHTIF3 | \
					       	CTEIF3 | CDMEIF3 | CFEIF3));

			/* program channel4 */
			val |= DMA_CHANNEL_4;

			/* Memmory increment */
			val |= MINC_ON;

			/* Peripherial word size */
			val |= PSIZE_32bit;

			/* Memmory word size */
			val |= MSIZE_32bit;

			/* Priority level  */
			val |= PRIORITY_MED;

			/* Peripherial Flow controller */
			val |= PERIPH_F_CRTL;

			/* Peripherial burst size (4 beats)  */
			val |= PBURST_INCR4;

			/* Memmory burst size  */
			val |= MBURST_SINGLE;

			/* Write length*/
			writel(config.datalength,&STM32_DMA2->s3ndtr);

			//writel((readl(&STM32_DMA2->s3fcr) & 0xFFFFFFFC),&STM32_DMA2->s3fcr);
			//setbits_le32(&STM32_DMA2->s3fcr, 0x3);

			/* Write TARGET and SOURCE ADRESs*/
			if(config.dir == DMA_MEMORY_TO_PERIPH){
				debug("DMA direction: MEMMORY -> PERIPH\n");
				/* Configure DMA Stream destination address */
				writel(config.source_adress,&STM32_DMA2->s3m0ar);
				writel(config.target_adress,&STM32_DMA2->s3par);
				/*direction*/
				val |= DIR_M_to_P;
			} else {
				debug("DMA direction: PERIPH -> MEMMORY\n");
				writel(config.source_adress,&STM32_DMA2->s3par);
				writel(config.target_adress,&STM32_DMA2->s3m0ar);
				/*direction*/
				val |= DIR_P_to_M;
			}
			debug("M0 AR ADRESS: %08x\n",readl(&STM32_DMA2->s3par));
			debug("PAR ADDRESS: %08x\n",readl(&STM32_DMA2->s3m0ar));
			/*fifo threashold*/

			/* Write control register */
			writel(val,&STM32_DMA2->s3cr);

			/* enable dma2 stream 6 channel 4*/
			setbits_le32(&STM32_DMA2->s3cr,0x1);

			break;
		default:
			break;
	}
	return 0;

}

int disable_dma(enum dma_function dma_function){
	switch(dma_function) {
		case SDMMC1:
			debug("Disable DMA\n");
			writel(0, &STM32_DMA2->s3cr);
			break;
		default:
			break;
	}
	return 0;
}
