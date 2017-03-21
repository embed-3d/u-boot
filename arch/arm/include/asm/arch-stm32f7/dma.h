/*
 * stm32_dmac.h -- STM32 DMA driver
 *
 * (C) Copyright 2016, Philipp Rossak <embed3d@gmail.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _STM32_DMA_H_
#define _STM32_DMA_H_
/*
 *  DMA low interrupt flag clear register
 */
// clear transfer complete interrrupt flag
#define CTCIF0		BIT(5)
#define CTCIF1		BIT(11)
#define CTCIF2		BIT(21)
#define CTCIF3		BIT(27)

// clear half transfer interrupt flag
#define CHTIF0		BIT(4)
#define CHTIF1		BIT(10)
#define CHTIF2		BIT(20)
#define CHTIF3		BIT(26)

// clear transfer error interrupt flag
#define CTEIF0		BIT(3)
#define CTEIF1		BIT(9)
#define CTEIF2		BIT(19)
#define CTEIF3		BIT(25)

// clear direct mode error interrupt flag
#define CDMEIF0		BIT(2)
#define CDMEIF1		BIT(8)
#define CDMEIF2		BIT(18)
#define CDMEIF3		BIT(24)

// clear fifo error interrupt flag
#define CFEIF0		BIT(0)
#define CFEIF1		BIT(6)
#define CFEIF2		BIT(16)
#define CFEIF3		BIT(22)

#define CLEAR_ALL_LIFCR ( CTCIF0 || CTCIF1 || CTCIF2 || CTCIF3 || \
		CHTIF0 || CHTIF1 || CHTIF6 || CHTIF3 || \
		CTEIF0 || CTEIF1 || CTEIF2 || CTEIF3 || \
		CDMEIF0 || CDMEIF1 || CDMEIF2 || CDMEIF3 || \
		CFEIF0 || CFEIF1 || CFEIF2 || CFEIF3 )
/*
 *  DMA high interrupt flag clear register
 */
// clear transfer complete interrrupt flag
#define CTCIF4		BIT(5)
#define CTCIF5		BIT(11)
#define CTCIF6		BIT(21)
#define CTCIF7		BIT(27)

// clear half transfer interrupt flag
#define CHTIF4		BIT(4)
#define CHTIF5		BIT(10)
#define CHTIF6		BIT(20)
#define CHTIF7		BIT(26)

// clear transfer error interrupt flag
#define CTEIF4		BIT(3)
#define CTEIF5		BIT(9)
#define CTEIF6		BIT(19)
#define CTEIF7		BIT(25)

/* clear direct mode error interrupt flag */
#define CDMEIF4		BIT(2)
#define CDMEIF5		BIT(8)
#define CDMEIF6		BIT(18)
#define CDMEIF7		BIT(24)

/* clear fifo error interrupt flag */
#define CFEIF4		BIT(0)
#define CFEIF5		BIT(6)
#define CFEIF6		BIT(16)
#define CFEIF7		BIT(22)

#define CLEAR_ALL_HIFCR (CLEAR_ALL_LIFCR) /* same as low register*/

/*
#define CLEAR_ALL_HIFCR ( CTCIF4 || CTCIF5 || CTCIF6 || CTCIF7 || \
		CHTIF4 || CHTIF5 || CHTIF6 || CHTIF7 || \
		CTEIF4 || CTEIF5 || CTEIF6 || CTEIF7 || \
		CDMEIF4 || CDMEIF5 || CDMEIF6 || CDMEIF7 || \
		CFEIF4 || CFEIF5 || CFEIF6 || CFEIF7 )
*/

/*
 * Configuration Register
 */

/* Channel Definitions */
#define DMA_CHANNEL_0	(0 << 25)
#define DMA_CHANNEL_1	(1 << 25)
#define DMA_CHANNEL_2	(2 << 25)
#define DMA_CHANNEL_3	(3 << 25)
#define DMA_CHANNEL_4	(4 << 25)
#define DMA_CHANNEL_5	(5 << 25)
#define DMA_CHANNEL_6	(6 << 25)
#define DMA_CHANNEL_7	(7 << 25)

/* MBURST */
#define MBURST_SINGLE	(0 << 23)
#define MBURST_INCR4	(1 << 23)
#define MBURST_INCR8	(2 << 23)
#define MBURST_INCR16	(3 << 23)

/* PBURST */
#define PBURST_SINGLE	(0 << 21)
#define PBURST_INCR4	(1 << 21)
#define PBURST_INCR8	(2 << 21)
#define PBURST_INCR16	(3 << 21)

/* CURRENT Target */
#define CT_M0		(0 << 19)
#define CT_M1		(1 << 19)

/* Priority */
#define PRIORITY_LOW	(0 << 16)
#define PRIORITY_MED	(1 << 16)
#define PRIORITY_HIGH	(2 << 16)
#define PRIORITY_VHIGH	(3 << 16)

/* Memmory increment mode */
#define MINC_ON	 BIT(10)
#define MINC_OFF	(0 << 10)

/* Memmory Size */
#define MSIZE_8bit	(0 << 13)
#define MSIZE_16bit	(1 << 13)
#define MSIZE_32bit	(2 << 13)

/* Peripheral Size*/
#define PSIZE_8bit	(0 << 11)
#define PSIZE_16bit	(1 << 11)
#define PSIZE_32bit	(2 << 11)

