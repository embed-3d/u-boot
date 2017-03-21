/*
 * stm32_mmc.h -- STM32 SDHCI driver
 *
 * (C) Copyright 2016, Philipp Rossak <embed3d@gmail.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _STM32_SDHCI_H_
#define _STM32_SDHCI_H_

/*
 * SDMMC power control register
 */
#define STM32_SDIO_PWRCTRL_OFF		(0 << 0)
#define STM32_SDIO_PWRCTRL_RESV		(1 << 0)
#define STM32_SDIO_PWRCTRL_RESV_PU	(2 << 0)
#define STM32_SDIO_PWRCTRL_P_ON		(3 << 0)

/*
 * SDMMC clock control register
 */
#define STM32_SDIO_CLKEN	(1 << 8)
#define STM32_SDIO_PWRSAV	(1 << 9)
#define STM32_SDIO_BYPASS	(1 << 10)
#define STM32_SDIO_NEGEDGE	(1 << 13)
#define STM32_SDIO_HWFC_EN	(1 << 14)

#define STM32_SDIO_WIDBUS_1b	(0 << 11)
#define STM32_SDIO_WIDBUS_4b	(1 << 11)
#define STM32_SDIO_WIDBUS_8b	(2 << 11)

/*
 * SDMMC command register
 */
#define STM32_SDIO_WAITINT	(1 << 8)
#define STM32_SDIO_WAITPEND	(1 << 9)
#define STM32_SDIO_CPSMEN	(1 << 10)
#define STM32_SDIO_SDIOSuspend	(1 << 11)

#define STM32_SDIO_WAITRESP_NO		(2 << 6)
#define STM32_SDIO_WAITRESP_SHORT	(1 << 6)
#define STM32_SDIO_WAITRESP_LONG	(3 << 6)

/*
 * SDMMC data control register
 * control bits
 */
#define STM32_SDIO_DTEN		(1 << 0)
#define STM32_SDIO_DTDIR	(1 << 1)
#define STM32_SDIO_DTMODE	(1 << 2)
#define STM32_SDIO_DMAEN	(1 << 3)
#define STM32_SDIO_RWSTART	(1 << 8)
#define STM32_SDIO_RWSTOP	(1 << 9)
#define STM32_SDIO_RWMOD	(1 << 10)
#define STM32_SDIO_SDIOEN	(1 << 11)

/* blocksize bits */
#define STM32_SDIO_DBLOCKSIZE_1b	(0 << 4)
#define STM32_SDIO_DBLOCKSIZE_2b	(1 << 4)
#define STM32_SDIO_DBLOCKSIZE_4b	(2 << 4)
#define STM32_SDIO_DBLOCKSIZE_8b	(3 << 4)
#define STM32_SDIO_DBLOCKSIZE_16b	(4 << 4)
#define STM32_SDIO_DBLOCKSIZE_32b	(5 << 4)
#define STM32_SDIO_DBLOCKSIZE_64b	(6 << 4)
#define STM32_SDIO_DBLOCKSIZE_128b	(7 << 4)
#define STM32_SDIO_DBLOCKSIZE_256b	(8 << 4)
#define STM32_SDIO_DBLOCKSIZE_512b	(9 << 4)
#define STM32_SDIO_DBLOCKSIZE_1024b	(10 << 4)
#define STM32_SDIO_DBLOCKSIZE_2048b	(11 << 4)
#define STM32_SDIO_DBLOCKSIZE_4096b	(12 << 4)
#define STM32_SDIO_DBLOCKSIZE_8192b	(13 << 4)
#define STM32_SDIO_DBLOCKSIZE_16384b	(14 << 4)

/*
 * SDMMC status register
 */
