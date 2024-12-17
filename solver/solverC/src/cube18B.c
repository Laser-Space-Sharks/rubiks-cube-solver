#include "cube18B.h"
#include "main.h"
#include "alg.h"

cubie_e SOLVED_CUBIES[20] = {
    CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
    CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD, 
    CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_URF, CUBIE_UBR, CUBIE_ULB, CUBIE_LU, CUBIE_UFL
};

void print_cube18B(const cube18B_s* cube) {
    for (int i = 0; i < 18; i++) {
        printf("%s ", cubiePrints[cube->cubies[i]]);
    } printf("\n");
}

static const face_e faceAfterMove[NUM_FACES+1][NUM_MOVES] = {
    {// U
        FACE_U, FACE_U, FACE_U, // U
        FACE_B, FACE_D, FACE_F, // R
        FACE_R, FACE_D, FACE_L, // F
        FACE_F, FACE_D, FACE_B, // L
        FACE_L, FACE_D, FACE_R, // B
        FACE_U, FACE_U, FACE_U, // D
    }, 
    { // R
        FACE_F, FACE_L, FACE_B, // U
        FACE_R, FACE_R, FACE_R, // R
        FACE_D, FACE_L, FACE_U, // F
        FACE_R, FACE_R, FACE_R, // L
        FACE_U, FACE_L, FACE_D, // B
        FACE_B, FACE_L, FACE_F, // D
    }, 
    { // F
        FACE_L, FACE_B, FACE_R, // U
        FACE_U, FACE_B, FACE_D, // R
        FACE_F, FACE_F, FACE_F, // F
        FACE_D, FACE_B, FACE_U, // L
        FACE_F, FACE_F, FACE_F, // B
        FACE_R, FACE_B, FACE_L, // D
    }, 
    { // L
        FACE_B, FACE_R, FACE_F, // U
        FACE_L, FACE_L, FACE_L, // R
        FACE_U, FACE_R, FACE_D, // F
        FACE_L, FACE_L, FACE_L, // L
        FACE_D, FACE_R, FACE_U, // B
        FACE_F, FACE_R, FACE_B, // D
    }, 
    { // B
        FACE_R, FACE_F, FACE_L, // U
        FACE_D, FACE_F, FACE_U, // R
        FACE_B, FACE_B, FACE_B, // F
        FACE_U, FACE_F, FACE_D, // L
        FACE_B, FACE_B, FACE_B, // B
        FACE_L, FACE_F, FACE_R, // D
    }, 
    { // D
        FACE_D, FACE_D, FACE_D, // U
        FACE_F, FACE_U, FACE_B, // R
        FACE_L, FACE_U, FACE_R, // F
        FACE_B, FACE_U, FACE_F, // L
        FACE_R, FACE_U, FACE_L, // B
        FACE_D, FACE_D, FACE_D, // D
    },
    { // N
        FACE_NULL, FACE_NULL, FACE_NULL, // U
        FACE_NULL, FACE_NULL, FACE_NULL, // R
        FACE_NULL, FACE_NULL, FACE_NULL, // F
        FACE_NULL, FACE_NULL, FACE_NULL, // L
        FACE_NULL, FACE_NULL, FACE_NULL, // B
        FACE_NULL, FACE_NULL, FACE_NULL, // D
    }
};

