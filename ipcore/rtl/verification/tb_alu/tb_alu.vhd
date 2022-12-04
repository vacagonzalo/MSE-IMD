LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
USE ieee.math_real.ALL;
USE std.textio.ALL;

ENTITY tb_alu IS
    GENERIC (data_width : INTEGER := 32);
END ENTITY tb_alu;

ARCHITECTURE testbench OF tb_alu IS

    COMPONENT alu IS
        GENERIC (data_width : INTEGER := 32);
        PORT (
            -- Input control signals
            clock_i : IN STD_LOGIC;
            enable_i : IN STD_LOGIC;
            reset_n_i : IN STD_LOGIC;

            -- Output control signals
            ready_for_data_o : OUT STD_LOGIC;

            -- Configuration
            operation_i : IN STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0);

            -- Input data
            operand1_i : IN STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0);
            operand2_i : IN STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0);

            -- Output data
            result_o : OUT STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0);
            carry_o : OUT STD_LOGIC
        );
    END COMPONENT alu;

    SIGNAL tb_clock_is : STD_LOGIC := '0';
    SIGNAL tb_enable_is : STD_LOGIC := '0';
    SIGNAL tb_reset_n_is : STD_LOGIC := '1';

    SIGNAL tb_operation_is : STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0) := (OTHERS => '0');

    SIGNAL tb_operand1_is : STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0) := "11010101010101010101010101010101";
    SIGNAL tb_operand2_is : STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0) := "10001111000011110000111100001111";

    CONSTANT simulation_clocks : INTEGER := 100;
    SIGNAL tb_clock_counter_s : INTEGER := 0;
BEGIN

    dut : alu
    GENERIC MAP(data_width => data_width)
    PORT MAP(
        -- Input control signals
        clock_i => tb_clock_is,
        enable_i => tb_enable_is,
        reset_n_i => tb_reset_n_is,

        -- Output control signals
        ready_for_data_o => OPEN,

        -- Configuration
        operation_i => tb_operation_is,

        -- Input data
        operand1_i => tb_operand1_is,
        operand2_i => tb_operand2_is,

        -- Output data
        result_o => OPEN,
        carry_o => OPEN
    );

    tb_clock_is <= NOT tb_clock_is AFTER 10 ns;

    tb_reset_n_is <= '0' AFTER 25 ns;
    tb_enable_is <= '1' AFTER 30 ns;

    tb_operation_is <=
        x"00000000",
        x"00000001" AFTER 150 ns,
        x"00000002" AFTER 300 ns,
        x"00000003" AFTER 450 ns,
        x"00000004" AFTER 600 ns,
        x"00000005" AFTER 750 ns,
        x"00000006" AFTER 900 ns;

    u_clk_counter : PROCESS (tb_clock_is)
    BEGIN
        IF rising_edge(tb_clock_is) THEN
            tb_clock_counter_s <= tb_clock_counter_s + 1;
        END IF;
    END PROCESS;

    PROCESS (tb_clock_is)
        VARIABLE l : line;
    BEGIN
        IF tb_clock_counter_s >= simulation_clocks THEN
            -- END OF SIMULATION
            write(l, STRING'("                                "));
            writeline(output, l);
            write(l, STRING'("######## SIMULATION END ########"));
            write(l, STRING'("                                "));
            writeline(output, l);
            ASSERT false -- este assert se pone para abortar la simulacion
            REPORT "[INFO] Fin de la simulacion"
                SEVERITY failure;
        END IF;
    END PROCESS;

END ARCHITECTURE testbench;