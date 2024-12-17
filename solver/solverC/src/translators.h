#ifndef TRANSLATORS_H
#define TRANSLATORS_H

#include "cube18B.h"
#include "shift_cube.h"
#include "lookup_tables.h"
#include "main.h"
#include <stdbool.h>


static const facelet_pos_s NULL_POS = {
    FACE_NULL, 8
};

static bool facelet_pos_eq(facelet_pos_s pos1, facelet_pos_s pos2) {
    return (pos1.index == pos2.index && pos1.face == pos2.face);
}

static face_e facelet_at_facelet_pos(const shift_cube_s* cube, facelet_pos_s pos) {
    if (facelet_pos_eq(pos, NULL_POS)) return FACE_NULL;
    return (((cube->state[pos.face])>>(4*pos.index))&0xF);
}

static const uint8_t colorsAtEdgePosInd_to_cubieAndSolvedCubie[2][24][NUM_EDGES] = {
	{
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
		{CUBIE_UB , CUBIE_UR , CUBIE_UF , CUBIE_UL , CUBIE_BR , CUBIE_RF , CUBIE_FL , CUBIE_LB , CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , },
		{CUBIE_BU , CUBIE_RU , CUBIE_FU , CUBIE_LU , CUBIE_RB , CUBIE_FR , CUBIE_LF , CUBIE_BL , CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , },
	},
	{
		{13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, },
		{13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, },
		{12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, },
		{12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, },
		{18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, },
		{18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, },
		{14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, },
		{14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, },
		{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, },
		{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, },
		{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, },
		{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, },
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, },
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, },
		{10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, },
		{10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, },
		{8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, },
		{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, },
		{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, },
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },
	},
};
static const uint8_t colorsAtCornerPosInd_to_cubieAndSolvedCubie[2][48][NUM_CORNERS] = {
	{
		{CUBIE_LBU, CUBIE_RBU, CUBIE_RFU, CUBIE_FLU, CUBIE_FLD, CUBIE_RFD, CUBIE_RBD, CUBIE_LBD, },
		{CUBIE_ULB, CUBIE_URB, CUBIE_URF, CUBIE_UFL, CUBIE_DFL, CUBIE_DRF, CUBIE_DRB, CUBIE_DLB, },
		{CUBIE_BUL, CUBIE_BUR, CUBIE_FUR, CUBIE_LUF, CUBIE_LDF, CUBIE_FDR, CUBIE_BDR, CUBIE_BDL, },
		{CUBIE_LBU, CUBIE_RBU, CUBIE_RFU, CUBIE_FLU, CUBIE_FLD, CUBIE_RFD, CUBIE_RBD, CUBIE_LBD, },
		{CUBIE_ULB, CUBIE_URB, CUBIE_URF, CUBIE_UFL, CUBIE_DFL, CUBIE_DRF, CUBIE_DRB, CUBIE_DLB, },
		{CUBIE_BUL, CUBIE_BUR, CUBIE_FUR, CUBIE_LUF, CUBIE_LDF, CUBIE_FDR, CUBIE_BDR, CUBIE_BDL, },
		{CUBIE_LBU, CUBIE_RBU, CUBIE_RFU, CUBIE_FLU, CUBIE_FLD, CUBIE_RFD, CUBIE_RBD, CUBIE_LBD, },
		{CUBIE_ULB, CUBIE_URB, CUBIE_URF, CUBIE_UFL, CUBIE_DFL, CUBIE_DRF, CUBIE_DRB, CUBIE_DLB, },
		{CUBIE_BUL, CUBIE_BUR, CUBIE_FUR, CUBIE_LUF, CUBIE_LDF, CUBIE_FDR, CUBIE_BDR, CUBIE_BDL, },
		{CUBIE_LBU, CUBIE_RBU, CUBIE_RFU, CUBIE_FLU, CUBIE_FLD, CUBIE_RFD, CUBIE_RBD, CUBIE_LBD, },
		{CUBIE_ULB, CUBIE_URB, CUBIE_URF, CUBIE_UFL, CUBIE_DFL, CUBIE_DRF, CUBIE_DRB, CUBIE_DLB, },
		{CUBIE_BUL, CUBIE_BUR, CUBIE_FUR, CUBIE_LUF, CUBIE_LDF, CUBIE_FDR, CUBIE_BDR, CUBIE_BDL, },
		{CUBIE_ULB, CUBIE_URB, CUBIE_URF, CUBIE_UFL, CUBIE_DFL, CUBIE_DRF, CUBIE_DRB, CUBIE_DLB, },
		{CUBIE_LBU, CUBIE_RBU, CUBIE_RFU, CUBIE_FLU, CUBIE_FLD, CUBIE_RFD, CUBIE_RBD, CUBIE_LBD, },
		{CUBIE_BUL, CUBIE_BUR, CUBIE_FUR, CUBIE_LUF, CUBIE_LDF, CUBIE_FDR, CUBIE_BDR, CUBIE_BDL, },
		{CUBIE_ULB, CUBIE_URB, CUBIE_URF, CUBIE_UFL, CUBIE_DFL, CUBIE_DRF, CUBIE_DRB, CUBIE_DLB, },
		{CUBIE_LBU, CUBIE_RBU, CUBIE_RFU, CUBIE_FLU, CUBIE_FLD, CUBIE_RFD, CUBIE_RBD, CUBIE_LBD, },
		{CUBIE_BUL, CUBIE_BUR, CUBIE_FUR, CUBIE_LUF, CUBIE_LDF, CUBIE_FDR, CUBIE_BDR, CUBIE_BDL, },
		{CUBIE_ULB, CUBIE_URB, CUBIE_URF, CUBIE_UFL, CUBIE_DFL, CUBIE_DRF, CUBIE_DRB, CUBIE_DLB, },
		{CUBIE_LBU, CUBIE_RBU, CUBIE_RFU, CUBIE_FLU, CUBIE_FLD, CUBIE_RFD, CUBIE_RBD, CUBIE_LBD, },
		{CUBIE_BUL, CUBIE_BUR, CUBIE_FUR, CUBIE_LUF, CUBIE_LDF, CUBIE_FDR, CUBIE_BDR, CUBIE_BDL, },
		{CUBIE_ULB, CUBIE_URB, CUBIE_URF, CUBIE_UFL, CUBIE_DFL, CUBIE_DRF, CUBIE_DRB, CUBIE_DLB, },
		{CUBIE_LBU, CUBIE_RBU, CUBIE_RFU, CUBIE_FLU, CUBIE_FLD, CUBIE_RFD, CUBIE_RBD, CUBIE_LBD, },
		{CUBIE_BUL, CUBIE_BUR, CUBIE_FUR, CUBIE_LUF, CUBIE_LDF, CUBIE_FDR, CUBIE_BDR, CUBIE_BDL, },
		{CUBIE_BLU, CUBIE_BRU, CUBIE_FRU, CUBIE_LFU, CUBIE_LFD, CUBIE_FRD, CUBIE_BRD, CUBIE_BLD, },
		{CUBIE_UBL, CUBIE_UBR, CUBIE_UFR, CUBIE_ULF, CUBIE_DLF, CUBIE_DFR, CUBIE_DBR, CUBIE_DBL, },
		{CUBIE_LUB, CUBIE_RUB, CUBIE_RUF, CUBIE_FUL, CUBIE_FDL, CUBIE_RDF, CUBIE_RDB, CUBIE_LDB, },
		{CUBIE_BLU, CUBIE_BRU, CUBIE_FRU, CUBIE_LFU, CUBIE_LFD, CUBIE_FRD, CUBIE_BRD, CUBIE_BLD, },
		{CUBIE_UBL, CUBIE_UBR, CUBIE_UFR, CUBIE_ULF, CUBIE_DLF, CUBIE_DFR, CUBIE_DBR, CUBIE_DBL, },
		{CUBIE_LUB, CUBIE_RUB, CUBIE_RUF, CUBIE_FUL, CUBIE_FDL, CUBIE_RDF, CUBIE_RDB, CUBIE_LDB, },
		{CUBIE_BLU, CUBIE_BRU, CUBIE_FRU, CUBIE_LFU, CUBIE_LFD, CUBIE_FRD, CUBIE_BRD, CUBIE_BLD, },
		{CUBIE_UBL, CUBIE_UBR, CUBIE_UFR, CUBIE_ULF, CUBIE_DLF, CUBIE_DFR, CUBIE_DBR, CUBIE_DBL, },
		{CUBIE_LUB, CUBIE_RUB, CUBIE_RUF, CUBIE_FUL, CUBIE_FDL, CUBIE_RDF, CUBIE_RDB, CUBIE_LDB, },
		{CUBIE_BLU, CUBIE_BRU, CUBIE_FRU, CUBIE_LFU, CUBIE_LFD, CUBIE_FRD, CUBIE_BRD, CUBIE_BLD, },
		{CUBIE_UBL, CUBIE_UBR, CUBIE_UFR, CUBIE_ULF, CUBIE_DLF, CUBIE_DFR, CUBIE_DBR, CUBIE_DBL, },
		{CUBIE_LUB, CUBIE_RUB, CUBIE_RUF, CUBIE_FUL, CUBIE_FDL, CUBIE_RDF, CUBIE_RDB, CUBIE_LDB, },
		{CUBIE_UBL, CUBIE_UBR, CUBIE_UFR, CUBIE_ULF, CUBIE_DLF, CUBIE_DFR, CUBIE_DBR, CUBIE_DBL, },
		{CUBIE_BLU, CUBIE_BRU, CUBIE_FRU, CUBIE_LFU, CUBIE_LFD, CUBIE_FRD, CUBIE_BRD, CUBIE_BLD, },
		{CUBIE_LUB, CUBIE_RUB, CUBIE_RUF, CUBIE_FUL, CUBIE_FDL, CUBIE_RDF, CUBIE_RDB, CUBIE_LDB, },
		{CUBIE_UBL, CUBIE_UBR, CUBIE_UFR, CUBIE_ULF, CUBIE_DLF, CUBIE_DFR, CUBIE_DBR, CUBIE_DBL, },
		{CUBIE_BLU, CUBIE_BRU, CUBIE_FRU, CUBIE_LFU, CUBIE_LFD, CUBIE_FRD, CUBIE_BRD, CUBIE_BLD, },
		{CUBIE_LUB, CUBIE_RUB, CUBIE_RUF, CUBIE_FUL, CUBIE_FDL, CUBIE_RDF, CUBIE_RDB, CUBIE_LDB, },
		{CUBIE_UBL, CUBIE_UBR, CUBIE_UFR, CUBIE_ULF, CUBIE_DLF, CUBIE_DFR, CUBIE_DBR, CUBIE_DBL, },
		{CUBIE_BLU, CUBIE_BRU, CUBIE_FRU, CUBIE_LFU, CUBIE_LFD, CUBIE_FRD, CUBIE_BRD, CUBIE_BLD, },
		{CUBIE_LUB, CUBIE_RUB, CUBIE_RUF, CUBIE_FUL, CUBIE_FDL, CUBIE_RDF, CUBIE_RDB, CUBIE_LDB, },
		{CUBIE_UBL, CUBIE_UBR, CUBIE_UFR, CUBIE_ULF, CUBIE_DLF, CUBIE_DFR, CUBIE_DBR, CUBIE_DBL, },
		{CUBIE_BLU, CUBIE_BRU, CUBIE_FRU, CUBIE_LFU, CUBIE_LFD, CUBIE_FRD, CUBIE_BRD, CUBIE_BLD, },
		{CUBIE_LUB, CUBIE_RUB, CUBIE_RUF, CUBIE_FUL, CUBIE_FDL, CUBIE_RDF, CUBIE_RDB, CUBIE_LDB, },
	},
	{
		{15, 15, 15, 15, 15, 15, 15, 15, },
		{15, 15, 15, 15, 15, 15, 15, 15, },
		{15, 15, 15, 15, 15, 15, 15, 15, },
		{19, 19, 19, 19, 19, 19, 19, 19, },
		{19, 19, 19, 19, 19, 19, 19, 19, },
		{19, 19, 19, 19, 19, 19, 19, 19, },
		{17, 17, 17, 17, 17, 17, 17, 17, },
		{17, 17, 17, 17, 17, 17, 17, 17, },
		{17, 17, 17, 17, 17, 17, 17, 17, },
		{16, 16, 16, 16, 16, 16, 16, 16, },
		{16, 16, 16, 16, 16, 16, 16, 16, },
		{16, 16, 16, 16, 16, 16, 16, 16, },
		{5, 5, 5, 5, 5, 5, 5, 5, },
		{5, 5, 5, 5, 5, 5, 5, 5, },
		{5, 5, 5, 5, 5, 5, 5, 5, },
		{11, 11, 11, 11, 11, 11, 11, 11, },
		{11, 11, 11, 11, 11, 11, 11, 11, },
		{11, 11, 11, 11, 11, 11, 11, 11, },
		{9, 9, 9, 9, 9, 9, 9, 9, },
		{9, 9, 9, 9, 9, 9, 9, 9, },
		{9, 9, 9, 9, 9, 9, 9, 9, },
		{7, 7, 7, 7, 7, 7, 7, 7, },
		{7, 7, 7, 7, 7, 7, 7, 7, },
		{7, 7, 7, 7, 7, 7, 7, 7, },
		{15, 15, 15, 15, 15, 15, 15, 15, },
		{15, 15, 15, 15, 15, 15, 15, 15, },
		{15, 15, 15, 15, 15, 15, 15, 15, },
		{19, 19, 19, 19, 19, 19, 19, 19, },
		{19, 19, 19, 19, 19, 19, 19, 19, },
		{19, 19, 19, 19, 19, 19, 19, 19, },
		{17, 17, 17, 17, 17, 17, 17, 17, },
		{17, 17, 17, 17, 17, 17, 17, 17, },
		{17, 17, 17, 17, 17, 17, 17, 17, },
		{16, 16, 16, 16, 16, 16, 16, 16, },
		{16, 16, 16, 16, 16, 16, 16, 16, },
		{16, 16, 16, 16, 16, 16, 16, 16, },
		{5, 5, 5, 5, 5, 5, 5, 5, },
		{5, 5, 5, 5, 5, 5, 5, 5, },
		{5, 5, 5, 5, 5, 5, 5, 5, },
		{11, 11, 11, 11, 11, 11, 11, 11, },
		{11, 11, 11, 11, 11, 11, 11, 11, },
		{11, 11, 11, 11, 11, 11, 11, 11, },
		{9, 9, 9, 9, 9, 9, 9, 9, },
		{9, 9, 9, 9, 9, 9, 9, 9, },
		{9, 9, 9, 9, 9, 9, 9, 9, },
		{7, 7, 7, 7, 7, 7, 7, 7, },
		{7, 7, 7, 7, 7, 7, 7, 7, },
		{7, 7, 7, 7, 7, 7, 7, 7, },
	},
};
static const uint8_t cubieAndSolvedCubieInd_to_colorsAtPosInd[2][48][20] = {
	{
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_RDF, CUBIE_RB , CUBIE_BDR, CUBIE_BL , CUBIE_LDB, CUBIE_LF , CUBIE_FDL, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_RFU, CUBIE_BRU, CUBIE_LBU, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_FRD, CUBIE_BR , CUBIE_RBD, CUBIE_LB , CUBIE_BLD, CUBIE_FL , CUBIE_LFD, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_URF, CUBIE_UBR, CUBIE_ULB, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_DFR, CUBIE_RB , CUBIE_DRB, CUBIE_BL , CUBIE_DBL, CUBIE_LF , CUBIE_DLF, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_FUR, CUBIE_RUB, CUBIE_BUL, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_RDF, CUBIE_BR , CUBIE_BDR, CUBIE_LB , CUBIE_LDB, CUBIE_FL , CUBIE_FDL, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_RFU, CUBIE_BRU, CUBIE_LBU, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_FRD, CUBIE_RB , CUBIE_RBD, CUBIE_BL , CUBIE_BLD, CUBIE_LF , CUBIE_LFD, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_URF, CUBIE_UBR, CUBIE_ULB, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_DFR, CUBIE_BR , CUBIE_DRB, CUBIE_LB , CUBIE_DBL, CUBIE_FL , CUBIE_DLF, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_FUR, CUBIE_RUB, CUBIE_BUL, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_RDF, CUBIE_RB , CUBIE_BDR, CUBIE_BL , CUBIE_LDB, CUBIE_LF , CUBIE_FDL, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_RFU, CUBIE_BRU, CUBIE_LBU, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_FRD, CUBIE_BR , CUBIE_RBD, CUBIE_LB , CUBIE_BLD, CUBIE_FL , CUBIE_LFD, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_URF, CUBIE_UBR, CUBIE_ULB, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_DFR, CUBIE_RB , CUBIE_DRB, CUBIE_BL , CUBIE_DBL, CUBIE_LF , CUBIE_DLF, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_FUR, CUBIE_RUB, CUBIE_BUL, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_RFD, CUBIE_BR , CUBIE_BRD, CUBIE_LB , CUBIE_LBD, CUBIE_FL , CUBIE_FLD, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_RUF, CUBIE_BUR, CUBIE_LUB, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_FDR, CUBIE_BR , CUBIE_RDB, CUBIE_LB , CUBIE_BDL, CUBIE_FL , CUBIE_LDF, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_UFR, CUBIE_URB, CUBIE_UBL, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_DRF, CUBIE_RB , CUBIE_DBR, CUBIE_BL , CUBIE_DLB, CUBIE_LF , CUBIE_DFL, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_FRU, CUBIE_RBU, CUBIE_BLU, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_DRF, CUBIE_BR , CUBIE_DBR, CUBIE_LB , CUBIE_DLB, CUBIE_FL , CUBIE_DFL, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_FRU, CUBIE_RBU, CUBIE_BLU, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_FDR, CUBIE_RB , CUBIE_RDB, CUBIE_BL , CUBIE_BDL, CUBIE_LF , CUBIE_LDF, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_UFR, CUBIE_URB, CUBIE_UBL, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_RFD, CUBIE_RB , CUBIE_BRD, CUBIE_BL , CUBIE_LBD, CUBIE_LF , CUBIE_FLD, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_RUF, CUBIE_BUR, CUBIE_LUB, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_DRF, CUBIE_BR , CUBIE_DBR, CUBIE_LB , CUBIE_DLB, CUBIE_FL , CUBIE_DFL, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_FRU, CUBIE_RBU, CUBIE_BLU, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_FDR, CUBIE_BR , CUBIE_RDB, CUBIE_LB , CUBIE_BDL, CUBIE_FL , CUBIE_LDF, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_UFR, CUBIE_URB, CUBIE_UBL, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_RFD, CUBIE_RB , CUBIE_BRD, CUBIE_BL , CUBIE_LBD, CUBIE_LF , CUBIE_FLD, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_RUF, CUBIE_BUR, CUBIE_LUB, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_DRF, CUBIE_RB , CUBIE_DBR, CUBIE_BL , CUBIE_DLB, CUBIE_LF , CUBIE_DFL, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_FRU, CUBIE_RBU, CUBIE_BLU, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_FDR, CUBIE_BR , CUBIE_RDB, CUBIE_LB , CUBIE_BDL, CUBIE_FL , CUBIE_LDF, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_UFR, CUBIE_URB, CUBIE_UBL, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_RFD, CUBIE_BR , CUBIE_BRD, CUBIE_LB , CUBIE_LBD, CUBIE_FL , CUBIE_FLD, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_RUF, CUBIE_BUR, CUBIE_LUB, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_DFR, CUBIE_RB , CUBIE_DRB, CUBIE_BL , CUBIE_DBL, CUBIE_LF , CUBIE_DLF, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_FUR, CUBIE_RUB, CUBIE_BUL, },
		{CUBIE_DF , CUBIE_DR , CUBIE_DB , CUBIE_DL , CUBIE_RF , CUBIE_FRD, CUBIE_BR , CUBIE_RBD, CUBIE_LB , CUBIE_BLD, CUBIE_FL , CUBIE_LFD, CUBIE_UF , CUBIE_UR , CUBIE_UB , CUBIE_URF, CUBIE_UBR, CUBIE_ULB, },
		{CUBIE_FD , CUBIE_RD , CUBIE_BD , CUBIE_LD , CUBIE_FR , CUBIE_RDF, CUBIE_RB , CUBIE_BDR, CUBIE_BL , CUBIE_LDB, CUBIE_LF , CUBIE_FDL, CUBIE_FU , CUBIE_RU , CUBIE_BU , CUBIE_RFU, CUBIE_BRU, CUBIE_LBU, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_DRF, CUBIE_UR , CUBIE_DBR, CUBIE_UR , CUBIE_DLB, CUBIE_UR , CUBIE_DFL, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FRU, CUBIE_RBU, CUBIE_BLU, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FDR, CUBIE_UR , CUBIE_RDB, CUBIE_UR , CUBIE_BDL, CUBIE_UR , CUBIE_LDF, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UFR, CUBIE_URB, CUBIE_UBL, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RFD, CUBIE_UR , CUBIE_BRD, CUBIE_UR , CUBIE_LBD, CUBIE_UR , CUBIE_FLD, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RUF, CUBIE_BUR, CUBIE_LUB, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_DRF, CUBIE_UR , CUBIE_DBR, CUBIE_UR , CUBIE_DLB, CUBIE_UR , CUBIE_DFL, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FRU, CUBIE_RBU, CUBIE_BLU, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FDR, CUBIE_UR , CUBIE_RDB, CUBIE_UR , CUBIE_BDL, CUBIE_UR , CUBIE_LDF, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UFR, CUBIE_URB, CUBIE_UBL, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RFD, CUBIE_UR , CUBIE_BRD, CUBIE_UR , CUBIE_LBD, CUBIE_UR , CUBIE_FLD, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RUF, CUBIE_BUR, CUBIE_LUB, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_DRF, CUBIE_UR , CUBIE_DBR, CUBIE_UR , CUBIE_DLB, CUBIE_UR , CUBIE_DFL, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FRU, CUBIE_RBU, CUBIE_BLU, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FDR, CUBIE_UR , CUBIE_RDB, CUBIE_UR , CUBIE_BDL, CUBIE_UR , CUBIE_LDF, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UFR, CUBIE_URB, CUBIE_UBL, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RFD, CUBIE_UR , CUBIE_BRD, CUBIE_UR , CUBIE_LBD, CUBIE_UR , CUBIE_FLD, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RUF, CUBIE_BUR, CUBIE_LUB, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_DFR, CUBIE_UR , CUBIE_DRB, CUBIE_UR , CUBIE_DBL, CUBIE_UR , CUBIE_DLF, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FUR, CUBIE_RUB, CUBIE_BUL, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FRD, CUBIE_UR , CUBIE_RBD, CUBIE_UR , CUBIE_BLD, CUBIE_UR , CUBIE_LFD, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_URF, CUBIE_UBR, CUBIE_ULB, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RDF, CUBIE_UR , CUBIE_BDR, CUBIE_UR , CUBIE_LDB, CUBIE_UR , CUBIE_FDL, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RFU, CUBIE_BRU, CUBIE_LBU, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RDF, CUBIE_UR , CUBIE_BDR, CUBIE_UR , CUBIE_LDB, CUBIE_UR , CUBIE_FDL, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RFU, CUBIE_BRU, CUBIE_LBU, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FRD, CUBIE_UR , CUBIE_RBD, CUBIE_UR , CUBIE_BLD, CUBIE_UR , CUBIE_LFD, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_URF, CUBIE_UBR, CUBIE_ULB, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_DFR, CUBIE_UR , CUBIE_DRB, CUBIE_UR , CUBIE_DBL, CUBIE_UR , CUBIE_DLF, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FUR, CUBIE_RUB, CUBIE_BUL, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RDF, CUBIE_UR , CUBIE_BDR, CUBIE_UR , CUBIE_LDB, CUBIE_UR , CUBIE_FDL, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RFU, CUBIE_BRU, CUBIE_LBU, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FRD, CUBIE_UR , CUBIE_RBD, CUBIE_UR , CUBIE_BLD, CUBIE_UR , CUBIE_LFD, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_URF, CUBIE_UBR, CUBIE_ULB, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_DFR, CUBIE_UR , CUBIE_DRB, CUBIE_UR , CUBIE_DBL, CUBIE_UR , CUBIE_DLF, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FUR, CUBIE_RUB, CUBIE_BUL, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RDF, CUBIE_UR , CUBIE_BDR, CUBIE_UR , CUBIE_LDB, CUBIE_UR , CUBIE_FDL, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RFU, CUBIE_BRU, CUBIE_LBU, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FRD, CUBIE_UR , CUBIE_RBD, CUBIE_UR , CUBIE_BLD, CUBIE_UR , CUBIE_LFD, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_URF, CUBIE_UBR, CUBIE_ULB, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_DFR, CUBIE_UR , CUBIE_DRB, CUBIE_UR , CUBIE_DBL, CUBIE_UR , CUBIE_DLF, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FUR, CUBIE_RUB, CUBIE_BUL, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RFD, CUBIE_UR , CUBIE_BRD, CUBIE_UR , CUBIE_LBD, CUBIE_UR , CUBIE_FLD, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_RUF, CUBIE_BUR, CUBIE_LUB, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FDR, CUBIE_UR , CUBIE_RDB, CUBIE_UR , CUBIE_BDL, CUBIE_UR , CUBIE_LDF, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UFR, CUBIE_URB, CUBIE_UBL, },
		{CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_DRF, CUBIE_UR , CUBIE_DBR, CUBIE_UR , CUBIE_DLB, CUBIE_UR , CUBIE_DFL, CUBIE_UR , CUBIE_UR , CUBIE_UR , CUBIE_FRU, CUBIE_RBU, CUBIE_BLU, },
	},
	{
		{1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 1, 2, 2, 2, },
		{1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 1, 2, 2, 2, },
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, },
		{2, 2, 2, 2, 2, 3, 2, 3, 2, 3, 2, 3, 2, 2, 2, 3, 3, 3, },
		{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, },
		{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{5, 5, 5, 5, 5, 0, 5, 0, 5, 0, 5, 0, 5, 5, 5, 0, 0, 0, },
		{5, 5, 5, 5, 5, 1, 5, 1, 5, 1, 5, 1, 5, 5, 5, 1, 1, 1, },
		{4, 4, 4, 4, 4, 1, 4, 1, 4, 1, 4, 1, 4, 4, 4, 1, 1, 1, },
		{4, 4, 4, 4, 4, 1, 4, 1, 4, 1, 4, 1, 4, 4, 4, 1, 1, 1, },
		{9, 9, 9, 9, 9, 5, 9, 5, 9, 5, 9, 5, 9, 9, 9, 5, 5, 5, },
		{9, 9, 9, 9, 9, 5, 9, 5, 9, 5, 9, 5, 9, 9, 9, 5, 5, 5, },
		{6, 6, 6, 6, 6, 5, 6, 5, 6, 5, 6, 5, 6, 6, 6, 5, 5, 5, },
		{6, 6, 6, 6, 6, 4, 6, 4, 6, 4, 6, 4, 6, 6, 6, 4, 4, 4, },
		{8, 8, 8, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 8, 8, 4, 4, 4, },
		{8, 8, 8, 8, 8, 4, 8, 4, 8, 4, 8, 4, 8, 8, 8, 4, 4, 4, },
		{7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, },
		{7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, },
		{11, 11, 11, 11, 11, 7, 11, 7, 11, 7, 11, 7, 11, 11, 11, 7, 7, 7, },
		{11, 11, 11, 11, 11, 6, 11, 6, 11, 6, 11, 6, 11, 11, 11, 6, 6, 6, },
		{10, 10, 10, 10, 10, 6, 10, 6, 10, 6, 10, 6, 10, 10, 10, 6, 6, 6, },
		{10, 10, 10, 10, 10, 6, 10, 6, 10, 6, 10, 6, 10, 10, 10, 6, 6, 6, },
		{0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 2, 2, 2, },
		{0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 2, 2, 2, },
		{0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 2, 2, 2, },
		{0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 3, 3, 3, },
		{0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 3, 3, 3, },
		{0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 3, 3, 3, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, },
		{0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, },
		{0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, },
		{0, 0, 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 0, 0, 5, 5, 5, },
		{0, 0, 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 0, 0, 5, 5, 5, },
		{0, 0, 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 0, 0, 5, 5, 5, },
		{0, 0, 0, 0, 0, 4, 0, 4, 0, 4, 0, 4, 0, 0, 0, 4, 4, 4, },
		{0, 0, 0, 0, 0, 4, 0, 4, 0, 4, 0, 4, 0, 0, 0, 4, 4, 4, },
		{0, 0, 0, 0, 0, 4, 0, 4, 0, 4, 0, 4, 0, 0, 0, 4, 4, 4, },
		{0, 0, 0, 0, 0, 7, 0, 7, 0, 7, 0, 7, 0, 0, 0, 7, 7, 7, },
		{0, 0, 0, 0, 0, 7, 0, 7, 0, 7, 0, 7, 0, 0, 0, 7, 7, 7, },
		{0, 0, 0, 0, 0, 7, 0, 7, 0, 7, 0, 7, 0, 0, 0, 7, 7, 7, },
		{0, 0, 0, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 0, 6, 6, 6, },
		{0, 0, 0, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 0, 6, 6, 6, },
		{0, 0, 0, 0, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 0, 6, 6, 6, },
	},
};