static const uint8_t colorSequence_to_solvedCubieInd[NUM_FACES+1][NUM_FACES+1][NUM_FACES+1] = {
	{
		{21, 21, 21, 21, 21, 21, 21, },
		{21, 21, 15, 21, 16, 21, 13, },
		{21, 15, 21, 19, 21, 21, 12, },
		{21, 21, 19, 21, 17, 21, 18, },
		{21, 16, 21, 17, 21, 21, 14, },
		{21, 21, 21, 21, 21, 21, 21, },
		{21, 13, 12, 18, 14, 21, 21, },
	},
	{
		{21, 21, 15, 21, 16, 21, 13, },
		{21, 21, 21, 21, 21, 21, 21, },
		{15, 21, 21, 21, 21, 5, 4, },
		{21, 21, 21, 21, 21, 21, 21, },
		{16, 21, 21, 21, 21, 7, 6, },
		{21, 21, 5, 21, 7, 21, 1, },
		{13, 21, 4, 21, 6, 1, 21, },
	},
	{
		{21, 15, 21, 19, 21, 21, 12, },
		{15, 21, 21, 21, 21, 5, 4, },
		{21, 21, 21, 21, 21, 21, 21, },
		{19, 21, 21, 21, 21, 11, 10, },
		{21, 21, 21, 21, 21, 21, 21, },
		{21, 5, 21, 11, 21, 21, 0, },
		{12, 4, 21, 10, 21, 0, 21, },
	},
	{
		{21, 21, 19, 21, 17, 21, 18, },
		{21, 21, 21, 21, 21, 21, 21, },
		{19, 21, 21, 21, 21, 11, 10, },
		{21, 21, 21, 21, 21, 21, 21, },
		{17, 21, 21, 21, 21, 9, 8, },
		{21, 21, 11, 21, 9, 21, 3, },
		{18, 21, 10, 21, 8, 3, 21, },
	},
	{
		{21, 16, 21, 17, 21, 21, 14, },
		{16, 21, 21, 21, 21, 7, 6, },
		{21, 21, 21, 21, 21, 21, 21, },
		{17, 21, 21, 21, 21, 9, 8, },
		{21, 21, 21, 21, 21, 21, 21, },
		{21, 7, 21, 9, 21, 21, 2, },
		{14, 6, 21, 8, 21, 2, 21, },
	},
	{
		{21, 21, 21, 21, 21, 21, 21, },
		{21, 21, 5, 21, 7, 21, 1, },
		{21, 5, 21, 11, 21, 21, 0, },
		{21, 21, 11, 21, 9, 21, 3, },
		{21, 7, 21, 9, 21, 21, 2, },
		{21, 21, 21, 21, 21, 21, 21, },
		{21, 1, 0, 3, 2, 21, 21, },
	},
	{
		{21, 13, 12, 18, 14, 21, 21, },
		{13, 21, 4, 21, 6, 1, 21, },
		{12, 4, 21, 10, 21, 0, 21, },
		{18, 21, 10, 21, 8, 3, 21, },
		{14, 6, 21, 8, 21, 2, 21, },
		{21, 1, 0, 3, 2, 21, 21, },
		{21, 21, 21, 21, 21, 21, 21, },
	},
};

