#ifndef CUBE18B_H
#define CUBE18B_H

#include "main.h"
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

static const cubie_e cubieDefinition_to_cubie[NUM_FACES][NUM_FACES][NUM_FACES+1] = {
	{
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_URF, CUBIE_NULL, CUBIE_URB, CUBIE_NULL, CUBIE_UR , },
		{CUBIE_NULL, CUBIE_UFR, CUBIE_NULL, CUBIE_UFL, CUBIE_NULL, CUBIE_NULL, CUBIE_UF , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_ULF, CUBIE_NULL, CUBIE_ULB, CUBIE_NULL, CUBIE_UL , },
		{CUBIE_NULL, CUBIE_UBR, CUBIE_NULL, CUBIE_UBL, CUBIE_NULL, CUBIE_NULL, CUBIE_UB , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
	},
	{
		{CUBIE_NULL, CUBIE_NULL, CUBIE_RUF, CUBIE_NULL, CUBIE_RUB, CUBIE_NULL, CUBIE_RU , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_RFU, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_RFD, CUBIE_RF , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_RBU, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_RBD, CUBIE_RB , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_RDF, CUBIE_NULL, CUBIE_RDB, CUBIE_NULL, CUBIE_RD , },
	},
	{
		{CUBIE_NULL, CUBIE_FUR, CUBIE_NULL, CUBIE_FUL, CUBIE_NULL, CUBIE_NULL, CUBIE_FU , },
		{CUBIE_FRU, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_FRD, CUBIE_FR , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_FLU, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_FLD, CUBIE_FL , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_NULL, CUBIE_FDR, CUBIE_NULL, CUBIE_FDL, CUBIE_NULL, CUBIE_NULL, CUBIE_FD , },
	},
	{
		{CUBIE_NULL, CUBIE_NULL, CUBIE_LUF, CUBIE_NULL, CUBIE_LUB, CUBIE_NULL, CUBIE_LU , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_LFU, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_LFD, CUBIE_LF , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_LBU, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_LBD, CUBIE_LB , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_LDF, CUBIE_NULL, CUBIE_LDB, CUBIE_NULL, CUBIE_LD , },
	},
	{
		{CUBIE_NULL, CUBIE_BUR, CUBIE_NULL, CUBIE_BUL, CUBIE_NULL, CUBIE_NULL, CUBIE_BU , },
		{CUBIE_BRU, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_BRD, CUBIE_BR , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_BLU, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_BLD, CUBIE_BL , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_NULL, CUBIE_BDR, CUBIE_NULL, CUBIE_BDL, CUBIE_NULL, CUBIE_NULL, CUBIE_BD , },
	},
	{
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_DRF, CUBIE_NULL, CUBIE_DRB, CUBIE_NULL, CUBIE_DR , },
		{CUBIE_NULL, CUBIE_DFR, CUBIE_NULL, CUBIE_DFL, CUBIE_NULL, CUBIE_NULL, CUBIE_DF , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_DLF, CUBIE_NULL, CUBIE_DLB, CUBIE_NULL, CUBIE_DL , },
		{CUBIE_NULL, CUBIE_DBR, CUBIE_NULL, CUBIE_DBL, CUBIE_NULL, CUBIE_NULL, CUBIE_DB , },
		{CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, CUBIE_NULL, },
	},
};
static const face_e cubieDefinitions[NUM_SEQUENCES][3] = {
    {FACE_U, FACE_R, FACE_NULL}, //UR 
    {FACE_R, FACE_U, FACE_NULL}, //RU 
    {FACE_U, FACE_F, FACE_NULL}, //UF 
    {FACE_F, FACE_U, FACE_NULL}, //FU 
    {FACE_U, FACE_L, FACE_NULL}, //UL 
    {FACE_L, FACE_U, FACE_NULL}, //LU 
    {FACE_U, FACE_B, FACE_NULL}, //UB 
    {FACE_B, FACE_U, FACE_NULL}, //BU 
    {FACE_R, FACE_F, FACE_NULL}, //RF 
    {FACE_F, FACE_R, FACE_NULL}, //FR 
    {FACE_R, FACE_B, FACE_NULL}, //RB 
    {FACE_B, FACE_R, FACE_NULL}, //BR 
    {FACE_R, FACE_D, FACE_NULL}, //RD 
    {FACE_D, FACE_R, FACE_NULL}, //DR 
    {FACE_F, FACE_L, FACE_NULL}, //FL 
    {FACE_L, FACE_F, FACE_NULL}, //LF 
    {FACE_F, FACE_D, FACE_NULL}, //FD 
    {FACE_D, FACE_F, FACE_NULL}, //DF 
    {FACE_L, FACE_B, FACE_NULL}, //LB 
    {FACE_B, FACE_L, FACE_NULL}, //BL 
    {FACE_L, FACE_D, FACE_NULL}, //LD 
    {FACE_D, FACE_L, FACE_NULL}, //DL 
    {FACE_B, FACE_D, FACE_NULL}, //BD 
    {FACE_D, FACE_B, FACE_NULL}, //DB 
    {FACE_F, FACE_U, FACE_R},    //FUR
    {FACE_U, FACE_R, FACE_F},    //URF
    {FACE_R, FACE_F, FACE_U},    //RFU
    {FACE_L, FACE_U, FACE_F},    //LUF
    {FACE_U, FACE_F, FACE_L},    //UFL
    {FACE_F, FACE_L, FACE_U},    //FLU
    {FACE_B, FACE_U, FACE_L},    //BUL
    {FACE_U, FACE_L, FACE_B},    //ULB
    {FACE_L, FACE_B, FACE_U},    //LBU
    {FACE_R, FACE_U, FACE_B},    //RUB
    {FACE_U, FACE_B, FACE_R},    //UBR
    {FACE_B, FACE_R, FACE_U},    //BRU
    {FACE_F, FACE_R, FACE_D},    //FRD
    {FACE_D, FACE_F, FACE_R},    //DFR
    {FACE_R, FACE_D, FACE_F},    //RDF
    {FACE_L, FACE_F, FACE_D},    //LFD
    {FACE_D, FACE_L, FACE_F},    //DLF
    {FACE_F, FACE_D, FACE_L},    //FDL
    {FACE_B, FACE_L, FACE_D},    //BLD
    {FACE_D, FACE_B, FACE_L},    //DBL
    {FACE_L, FACE_D, FACE_B},    //LDB
    {FACE_R, FACE_B, FACE_D},    //RBD
    {FACE_D, FACE_R, FACE_B},    //DRB
    {FACE_B, FACE_D, FACE_R},    //BDR
    {FACE_F, FACE_R, FACE_U},    //FRU
    {FACE_U, FACE_F, FACE_R},    //UFR
    {FACE_R, FACE_U, FACE_F},    //RUF
    {FACE_L, FACE_F, FACE_U},    //LFU
    {FACE_U, FACE_L, FACE_F},    //ULF
    {FACE_F, FACE_U, FACE_L},    //FUL
    {FACE_B, FACE_L, FACE_U},    //BLU
    {FACE_U, FACE_B, FACE_L},    //UBL
    {FACE_L, FACE_U, FACE_B},    //LUB
    {FACE_R, FACE_B, FACE_U},    //RBU
    {FACE_U, FACE_R, FACE_B},    //URB
    {FACE_B, FACE_U, FACE_R},    //BUR
    {FACE_F, FACE_D, FACE_R},    //FDR
    {FACE_D, FACE_R, FACE_F},    //DRF
    {FACE_R, FACE_F, FACE_D},    //RFD
    {FACE_L, FACE_D, FACE_F},    //LDF
    {FACE_D, FACE_F, FACE_L},    //DFL
    {FACE_F, FACE_L, FACE_D},    //FLD
    {FACE_B, FACE_D, FACE_L},    //BDL
    {FACE_D, FACE_L, FACE_B},    //DLB
    {FACE_L, FACE_B, FACE_D},    //LBD
    {FACE_R, FACE_D, FACE_B},    //RDB
    {FACE_D, FACE_B, FACE_R},    //DBR
    {FACE_B, FACE_R, FACE_D},    //BRD
};
static const char* cubiePrints[73] = {
    "UR ",
    "RU ",
    "UF ",
    "FU ",
    "UL ",
    "LU ",
    "UB ",
    "BU ",
    "RF ",
    "FR ",
    "RB ",
    "BR ",
    "RD ",
    "DR ",
    "FL ",
    "LF ",
    "FD ",
    "DF ",
    "LB ",
    "BL ",
    "LD ",
    "DL ",
    "BD ",
    "DB ",
    "FUR",
    "URF",
    "RFU",
    "LUF",
    "UFL",
    "FLU",
    "BUL",
    "ULB",
    "LBU",
    "RUB",
    "UBR",
    "BRU",
    "FRD",
    "DFR",
    "RDF",
    "LFD",
    "DLF",
    "FDL",
    "BLD",
    "DBL",
    "LDB",
    "RBD",
    "DRB",
    "BDR",
    "FRU",
    "UFR",
    "RUF",
    "LFU",
    "ULF",
    "FUL",
    "BLU",
    "UBL",
    "LUB",
    "RBU",
    "URB",
    "BUR",
    "FDR",
    "DRF",
    "RFD",
    "LDF",
    "DFL",
    "FLD",
    "BDL",
    "DLB",
    "LBD",
    "RDB",
    "DBR",
    "BRD",
    "NULL"
};
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


#endif // CUBE18B_H
