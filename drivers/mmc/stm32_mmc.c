/*
 * stm32_mmc.c -- STM32 SDHCI driver
 *
 * (C) Copyright 2016, Philipp Rossak <embed3d@gmail.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#define SDMMC_DMA

#include <common.h>
#include <dm.h>
#include <linux/err.h>
#include <fdtdec.h>
#include <mmc.h>
#include <dwmmc.h>
#include <libfdt.h>
#include "stm32_mmc.h"
#include <asm/arch/stm32.h>
#include <asm/arch/gpio.h>
#include <asm/arch/rcc.h>
#ifdef SDMMC_DMA
#include <asm/arch/dma.h>
#endif
DECLARE_GLOBAL_DATA_PTR;

static u32 stm32_block_config(uint blocksize) {

	u32 ret = 0;

	switch (blocksize) {
	case 1:
		ret = STM32_SDIO_DBLOCKSIZE_1b;
		break;
	case 2:
		ret = STM32_SDIO_DBLOCKSIZE_2b;
		break;
	case 4:
		ret = STM32_SDIO_DBLOCKSIZE_4b;
		break;
	case 8:
		ret = STM32_SDIO_DBLOCKSIZE_8b;
		break;
	case 16:
		ret = STM32_SDIO_DBLOCKSIZE_16b;
		break;
	case 32:
		ret = STM32_SDIO_DBLOCKSIZE_32b;
		break;
	case 64:
		ret = STM32_SDIO_DBLOCKSIZE_64b;
		break;
	case 128:
		ret = STM32_SDIO_DBLOCKSIZE_128b;
		break;
	case 256:
		ret = STM32_SDIO_DBLOCKSIZE_256b;
		break;
	case 512:
		ret = STM32_SDIO_DBLOCKSIZE_512b;
		break;
	case 1024:
		ret = STM32_SDIO_DBLOCKSIZE_1024b;
		break;
	case 2048:
		ret = STM32_SDIO_DBLOCKSIZE_2048b;
		break;
	case 4096:
		ret = STM32_SDIO_DBLOCKSIZE_4096b;
		break;
	case 8192:
		ret = STM32_SDIO_DBLOCKSIZE_8192b;
		break;
	case 16384:
		ret = STM32_SDIO_DBLOCKSIZE_16384b;
		break;
	}
	return ret;
}

static int mmc_trans_data(struct udevice *dev, struct mmc_data *data, ushort cmx)
{
	struct stm32_mmc_dev *priv = dev_get_priv(dev);
	struct stm32_mmc_reg *regs = priv->reg;

	int ret = 0;
	u32 error_flags;
	const int reading = (data->flags & MMC_DATA_READ);
	debug("START DATA TRANSFER\n");
	debug("CMD: %d\n",cmx);

#ifdef SDMMC_DMA
	int timeout = 10;
	if(reading){
		if (cmx == MMC_CMD_READ_MULTIPLE_BLOCK)
			debug("read mulit block \n");
		if (cmx == MMC_CMD_READ_SINGLE_BLOCK)
			debug("read single block \n");
		unsigned timeout_usecs = ((data->blocksize * data->blocks) >> 8) * 1000;

		error_flags = STM32_SDIO_RXOVERR | STM32_SDIO_DTIMEOUT | \
				  STM32_SDIO_CTIMEOUT |STM32_SDIO_DCRCFAIL | \
				  STM32_SDIO_DTIMEOUT;

		while(!(readl(&regs->sta) & STM32_SDIO_CMDREND)){
			if(readl(&regs->sta) & error_flags){
				debug("Error Flags STA1: %08x------------------\n",readl(&regs->sta));
				ret = -12;
				goto out;
			}
			if(!timeout--){
				ret = -110;
				goto out;
			}
			udelay(1);
		}
		while(!(readl(&regs->sta) & STM32_SDIO_DBCKEND)){
			if(readl(&regs->sta) & error_flags){
				debug("Error Flags STA2: %08x------------------\n",readl(&regs->sta));
				ret = -12;
				goto out;
			}
			if(timeout_usecs --){
				debug("Timeout STA2: %08x------------------\n",readl(&regs->sta));
				ret = -ETIMEDOUT;
				goto out;
			}

			udelay(1);
		}

		while(!(readl(&regs->sta) & STM32_SDIO_DATAEND)){
			debug("STA3: %08x------------------\n",readl(&regs->sta));
			timeout++;
			if(timeout > 10){
				ret = -111;
				goto out;
			}
			udelay(1);
		}
		if(readl(&regs->sta) & STM32_SDIO_RXOVERR){
			ret = -1;
			goto out;
		}
	}
	for(int i = 0; i < ((data->blocksize * data->blocks) >> 2); i++){
		debug("data[%d] from card: %08x\n",i,data->dest[i]);
	}
#else
	u32 status_bit;
	if(cmx == MMC_CMD_READ_MULTIPLE_BLOCK){
		if (reading){
			error_flags = STM32_SDIO_RXOVERR | STM32_SDIO_DCRCFAIL | \
					STM32_SDIO_DTIMEOUT | STM32_SDIO_DATAEND ;
			status_bit = STM32_SDIO_RXFIFOHF;
		} else {
			status_bit = STM32_SDIO_TXDAVL;
		}
	} else if (cmx == MMC_CMD_READ_SINGLE_BLOCK){
		if (reading){
			error_flags = STM32_SDIO_RXOVERR | STM32_SDIO_DCRCFAIL | \
				STM32_SDIO_DTIMEOUT | STM32_SDIO_DBCKEND ;
			//status_bit = STM32_SDIO_RXDAVL;
			status_bit = STM32_SDIO_RXFIFOHF  ;
		} else {
			status_bit = STM32_SDIO_TXDAVL;
		}
	} else {
		if (reading){
			error_flags = STM32_SDIO_RXOVERR | STM32_SDIO_DCRCFAIL | \
				STM32_SDIO_DTIMEOUT | STM32_SDIO_DBCKEND ;
			status_bit = STM32_SDIO_RXDAVL ;
		} else {
			status_bit = STM32_SDIO_TXDAVL;
			error_flags = STM32_SDIO_RXOVERR | STM32_SDIO_DCRCFAIL | \
				STM32_SDIO_DTIMEOUT | STM32_SDIO_DBCKEND ;
		}
	}


	unsigned i;
	unsigned *buff = (unsigned int *)(reading ? data->dest : data->src);
	//	u32 data_buffer;
	unsigned byte_cnt = data->blocksize * data->blocks;
	unsigned timeout_usecs = (byte_cnt >> 8) * 1000;
	//	unsigned timeout_usecs = 20;
	//	unsigned timeout = 0;
	debug("Timeout in usecs: %d\n",timeout_usecs);
	debug("Planned Loops: %d\n",byte_cnt >> 2);
	debug("Amount of Blocks: %d\n",data->blocks);
	debug("Amount of Bytes: %d\n",byte_cnt);


	for (i = 0; i < (byte_cnt >> 2); i++) {
		//debug("Data Loop %d\n", i);
		//debug("STA: %08x------------------\n",readl(&regs->sta));
		while ((readl(&regs->sta) & status_bit) == 0) {
			if (!timeout_usecs-- ){
				ret = -ETIMEDOUT;
				debug("TIMEOUT: ------------------\n");
				goto out;
			} else if ((readl(&regs->sta) & error_flags)) {
				ret = -111;
				debug("ERROR STA: %x------------------\n",readl(&regs->sta));
				goto out;
			}

			udelay(1);
			debug("Wait valid status bit transfer \n");
			debug("STA: %08x------------------\n",readl(&regs->sta));
		}
		if (readl(&regs->sta) & error_flags) {
			debug("ERROR STA: %x------------------\n",readl(&regs->sta));
			ret = -113;
			goto out;
		}
		//debug("Data Transfer now\n");
		if (reading) {
			buff[i] = readl(&regs->fifo[0]);
			//debug("Read data %08x\n",buff[i]);
			//debug("Read done\n");
		}
	}
#endif
out:
	debug("END DATA TRANSFER\n");
	debug("Ret: %d  -  STA  %08x\n\n", ret, readl(&regs->sta));
	return ret;
}

static int mmc_wait(struct udevice *dev, struct mmc_cmd *cmd, u8 response)
{
	struct stm32_mmc_dev *priv = dev_get_priv(dev);
	struct stm32_mmc_reg *regs = priv->reg;
	u32 timeout = 10000;
	u32 status_flags;
	u32 error_flags;
	int ret = 0;

	if(response == MMC_RSP_NONE){
		status_flags = STM32_SDIO_CMDSENT;
		error_flags = STM32_SDIO_CTIMEOUT;
	}
	else{
		status_flags = (STM32_SDIO_CMDREND | STM32_SDIO_RXFIFOHF );
		error_flags = (STM32_SDIO_CCRCFAIL | STM32_SDIO_CTIMEOUT);
	}

	u32 status = readl(&regs->sta);

	while (timeout > 0) {
		timeout--;
		status = readl(&regs->sta);
		if (status & status_flags)
			goto out;
		if (status & error_flags)
			goto out;
		//debug("Wait Status ok: %08x\n",status_flags);
		//debug("Wait Status error: %08x \n",error_flags);
		debug("Status Register %08x\n", status);
		udelay(1);
	}
out:


	if ((status & STM32_SDIO_CTIMEOUT))
		ret = -ETIMEDOUT;
	if(status & STM32_SDIO_DCRCFAIL)
	       ret = -111;
	if(status & STM32_SDIO_DTIMEOUT)
	       ret = -ETIMEDOUT;


	// disabled because of crc command problem cmd->resp_type & MMC_RSP_CRC
	if ((status & STM32_SDIO_CCRCFAIL) != 0 && (cmd->resp_type & MMC_RSP_CRC) != 0)
		debug("CRC_ERROR ---------------------------------------\n");

//out:
	debug("Status Register %08x\n", status);
//	debug("Resp_CMD %x \n",readl(&regs->respcmd));
//	debug("Resp_R1 %x \n",readl(&regs->resp1));
	debug("Ret status %d\n",ret);
	return ret;
}



/**
 * send_cmd() - Send a command to the MMC device
 *
 * @dev:	Device to receive the command
 * @cmd:	Command to send
 * @data:	Additional data to send/receive
 * @return 0 if OK, -ve on error
 */
