// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2020 NXP
 */

#include <common.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/imx8ulp-pins.h>
#include <asm/arch/clock.h>
#include <asm/arch/pcc.h>
#include <asm/arch/sys_proto.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_NXP_FSPI) || defined(CONFIG_FSL_FSPI_NAND)
#define FSPI_PAD_CTRL	(PAD_CTL_PUS_UP | PAD_CTL_DSE)
static iomux_cfg_t const flexspi2_pads[] = {
	IMX8ULP_PAD_PTD12__FLEXSPI2_A_SS0_B | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTD13__FLEXSPI2_A_SCLK | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTD17__FLEXSPI2_A_DATA0 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTD16__FLEXSPI2_A_DATA1 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTD15__FLEXSPI2_A_DATA2 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTD14__FLEXSPI2_A_DATA3 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTD22__FLEXSPI2_A_DATA4 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTD21__FLEXSPI2_A_DATA5 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTD20__FLEXSPI2_A_DATA6 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTD19__FLEXSPI2_A_DATA7 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
};

static iomux_cfg_t const flexspi0_pads[] = {
	IMX8ULP_PAD_PTC5__FLEXSPI0_A_SS0_b | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTC6__FLEXSPI0_A_SCLK | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTC10__FLEXSPI0_A_DATA0 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTC9__FLEXSPI0_A_DATA1 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTC8__FLEXSPI0_A_DATA2 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTC7__FLEXSPI0_A_DATA3 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTC4__FLEXSPI0_A_DATA4 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTC3__FLEXSPI0_A_DATA5 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTC2__FLEXSPI0_A_DATA6 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
	IMX8ULP_PAD_PTC1__FLEXSPI0_A_DATA7 | MUX_PAD_CTRL(FSPI_PAD_CTRL),
};

static void setup_flexspi(void)
{
	imx8ulp_iomux_setup_multiple_pads(flexspi2_pads, ARRAY_SIZE(flexspi2_pads));

	init_clk_fspi(0);
}

static void setup_rtd_flexspi0(void)
{
	imx8ulp_iomux_setup_multiple_pads(flexspi0_pads, ARRAY_SIZE(flexspi0_pads));

	/* Set PCC of flexspi0, 192Mhz % 4 = 48Mhz */
	writel(0xD6000003, 0x280300e4);
}

#endif

#if IS_ENABLED(CONFIG_FEC_MXC)
#define ENET_CLK_PAD_CTRL	(PAD_CTL_PUS_UP | PAD_CTL_DSE | PAD_CTL_IBE_ENABLE)
static iomux_cfg_t const enet_clk_pads[] = {
	IMX8ULP_PAD_PTE19__ENET0_REFCLK | MUX_PAD_CTRL(ENET_CLK_PAD_CTRL),
	IMX8ULP_PAD_PTF10__ENET0_1588_CLKIN | MUX_PAD_CTRL(ENET_CLK_PAD_CTRL),
};

static int setup_fec(void)
{
	/*
	 * Since ref clock and timestamp clock are from external,
	 * set the iomux prior the clock enablement
	 */
	imx8ulp_iomux_setup_multiple_pads(enet_clk_pads, ARRAY_SIZE(enet_clk_pads));

	/* Select enet time stamp clock: 001 - External Timestamp Clock */
	cgc1_enet_stamp_sel(1);

	/* enable FEC PCC */
	pcc_clock_enable(4, ENET_PCC4_SLOT, true);
	pcc_reset_peripheral(4, ENET_PCC4_SLOT, false);

	return 0;
}

int board_phy_config(struct phy_device *phydev)
{
	if (phydev->drv->config)
		phydev->drv->config(phydev);
	return 0;
}
#endif

#define I2C_PAD_CTRL	(PAD_CTL_ODE)
static iomux_cfg_t const lpi2c0_pads[] = {
	IMX8ULP_PAD_PTA8__LPI2C0_SCL | MUX_PAD_CTRL(I2C_PAD_CTRL),
	IMX8ULP_PAD_PTA9__LPI2C0_SDA | MUX_PAD_CTRL(I2C_PAD_CTRL),
};

#define TPM_PAD_CTRL	(PAD_CTL_DSE)
static iomux_cfg_t const tpm0_pads[] = {
	IMX8ULP_PAD_PTA3__TPM0_CH2 | MUX_PAD_CTRL(TPM_PAD_CTRL),
};

void mipi_dsi_mux_panel(void)
{
	int ret;
	struct gpio_desc desc;

	/* It is temp solution to directly access i2c, need change to rpmsg later */

	/* enable lpi2c0 clock and iomux */
	imx8ulp_iomux_setup_multiple_pads(lpi2c0_pads, ARRAY_SIZE(lpi2c0_pads));
	writel(0xD2000000, 0x28091060);

	ret = dm_gpio_lookup_name("gpio@20_9", &desc);
	if (ret) {
		printf("%s lookup gpio@20_9 failed ret = %d\n", __func__, ret);
		return;
	}

	ret = dm_gpio_request(&desc, "dsi_mux");
	if (ret) {
		printf("%s request dsi_mux failed ret = %d\n", __func__, ret);
		return;
	}

	dm_gpio_set_dir_flags(&desc, GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
}

void mipi_dsi_panel_backlight(void)
{
	/* It is temp solution to directly access pwm, need change to rpmsg later */
	imx8ulp_iomux_setup_multiple_pads(tpm0_pads, ARRAY_SIZE(tpm0_pads));
	writel(0xD4000001, 0x28091054);

	/* Use center-aligned PWM mode, CPWMS=1, MSnB:MSnA = 10, ELSnB:ELSnA = 00 */
	writel(1000, 0x28095018);
	writel(1000, 0x28095034); /* MOD = CV, full duty */
	writel(0x28, 0x28095010);
	writel(0x20, 0x28095030);
}

int board_init(void)
{
#if defined(CONFIG_NXP_FSPI) || defined(CONFIG_FSL_FSPI_NAND)
	setup_flexspi();

	if (get_boot_mode() == SINGLE_BOOT) {
		setup_rtd_flexspi0();
	}
#endif

	if (IS_ENABLED(CONFIG_FEC_MXC))
		setup_fec();

	if (IS_ENABLED(CONFIG_DM_VIDEO)) {
		mipi_dsi_mux_panel();
		mipi_dsi_panel_backlight();
	}

	return 0;
}

int board_early_init_f(void)
{
	return 0;
}

int board_late_init(void)
{
	env_set("sec_boot", "no");
#ifdef CONFIG_AHAB_BOOT
	env_set("sec_boot", "yes");
#endif

	return 0;
}

#ifdef CONFIG_FSL_FASTBOOT
#ifdef CONFIG_ANDROID_RECOVERY
int is_recovery_key_pressing(void)
{
	return 0; /*TODO*/
}
#endif /*CONFIG_ANDROID_RECOVERY*/
#endif /*CONFIG_FSL_FASTBOOT*/