static const cubie_e cubieAfterMove[NUM_MOVES][NUM_CUBIES] = {
	{CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_UR , CUBIE_RU , CUBIE_RF , CUBIE_FR , CUBIE_RB , CUBIE_BR , CUBIE_RD , CUBIE_DR , CUBIE_FL , CUBIE_LF , CUBIE_FD , CUBIE_DF , CUBIE_LB , CUBIE_BL , CUBIE_LD , CUBIE_DL , CUBIE_BD , CUBIE_DB , CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, },
	{CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_RF , CUBIE_FR , CUBIE_RB , CUBIE_BR , CUBIE_RD , CUBIE_DR , CUBIE_FL , CUBIE_LF , CUBIE_FD , CUBIE_DF , CUBIE_LB , CUBIE_BL , CUBIE_LD , CUBIE_DL , CUBIE_BD , CUBIE_DB , CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, },
	{CUBIE_UB , CUBIE_BU , CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_RF , CUBIE_FR , CUBIE_RB , CUBIE_BR , CUBIE_RD , CUBIE_DR , CUBIE_FL , CUBIE_LF , CUBIE_FD , CUBIE_DF , CUBIE_LB , CUBIE_BL , CUBIE_LD , CUBIE_DL , CUBIE_BD , CUBIE_DB , CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, },
	{CUBIE_BR , CUBIE_RB , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_RU , CUBIE_UR , CUBIE_RD , CUBIE_DR , CUBIE_RF , CUBIE_FR , CUBIE_FL , CUBIE_LF , CUBIE_FD , CUBIE_DF , CUBIE_LB , CUBIE_BL , CUBIE_LD , CUBIE_DL , CUBIE_BD , CUBIE_DB , CUBIE_UBR, CUBIE_BRU, CUBIE_RUB, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RBD, CUBIE_BDR, CUBIE_DRB, CUBIE_URF, CUBIE_FUR, CUBIE_RFU, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RDF, CUBIE_FRD, CUBIE_DFR, },
	{CUBIE_DR , CUBIE_RD , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_RB , CUBIE_BR , CUBIE_RF , CUBIE_FR , CUBIE_RU , CUBIE_UR , CUBIE_FL , CUBIE_LF , CUBIE_FD , CUBIE_DF , CUBIE_LB , CUBIE_BL , CUBIE_LD , CUBIE_DL , CUBIE_BD , CUBIE_DB , CUBIE_BDR, CUBIE_DRB, CUBIE_RBD, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RDF, CUBIE_DFR, CUBIE_FRD, CUBIE_BRU, CUBIE_UBR, CUBIE_RUB, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RFU, CUBIE_URF, CUBIE_FUR, },
	{CUBIE_FR , CUBIE_RF , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_RD , CUBIE_DR , CUBIE_RU , CUBIE_UR , CUBIE_RB , CUBIE_BR , CUBIE_FL , CUBIE_LF , CUBIE_FD , CUBIE_DF , CUBIE_LB , CUBIE_BL , CUBIE_LD , CUBIE_DL , CUBIE_BD , CUBIE_DB , CUBIE_DFR, CUBIE_FRD, CUBIE_RDF, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RFU, CUBIE_FUR, CUBIE_URF, CUBIE_DRB, CUBIE_BDR, CUBIE_RBD, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RUB, CUBIE_BRU, CUBIE_UBR, },
	{CUBIE_UR , CUBIE_RU , CUBIE_RF , CUBIE_FR , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_DF , CUBIE_FD , CUBIE_RB , CUBIE_BR , CUBIE_RD , CUBIE_DR , CUBIE_FU , CUBIE_UF , CUBIE_FL , CUBIE_LF , CUBIE_LB , CUBIE_BL , CUBIE_LD , CUBIE_DL , CUBIE_BD , CUBIE_DB , CUBIE_FRD, CUBIE_RDF, CUBIE_DFR, CUBIE_URF, CUBIE_RFU, CUBIE_FUR, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_FDL, CUBIE_LFD, CUBIE_DLF, CUBIE_UFL, CUBIE_LUF, CUBIE_FLU, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, },
	{CUBIE_UR , CUBIE_RU , CUBIE_DF , CUBIE_FD , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_LF , CUBIE_FL , CUBIE_RB , CUBIE_BR , CUBIE_RD , CUBIE_DR , CUBIE_FR , CUBIE_RF , CUBIE_FU , CUBIE_UF , CUBIE_LB , CUBIE_BL , CUBIE_LD , CUBIE_DL , CUBIE_BD , CUBIE_DB , CUBIE_FDL, CUBIE_DLF, CUBIE_LFD, CUBIE_RDF, CUBIE_DFR, CUBIE_FRD, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_FLU, CUBIE_UFL, CUBIE_LUF, CUBIE_RFU, CUBIE_URF, CUBIE_FUR, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, },
	{CUBIE_UR , CUBIE_RU , CUBIE_LF , CUBIE_FL , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_UF , CUBIE_FU , CUBIE_RB , CUBIE_BR , CUBIE_RD , CUBIE_DR , CUBIE_FD , CUBIE_DF , CUBIE_FR , CUBIE_RF , CUBIE_LB , CUBIE_BL , CUBIE_LD , CUBIE_DL , CUBIE_BD , CUBIE_DB , CUBIE_FLU, CUBIE_LUF, CUBIE_UFL, CUBIE_DLF, CUBIE_LFD, CUBIE_FDL, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_FUR, CUBIE_RFU, CUBIE_URF, CUBIE_DFR, CUBIE_RDF, CUBIE_FRD, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, },
	{CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_FL , CUBIE_LF , CUBIE_UB , CUBIE_BU , CUBIE_RF , CUBIE_FR , CUBIE_RB , CUBIE_BR , CUBIE_RD , CUBIE_DR , CUBIE_DL , CUBIE_LD , CUBIE_FD , CUBIE_DF , CUBIE_LU , CUBIE_UL , CUBIE_LB , CUBIE_BL , CUBIE_BD , CUBIE_DB , CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LFD, CUBIE_FDL, CUBIE_DLF, CUBIE_UFL, CUBIE_FLU, CUBIE_LUF, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LDB, CUBIE_BLD, CUBIE_DBL, CUBIE_ULB, CUBIE_BUL, CUBIE_LBU, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, },
	{CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_DL , CUBIE_LD , CUBIE_UB , CUBIE_BU , CUBIE_RF , CUBIE_FR , CUBIE_RB , CUBIE_BR , CUBIE_RD , CUBIE_DR , CUBIE_BL , CUBIE_LB , CUBIE_FD , CUBIE_DF , CUBIE_LF , CUBIE_FL , CUBIE_LU , CUBIE_UL , CUBIE_BD , CUBIE_DB , CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LDB, CUBIE_DBL, CUBIE_BLD, CUBIE_FDL, CUBIE_DLF, CUBIE_LFD, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LBU, CUBIE_ULB, CUBIE_BUL, CUBIE_FLU, CUBIE_UFL, CUBIE_LUF, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, },
	{CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_BL , CUBIE_LB , CUBIE_UB , CUBIE_BU , CUBIE_RF , CUBIE_FR , CUBIE_RB , CUBIE_BR , CUBIE_RD , CUBIE_DR , CUBIE_UL , CUBIE_LU , CUBIE_FD , CUBIE_DF , CUBIE_LD , CUBIE_DL , CUBIE_LF , CUBIE_FL , CUBIE_BD , CUBIE_DB , CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LBU, CUBIE_BUL, CUBIE_ULB, CUBIE_DBL, CUBIE_BLD, CUBIE_LDB, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LUF, CUBIE_FLU, CUBIE_UFL, CUBIE_DLF, CUBIE_FDL, CUBIE_LFD, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, },
	{CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_LB , CUBIE_BL , CUBIE_RF , CUBIE_FR , CUBIE_UB , CUBIE_BU , CUBIE_RD , CUBIE_DR , CUBIE_FL , CUBIE_LF , CUBIE_FD , CUBIE_DF , CUBIE_DB , CUBIE_BD , CUBIE_LD , CUBIE_DL , CUBIE_BR , CUBIE_RB , CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BLD, CUBIE_LDB, CUBIE_DBL, CUBIE_ULB, CUBIE_LBU, CUBIE_BUL, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BDR, CUBIE_RBD, CUBIE_DRB, CUBIE_UBR, CUBIE_RUB, CUBIE_BRU, },
	{CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_DB , CUBIE_BD , CUBIE_RF , CUBIE_FR , CUBIE_LB , CUBIE_BL , CUBIE_RD , CUBIE_DR , CUBIE_FL , CUBIE_LF , CUBIE_FD , CUBIE_DF , CUBIE_RB , CUBIE_BR , CUBIE_LD , CUBIE_DL , CUBIE_BU , CUBIE_UB , CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BDR, CUBIE_DRB, CUBIE_RBD, CUBIE_LDB, CUBIE_DBL, CUBIE_BLD, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BRU, CUBIE_UBR, CUBIE_RUB, CUBIE_LBU, CUBIE_ULB, CUBIE_BUL, },
	{CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_RB , CUBIE_BR , CUBIE_RF , CUBIE_FR , CUBIE_DB , CUBIE_BD , CUBIE_RD , CUBIE_DR , CUBIE_FL , CUBIE_LF , CUBIE_FD , CUBIE_DF , CUBIE_UB , CUBIE_BU , CUBIE_LD , CUBIE_DL , CUBIE_BL , CUBIE_LB , CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BRU, CUBIE_RUB, CUBIE_UBR, CUBIE_DRB, CUBIE_RBD, CUBIE_BDR, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BUL, CUBIE_LBU, CUBIE_ULB, CUBIE_DBL, CUBIE_LDB, CUBIE_BLD, },
	{CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_RF , CUBIE_FR , CUBIE_RB , CUBIE_BR , CUBIE_BD , CUBIE_DB , CUBIE_FL , CUBIE_LF , CUBIE_RD , CUBIE_DR , CUBIE_LB , CUBIE_BL , CUBIE_FD , CUBIE_DF , CUBIE_LD , CUBIE_DL , CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, },
	{CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_RF , CUBIE_FR , CUBIE_RB , CUBIE_BR , CUBIE_LD , CUBIE_DL , CUBIE_FL , CUBIE_LF , CUBIE_BD , CUBIE_DB , CUBIE_LB , CUBIE_BL , CUBIE_RD , CUBIE_DR , CUBIE_FD , CUBIE_DF , CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, },
	{CUBIE_UR , CUBIE_RU , CUBIE_UF , CUBIE_FU , CUBIE_UL , CUBIE_LU , CUBIE_UB , CUBIE_BU , CUBIE_RF , CUBIE_FR , CUBIE_RB , CUBIE_BR , CUBIE_FD , CUBIE_DF , CUBIE_FL , CUBIE_LF , CUBIE_LD , CUBIE_DL , CUBIE_LB , CUBIE_BL , CUBIE_BD , CUBIE_DB , CUBIE_RD , CUBIE_DR , CUBIE_FUR, CUBIE_URF, CUBIE_RFU, CUBIE_LUF, CUBIE_UFL, CUBIE_FLU, CUBIE_BUL, CUBIE_ULB, CUBIE_LBU, CUBIE_RUB, CUBIE_UBR, CUBIE_BRU, CUBIE_LFD, CUBIE_DLF, CUBIE_FDL, CUBIE_BLD, CUBIE_DBL, CUBIE_LDB, CUBIE_RBD, CUBIE_DRB, CUBIE_BDR, CUBIE_FRD, CUBIE_DFR, CUBIE_RDF, },
};

