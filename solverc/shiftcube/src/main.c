#include "main.h"
#include "alg.h"
#include "servoCoder.h"
#include "shift_cube.h"
#include "solver.h"
#include "tests.h"

#include <string.h>

typedef enum inputs {
    INPUT_SCRAMBLE,
    INPUT_SHIFTCUBE,
    NUM_INPUTS,
} input_e;

typedef enum outputs {
    OUTPUT_ALG,
    OUTPUT_SERVOCODE,
    NUM_OUTPUTS,
} output_e;

#define help_str \
    "Usage: ./solver [OPTION]... INPUT...\n" \
    "Solve input as scramble algorithm or from a valid shiftcube state.\n" \
    "\n" \
    "  -i, --input      specify input mode, either scramble or shiftcube\n" \
    "  -o, --output     specify output mode, either alg or servocode\n" \
    "      --help       show this message then exit\n" \
    "\n" \
    "Inputs:\n" \
    "  scramble         pass the scramble as a string.\n" \
    "  shiftcube        pass the shiftcube faces as 6 printed uint32_ts separated by spaces\n" \
    "Outputs:\n" \
    "  alg              prints the solution as an algorithm.\n" \
    "  servocode        print converted solution in servocode.\n" \
    "\n" \
    "Examples:\n" \
    "./solver -i scramble -o servocode \"F U2 R3\"  Apply the input scramble to a cube then output solution as servocode\n" \
    "./solver -o alg \"F2 B2 R2 L2 U2 D2\"          Apply the algorithm to a cube then output solution alg.\n"

int main(int argc, char *argv[]) {
    input_e input = INPUT_SCRAMBLE;
    output_e output = OUTPUT_ALG;
    if (argc == 1) {
        printf("Not enough arguments provided.\n");
        printf("Try './solver --help' for more information.\n");
        return 1;
    }

    size_t i = 1;
    // check for optional inputs
    for (; i < argc; i++) {
        if (!strcmp("-i", argv[i]) || !strcmp("--input", argv[i])) {
            if (++i == argc) {
                printf("Input option not provided.\n");
                return 1;
            }
            if (!strcmp("scramble", argv[i])) {
                input = INPUT_SCRAMBLE;
            } else if (!strcmp("shiftcube", argv[i])) {
                input = INPUT_SHIFTCUBE;
            } else {
                printf("Invalid input option provided: %s\n", argv[i]);
                return 1;
            }
        } else if (!strcmp("-o", argv[i]) || !strcmp("--output", argv[i])) {
            if (++i == argc) {
                printf("Output option not provided.\n");
                return 1;
            }
            if (!strcmp("alg", argv[i])) {
                output = OUTPUT_ALG;
            } else if (!strcmp("servocode", argv[i])) {
                output = OUTPUT_SERVOCODE;
            } else {
                printf("Invalid output option provided: %s\n", argv[i]);
                return 1;
            }
        } else if (!strcmp("--help", argv[i])) {
            printf(help_str);
            return 0;
        } else {
            break;
        }
    }

    shift_cube_s cube = SOLVED_SHIFTCUBE;

    if (input == INPUT_SCRAMBLE) {
        if (i + 1 != argc) {
            printf("Too many input parameters for scramble.\n");
            return 1;
        }
        alg_s *scramble_alg = alg_from_str(argv[i]);
        if (!scramble_alg) {
            printf("Provided scramble was an invalid algorithm.\n");
            return 1;
        }
        apply_alg(&cube, scramble_alg);
        alg_free(scramble_alg);
    } else if (input == INPUT_SHIFTCUBE) {
        if (argc - i != 6) {
            printf("Too many faces passed for shiftcube input.\n");
            return 1;
        }
        for (face_e face = FACE_U; i < argc; face++, i++) {
            char *end_ptr;
            uint32_t face_data = strtoul(argv[i], &end_ptr, 10);
            if (argv[i] == end_ptr || face_data > SOLVED_FACE_D) {
                printf("%d doesn't convert to a valid shiftcube face\n", face);
                return 1;
            }
            cube.state[face] = face_data;
        }
    }

    init_solver();
    cube_table_s *f2l_table = gen_f2l_table();
    cube_alg_table_s *ll_table = gen_last_layer_table();
    alg_s *solve = solve_cube(cube, f2l_table, ll_table);
    if (!solve) {
        return 1;
    }

    if (output == OUTPUT_ALG) {
        print_alg(solve);
        alg_free(solve);
        cube_table_free(f2l_table);
        cube_alg_table_free(ll_table);
        return 0;
    }

    inter_move_table_s *inter_move_table = inter_move_table_create();
    RobotSolution servo_code = servoCode_compiler_Ofastest(solve, inter_move_table);
    for (size_t i = 0; i < servo_code.size; i++) {
        RobotState_s state = servo_code.solution[i];
        print_RobotState(state); printf(" ");
    }
    free(servo_code.solution);
    inter_move_table_free(inter_move_table);
    cleanup_solver();

    return 0;
}