#define STM32_SDIO_CCRCFAIL	(1 << 0)  /* Command response received (CRC check failed) */
#define STM32_SDIO_DCRCFAIL	(1 << 1)  /* Data block sent/received (CRC check failed) */
#define STM32_SDIO_CTIMEOUT	(1 << 2)  /* Command response timeout */
#define STM32_SDIO_DTIMEOUT	(1 << 3)  /* Data timeout */
#define STM32_SDIO_TXUNDERR	(1 << 4)  /* Transmit FIFO underrun error */
#define STM32_SDIO_RXOVERR	(1 << 5)  /* Received FIFO overrun error */
#define STM32_SDIO_CMDREND	(1 << 6)  /* Command response received (CRC check passed) */
#define STM32_SDIO_CMDSENT	(1 << 7)  /* Command sent (no response required) */
#define STM32_SDIO_DATAEND	(1 << 8)  /* Data end (data counter, SDIDCOUNT, is zero) */
#define STM32_SDIO_DBCKEND	(1 << 10) /* Data block sent/received (CRC check passed) */
#define STM32_SDIO_CMDACT	(1 << 11) /* Command transfer in progress */
#define STM32_SDIO_TXACT	(1 << 12) /* Data transmit in progress */
#define STM32_SDIO_RXACT	(1 << 13) /* Data receive in progress */
#define STM32_SDIO_TXFIFOHE	(1 << 14) /* Transmit FIFO half empty: at least 8 words can be written into the FIFO */
#define STM32_SDIO_RXFIFOHF	(1 << 15) /* Receive FIFO half full: there are at least 8 words in the FIFO */
#define STM32_SDIO_TXFIFOF	(1 << 16) /* Transmit FIFO full */
#define STM32_SDIO_RXFIFOF	(1 << 17) /* Receive FIFO full */
#define STM32_SDIO_TXFIFOE	(1 << 18) /* Transmit FIFO empty */
#define STM32_SDIO_RXFIFOE	(1 << 19) /* Receive FIFO empty */
#define STM32_SDIO_TXDAVL	(1 << 20) /* Data available in transmit FIFO */
#define STM32_SDIO_RXDAVL	(1 << 21) /* Data available in receive FIFO */
#define STM32_SDIO_SDIOIT	(1 << 22) /* SDIO interrupt received */

/*
 * SDMMC interrupt clear register
 */
#define STM32_SDIO_CCRCFAILC	(1 << 0)  /* CCRCFAIL flag clear bit */
#define STM32_SDIO_DCRCFAILC	(1 << 1)  /* DCRCFAIL flag clear bit */
#define STM32_SDIO_CTIMEOUTC	(1 << 2)  /* CTIMEOUT flag clear bit */
#define STM32_SDIO_DTIMEOUTC	(1 << 3)  /* DTIMEOUT flag clear bit */
#define STM32_SDIO_TXUNDERRC	(1 << 4)  /* TXUNDERR flag clear bit */
#define STM32_SDIO_RXOVERRC	(1 << 5)  /* RXOVERR flag clear bit */
#define STM32_SDIO_CMDRENDC	(1 << 6)  /* CMDREND flag clear bit */
#define STM32_SDIO_CMDSENTC	(1 << 7)  /* CMDSENT flag clear bit */
#define STM32_SDIO_DATAENDC	(1 << 8)  /* DATAEND flag clear bit */
#define STM32_SDIO_DBCKENDC	(1 << 10) /* DBCKEND flag clear bit */
#define STM32_SDIO_SDIOITC	(1 << 22) /* SDIOIT flag clear bit */
/*
 * SDMMC mask register
 */