/*
static void init_cubieDefinition_to_cubie() {
    
    Tables Used: 
        cubeDefinitions[]
    Functions Used: None
    External Types Used: 
        face_e
    
    for (uint8_t ind1 = 0; ind1 < 6; ind1++) {
        for (uint8_t ind2 = 0; ind2 < 6; ind2++) {
            for (uint8_t ind3 = 0; ind3 < 7; ind3++) {
                cubieDefinition_to_cubie[ind1][ind2][ind3] = CUBIE_NULL;
            }
        }
    }
    for (cubie_e cubie = 0; cubie < NUM_SEQUENCES; cubie++) {
        face_e cubieDef[3] = {
            cubieDefinitions[cubie][0],
            cubieDefinitions[cubie][1],
            cubieDefinitions[cubie][2],
        };
        if (cubieDef[0] >= 6) {
            printf("Whoops! init_cubieDefinition_to_cubie goofed up: cubieDef[0] = %c\n", facePrints[cubieDef[0]]);
        }
        if (cubieDef[1] >= 6) {
            printf("Whoops! init_cubieDefinition_to_cubie goofed up: cubieDef[0] = %c\n", facePrints[cubieDef[1]]);
        }
        if (cubieDef[2] >= 7) {
            printf("Whoops! init_cubieDefinition_to_cubie goofed up: cubieDef[0] = %c\n", facePrints[cubieDef[2]]);
        }
        cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2]] = cubie;
    } print_cubieDefinition_to_cubie();
    //print_cubieDefinition_to_cubie();
}

void print_cubieDefinition_to_cubie() {
    printf("static const cubie_e cubieDefinition_to_cubie[NUM_FACES][NUM_FACES][NUM_FACES+1] = {\n");
    for (int i = 0; i < 6; i++) {
        printf("\t{\n");
        for (int j = 0; j < 6; j++) {
            printf("\t\t{");
            for (int k = 0; k < 7; k++) {
                cubie_e a = cubieDefinition_to_cubie[i][j][k];
                printf("CUBIE_%s, ", cubiePrints[a]);
            } printf("},\n");
        } printf("\t},\n");
    } printf("};\n");
}*/
/*
static void init_colorSequence_to_solvedCubieInd() {
    
    Tables Used: 
        SOLVED_CUBIES[]
        cubieDefinitions[]
    Functions Used: None
    External Types Used: 
        face_e
    
    for (int ind1 = 0; ind1 < 7; ind1++) {
        for (int ind2 = 0; ind2 < 7; ind2++) {
            for (int ind3 = 0; ind3 < 7; ind3++) {
                colorSequence_to_solvedCubieInd[ind1][ind2][ind3] = 21;
            }
        }
    }
    for (int solved = 0; solved < 20; solved++) {
        cubie_e solvedCubie = SOLVED_CUBIES[solved];
        if (solvedCubie > 72) {
            printf("Whoops! init_colorSequence_to_solvedCubieInd goofed up: solvedCube = %hhu\n", solvedCubie);
        }
        face_e solvedCubieDef[3] = {
            cubieDefinitions[solvedCubie][0],
            cubieDefinitions[solvedCubie][1],
            cubieDefinitions[solvedCubie][2],
        };
        face_e sequences[6][3] = {
            {solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2]},
            {solvedCubieDef[0], solvedCubieDef[2], solvedCubieDef[1]},
            {solvedCubieDef[1], solvedCubieDef[0], solvedCubieDef[2]},
            {solvedCubieDef[1], solvedCubieDef[2], solvedCubieDef[0]},
            {solvedCubieDef[2], solvedCubieDef[0], solvedCubieDef[1]},
            {solvedCubieDef[2], solvedCubieDef[1], solvedCubieDef[0]},
        };
        for (int row = 0; row < 6; row++) {
            if (sequences[row][0] >= 7) {
                printf("Whoops! init_colorSequence_to_solvedCubieInd goofed up: sequences[row][0] = %hhu\n", sequences[row][0]);
            }
            if (sequences[row][1] >= 7) {
                printf("Whoops! init_colorSequence_to_solvedCubieInd goofed up: sequences[row][1] = %hhu\n", sequences[row][1]);
            }
            if (sequences[row][2] >= 7) {
                printf("Whoops! init_colorSequence_to_solvedCubieInd goofed up: sequences[row][2] = %hhu\n", sequences[row][2]);
            }
            colorSequence_to_solvedCubieInd[sequences[row][0]][sequences[row][1]][sequences[row][2]] = solved;

        }
    }print_colorSequence_to_solvedCubieInd();
}

void print_colorSequence_to_solvedCubieInd() {
    printf("static const uint8_t colorSequence_to_solvedCubieInd[NUM_FACES+1][NUM_FACES+1][NUM_FACES+1] = {\n");
    for (int ind1 = 0; ind1 < 7; ind1++) {
        printf("\t{\n");
        for (int ind2 = 0; ind2 < 7; ind2++) {
            printf("\t\t{");
            for (int ind3 = 0; ind3 < 7; ind3++) {
                uint8_t solvedCubieInd = colorSequence_to_solvedCubieInd[ind1][ind2][ind3];
                printf("%hhu, ", solvedCubieInd);
                
                //if (solvedCubieInd == 21) {
                //    printf("\t(%c, %c, %c) : 21 \n", 
                //        facePrints[ind1], 
                //        facePrints[ind2], 
                //        facePrints[ind3]
                //    );
                //} else {
                //    cubie_e solved_cubie = SOLVED_CUBIES[solvedCubieInd];
                //    printf("\t(%c, %c, %c) : %s****\n", 
                //        facePrints[ind1], 
                //        facePrints[ind2], 
                //        facePrints[ind3], 
                //        cubiePrints[solved_cubie]
                //    );
                //}
            } printf("},\n");
        } printf("\t},\n");
    } printf("};\n");
}
*/
/*
// mainly for apply_move
static void init_cubieAfterMove() {
    
    Tables Used:
        cubieDefinitions[]
        faceAfterMove[]
        cubieDefinition_to_cubie[]
    Functions Used:
        find_face_in_3
    External Types Used:
        face_e
    
    //printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    for (cubie_e cubie0 = 0; cubie0 < NUM_CUBIES; cubie0++) {
        for (move_e move = 0; move < NUM_MOVES; move++) {
            face_e cubie0def[3] = {
                cubieDefinitions[cubie0][0],
                cubieDefinitions[cubie0][1],
                cubieDefinitions[cubie0][2],
            };
            if (cubie0def[0] >= 7) {
                printf("Whoops! init_cubieAfterMove goofed up: cubie0def[0] = %hhu\n", cubie0def[0]);
            }
            if (cubie0def[1] >= 7) {
                printf("Whoops! init_cubieAfterMove goofed up: cubie0def[1] = %hhu\n", cubie0def[1]);
            }
            if (cubie0def[2] >= 7) {
                printf("Whoops! init_cubieAfterMove goofed up: cubie0def[2] = %hhu\n", cubie0def[2]);
            }
            if (find_face_in_3(cubie0def[0], cubie0def[1], cubie0def[2], move_faces[move]) == -1) {
                cubieAfterMove[move][cubie0] = cubie0;
            } else {
                face_e cubie1def[3] = {
                    faceAfterMove[cubie0def[0]][move],
                    faceAfterMove[cubie0def[1]][move],
                    faceAfterMove[cubie0def[2]][move],
                };
                if (cubie1def[0] >= 6) {
                    printf("Whoops! init_cubieAfterMove goofed up: cubie1def[0] = %hhu\n", cubie1def[0]);
                }
                if (cubie1def[1] >= 6) {
                    printf("Whoops! init_cubieAfterMove goofed up: cubie1def[1] = %hhu\n", cubie1def[1]);
                }
                if (cubie1def[2] >= 7) {
                    printf("Whoops! init_cubieAfterMove goofed up: cubie1def[2] = %hhu\n", cubie1def[2]);
                }
                cubieAfterMove[move][cubie0] = cubieDefinition_to_cubie[cubie1def[0]][cubie1def[1]][cubie1def[2]];
            }
        }
    } print_cubieAfterMove();
    //printf("cubieAfterMove[U][3][UR] = %s\n", cubiePrints[cubieAfterMove[FACE_U][3][CUBIE_UR]]);
    //print_cubieAfterMove();
}

void print_cubieAfterMove() {
    printf("static const cubie_e cubieAfterMove[NUM_MOVES][NUM_CUBIES] = {\n");
    //printf("\t      U1 , U2 , U3 , R1 , R2 , R3 , F1 , F2 , F3 , L1 , L2 , L3 , B1 , B2 , B3 , D1 , D2 , D3\n");
    for (move_e move = 0; move < NUM_MOVES; move++) {
        printf("\t{");
        for (cubie_e cubie = 0; cubie < 48; cubie++) {
            cubie_e nextcubie = cubieAfterMove[move][cubie];
            printf("CUBIE_%s, ", cubiePrints[nextcubie]);
        } printf("},\n");
    } printf("};\n");
}
*/