/*
// mainly for cube18B_from_shiftcube()
static void init_colorsAtEdgePosInd_to_cubieAndSolvedCubie() {
    
    Tables Used:
        cubieDefinitions[]
        colorSequence_to_solvedCubieInd[]
        SOLVED_CUBIES[]
        edge_pieces[]
        cubieDefinition_to_cubie[]
    Functions Used:
        find_face_in_3()
    External Types Used:
        face_e
        facelet_pos_s
    
    //printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    for (int row = 0; row < NUM_EDGES; row++) { //*********
        for (cubie_e colors = 0; colors < 24; colors++) { //*********
            face_e colorsArr[3] = {
                cubieDefinitions[colors][0],
                cubieDefinitions[colors][1],
                cubieDefinitions[colors][2],
            };
            if (colorsArr[0] >= 7) {
                printf("Whoops! init_colorsAtEdgePosInd_to_cubieAndSolvedCubie goofed up: colorsArr[0] = %hhu\n", colorsArr[0]);
            }
            if (colorsArr[1] >= 7) {
                printf("Whoops! init_colorsAtEdgePosInd_to_cubieAndSolvedCubie goofed up: colorsArr[1] = %hhu\n", colorsArr[1]);
            }
            if (colorsArr[2] >= 7) {
                printf("Whoops! init_colorsAtEdgePosInd_to_cubieAndSolvedCubie goofed up: colorsArr[2] = %hhu\n", colorsArr[2]);
            }
            uint8_t solved_cubieInd = colorSequence_to_solvedCubieInd[colorsArr[0]][colorsArr[1]][colorsArr[2]]; //*********
            if (solved_cubieInd >= 20) {
                printf("Whoops! init_colorsAtEdgePosInd_to_cubieAndSolvedCubie goofed up: solved_cubieInd = %hhu\n", solved_cubieInd);
            }
            //if (colors == CUBIE_UL) {
            //    printf("Solved cubie ind:%hhu\tColors %s\n", solved_cubieInd, cubiePrints[colors]);
            //}
            cubie_e solvedCubie = SOLVED_CUBIES[solved_cubieInd];
            face_e solvedCubieDef[3] = {
                cubieDefinitions[solvedCubie][0],
                cubieDefinitions[solvedCubie][1],
                cubieDefinitions[solvedCubie][2],
            };
            //-------------------------------------------------
            facelet_pos_s edgeLocs[2] = {
                edge_pieces[row][0],
                edge_pieces[row][1],
            };
            face_e PosCubieDef[3] = {edgeLocs[0].face, edgeLocs[1].face, FACE_NULL};
            //-------------------------------------------------
            
            The colors on the colorsArr, will likely be a different ordering than the same colors on solvedCubieDef.
            The ordering of the colors in solvedCubieDef is the same ordering as the faces that hold those colors on CubieDef.
            The textbook defintion of a solvedCubie, is a cubie such that the facesArr is exactly the same as the colorsArr.
                PosCubie: [L, B, D]   <-->         Cubie: [D, B, L]
                           |  |  |                         |  |  |
                  Colors: [B, L, U]   <-->   solvedCubie: [U, L, B]
            
            face_e cubieDef[3];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[0])] = PosCubieDef[0];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[1])] = PosCubieDef[1];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[2])] = PosCubieDef[2];
            if (cubieDef[0] >= 6) {
                printf("Whoops! init_colorsAtEdgePosInd_to_cubieAndSolvedCubie goofed up: cubieDef[0] = %hhu\n", cubieDef[0]);
            }
            if (cubieDef[1] >= 6) {
                printf("Whoops! init_colorsAtEdgePosInd_to_cubieAndSolvedCubie goofed up: cubieDef[1] = %hhu\n", cubieDef[1]);
            }
            if (cubieDef[2] >= 7) {
                printf("Whoops! init_colorsAtEdgePosInd_to_cubieAndSolvedCubie goofed up: cubieDef[2] = %hhu\n", cubieDef[2]);
            }

            cubie_e cubie = cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2]]; //*********

            colorsAtEdgePosInd_to_cubieAndSolvedCubie[0][colors][row] = cubie;
            colorsAtEdgePosInd_to_cubieAndSolvedCubie[1][colors][row] = solved_cubieInd;
        }
    }
    print_colorsAtEdgePosInd_to_cubieAndSolvedCubie();
}

void print_colorsAtEdgePosInd_to_cubieAndSolvedCubie() {
    printf("static const uint8_t colorsAtEdgePosInd_to_cubieAndSolvedCubie[2][24][NUM_EDGES] = {\n");
    printf("\t{\n");
    for (cubie_e color = 0; color < 24; color++) {
        printf("\t\t{");
        for (uint8_t edgePosInd = 0; edgePosInd < NUM_EDGES; edgePosInd++) {
            cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[0][color][edgePosInd];
            //uint8_t solvedCubieInd  = colorsAtEdgePosInd_to_cubieAndSolvedCubie[1][color][edgePosInd];
            printf("CUBIE_%s, ", cubiePrints[cubie]);
            //cubie_e solved_cubie = SOLVED_CUBIES[solvedCubieInd];
            //facelet_pos_s edge[2] = {
            //    edge_pieces[edgePosInd][0],
            //    edge_pieces[edgePosInd][1],
            //};
            //face_e colorDef[3] = {
            //    cubieDefinitions[color][0],
            //    cubieDefinitions[color][1],
            //    FACE_NULL,
            //};
            //printf("\t<color>(%c, %c, N), <Pos>(%c, %c, N) -> <solved>%s at <cubie>%s\n", 
            //    facePrints[colorDef[0]], 
            //    facePrints[colorDef[1]], 
            //    facePrints[edge[0].face], 
            //    facePrints[edge[1].face], 
            //    cubiePrints[solved_cubie], 
            //    cubiePrints[cubie]
            //);
        } printf("},\n");
    } printf("\t},\n");
    printf("\t{\n");
    for (cubie_e color = 0; color < 24; color++) {
        printf("\t\t{");
        for (uint8_t edgePosInd = 0; edgePosInd < NUM_EDGES; edgePosInd++) {
            //cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[0][color][edgePosInd];
            uint8_t solvedCubieInd  = colorsAtEdgePosInd_to_cubieAndSolvedCubie[1][color][edgePosInd];
            printf("%hhu, ", solvedCubieInd);
            //cubie_e solved_cubie = SOLVED_CUBIES[solvedCubieInd];
            //facelet_pos_s edge[2] = {
            //    edge_pieces[edgePosInd][0],
            //    edge_pieces[edgePosInd][1],
            //};
            //face_e colorDef[3] = {
            //    cubieDefinitions[color][0],
            //    cubieDefinitions[color][1],
            //    FACE_NULL,
            //};
            //printf("\t<color>(%c, %c, N), <Pos>(%c, %c, N) -> <solved>%s at <cubie>%s\n", 
            //    facePrints[colorDef[0]], 
            //    facePrints[colorDef[1]], 
            //    facePrints[edge[0].face], 
            //    facePrints[edge[1].face], 
            //    cubiePrints[solved_cubie], 
            //    cubiePrints[cubie]
            //);
        } printf("},\n");
    } printf("\t},\n");
    printf("};\n");
}*/
/*
// mainly for cube18B_from_shiftcube()
static void init_colorsAtCornerPosInd_to_cubieAndSolvedCubie() {
    
    //Tables Used:
    //    cubieDefinitions[]
    //    colorSequence_to_solvedCubieInd[]
    //    SOLVED_CUBIES[]
    //    corner_pieces[]
    //    cubieDefinition_to_cubie[]
    //Functions Used:
    //    find_face_in_3()
    //External Types Used:
    //    face_e
    //    facelet_pos_s

    //printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    //<color>(U, F, R), <Pos>(U, R, F) -> <cubie>UFR of <solved>URF

    for (cubie_e colors = 24; colors < 72; colors++) {
        face_e colorsArr[3] = {
            cubieDefinitions[colors][0],
            cubieDefinitions[colors][1],
            cubieDefinitions[colors][2],
        };
        if (colorsArr[0] >= 7) {
            printf("Whoops! init_colorsAtCornerPosInd_to_cubieAndSolvedCubie goofed up: colorsArr[0] = %hhu\n", colorsArr[0]);
        }
        if (colorsArr[1] >= 7) {
            printf("Whoops! init_colorsAtCornerPosInd_to_cubieAndSolvedCubie goofed up: colorsArr[1] = %hhu\n", colorsArr[1]);
        }
        if (colorsArr[2] >= 7) {
            printf("Whoops! init_colorsAtCornerPosInd_to_cubieAndSolvedCubie goofed up: colorsArr[2] = %hhu\n", colorsArr[2]);
        }
        uint8_t solved_cubieInd = colorSequence_to_solvedCubieInd[colorsArr[0]][colorsArr[1]][colorsArr[2]]; //*********
        cubie_e solvedCubie = SOLVED_CUBIES[solved_cubieInd];
        face_e solvedCubieDef[3] = {
            cubieDefinitions[solvedCubie][0],
            cubieDefinitions[solvedCubie][1],
            cubieDefinitions[solvedCubie][2],
        };

        for (int row = 0; row < NUM_CORNERS; row++) {
            face_e PosCubieDef[3] = {corner_pieces[row][0].face, corner_pieces[row][1].face, corner_pieces[row][2].face};
            //-------------------------------------------------
            
            //The colors on the colorsArr, will likely be a different ordering than the same colors on solvedCubieDef.
            //The ordering of the colors in solvedCubieDef is the same ordering as the faces that hold those colors on CubieDef.
            //The textbook defintion of a solvedCubie, is a cubie such that the facesArr is exactly the same as the colorsArr.
            //    PosCubie: [L, B, D]   <-->         Cubie: [D, B, L]
            //               |  |  |                         |  |  |
            //      Colors: [B, L, U]   <-->   solvedCubie: [U, L, B]
            
            face_e cubieDef[3];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[0])] = PosCubieDef[0];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[1])] = PosCubieDef[1];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[2])] = PosCubieDef[2];

            if (cubieDef[0] >= 6) {
                printf("Whoops! init_colorsAtCornerPosInd_to_cubieAndSolvedCubie goofed up: cubieDef[0] = %hhu\n", cubieDef[0]);
            }
            if (cubieDef[1] >= 6) {
                printf("Whoops! init_colorsAtCornerPosInd_to_cubieAndSolvedCubie goofed up: cubieDef[1] = %hhu\n", cubieDef[1]);
            }
            if (cubieDef[2] >= 7) {
                printf("Whoops! init_colorsAtCornerPosInd_to_cubieAndSolvedCubie goofed up: cubieDef[2] = %hhu\n", cubieDef[2]);
            }
            cubie_e cubie = cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2]];

            colorsAtCornerPosInd_to_cubieAndSolvedCubie[0][colors-24][row] = cubie;
            colorsAtCornerPosInd_to_cubieAndSolvedCubie[1][colors-24][row] = solved_cubieInd;
        }
    }
    //printf("<color>(U, R, F) for <pos>(U, R, F) -> <cubie>%s\n", cubiePrints[colorsAtCornerPosInd_to_cubieAndSolvedCubie[CUBIE_URF-24][2][0]]);
    //printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    print_colorsAtCornerPosInd_to_cubieAndSolvedCubie();
}

void print_colorsAtCornerPosInd_to_cubieAndSolvedCubie() {
    printf("static const uint8_t colorsAtCornerPosInd_to_cubieAndSolvedCubie[2][48][NUM_CORNERS] = {\n");
    printf("\t{\n");
    for (cubie_e color = 24; color < 72; color++) {
        printf("\t\t{");
        for (uint8_t cornerPosInd = 0; cornerPosInd < NUM_CORNERS; cornerPosInd++) {
            cubie_e cubie = colorsAtCornerPosInd_to_cubieAndSolvedCubie[0][color-24][cornerPosInd];
            //uint8_t solvedCubieInd  = colorsAtEdgePosInd_to_cubieAndSolvedCubie[1][color][edgePosInd];
            printf("CUBIE_%s, ", cubiePrints[cubie]);
            //cubie_e solved_cubie = SOLVED_CUBIES[solvedCubieInd];
            //facelet_pos_s edge[2] = {
            //    edge_pieces[edgePosInd][0],
            //    edge_pieces[edgePosInd][1],
            //};
            //face_e colorDef[3] = {
            //    cubieDefinitions[color][0],
            //    cubieDefinitions[color][1],
            //    FACE_NULL,
            //};
            //printf("\t<color>(%c, %c, N), <Pos>(%c, %c, N) -> <solved>%s at <cubie>%s\n", 
            //    facePrints[colorDef[0]], 
            //    facePrints[colorDef[1]], 
            //    facePrints[edge[0].face], 
            //    facePrints[edge[1].face], 
            //    cubiePrints[solved_cubie], 
            //    cubiePrints[cubie]
            //);
        } printf("},\n");
    } printf("\t},\n");
    printf("\t{\n");
    for (cubie_e color = 24; color < 72; color++) {
        printf("\t\t{");
        for (uint8_t cornerPosInd = 0; cornerPosInd < NUM_CORNERS; cornerPosInd++) {
            //cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[0][color][edgePosInd];
            uint8_t solvedCubieInd  = colorsAtCornerPosInd_to_cubieAndSolvedCubie[1][color-24][cornerPosInd];
            printf("%hhu, ", solvedCubieInd);
            //cubie_e solved_cubie = SOLVED_CUBIES[solvedCubieInd];
            //facelet_pos_s edge[2] = {
            //    edge_pieces[edgePosInd][0],
            //    edge_pieces[edgePosInd][1],
            //};
            //face_e colorDef[3] = {
            //    cubieDefinitions[color][0],
            //    cubieDefinitions[color][1],
            //    FACE_NULL,
            //};
            //printf("\t<color>(%c, %c, N), <Pos>(%c, %c, N) -> <solved>%s at <cubie>%s\n", 
            //    facePrints[colorDef[0]], 
            //    facePrints[colorDef[1]], 
            //    facePrints[edge[0].face], 
            //    facePrints[edge[1].face], 
            //    cubiePrints[solved_cubie], 
            //    cubiePrints[cubie]
            //);
        } printf("},\n");
    } printf("\t},\n");
    printf("};\n");
}*/
/*
// mainly for shiftCube_from_cube18B()
static void init_cubieAndSolvedCubieInd_to_colorsAtPosInd() {
    
    //Tables Used:
    //    colorsAtCornerPosInd_to_cubieAndSolvedCubie[]
    //    colorsAtCornerPosInd_to_cubieAndSolvedCubie[]
    //Functions Used:
    //External Types Used:
    //    face_e
    
    // <cubie>UR  of <solved>FRD -> <color>(R, D, F), <Pos>(U, R, F), at 2
//    cubie_e color = CUBIE_URF;
//    uint8_t pos_ind = 2;
//    cubie_e nine = colorsAtCornerPosInd_to_cubieAndSolvedCubie[color-24][pos_ind][0];
//    uint8_t solve_ind_nine = colorsAtCornerPosInd_to_cubieAndSolvedCubie[color-24][pos_ind][1];
    //printf("Color URF on pos URF -> <cubie>%s of <solved>%s\n", cubiePrints[nine], cubiePrints[SOLVED_CUBIES[solve_ind_nine]]);
    // edges
    for (cubie_e colors = 0; colors < 24; colors++) {
        for (uint8_t edgePosInd = 0; edgePosInd < NUM_EDGES; edgePosInd++) {
            cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[0][colors][edgePosInd];
            uint8_t solvedCubieInd = colorsAtEdgePosInd_to_cubieAndSolvedCubie[1][colors][edgePosInd];
            if (cubie >= 24) {
                printf("Whoops! init_cubieAndSolvedCubieInd_to_colorsAtPosInd goofed up at edges: cubie = %hhu\n", cubie);
            }
            if (solvedCubieInd >= 20) {
                printf("Whoops! init_cubieAndSolvedCubieInd_to_colorsAtPosInd goofed up at edges: solvedCubieInd = %hhu\n", solvedCubieInd);
            }
            //printf("<color>%s\t<edge pos>%hhu\t->\t<cubie>%s\t<solved ind>%hhu\n", cubiePrints[colors], edgePosInd, cubiePrints[cubie], solvedCubieInd);
            cubieAndSolvedCubieInd_to_colorsAtPosInd[0][cubie][solvedCubieInd] = colors;
            cubieAndSolvedCubieInd_to_colorsAtPosInd[1][cubie][solvedCubieInd] = edgePosInd;
        }
    }
    // corners
    for (cubie_e colors = 24; colors < 72; colors++) {
        for (uint8_t cornerPosInd = 0; cornerPosInd < NUM_CORNERS; cornerPosInd++) {
            cubie_e cubie = colorsAtCornerPosInd_to_cubieAndSolvedCubie[0][colors-24][cornerPosInd];
            uint8_t solvedCubieInd = colorsAtCornerPosInd_to_cubieAndSolvedCubie[1][colors-24][cornerPosInd];
            if (cubie < 24 || cubie >= 72) {
                printf("Whoops! init_cubieAndSolvedCubieInd_to_colorsAtPosInd goofed up at corners: cubie = %hhu\n", cubie);
            }
            if (solvedCubieInd >= 20) {
                printf("Whoops! init_cubieAndSolvedCubieInd_to_colorsAtPosInd goofed up at corners: solvedCubieInd = %hhu\n", solvedCubieInd);
            }
            //printf("<color>%s <corner pos>%hhu -> <cubie>%s <solved ind>%hhu\n", cubiePrints[colors], cornerPosInd, cubiePrints[cubie], solvedCubieInd);
            cubieAndSolvedCubieInd_to_colorsAtPosInd[0][cubie-24][solvedCubieInd] = colors;
            cubieAndSolvedCubieInd_to_colorsAtPosInd[1][cubie-24][solvedCubieInd] = cornerPosInd;
        }
    } print_cubieAndSolvedCubieInd_to_colorsAtPosInd();
}

void print_cubieAndSolvedCubieInd_to_colorsAtPosInd() {
    printf("static const uint8_t cubieAndSolvedCubieInd_to_colorsAtPosInd[2][48][20] = {\n");
    printf("\t{\n");
    for (cubie_e cubie = 0; cubie < 48; cubie++) {
        printf("\t\t{");
        for (uint8_t solvedCubieInd = 0; solvedCubieInd < 18; solvedCubieInd++) {
            cubie_e color = cubieAndSolvedCubieInd_to_colorsAtPosInd[0][cubie][solvedCubieInd];
            printf("CUBIE_%s, ", cubiePrints[color]);
        } printf("},\n");
    } printf("\t},\n");
    printf("\t{\n");
    for (cubie_e cubie = 0; cubie < 48; cubie++) {
        printf("\t\t{");
        for (uint8_t solvedCubieInd = 0; solvedCubieInd < 18; solvedCubieInd++) {
            uint8_t PosInd = cubieAndSolvedCubieInd_to_colorsAtPosInd[1][cubie][solvedCubieInd];
            printf("%hhu, ", PosInd);
        } printf("},\n");
    } printf("\t},\n");
    printf("};\n");
}*/