/* Direction */
#define DIR_P_to_M	(0 << 6)
#define DIR_M_to_P	(1 << 6)
#define DIR_M_to_M	(2 << 6)

/* Peripheral Flow control*/
#define PERIPH_F_CRTL	BIT(5)
#define DMA_F_CRTL	(0 << 5)
struct stm32_dma_reg {
	u32 lisr; 	/* 0x00 low interrupt status register		*/
	u32 hisr; 	/* 0x04 high interrupt status register		*/
	u32 lifcr;	/* 0x08 low interrupt flag clear register	*/
	u32 hifcr;	/* 0x0C high interrupt flag clear register	*/
	u32 s0cr;	/* 0x10 stream 0 configuration register		*/
	u32 s0ndtr;	/* 0x14 stream 0 number of data register	*/
	u32 s0par;	/* 0x18 stream 0 peripheral address register	*/
	u32 s0m0ar;	/* 0x1C stream 0 memory 0 address register	*/
	u32 s0m1ar;	/* 0x20 stream 0 memory 1 address register	*/
	u32 s0fcr;	/* 0x24 stream 0 FIFO control register		*/
	u32 s1cr;	/* 0x28 stream 1 configuration register		*/
	u32 s1ndtr;	/* 0x2C stream 1 number of data register	*/
	u32 s1par;	/* 0x30 stream 1 peripheral address register	*/
	u32 s1m0ar;	/* 0x34 stream 1 memory 0 address register	*/
	u32 s1m1ar;	/* 0x38 stream 1 memory 1 address register	*/
	u32 s1fcr;	/* 0x3C stream 1 FIFO control register		*/
	u32 s2cr;	/* 0x40 stream 2 configuration register		*/
	u32 s2ndtr;	/* 0x44 stream 2 number of data register	*/
	u32 s2par;	/* 0x48 stream 2 peripheral address register	*/
	u32 s2m0ar;	/* 0x4C stream 2 memory 0 address register	*/
	u32 s2m1ar;	/* 0x50 stream 2 memory 1 address register	*/
	u32 s2fcr;	/* 0x54 stream 2 FIFO control register		*/
	u32 s3cr;	/* 0x58 stream 3 configuration register		*/
	u32 s3ndtr;	/* 0x5C stream 3 number of data register	*/
	u32 s3par;	/* 0x60 stream 3 peripheral address register	*/
	u32 s3m0ar;	/* 0x64 stream 3 memory 0 address register	*/
	u32 s3m1ar;	/* 0x68 stream 3 memory 1 address register	*/
	u32 s3fcr;	/* 0x6C stream 3 FIFO control register		*/
	u32 s4cr;	/* 0x70 stream 4 configuration register		*/
	u32 s4ndtr;	/* 0x74 stream 4 number of data register	*/
	u32 s4par;	/* 0x78 stream 4 peripheral address register	*/
	u32 s4m0ar;	/* 0x7C stream 4 memory 0 address register	*/
	u32 s4m1ar;	/* 0x80 stream 4 memory 1 address register	*/
	u32 s4fcr;	/* 0x84 stream 4 FIFO control register		*/
	u32 s5cr;	/* 0x88 stream 5 configuration register		*/
	u32 s5ndtr;	/* 0x8C stream 5 number of data register	*/
	u32 s5par;	/* 0x90 stream 5 peripheral address register	*/
	u32 s5m0ar;	/* 0x94 stream 5 memory 0 address register	*/
	u32 s5m1ar;	/* 0x98 stream 5 memory 1 address register	*/
	u32 s5fcr;	/* 0x9C stream 5 FIFO control register		*/
	u32 s6cr;	/* 0xA0 stream 6 configuration register		*/
	u32 s6ndtr;	/* 0xA4 stream 6 number of data register	*/
	u32 s6par;	/* 0xA8 stream 6 peripheral address register	*/
	u32 s6m0ar;	/* 0xAC stream 6 memory 0 address register	*/
	u32 s6m1ar;	/* 0xB0 stream 6 memory 1 address register	*/
	u32 s6fcr;	/* 0xB4 stream 6 FIFO control register		*/
	u32 s7cr;	/* 0xB8 stream 7 configuration register		*/
	u32 s7ndtr;	/* 0xBC stream 7 number of data register	*/
	u32 s7par;	/* 0xC0 stream 7 peripheral address register	*/
	u32 s7m0ar;	/* 0xC4 stream 7 memory 0 address register	*/
	u32 s7m1ar;	/* 0xC8 stream 7 memory 1 address register	*/
	u32 s7fcr;	/* 0xCC stream 7 FIFO control register		*/
};

#define STM32_DMA1		((struct stm32_dma_reg *)DMA1_BASE)
#define STM32_DMA2		((struct stm32_dma_reg *)DMA2_BASE)

/* defines the data direction */
enum direction{
	DMA_MEMORY_TO_PERIPH,
	DMA_PERIPH_TO_MEMORY,
};

/* dma config struct */
struct dma_config{
	u32 source_adress;
	u32 target_adress;
	u16 datalength;
	enum direction dir;
};

/* dma select */
enum dma_select{
	DMA1,
	DMA2,
};

/* dma functionality */
enum dma_function{
	SDMMC1,
};

/* initializes the dma */
int initialize_dma(enum dma_select dma);

/* configures different dma_modes */
int configure_dma(enum dma_function dma_function ,struct dma_config config);

#endif