cube18B_xcross_s cube18B_xcross_from_cube18B(const cube18B_s* cube) {
    /*
    Tables Used: None
    Functions Used: None
    External Types Used: None
    */
    cube18B_xcross_s xcross_portion = {
        .cubies = {
            cube->cubies[0],
            cube->cubies[1],
            cube->cubies[2],
            cube->cubies[3],
            cube->cubies[4],
            cube->cubies[5],
            cube->cubies[6],
            cube->cubies[7],
            cube->cubies[8],
            cube->cubies[9],
            cube->cubies[10],
            cube->cubies[11]
        }
    }; return xcross_portion;
}

cube18B_1LLL_s cube18B_1LLL_from_cube18B(const cube18B_s* cube) {
    /*
    Tables Used: None
    Functions Used: None
    External Types Used: None
    */
    cube18B_1LLL_s LL_portion = {
        .cubies = {
            cube->cubies[12],
            cube->cubies[13],
            cube->cubies[14],
            cube->cubies[15],
            cube->cubies[16],
            cube->cubies[17]
        }
    }; return LL_portion;
}

cube18B_s cube18B_from_xcross_and_1LLL(const cube18B_xcross_s* xcross, const cube18B_1LLL_s* LL) {
    /*
    Tables Used: None
    Functions Used: None
    External Types Used: None
    */
    cube18B_s cube = {
        .cubies = {
            xcross->cubies[0],
            xcross->cubies[1],
            xcross->cubies[2],
            xcross->cubies[3],
            xcross->cubies[4],
            xcross->cubies[5],
            xcross->cubies[6],
            xcross->cubies[7],
            xcross->cubies[8],
            xcross->cubies[9],
            xcross->cubies[10],
            xcross->cubies[11],
            LL->cubies[0],
            LL->cubies[1],
            LL->cubies[2],
            LL->cubies[3],
            LL->cubies[4],
            LL->cubies[5]
        }
    }; return cube;
}