static cube18B_s cube18B_from_shiftCube(const shift_cube_s* shiftcube) {
    /*
    Tables Used:
        cubieDefinition_to_cubie[]
        edge_pieces[]
        colorsAtEdgePosInd_to_cubieAndSolvedCubie[]
        corner_pieces[]
        colorsAtCornerPosInd_to_cubieAndSolvedCubie[]
    Functions Used:
        facelet_at_facelet_pos()
    External Types Used: 
        shiftCube_s
        face_e
    */
    cube18B_s cube18B;
    for (int i = 0; i < NUM_EDGES; i++) {
        face_e facelet_colors[3] = {
            facelet_at_facelet_pos(shiftcube, edge_pieces[i][0]),
            facelet_at_facelet_pos(shiftcube, edge_pieces[i][1]),
            FACE_NULL
        };
        if (facelet_colors[0] == 0 && facelet_colors[1] == 0) continue;
        cubie_e colorsequence = cubieDefinition_to_cubie[facelet_colors[0]][facelet_colors[1]][facelet_colors[2]];
        cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[0][colorsequence][i];
        uint8_t solved_cubieInd = colorsAtEdgePosInd_to_cubieAndSolvedCubie[1][colorsequence][i];
        if (solved_cubieInd < 18) cube18B.cubies[solved_cubieInd] = cubie;
    } for (int i = 0; i < NUM_CORNERS; i++) {
        face_e facelet_colors[3] = {
            facelet_at_facelet_pos(shiftcube, corner_pieces[i][0]),
            facelet_at_facelet_pos(shiftcube, corner_pieces[i][1]),
            facelet_at_facelet_pos(shiftcube, corner_pieces[i][2]),
        };
        if (facelet_colors[0] == 0 && facelet_colors[1] == 0 && facelet_colors[2] == 0) continue;
        cubie_e colorsequence = cubieDefinition_to_cubie[facelet_colors[0]][facelet_colors[1]][facelet_colors[2]];
        cubie_e cubie = colorsAtCornerPosInd_to_cubieAndSolvedCubie[0][colorsequence-24][i];
        uint8_t solved_cubieInd = colorsAtCornerPosInd_to_cubieAndSolvedCubie[1][colorsequence-24][i];
        if (solved_cubieInd < 18) cube18B.cubies[solved_cubieInd] = cubie;
    } return cube18B;
}