static int stm32_mmc_send_cmd(struct udevice *dev, struct mmc_cmd *cmd,
				struct mmc_data *data)
{
	struct stm32_mmc_dev *priv = dev_get_priv(dev);
	struct stm32_mmc_reg *regs = priv->reg;

	ushort command = cmd->cmdidx;
	int error = 0;

	u32 cmdval = readl(&regs->cmd);

	/* clear CMD */
	cmdval &= STM32_SDIO_CMD_CLEAR_MASK;
	cmdval = STM32_SDIO_CPSMEN;

	if (cmd->resp_type & MMC_RSP_BUSY)
		debug("mmc cmd %d check rsp busy\n", cmd->cmdidx);
	if (cmd->cmdidx == 12)
		return 0;

	else if (cmd->resp_type & MMC_RSP_136){
		cmdval |= STM32_SDIO_WAITRESP_LONG;
		debug("STM32_SDIO_WAITRESP_LONG\n");
	}
	else if (cmd->resp_type == MMC_RSP_NONE){
		cmdval |= STM32_SDIO_WAITRESP_NO;
		debug("STM32_SDIO_WAITRESP_NO\n");
	}
	else{
		cmdval |= STM32_SDIO_WAITRESP_SHORT;
		debug("STM32_SDIO_WAITRESP_SHORT\n");
	}