bool compare_cube18Bs(const cube18B_s* cube1, const cube18B_s* cube2) {
    bool res = true;
    for (int i = 0; i < 18; i++) {
        if (cube1->cubies[i] != cube2->cubies[i]) {
            res = false;
            break;
        }
    } return res;
}

void cube18B_apply_move(cube18B_s* cube, move_e move) {
    /*
    Tables Used: 
        cubieAfterMove[]
    Functions Used: None
    External Types Used: 
        move_s
        face_e
    */
    cube->cubies[0]  = cubieAfterMove[move][cube->cubies[0]];
    cube->cubies[1]  = cubieAfterMove[move][cube->cubies[1]];
    cube->cubies[2]  = cubieAfterMove[move][cube->cubies[2]];
    cube->cubies[3]  = cubieAfterMove[move][cube->cubies[3]];
    cube->cubies[4]  = cubieAfterMove[move][cube->cubies[4]];
    cube->cubies[5]  = cubieAfterMove[move][cube->cubies[5]];
    cube->cubies[6]  = cubieAfterMove[move][cube->cubies[6]];
    cube->cubies[7]  = cubieAfterMove[move][cube->cubies[7]];
    cube->cubies[8]  = cubieAfterMove[move][cube->cubies[8]];
    cube->cubies[9]  = cubieAfterMove[move][cube->cubies[9]];
    cube->cubies[10] = cubieAfterMove[move][cube->cubies[10]];
    cube->cubies[11] = cubieAfterMove[move][cube->cubies[11]];
    cube->cubies[12] = cubieAfterMove[move][cube->cubies[12]];
    cube->cubies[13] = cubieAfterMove[move][cube->cubies[13]];
    cube->cubies[14] = cubieAfterMove[move][cube->cubies[14]];
    cube->cubies[15] = cubieAfterMove[move][cube->cubies[15]];
    cube->cubies[16] = cubieAfterMove[move][cube->cubies[16]];
    cube->cubies[17] = cubieAfterMove[move][cube->cubies[17]];
}