static void paint_facelet_onto_shiftCube(shift_cube_s* shiftcube, facelet_pos_s pos, face_e color) {
    /*
    Tables Used: None
    Functions Used: None
    External Types Used: 
        shiftCube_s
        facelet_pos_s
        face_e
    */
    shiftcube->state[pos.face] |= (color<<(4*pos.index));
}

static void paint_cubie_onto_shiftCube(shift_cube_s* shiftcube, cubie_e cubie, uint8_t solvedCubieInd) {
    /*
    Tables Used:
        cubieAndSolvedCubieInd_to_colorsAtPosInd[]
        cubieDefinitions[]
        corner_pieces[]
        edge_pieces[]
    Functions Used: 
        paint_facelet_onto_shiftcube()
    External Types Used: 
        shiftCube_s
        facelet_pos_s
        face_e
    */
    bool isEdge[20] = {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0};
    bool isCorner = !isEdge[solvedCubieInd];
    if (isCorner) {
        cubie_e colors = cubieAndSolvedCubieInd_to_colorsAtPosInd[0][cubie-24][solvedCubieInd];
        face_e colorsDef[3] = {
            cubieDefinitions[colors][0],
            cubieDefinitions[colors][1],
            cubieDefinitions[colors][2],
        };
        uint8_t cornerPosInd = cubieAndSolvedCubieInd_to_colorsAtPosInd[1][cubie-24][solvedCubieInd];
        facelet_pos_s positions[3] = {
            corner_pieces[cornerPosInd][0],
            corner_pieces[cornerPosInd][1],
            corner_pieces[cornerPosInd][2],
        };
        paint_facelet_onto_shiftCube(shiftcube, positions[0], colorsDef[0]);
        paint_facelet_onto_shiftCube(shiftcube, positions[1], colorsDef[1]);
        paint_facelet_onto_shiftCube(shiftcube, positions[2], colorsDef[2]);
    } else {
        cubie_e colors = cubieAndSolvedCubieInd_to_colorsAtPosInd[0][cubie][solvedCubieInd];
        face_e colorsDef[3] = {
            cubieDefinitions[colors][0],
            cubieDefinitions[colors][1],
            cubieDefinitions[colors][2],
        };
        uint8_t edgePosInd = cubieAndSolvedCubieInd_to_colorsAtPosInd[1][cubie][solvedCubieInd];
        facelet_pos_s positions[2] = {
            edge_pieces[edgePosInd][0],
            edge_pieces[edgePosInd][1],
        };
        paint_facelet_onto_shiftCube(shiftcube, positions[0], colorsDef[0]);
        paint_facelet_onto_shiftCube(shiftcube, positions[1], colorsDef[1]);
    }


    /*
    face_e solved_cubieDef[3] = {
        cubieDefinitions[solved_cubie][0],
        cubieDefinitions[solved_cubie][1],
        cubieDefinitions[solved_cubie][2],
    };
    facelet_pos_s positions[3] = {
        cubie_to_orderedPositions[cubie][0],
        cubie_to_orderedPositions[cubie][1],
        cubie_to_orderedPositions[cubie][2],
    };
    paint_facelet_onto_shiftCube(shiftcube, positions[0], solved_cubieDef[0]);
    paint_facelet_onto_shiftCube(shiftcube, positions[1], solved_cubieDef[1]);
    if (!facelet_pos_eq(positions[2], NULL_POS)) {
        paint_facelet_onto_shiftCube(shiftcube, positions[2], solved_cubieDef[2]);
    }
    */
}

static shift_cube_s shiftCube_from_cube18B(const cube18B_s* cube18B) {
    /*
    Tables Used: None
    Functions Used: 
        paint_cubie_onto_shiftCube()
    External Types Used: 
        shiftCube_s
    */
    // those two cubies we omitted will be null on shiftcube.
    shift_cube_s shiftcube = NULL_CUBE; // must be completely 0s
    // FD  RD  BD  LD  FR  FRD RB  RBD BL  BLD LF  LFD FU  RU  BU  URF UBR ULB
    for (uint8_t i = 0; i < 18; i++) {
        paint_cubie_onto_shiftCube(&shiftcube, cube18B->cubies[i], i);
    }
    return shiftcube;
}
#endif // TRANSLATORS_H