	if (data) {
		if ((u32)(long)data->dest & 0x3) {
			error = -1;
			goto out;
		}

		if (MMC_DATA_READ & data->flags)
			debug("Read data\n");

		if (MMC_DATA_WRITE & data->flags)
			debug("Write data\n");

#ifdef SDMMC_DMA
		/*config dma*/
		struct dma_config  dma ;
		if (MMC_DATA_READ & data->flags){
			dma.source_adress = &regs->fifo;
			dma.target_adress= &data->dest[0];
			dma.dir = DMA_PERIPH_TO_MEMORY;
		}else{
			dma.source_adress = &regs->fifo;
			//dma->target_adress
			dma.dir = DMA_MEMORY_TO_PERIPH;
		}
		/*first data length*/
		dma.datalength = ((data->blocks * data->blocksize) );

#endif


		debug("data block size %d \n",data->blocksize);

		/* Write Maximum available timeout */
		writel(STM32SD_DATATIMEOUT, &regs->dtimer);

		/* Write Data length Length = Blocks * Blocksize*/
		writel(data->blocks * data->blocksize, &regs->dlen);
#ifdef SDMMC_DMA
		/*configure dma*/
		configure_dma(SDMMC1,dma);
#endif
		/* Write Block size */
		u32 val = readl(&regs->dctrl);
		val &= STM32_SDIO_BLOCKSIZE_CLEAR_MASK;
		val |= stm32_block_config(data->blocksize);
		writel(val, &regs->dctrl);

		/* Transfer Direction */
		val = readl(&regs->dctrl);
		val &= STM32_SDIO_TRANSDIR_CLEAR_MASK;
		if (MMC_DATA_READ & data->flags)
			val |= STM32_SDIO_DTDIR;
		writel(val, &regs->dctrl);

		/* Transfer Mode */
		val = readl(&regs->dctrl);
		val &= STM32_SDIO_TRANSMODE_CLEAR_MASK;
		//val |= STM32_SDIO_DTMODE; /* SDIO Mode*/
		writel(val, &regs->dctrl);
#ifdef SDMMC_DMA
		/* ENable DMA mode */
		setbits_le32(&regs->dctrl,STM32_SDIO_DMAEN);
#endif
		/* ENABLE Data transfer*/
		setbits_le32(&regs->dctrl, STM32_SDIO_DTEN);

	}

