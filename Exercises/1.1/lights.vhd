LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
USE ieee.std_logic_unsigned.all;
ENTITY lights IS
PORT (
SW : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
KEY : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
CLOCK_50 : IN STD_LOGIC;
LEDG : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
DRAM_CLK, DRAM_CKE : OUT STD_LOGIC;
DRAM_ADDR : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
DRAM_BA_0, DRAM_BA_1 : BUFFER STD_LOGIC;
DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT STD_LOGIC;
DRAM_DQ : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
DRAM_UDQM, DRAM_LDQM : BUFFER STD_LOGIC;
LCD_DATA : inout STD_LOGIC_VECTOR(7 downto 0);
LCD_ON, LCD_BLON, LCD_EN, LCD_RS, LCD_RW : out STD_LOGIC;
IO_acknowledge : in std_logic := 'X'; 
IO_irq : in std_logic := 'X'; 
IO_address : out std_logic_vector(15 downto 0); 
IO_bus_enable : out std_logic; 
IO_byte_enable : out std_logic_vector(1 downto 0); 
IO_rw : out std_logic; 
IO_write_data : out std_logic_vector(15 downto 0); 
IO_read_data : in std_logic_vector(15 downto 0) := (others => 'X')
);
END lights;
ARCHITECTURE Structure OF lights IS
COMPONENT nios_system
PORT (
clk_clk : IN STD_LOGIC;
reset_reset_n : IN STD_LOGIC;
sdram_clk_clk : OUT STD_LOGIC;
leds_export : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
switches_export : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
keys_export : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
sdram_wire_addr : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
sdram_wire_ba : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
sdram_wire_cas_n : OUT STD_LOGIC;
sdram_wire_cke : OUT STD_LOGIC;
sdram_wire_cs_n : OUT STD_LOGIC;
sdram_wire_dq : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
sdram_wire_dqm : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
sdram_wire_ras_n : OUT STD_LOGIC;
sdram_wire_we_n : OUT STD_LOGIC;
lcd_data_DATA : inout std_LOGIC_VECTOR(7 downto 0);
lcd_data_ON : out STD_LOGIC;
lcd_data_BLON : out STD_LOGIC;
lcd_data_EN : out STD_LOGIC;
lcd_data_RS : out STD_LOGIC;
lcd_data_RW : out STD_LOGIC;
io_acknowledge : in std_logic := 'X'; 
io_irq : in std_logic := 'X'; 
io_address : out std_logic_vector(15 downto 0); 
io_bus_enable : out std_logic; 
io_byte_enable : out std_logic_vector(1 downto 0); 
io_rw : out std_logic; 
io_write_data : out std_logic_vector(15 downto 0); 
io_read_data : in std_logic_vector(15 downto 0) := (others => 'X'));
END COMPONENT;
SIGNAL DQM : STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL BA : STD_LOGIC_VECTOR(1 DOWNTO 0);
BEGIN
DRAM_BA_0 <= BA(0);
DRAM_BA_1 <= BA(1);
DRAM_UDQM <= DQM(1);
DRAM_LDQM <= DQM(0);
NiosII: nios_system
PORT MAP (
clk_clk => CLOCK_50,
keys_export => KEY,
reset_reset_n => '1',
sdram_clk_clk => DRAM_CLK,
leds_export => LEDG,
switches_export => SW,
sdram_wire_addr => DRAM_ADDR,
sdram_wire_ba => BA,
sdram_wire_cas_n => DRAM_CAS_N,
sdram_wire_cke => DRAM_CKE,
sdram_wire_cs_n => DRAM_CS_N,
sdram_wire_dq => DRAM_DQ,
sdram_wire_dqm => DQM,
sdram_wire_ras_n => DRAM_RAS_N,
sdram_wire_we_n => DRAM_WE_N,
lcd_data_DATA => LCD_DATA,
lcd_data_ON => LCD_ON,
lcd_data_EN => LCD_EN,
lcd_data_RS => LCD_RS,
lcd_data_RW => LCD_RW,
lcd_data_BLON => LCD_BLON,
io_acknowledge => IO_acknowledge,
io_irq => IO_irq,
io_address => IO_address,
io_bus_enable => IO_bus_enable,
io_byte_enable => IO_byte_enable,
io_rw => IO_rw,
io_write_data => IO_write_data,
io_read_data => IO_read_data);
END Structure;
