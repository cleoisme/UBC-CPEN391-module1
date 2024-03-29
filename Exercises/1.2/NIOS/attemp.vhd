LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
USE ieee.std_logic_unsigned.all;

ENTITY attemp IS
	PORT (
		--SW : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		KEY : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
		CLOCK_50 : IN STD_LOGIC;
		--LEDG : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		DRAM_CLK, DRAM_CKE : OUT STD_LOGIC;
		DRAM_ADDR : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
		DRAM_BA_0, DRAM_BA_1 : BUFFER STD_LOGIC;
		DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT STD_LOGIC;
		DRAM_DQ : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		DRAM_UDQM, DRAM_LDQM : BUFFER STD_LOGIC );
END attemp;

ARCHITECTURE Structure OF attemp IS
COMPONENT nios_e_system
	PORT (
		clk_clk : IN STD_LOGIC;
		reset_reset_n : IN STD_LOGIC;
		sdram_clk_clk : OUT STD_LOGIC;
		--leds_export : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		--switches_export : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		sdram_wire_addr : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
		sdram_wire_ba : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
		sdram_wire_cas_n : OUT STD_LOGIC;
		sdram_wire_cke : OUT STD_LOGIC;
		sdram_wire_cs_n : OUT STD_LOGIC;
		sdram_wire_dq : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		sdram_wire_dqm : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
		sdram_wire_ras_n : OUT STD_LOGIC;
		sdram_wire_we_n : OUT STD_LOGIC );
END COMPONENT;

SIGNAL DQM : STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL BA : STD_LOGIC_VECTOR(1 DOWNTO 0);

BEGIN
DRAM_BA_0 <= BA(0);
DRAM_BA_1 <= BA(1);
DRAM_UDQM <= DQM(1);
DRAM_LDQM <= DQM(0);

NiosII: nios_e_system
PORT MAP (
	clk_clk => CLOCK_50,
	reset_reset_n => KEY(0),
	sdram_clk_clk => DRAM_CLK,
	--leds_export => LEDG,
	--switches_export => SW,
	sdram_wire_addr => DRAM_ADDR,
	sdram_wire_ba => BA,
	sdram_wire_cas_n => DRAM_CAS_N,
	sdram_wire_cke => DRAM_CKE,
	sdram_wire_cs_n => DRAM_CS_N,
	sdram_wire_dq => DRAM_DQ,
	sdram_wire_dqm => DQM,
	sdram_wire_ras_n => DRAM_RAS_N,
	sdram_wire_we_n => DRAM_WE_N );
END Structure;