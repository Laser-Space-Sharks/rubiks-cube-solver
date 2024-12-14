#ifndef CUBE18B_H
#define CUBE18B_H

#include "main.h"
#include "alg.h"
#include <stdbool.h>


//####################################################################################################################################################
// The cube can be defined in 18 bytes, where each byte is a cubie.
// |           CROSS              |                               F2L                             |                        1LLL                   |
// | eFD     eRD     eBD     eLD  |  eFR     dFR     eRB     dRB     eBL     dBL     eLF     dLF  |  eFU     eRU     eBU     uFR     uRB     uBL  |
// [[Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte]]
// Experimentally tested via the 1LLL table hashing that we don't need the last 2 1LLL cubies. That's why cube18B is not cube20B.
//####################################################################################################################################################

void init_all_tables_in_cube18Bc();
void init_cubieDefinition_to_cubie();                    // do not share
void init_colorSequence_to_solvedCubieInd();             // do not share
void init_cubieToOrderedPositions();                     // do not share
void init_cubieAfterMove();                              // do not share
void init_colorsAtEdgePosInd_to_cubieAndSolvedCubie();   // do not share
void init_colorsAtCornerPosInd_to_cubieAndSolvedCubie(); // do not share
cubie_e cubie_from_cubieDefinition(const face_e* cubieDef);
uint8_t solvedCubieInd_from_colorSequence(const face_e* colorsArr); // do not share
cube18B_s cube18B_from_shiftCube(const shift_cube_s* shiftcube);
void paint_facelet_onto_shiftCube(shift_cube_s* shiftcube, facelet_pos_s pos, face_e color); // do not share
void paint_cubie_onto_shiftCube(shift_cube_s* shiftcube, cubie_e cubie, cubie_e solved_cubie); // do not share
shift_cube_s shiftCube_from_cube18B(const cube18B_s* cube18B);
cube18B_xcross_s cube18B_xcross_from_cube18B(const cube18B_s* cube);
cube18B_1LLL_s cube18B_1LLL_from_cube18B(const cube18B_s* cube);
cube18B_s cube18B_from_xcross_and_1LLL(const cube18B_xcross_s* xcross, const cube18B_1LLL_s* LL);
bool compare_cube18Bs(const cube18B_s* cube1, const cube18B_s* cube2);
void print_cube18B(const cube18B_s* cube);
void cube18B_apply_move(cube18B_s* cube, move_s move);
void cube18B_xcross_apply_move(cube18B_xcross_s* cube, move_s move);
void cube18B_1LLL_apply_move(cube18B_1LLL_s* cube, move_s move);
void cube18B_apply_alg(cube18B_s *cube, const alg_s *alg);
void cube18B_xcross_apply_alg(cube18B_xcross_s *cube, const alg_s *alg);
void cube18B_1LLL_apply_alg(cube18B_1LLL_s *cube, const alg_s *alg);

#endif // CUBE18B_H