void cube18B_xcross_apply_move(cube18B_xcross_s* cube, move_e move) {
    /*
    Tables Used: 
        cubieAfterMove[]
    Functions Used: None
    External Types Used: 
        move_s
        face_e
    */
    cube->cubies[0]  = cubieAfterMove[move][cube->cubies[0]];
    cube->cubies[1]  = cubieAfterMove[move][cube->cubies[1]];
    cube->cubies[2]  = cubieAfterMove[move][cube->cubies[2]];
    cube->cubies[3]  = cubieAfterMove[move][cube->cubies[3]];
    cube->cubies[4]  = cubieAfterMove[move][cube->cubies[4]];
    cube->cubies[5]  = cubieAfterMove[move][cube->cubies[5]];
    cube->cubies[6]  = cubieAfterMove[move][cube->cubies[6]];
    cube->cubies[7]  = cubieAfterMove[move][cube->cubies[7]];
    cube->cubies[8]  = cubieAfterMove[move][cube->cubies[8]];
    cube->cubies[9]  = cubieAfterMove[move][cube->cubies[9]];
    cube->cubies[10] = cubieAfterMove[move][cube->cubies[10]];
    cube->cubies[11] = cubieAfterMove[move][cube->cubies[11]];
}

void cube18B_1LLL_apply_move(cube18B_1LLL_s* cube, move_e move) {
    /*
    Tables Used: 
        cubieAfterMove[]
    Functions Used: None
    External Types Used: 
        move_s
        face_e
    */
    cube->cubies[0] = cubieAfterMove[move][cube->cubies[0]];
    cube->cubies[1] = cubieAfterMove[move][cube->cubies[1]];
    cube->cubies[2] = cubieAfterMove[move][cube->cubies[2]];
    cube->cubies[3] = cubieAfterMove[move][cube->cubies[3]];
    cube->cubies[4] = cubieAfterMove[move][cube->cubies[4]];
    cube->cubies[5] = cubieAfterMove[move][cube->cubies[5]];
}