#define STM32_SDIO_CCRCFAILIE 	(1 << 0)  /* Command CRC fail interrupt enable */
#define STM32_SDIO_DCRCFAILIE	(1 << 1)  /* Data CRC fail interrupt enable */
#define STM32_SDIO_CTIMEOUTIE	(1 << 2)  /* Command timeout interrupt enable */
#define STM32_SDIO_DTIMEOUTIE	(1 << 3)  /* Data timeout interrupt enable */
#define STM32_SDIO_TXUNDERRIE	(1 << 4)  /* Tx FIFO underrun error interrupt enable */
#define STM32_SDIO_RXOVERRIE	(1 << 5)  /* Rx FIFO overrun error interrupt enable */
#define STM32_SDIO_CMDRENDIE	(1 << 6)  /* Command response received interrupt enable */
#define STM32_SDIO_CMDSENTIE	(1 << 7)  /* Command sent interrupt enable */
#define STM32_SDIO_DATAENDIE	(1 << 8)  /* Data end interrupt enable */
#define STM32_SDIO_DBCKENDIE	(1 << 10) /* Data block end interrupt enable */
#define STM32_SDIO_CMDACTIE	(1 << 11) /* Command acting interrupt enable */
#define STM32_SDIO_TXACTIE	(1 << 12) /* Data transmit acting interrupt enable */
#define STM32_SDIO_RXACTIE	(1 << 13) /* Data receive acting interrupt enable */
#define STM32_SDIO_TXFIFOHEIE	(1 << 14) /* Tx FIFO half empty interrupt enable */
#define STM32_SDIO_RXFIFOHFIE	(1 << 15) /* Rx FIFO half full interrupt enable */
#define STM32_SDIO_TXFIFOFIE	(1 << 16) /* Tx FIFO full interrupt enable */
#define STM32_SDIO_RXFIFOFIE	(1 << 17) /* Rx FIFO full interrupt enable */
#define STM32_SDIO_TXFIFOEIE	(1 << 18) /* Tx FIFO empty interrupt enable */
#define STM32_SDIO_RXFIFOEIE	(1 << 19) /* Rx FIFO empty interrupt enable */
#define STM32_SDIO_TXDAVLIE	(1 << 20) /* Data available in Tx FIFO interrupt enable */
#define STM32_SDIO_RXDAVLIE	(1 << 21) /* Data available in Rx FIFO interrupt enable */
#define STM32_SDIO_SDIOITIE	(1 << 22) /* SDIO mode interrupt received interrupt enable */


/**
  * SDIO Intialization Frequency (400KHz max)
  */
#define STM32_INIT_CLK_DIV                ((u8)0xB2)

/**
  * SDIO Data Transfer Frequency (50MHz max)
  */
#define STM32_TRANSFER_CLK_DIV            ((u8)0x1)

#define STM32_SDIO_CMD_CLEAR_MASK	((u32)0xFFFFF800)

#define STM32_SDIO_BLOCKSIZE_CLEAR_MASK	((u32)0xFFFFFF0F)

#define STM32_SDIO_TRANSDIR_CLEAR_MASK	((u32)0xFFFFFFFD)

#define STM32_SDIO_TRANSMODE_CLEAR_MASK	((u32)0xFFFFFFFFB)

#define STM32SD_DATATIMEOUT		((u32)0xFFFFFFFF)

#define STM32_SDIO_BUSWIDTH_CLEAR_MASK ((u32)0xFFFFE7FF)

#define STM32_SDIO_CLKREG_SETTING_CLEARMASK ((u32)0x00000100)

struct stm32_mmc_reg {
	u32 power; 		/* 0x00 power control register		*/
	u32 clkcr; 		/* 0x04 clock control register		*/
	u32 arg;		/* 0x08 argument register		*/
	u32 cmd;		/* 0x0C command register		*/
	u32 respcmd;		/* 0x10 command response register	*/
	u32 resp1;		/* 0x14 response 1 register		*/
	u32 resp2;		/* 0x18 response 2 register		*/
	u32 resp3;		/* 0x1C response 3 register		*/
	u32 resp4;		/* 0x20 response 4 register		*/
	u32 dtimer;		/* 0x24 data timer register		*/
	u32 dlen;		/* 0x28 data length register		*/
	u32 dctrl;		/* 0x2C data control register		*/
	u32 dcount;		/* 0x30 data counter register		*/
	u32 sta;		/* 0x34 status register		*/
	u32 icr;		/* 0x38 interrupt clear register	*/
	u32 mask;		/* 0x3C mask register			*/
	u32 rsv1[2];
	u32 fifocnt;		/* 0x48 FIFO counter register		*/
	u32 rsv2[13];
	u32 fifo[32];		/* 0x80 data FIFO register		*/
};

struct stm32_mmc_dev {
	unsigned mmc_no;
	uint32_t *mclkreg;
	unsigned fatal_err;
	struct stm32_mmc_reg *reg;
	struct mmc_config cfg;
	s8 gpio_cd;
	s8 gpio_wp;
};

/**
 * struct eth_pdata - Platform data for Ethernet MAC controllers
 *
 * @iobase: The base address of the hardware registers
 * @cfg:
 * @mmc:
 */
struct mmc_pdata {
	phys_addr_t iobase;
	struct mmc_config cfg;
	struct mmc mmc;
};


enum mmc_recive_state {
	SDIO_NO = 0,
	SDIO_R1,
	SDIO_R1b,
	SDIO_R2,
	SDIO_R3
};

#endif