	debug("mmc 0, cmd %d(0x%08x), arg 0x%08x\n", cmd->cmdidx, cmdval | \
			cmd->cmdidx, cmd->cmdarg);

	/* write command argument*/
	writel(cmd->cmdarg, &regs->arg);

	/*send command*/
	writel(cmdval | cmd->cmdidx, &regs->cmd);

	if (data) {

		error = mmc_trans_data(dev, data,command);
		if (error) {
			goto out;
		}
	}
	if(!data)
		error = mmc_wait(dev,cmd,cmd->resp_type);
	if (error)
		goto out;

	if (cmd->resp_type & MMC_RSP_BUSY) {

		debug("RSP Busy\n");
	}

	if (cmd->resp_type & MMC_RSP_136) {

		cmd->response[3] = readl(&regs->resp4);
		cmd->response[2] = readl(&regs->resp3);
		cmd->response[1] = readl(&regs->resp2);
		cmd->response[0] = readl(&regs->resp1);
		debug("mmc resp 0x%08x 0x%08x 0x%08x 0x%08x\n",
		      cmd->response[0], cmd->response[1],
		      cmd->response[2], cmd->response[3]);
	} else {
		cmd->response[0] = readl(&regs->resp1);
		debug("mmc resp 0x%08x\n", cmd->response[0]);
	}
	debug("\n");

out:
	/* Clear all the static flags */
	writel(0x000005FF, &regs->icr);
	return error;
}

/**
 * set_ios() - Set the I/O speed/width for an MMC device
 *
 * @dev:	Device to update
 * @return 0 if OK, -ve on error
 */
static int stm32_mmc_set_ios(struct udevice *dev)
{
	struct mmc_pdata *pdata = dev_get_platdata(dev);
	struct stm32_mmc_dev *priv = dev_get_priv(dev);
	struct stm32_mmc_reg *regs = priv->reg;
	debug("set ios: bus_width: %x, clock: %d\n",
			pdata->mmc.bus_width, pdata->mmc.clock);

	/* read out register */
	u32 val = readl(&regs->clkcr);

	/* clear all except the enable bit */
	val &= STM32_SDIO_CLKREG_SETTING_CLEARMASK;

	/* Enable HW FLOW control*/
	val |= STM32_SDIO_HWFC_EN;

	/* set buswidth */
	if(pdata->mmc.bus_width == 1){
		val |= STM32_SDIO_WIDBUS_1b;
	} else if (pdata->mmc.bus_width == 4){
		val |= STM32_SDIO_WIDBUS_4b;
	} else
		val |= STM32_SDIO_WIDBUS_8b;

	/* set clock devision */
	if(pdata->mmc.clock == 25000000)
		val |= 0;
	if(pdata->mmc.clock == 400000)
		val |= 123;

	debug("Set clk div: %d\n",val & 0xFF);

	writel(val,&regs->clkcr);

	debug("Set ios done\n");
	return 0;
}


static int stm32_mmc_get_cd(struct udevice *dev)
{
	unsigned gpio_cd = 45;
	/* read form gpio pin */
	int ret = gpio_get_value(gpio_cd);

	/*correct pinoffset and invert if no error */
	if(ret >= 0)
		return ! ( ret>>  (gpio_cd % 16));
	else
		return ret;
}

static int stm32_mmc_get_wp(struct udevice *dev)
{
	/* always write-enabled*/
	return 0;
}

static const struct dm_mmc_ops stm32_mmc_ops = {
	.send_cmd	= stm32_mmc_send_cmd,
	.set_ios 	= stm32_mmc_set_ios,
	.get_cd 	= stm32_mmc_get_cd,
	.get_wp		= stm32_mmc_get_wp,
};