void cube18B_apply_alg(cube18B_s *cube, const alg_s *alg) {
    for (size_t i = 0; i < alg->length; i++) {
        cube18B_apply_move(cube, alg->moves[i]);
    }
}

void cube18B_xcross_apply_alg(cube18B_xcross_s *cube, const alg_s *alg) {
    for (size_t i = 0; i < alg->length; i++) {
        cube18B_xcross_apply_move(cube, alg->moves[i]);
    }
}

void cube18B_1LLL_apply_alg(cube18B_1LLL_s *cube, const alg_s *alg) {
    for (size_t i = 0; i < alg->length; i++) {
        cube18B_1LLL_apply_move(cube, alg->moves[i]);
    }
}

cubie_e apply_alg_to_cubie(cubie_e cubie, const alg_s* alg) {
    for (int i = 0; i < alg->length; i++) {
        cubie = cubieAfterMove[alg->moves[i]][cubie];
    } return cubie;
}

cubieTable_s alg_to_cubieTable(const alg_s* alg) {
    cubieTable_s table;
    for (cubie_e i = 0; i < NUM_CUBIES; i++) {
        table.cubieShift[i] = apply_alg_to_cubie(i, alg);
    } return table;
}

void apply_cubieTable_to_cube(cube18B_s* cube, const cubieTable_s* table) {
    for (int i = 0; i < 18; i++) {
        cube->cubies[i] = table->cubieShift[cube->cubies[i]];
    }
}

cubieTable_s conjoin_cubeTables(const cubieTable_s* table1, const cubieTable_s* table2) {
    cubieTable_s table;
    for (cubie_e i = 0; i < NUM_CUBIES; i++) {
        table.cubieShift[i] = table2->cubieShift[table1->cubieShift[i]];
    } return table;
}