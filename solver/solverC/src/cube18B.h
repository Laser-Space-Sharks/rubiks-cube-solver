#ifndef CUBE18B_H
#define CUBE18B_H

#include "main.h"
#include "shift_cube.h"
#include "alg.h"
#include <stdbool.h>

typedef enum : uint8_t {
    CUBIE_UR  = 0,
    CUBIE_RU  = 1,
    CUBIE_UF  = 2,
    CUBIE_FU  = 3,
    CUBIE_UL  = 4,
    CUBIE_LU  = 5,
    CUBIE_UB  = 6,
    CUBIE_BU  = 7,
    CUBIE_RF  = 8,
    CUBIE_FR  = 9,
    CUBIE_RB  = 10,
    CUBIE_BR  = 11,
    CUBIE_RD  = 12,
    CUBIE_DR  = 13,
    CUBIE_FL  = 14,
    CUBIE_LF  = 15,
    CUBIE_FD  = 16,
    CUBIE_DF  = 17,
    CUBIE_LB  = 18,
    CUBIE_BL  = 19,
    CUBIE_LD  = 20,
    CUBIE_DL  = 21,
    CUBIE_BD  = 22,
    CUBIE_DB  = 23,
    CUBIE_FUR = 24,
    CUBIE_URF = 25,
    CUBIE_RFU = 26,
    CUBIE_LUF = 27,
    CUBIE_UFL = 28,
    CUBIE_FLU = 29,
    CUBIE_BUL = 30,
    CUBIE_ULB = 31,
    CUBIE_LBU = 32,
    CUBIE_RUB = 33,
    CUBIE_UBR = 34,
    CUBIE_BRU = 35,
    CUBIE_FRD = 36,
    CUBIE_DFR = 37,
    CUBIE_RDF = 38,
    CUBIE_LFD = 39,
    CUBIE_DLF = 40,
    CUBIE_FDL = 41,
    CUBIE_BLD = 42,
    CUBIE_DBL = 43,
    CUBIE_LDB = 44,
    CUBIE_RBD = 45,
    CUBIE_DRB = 46,
    CUBIE_BDR = 47, // This is the last real cubie. All cubies beyond are kept for colorSequence encoding
    CUBIE_FRU = 48,
    CUBIE_UFR = 49,
    CUBIE_RUF = 50,
    CUBIE_LFU = 51,
    CUBIE_ULF = 52,
    CUBIE_FUL = 53,
    CUBIE_BLU = 54,
    CUBIE_UBL = 55,
    CUBIE_LUB = 56,
    CUBIE_RBU = 57,
    CUBIE_URB = 58,
    CUBIE_BUR = 59,
    CUBIE_FDR = 60,
    CUBIE_DRF = 61,
    CUBIE_RFD = 62,
    CUBIE_LDF = 63,
    CUBIE_DFL = 64,
    CUBIE_FLD = 65,
    CUBIE_BDL = 66,
    CUBIE_DLB = 67,
    CUBIE_LBD = 68,
    CUBIE_RDB = 69,
    CUBIE_DBR = 70,
    CUBIE_BRD = 71,
    NUM_CUBIES = 48,
    NUM_SEQUENCES = 72,
    CUBIE_NULL = 72
} cubie_e;

//####################################################################################################################################################
// The cube can be defined in 18 bytes, where each byte is a cubie.
// |           CROSS              |                               F2L                             |                        1LLL                   |
// | eFD     eRD     eBD     eLD  |  eFR     dFR     eRB     dRB     eBL     dBL     eLF     dLF  |  eFU     eRU     eBU     uFR     uRB     uBL  |
// [[Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte]]
// Experimentally tested via the 1LLL table hashing that we don't need the last 2 1LLL cubies. That's why cube18B is not cube20B.
//####################################################################################################################################################

typedef struct {
    cubie_e cubies[18];
} cube18B_s;
typedef struct {
    cubie_e cubies[12];
} cube18B_xcross_s;
typedef struct {
    cubie_e cubies[6];
} cube18B_1LLL_s;
typedef struct {
    cubie_e cubieShift[48];
} cubieTable_s;

static const cube18B_s SOLVED_CUBE18B = {
    .cubies = {
        CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
        CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD, 
        CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_URF, CUBIE_UBR, CUBIE_ULB
    }
};

static const cube18B_xcross_s SOLVED_CUBE18B_XCROSS = {
    .cubies = {
        CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
        CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD
    }
};

static const cube18B_1LLL_s SOLVED_CUBE18B_1LLL = {
    .cubies = {
        CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_URF, CUBIE_UBR, CUBIE_ULB
    }
};

void init_all_tables_in_cube18Bc();
cube18B_s cube18B_from_shiftCube(const shift_cube_s* shiftcube);
shift_cube_s shiftCube_from_cube18B(const cube18B_s* cube18B);
cube18B_xcross_s cube18B_xcross_from_cube18B(const cube18B_s* cube);
cube18B_1LLL_s cube18B_1LLL_from_cube18B(const cube18B_s* cube);
cube18B_s cube18B_from_xcross_and_1LLL(const cube18B_xcross_s* xcross, const cube18B_1LLL_s* LL);
bool compare_cube18Bs(const cube18B_s* cube1, const cube18B_s* cube2);
void print_cube18B(const cube18B_s* cube);
void cube18B_apply_move(cube18B_s* cube, move_e move);
void cube18B_xcross_apply_move(cube18B_xcross_s* cube, move_e move);
void cube18B_1LLL_apply_move(cube18B_1LLL_s* cube, move_e move);
void cube18B_apply_alg(cube18B_s *cube, const alg_s *alg);
void cube18B_xcross_apply_alg(cube18B_xcross_s *cube, const alg_s *alg);
void cube18B_1LLL_apply_alg(cube18B_1LLL_s *cube, const alg_s *alg);
cubie_e apply_alg_to_cubie(cubie_e cubie, const alg_s* alg);
cubieTable_s alg_to_cubieTable(const alg_s* alg);
void apply_cubieTable_to_cube(cube18B_s* cube, const cubieTable_s* table);
cubieTable_s conjoin_cubeTables(const cubieTable_s* table1, const cubieTable_s* table2);
void print_cubieDefinitions();
void print_cubieDefinition_to_cubie();
void print_colorSequence_to_solvedCubieInd();
void print_cubie_to_orderedPositions();
void print_cubieAfterMove();
void print_colorsAtEdgePosInd_to_cubieAndSolvedCubie();
void print_colorsAtCornerPosInd_to_cubieAndSolvedCubie();
void print_cubieAndSolvedCubieInd_to_colorsAtPosInd();


#endif // CUBE18B_H