static int stm32_mmc_probe(struct udevice *dev) {

	debug ("Probing STM32 MMC driver\n");
	struct mmc_pdata *pdata = dev_get_platdata(dev);
	struct stm32_mmc_dev *priv = dev_get_priv(dev);

	priv->reg = (struct stm32_mmc_reg *)pdata->iobase;

	struct stm32_mmc_reg *regs = priv->reg;

	int ret;

	/* reset SDIO to default */
	writel(0x0, &regs->power);
	writel(0x0, &regs->clkcr);
	writel(0x0, &regs->arg);
	writel(0x0, &regs->cmd);
	writel(0x0, &regs->dtimer);
	writel(0x0, &regs->dlen);
	writel(0x0, &regs->dctrl);
	writel(0x00C007FF, &regs->icr);
	writel(0x0, &regs->mask);

	/* SDMMCLK = 50MHZ, SDIO_CK = SDMMCLK/(2 + SDIO_TRANSFER_CLK_DIV) */
	/* 400kHz, 1b width, byepass disabled, */

#ifdef SDMMC_DMA
	/* DMA config */
	ret = initialize_dma(DMA2);
	if (ret != 0)
		goto out;
#endif

	/* power on */
	writel(STM32_SDIO_PWRCTRL_P_ON, &regs->power);

	/* 1ms: required power up waiting time before starting the SD initialization
	     sequence */
	udelay(1000);

	/* enable clock */
	setbits_le32(&regs->clkcr,STM32_SDIO_CLKEN);

	ret = mmc_init(&pdata->mmc);

out:
	debug("Probe Status: %d\n", ret);
	return ret;
}

/**
 * mmc_bind() - Set up a new MMC device ready for probing
 *
 * A child block device is bound with the IF_TYPE_MMC interface type. This
 * allows the device to be used with CONFIG_BLK
 *
 * @dev:	MMC device to set up
 * @mmc:	MMC struct
 * @cfg:	MMC configuration
 * @return 0 if OK, -ve on error
 */
int stm32_mmc_bind(struct udevice *dev)
{
	struct mmc_pdata *pdata = dev_get_platdata(dev);
	struct mmc_config *cfg = &pdata->cfg;
	int ret;

	cfg->name = "STM32 SD/MMC";/*dev->name;*/
	cfg->host_caps |=  MMC_MODE_HS ;
	cfg->voltages =  MMC_VDD_32_33 | MMC_VDD_33_34 |   MMC_VDD_31_32 | MMC_VDD_35_36;
	cfg->b_max = U32_MAX;

	ret = mmc_bind(dev, &pdata->mmc, cfg);
	if (ret)
		return ret;

	return 0;
}

/**
 * mmc_unbind() - Unbind a MMC device's child block device
 *
 * @dev:	MMC device
 * @return 0 if OK, -ve on error
 */
int stm32_mmc_unbind(struct udevice *dev)
{
	mmc_unbind(dev);

	return 0;
}

static int stm32_mmc_ofdata_to_platdata(struct udevice *dev)
{
	struct mmc_pdata *pdata = dev_get_platdata(dev);
	pdata->iobase = dev_get_addr(dev);
	struct mmc_config *cfg = &pdata->cfg;

	const fdt32_t *cell;

	u32 bw = 0;

	cell = fdt_getprop(gd->fdt_blob, dev->of_offset,"bus-width",NULL);
	if (cell)
		bw=fdt32_to_cpu(*cell);

	debug("Bus width: %d\n",bw);

	if(bw == 4)
		cfg->host_caps = MMC_MODE_4BIT;
	else if(bw == 8)
		cfg->host_caps = MMC_MODE_8BIT;
	else {
		debug("%s: Invalid BUS width '%p'\n", __func__, cell);
		return -EINVAL;
	}

	cell = fdt_getprop(gd->fdt_blob, dev->of_offset,"mmc-min-frequency",NULL);
	if (cell)
		cfg->f_min = fdt32_to_cpu(*cell);
	else {
		debug("%s: No Minimal Frequency given '%p'\n", __func__, cell);
		return -EINVAL;
	}

	cell = fdt_getprop(gd->fdt_blob, dev->of_offset,"mmc-max-frequency",NULL);
	if (cell)
		cfg->f_max = fdt32_to_cpu(*cell);
	else {
		debug("%s: No Maximal Frequency given '%p'\n", __func__, cell);
		return -EINVAL;
	}

	return 0;

}

static const struct udevice_id stm32_mmc_ids[] = {
	{ .compatible = "st,mmc" },
	{ }
};

U_BOOT_DRIVER(stm32_mmc) = {
	.name		= "stm32_mmc",
	.id		= UCLASS_MMC,
	.of_match	= stm32_mmc_ids,
	.ofdata_to_platdata = stm32_mmc_ofdata_to_platdata,
	.ops		= &stm32_mmc_ops,
	.bind		= stm32_mmc_bind,
	.unbind		= stm32_mmc_unbind,
	.probe		= stm32_mmc_probe,
	.platdata_auto_alloc_size = sizeof(struct mmc_pdata),
	.priv_auto_alloc_size = sizeof(struct stm32_mmc_dev),
};
