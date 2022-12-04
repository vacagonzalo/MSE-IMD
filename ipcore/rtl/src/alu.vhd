LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
USE ieee.math_real.ALL;

ENTITY alu IS
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
END ENTITY alu;

ARCHITECTURE rtl OF alu IS

    TYPE operation_t IS (operation_invalid, operation_or, operation_and, operation_add, operation_sub);

    FUNCTION get_operation(a : STD_LOGIC_VECTOR(31 DOWNTO 0))
        RETURN operation_t IS
        VARIABLE result : operation_t;
    BEGIN
        CASE a IS
            WHEN x"00000000" => result := operation_invalid;
            WHEN x"00000001" => result := operation_or;
            WHEN x"00000002" => result := operation_and;
            WHEN x"00000003" => result := operation_add;
            WHEN x"00000004" => result := operation_sub;
            WHEN OTHERS => result := operation_invalid;
        END CASE;
        RETURN result;
    END FUNCTION get_operation;

    FUNCTION addition(
        alpha : STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0);
        beta : STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0))
        RETURN STD_LOGIC_VECTOR IS
        VARIABLE rtn : STD_LOGIC_VECTOR(data_width DOWNTO 0);
    BEGIN
        rtn := STD_LOGIC_VECTOR(unsigned('0' & alpha) + unsigned('0' & beta));
        RETURN rtn;
    END FUNCTION addition;

    FUNCTION subtraction(
        alpha : STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0);
        beta : STD_LOGIC_VECTOR(data_width - 1 DOWNTO 0))
        RETURN STD_LOGIC_VECTOR IS
        VARIABLE rtn : STD_LOGIC_VECTOR(data_width DOWNTO 0);
    BEGIN
        rtn := STD_LOGIC_VECTOR(unsigned('0' & alpha) - unsigned('0' & beta));
        RETURN rtn;
    END FUNCTION subtraction;

    SIGNAL operation_s : operation_t := operation_invalid;
    SIGNAL result_s : STD_LOGIC_VECTOR(data_width DOWNTO 0);
    SIGNAL ready_for_data_s : STD_LOGIC;

BEGIN

    result_o <= result_s(data_width - 1 DOWNTO 0);
    carry_o <= result_s(result_s'left);
    ready_for_data_o <= ready_for_data_s;

    compute : PROCESS (clock_i)
    BEGIN
        IF (rising_edge(clock_i)) THEN
            IF (reset_n_i = '1') THEN
                ready_for_data_s <= '0';
                result_s <= (OTHERS => '0');
            ELSE
                IF (enable_i = '1') THEN
                    operation_s <= get_operation(operation_i);
                    CASE operation_s IS
                        WHEN operation_or =>
                            result_s <= '0' & (operand1_i OR operand2_i);
                        WHEN operation_and =>
                            result_s <= '0' & (operand1_i AND operand2_i);
                        WHEN operation_add =>
                            result_s <= addition(operand1_i, operand2_i);
                        WHEN operation_sub =>
                            result_s <= subtraction(operand1_i, operand2_i);
                        WHEN OTHERS =>
                            result_s <= (OTHERS => '0');
                    END CASE;
                    ready_for_data_s <= '1';
                ELSE
                    ready_for_data_s <= '0';
                END IF;
            END IF;
        END IF;
    END PROCESS compute;

END ARCHITECTURE rtl;