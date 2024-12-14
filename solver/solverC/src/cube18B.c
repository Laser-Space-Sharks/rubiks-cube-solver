#include "cube18B.h"
#include "main.h"
#include "lookup_tables.h"
#include "alg.h"

/*
EXTERNAL DEPENDENCIES:
    facelet_pos_s
    face_e
    move_s
    shiftcube_s
    NULL_CUBE
    corner_pieces[]
    edge_pieces[]
*/



facelet_pos_s NULL_POS = {
    FACE_NULL, -1
};

bool unordered_match_faces_2x2(face_e a1, face_e a2, face_e b1, face_e b2) {
    return ((a1 == b1 || a1 == b2) &&
            (a2 == b1 || a2 == b2));
}
bool unordered_match_faces_3x3(face_e a1, face_e a2, face_e a3, face_e b1, face_e b2, face_e b3) {
    return ((a1 == b1 || a1 == b2 || a1 == b3) &&
            (a2 == b1 || a2 == b2 || a2 == b3) &&
            (a3 == b1 || a3 == b2 || a3 == b3));
}
uint8_t find_face_in_2(face_e a1, face_e a2, face_e b) {
    uint8_t ind;
    if (b == a1) ind = 0;
    else ind = 1;
    return ind;
}
uint8_t find_face_in_3(face_e a1, face_e a2, face_e a3, face_e b) {
    uint8_t ind;
    if (b == a1) ind = 0;
    else if (b == a2) ind = 1;
    else ind = 2;
    return ind;
}
bool facelet_pos_eq(facelet_pos_s pos1, facelet_pos_s pos2) {
    return (pos1.index == pos2.index && pos1.face == pos2.face);
}

face_e facelet_at_facelet_pos(const shift_cube_s* cube, facelet_pos_s pos) {
    if (facelet_pos_eq(pos, NULL_POS)) return FACE_NULL;
    return (((cube->state[pos.face])>>(4*pos.index))&15);
}

cubie_e SOLVED_CUBIES[20] = {
    CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
    CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD, 
    CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_URF, CUBIE_UBR, CUBIE_ULB, CUBIE_LU, CUBIE_UFL
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
    {FACE_F, FACE_U, FACE_R},   //FUR
    {FACE_U, FACE_R, FACE_F},   //URF
    {FACE_R, FACE_F, FACE_U},   //RFU
    {FACE_L, FACE_U, FACE_F},   //LUF
    {FACE_U, FACE_F, FACE_L},   //UFL
    {FACE_F, FACE_L, FACE_U},   //FLU
    {FACE_B, FACE_U, FACE_L},   //BUL
    {FACE_U, FACE_L, FACE_B},   //ULB
    {FACE_L, FACE_B, FACE_U},   //LBU
    {FACE_R, FACE_U, FACE_B},   //RUB
    {FACE_U, FACE_B, FACE_R},   //UBR
    {FACE_B, FACE_R, FACE_U},   //BRU
    {FACE_F, FACE_R, FACE_D},    //FRD
    {FACE_D, FACE_R, FACE_F},    //DRF
    {FACE_F, FACE_D, FACE_R},    //FDR
    {FACE_L, FACE_F, FACE_D},    //LFD
    {FACE_D, FACE_L, FACE_F},    //DLF
    {FACE_F, FACE_L, FACE_D},    //FLD
    {FACE_B, FACE_L, FACE_D},    //BLD
    {FACE_D, FACE_B, FACE_L},    //DBL
    {FACE_L, FACE_D, FACE_B},    //LDB
    {FACE_R, FACE_B, FACE_D},    //RBD
    {FACE_D, FACE_R, FACE_B},    //DRB
    {FACE_B, FACE_D, FACE_R},    //BDR
    {FACE_F, FACE_R, FACE_U},    //FRU
    {FACE_U, FACE_R, FACE_F},    //UFR
    {FACE_F, FACE_U, FACE_R},    //RUF
    {FACE_L, FACE_F, FACE_U},    //LFU
    {FACE_U, FACE_L, FACE_F},    //ULF
    {FACE_F, FACE_L, FACE_U},    //FLU
    {FACE_B, FACE_L, FACE_U},    //BLU
    {FACE_U, FACE_B, FACE_L},    //UBL
    {FACE_L, FACE_U, FACE_B},    //LUB
    {FACE_R, FACE_B, FACE_U},    //RBU
    {FACE_U, FACE_R, FACE_B},    //URB
    {FACE_B, FACE_U, FACE_R},    //BUR
    {FACE_F, FACE_D, FACE_R},   //FDR
    {FACE_D, FACE_R, FACE_F},   //DRF
    {FACE_R, FACE_F, FACE_D},   //RFD
    {FACE_L, FACE_D, FACE_F},   //LDF
    {FACE_D, FACE_F, FACE_L},   //DFL
    {FACE_F, FACE_L, FACE_D},   //FLD
    {FACE_B, FACE_D, FACE_L},   //BDL
    {FACE_D, FACE_L, FACE_B},   //DLB
    {FACE_L, FACE_B, FACE_D},   //LBD
    {FACE_R, FACE_D, FACE_B},   //RDB
    {FACE_D, FACE_B, FACE_R},   //DBR
    {FACE_B, FACE_R, FACE_D},   //BRD
};
static const face_e faceAfterMove[NUM_FACES][NUM_FACES][4] = {
    {// U
        {FACE_U, FACE_U, FACE_U, FACE_U}, // U
        {FACE_U, FACE_B, FACE_D, FACE_F}, // R
        {FACE_U, FACE_R, FACE_D, FACE_L}, // F
        {FACE_U, FACE_F, FACE_D, FACE_B}, // L
        {FACE_U, FACE_L, FACE_D, FACE_R}, // B
        {FACE_U, FACE_U, FACE_U, FACE_U}, // D
    }, 
    { // R
        {FACE_R, FACE_F, FACE_L, FACE_B}, // U
        {FACE_R, FACE_R, FACE_R, FACE_R}, // R
        {FACE_R, FACE_D, FACE_L, FACE_U}, // F
        {FACE_R, FACE_R, FACE_R, FACE_R}, // L
        {FACE_R, FACE_U, FACE_L, FACE_D}, // B
        {FACE_R, FACE_B, FACE_L, FACE_F}, // D
    }, 
    { // F
        {FACE_F, FACE_L, FACE_B, FACE_R}, // U
        {FACE_F, FACE_U, FACE_B, FACE_D}, // R
        {FACE_F, FACE_F, FACE_F, FACE_F}, // F
        {FACE_F, FACE_D, FACE_B, FACE_U}, // L
        {FACE_F, FACE_F, FACE_F, FACE_F}, // B
        {FACE_F, FACE_R, FACE_B, FACE_L}, // D
    }, 
    { // L
        {FACE_L, FACE_B, FACE_R, FACE_F}, // U
        {FACE_L, FACE_L, FACE_L, FACE_L}, // R
        {FACE_L, FACE_U, FACE_R, FACE_D}, // F
        {FACE_L, FACE_L, FACE_L, FACE_L}, // L
        {FACE_L, FACE_D, FACE_R, FACE_U}, // B
        {FACE_L, FACE_F, FACE_R, FACE_B}, // D
    }, 
    { // B
        {FACE_B, FACE_R, FACE_F, FACE_L}, // U
        {FACE_B, FACE_D, FACE_F, FACE_U}, // R
        {FACE_B, FACE_B, FACE_B, FACE_B}, // F
        {FACE_B, FACE_U, FACE_F, FACE_D}, // L
        {FACE_B, FACE_B, FACE_B, FACE_B}, // B
        {FACE_B, FACE_L, FACE_F, FACE_R}, // D
    }, 
    { // D
        {FACE_D, FACE_D, FACE_D, FACE_D}, // U
        {FACE_D, FACE_F, FACE_U, FACE_B}, // R
        {FACE_D, FACE_L, FACE_U, FACE_R}, // F
        {FACE_D, FACE_B, FACE_U, FACE_F}, // L
        {FACE_D, FACE_R, FACE_U, FACE_L}, // B
        {FACE_D, FACE_D, FACE_D, FACE_D}, // D
    }, 
};

//                                                                                Initialization ordering:
cubie_e cubieDefinition_to_cubie[NUM_FACES][NUM_FACES][NUM_FACES+1];             // 1
uint8_t colorSequence_to_solvedCubieInd[NUM_FACES+1][NUM_FACES+1][NUM_FACES+1];  // 2
facelet_pos_s cubie_to_orderedPositions[NUM_SEQUENCES][3];                       // 3
cubie_e cubieAfterMove[NUM_FACES][NUM_SIDES][NUM_CUBIES];                        // 4: depends on 1
uint8_t colorsAtEdgePosInd_to_cubieAndSolvedCubie[24][NUM_EDGES][2];             // 5: depends on 1 and 2
uint8_t colorsAtCornerPosInd_to_cubieAndSolvedCubie[48][NUM_CORNERS][2];         // 6: depends on 1 and 2

void init_all_tables_in_cube18Bc() {
    init_cubieDefinition_to_cubie();
    init_colorSequence_to_solvedCubieInd();
    init_cubieToOrderedPositions();
    init_cubieAfterMove();
    init_colorsAtEdgePosInd_to_cubieAndSolvedCubie();
    init_colorsAtCornerPosInd_to_cubieAndSolvedCubie();
}
void init_cubieDefinition_to_cubie() {
    /*
    Tables Used: 
        cubeDefinitions[]
    Functions Used: None
    External Types Used: 
        face_e
    */
    for (cubie_e cubie = CUBIE_UR; cubie < NUM_SEQUENCES; cubie++) {
        face_e cubieDef[3] = {
            cubieDefinitions[cubie][0],
            cubieDefinitions[cubie][1],
            cubieDefinitions[cubie][2],
        };
        cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2] - (cubieDef[2] == FACE_NULL)] = cubie;
    }
}
void init_colorSequence_to_solvedCubieInd() {
    /*
    Tables Used: 
        SOLVED_CUBIES[]
        cubieDefinitions[]
    Functions Used: None
    External Types Used: 
        face_e
    */
    for (int solved = 0; solved < 20; solved++) {
        cubie_e solvedCubie = SOLVED_CUBIES[solved];
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
            uint8_t ind1 = sequences[row][0]-(sequences[row][0]==FACE_NULL);
            uint8_t ind2 = sequences[row][1]-(sequences[row][1]==FACE_NULL);
            uint8_t ind3 = sequences[row][2]-(sequences[row][2]==FACE_NULL);
            colorSequence_to_solvedCubieInd[ind1][ind2][ind3] = solved;
        }
    }
}
void init_cubieToOrderedPositions() {
    /*
    Tables Used:
        cubieDefinitions[]
        edge_pieces[]
        corner_pieces[]
    Functions Used:
        unordered_match_faces_2x2()
        find_face_in_2()
        unordered_match_faces_3x3()
        find_face_in_3()
    External Types Used:
        facelet_pos_s
        face_e
    */
    for (cubie_e cubie = CUBIE_UR; cubie < NUM_SEQUENCES; cubie++) {
        face_e cubieDef[3] = {
            cubieDefinitions[cubie][0],
            cubieDefinitions[cubie][1],
            cubieDefinitions[cubie][2],
        };
        if (cubieDef[2] == FACE_NULL) {
            facelet_pos_s edge_pos1;
            facelet_pos_s edge_pos2;
            for (int i = 0; i < NUM_EDGES; i++) {
                facelet_pos_s edge[2] = {
                    edge_pieces[i][0],
                    edge_pieces[i][1]
                };
                if (unordered_match_faces_2x2(edge[0].face, edge[1].face, cubieDef[1], cubieDef[2])) {
                    edge_pos1 = edge[find_face_in_2(cubieDef[0], cubieDef[1], edge[0].face)];
                    edge_pos2 = edge[find_face_in_2(cubieDef[0], cubieDef[1], edge[1].face)];
                    break;
                }
            }
            cubie_to_orderedPositions[cubie][0] = edge_pos1;
            cubie_to_orderedPositions[cubie][1] = edge_pos2;
            cubie_to_orderedPositions[cubie][2] = NULL_POS;
        } else {
            facelet_pos_s corner_pos1;
            facelet_pos_s corner_pos2;
            facelet_pos_s corner_pos3;
            for (int i = 0; i < NUM_CORNERS; i++) {
                facelet_pos_s corner[3] = {
                    corner_pieces[i][0],
                    corner_pieces[i][1],
                    corner_pieces[i][2],
                };
                if (unordered_match_faces_3x3(
                        corner[0].face, corner[1].face, corner[2].face,
                        cubieDef[0], cubieDef[1], cubieDef[2]
                    )) {
                    corner_pos1 = corner[find_face_in_3(cubieDef[0], cubieDef[1], cubieDef[2], corner[0].face)];
                    corner_pos2 = corner[find_face_in_3(cubieDef[0], cubieDef[1], cubieDef[2], corner[1].face)];
                    corner_pos3 = corner[find_face_in_3(cubieDef[0], cubieDef[1], cubieDef[2], corner[2].face)];
                    break;
                }
            }
            cubie_to_orderedPositions[cubie][0] = corner_pos1;
            cubie_to_orderedPositions[cubie][1] = corner_pos2;
            cubie_to_orderedPositions[cubie][2] = corner_pos3;
        }
    }
}
void init_cubieAfterMove() {
    /*
    Tables Used:
        cubieDefinitions[]
        faceAfterMove[]
    Functions Used:
        cubie_from_cubieDefinition()
    External Types Used:
        face_e
    */
    for (cubie_e cubie0 = CUBIE_UR; cubie0 < NUM_CUBIES; cubie0++) {
        for (face_e move_face = FACE_U; move_face < NUM_FACES; move_face++) {
            for (uint8_t move_count = 0; move_count < 4; move_count++) {
                face_e cubie0_face1 = cubieDefinitions[cubie0][0];
                face_e cubie0_face2 = cubieDefinitions[cubie0][1];
                face_e cubie0_face3 = cubieDefinitions[cubie0][2];
                if (cubie0_face1 != move_face &&
                    cubie0_face2 != move_face &&
                    cubie0_face3 != move_face) { // move_face would never be equal to FACE_NULL
                    cubieAfterMove[move_face][move_count][cubie0] = cubie0;
                } else {
                    face_e cubie1_face1 = faceAfterMove[cubie0_face1][move_face][move_count];
                    face_e cubie1_face2 = faceAfterMove[cubie0_face2][move_face][move_count];
                    face_e cubie1_face3;
                    if (cubie0_face3 == FACE_NULL) cubie1_face3 = FACE_NULL;
                    else cubie1_face3 = faceAfterMove[cubie0_face3][move_face][move_count];
    
                    face_e cubie1_def[3] = {cubie1_face1, cubie1_face2, cubie1_face3};
                    cubieAfterMove[move_face][move_count][cubie0] = cubie_from_cubieDefinition(cubie1_def);
                }
            }
        }
    }
}
void init_colorsAtEdgePosInd_to_cubieAndSolvedCubie() {
    /*
    Tables Used:
        cubieDefinitions[]
        SOLVED_CUBIES[]
        edge_pieces[]
    Functions Used:
        solvedCubieInd_from_colorSequence()
        find_face_in_3()
        cubie_from_cubieDefinition()
    External Types Used:
        face_e
        facelet_pos_s
    */
    for (int row = 0; row < NUM_EDGES; row++) { //*********
        for (cubie_e colors = 0; colors < 24; colors++) { //*********
            face_e colorsArr[3] = {
                cubieDefinitions[colors][0],
                cubieDefinitions[colors][1],
                cubieDefinitions[colors][2],
            };
            uint8_t solved_cubieInd = solvedCubieInd_from_colorSequence(colorsArr); //*********
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
            /*
            The colors on the colorsArr, will likely be a different ordering than the same colors on solvedCubieDef.
            The ordering of the colors in solvedCubieDef is the same ordering as the faces that hold those colors on CubieDef.
            The textbook defintion of a solvedCubie, is a cubie such that the facesArr is exactly the same as the colorsArr.
                PosCubie: [L, B, D]   <-->         Cubie: [D, B, L]
                           |  |  |                         |  |  |
                  Colors: [B, L, U]   <-->   solvedCubie: [U, L, B]
            */
            face_e cubieDef[3];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[0])] = PosCubieDef[0];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[1])] = PosCubieDef[1];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[2])] = PosCubieDef[2];
            cubie_e cubie = cubie_from_cubieDefinition(cubieDef); //*********

            colorsAtEdgePosInd_to_cubieAndSolvedCubie[colors][row][0] = cubie;
            colorsAtEdgePosInd_to_cubieAndSolvedCubie[colors][row][1] = solved_cubieInd;
        }
    }
}
void init_colorsAtCornerPosInd_to_cubieAndSolvedCubie() {
    /*
    Tables Used:
        cubieDefinitions[]
        SOLVED_CUBIES[]
        corner_pieces[]
    Functions Used:
        solvedCubieInd_from_colorSequence()
        find_face_in_3()
        cubie_from_cubieDefinition()
    External Types Used:
        face_e
        facelet_pos_s
    */
    for (int row = 0; row < NUM_CORNERS; row++) {
        for (cubie_e colors = 24; colors < 72; colors++) {
            face_e colorsArr[3] = {
                cubieDefinitions[colors][0],
                cubieDefinitions[colors][1],
                cubieDefinitions[colors][2],
            };
            uint8_t solved_cubieInd = solvedCubieInd_from_colorSequence(colorsArr); //*********
            cubie_e solvedCubie = SOLVED_CUBIES[solved_cubieInd];
            face_e solvedCubieDef[3] = {
                cubieDefinitions[solvedCubie][0],
                cubieDefinitions[solvedCubie][1],
                cubieDefinitions[solvedCubie][2],
            };
            //-------------------------------------------------
            facelet_pos_s CornerLocs[3] = {
                corner_pieces[row][0],
                corner_pieces[row][1],
                corner_pieces[row][2],
            };
            face_e PosCubieDef[3] = {CornerLocs[0].face, CornerLocs[1].face, CornerLocs[2].face};
            //-------------------------------------------------
            /*
            The colors on the colorsArr, will likely be a different ordering than the same colors on solvedCubieDef.
            The ordering of the colors in solvedCubieDef is the same ordering as the faces that hold those colors on CubieDef.
            The textbook defintion of a solvedCubie, is a cubie such that the facesArr is exactly the same as the colorsArr.
                PosCubie: [L, B, D]   <-->         Cubie: [D, B, L]
                           |  |  |                         |  |  |
                  Colors: [B, L, U]   <-->   solvedCubie: [U, L, B]
            */
            face_e cubieDef[3];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[0])] = PosCubieDef[0];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[1])] = PosCubieDef[1];
            cubieDef[find_face_in_3(solvedCubieDef[0], solvedCubieDef[1], solvedCubieDef[2], colorsArr[2])] = PosCubieDef[2];
            cubie_e cubie = cubie_from_cubieDefinition(cubieDef);

            colorsAtCornerPosInd_to_cubieAndSolvedCubie[colors-24][row][0] = cubie;
            colorsAtCornerPosInd_to_cubieAndSolvedCubie[colors-24][row][1] = solved_cubieInd;
        }
    }
}

cubie_e cubie_from_cubieDefinition(const face_e* cubieDef) {
    /*
    Tables Used:
        cubieDefinition_to_cubie[]
    Functions Used: None
    External Types Used: 
        face_e
    */
    return cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2] - (cubieDef[2] == FACE_NULL)];
}

uint8_t solvedCubieInd_from_colorSequence(const face_e* colorsArr) {
    /*
    Tables Used:
        colorSequence_to_solvedCubieInd[]
    Functions Used: None
    External Types Used: 
        face_e
    */
    uint8_t ind1 = colorsArr[0]-(colorsArr[0]==FACE_NULL);
    uint8_t ind2 = colorsArr[1]-(colorsArr[1]==FACE_NULL);
    uint8_t ind3 = colorsArr[2]-(colorsArr[2]==FACE_NULL);
    uint8_t solved_cubieInd = colorSequence_to_solvedCubieInd[ind1][ind2][ind3];
    return solved_cubieInd;
}

cube18B_s cube18B_from_shiftCube(const shift_cube_s* shiftcube) {
    /*
    Tables Used:
        edge_pieces[]
        colorsAtEdgePosInd_to_cubieAndSolvedCubie[]
        corner_pieces[]
    Functions Used:
        facelet_at_facelet_pos()
        cubie_from_cubieDefinition()
    External Types Used: 
        shiftCube_s
        facelet_pos_s
        face_e
    */
    cube18B_s cube18B;
    for (int i = 0; i < NUM_EDGES; i++) {
        face_e facelet_colors[3] = {
            facelet_at_facelet_pos(shiftcube, edge_pieces[i][0]),
            facelet_at_facelet_pos(shiftcube, edge_pieces[i][1]),
            FACE_NULL
        };
        cubie_e colorsequence = cubie_from_cubieDefinition(facelet_colors);
        cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorsequence][i][0];
        uint8_t solved_cubieInd = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorsequence][i][1];
        if (solved_cubieInd < 18) cube18B.cubies[solved_cubieInd] = cubie;
    } for (int i = 0; i < NUM_CORNERS; i++) {
        face_e facelet_colors[3] = {
            facelet_at_facelet_pos(shiftcube, corner_pieces[i][0]),
            facelet_at_facelet_pos(shiftcube, corner_pieces[i][1]),
            facelet_at_facelet_pos(shiftcube, corner_pieces[i][2]),
        };
        cubie_e colorsequence = cubie_from_cubieDefinition(facelet_colors);
        cubie_e cubie = colorsAtCornerPosInd_to_cubieAndSolvedCubie[colorsequence-24][i][0];
        uint8_t solved_cubieInd = colorsAtCornerPosInd_to_cubieAndSolvedCubie[colorsequence-24][i][1];
        if (solved_cubieInd < 18) cube18B.cubies[solved_cubieInd] = cubie;
    } return cube18B;
}

void paint_facelet_onto_shiftCube(shift_cube_s* shiftcube, facelet_pos_s pos, face_e color) {
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

void paint_cubie_onto_shiftCube(shift_cube_s* shiftcube, cubie_e cubie, cubie_e solved_cubie) {
    /*
    Tables Used:
        cubieDefinitions[]
        cubie_to_orderedPositions[]
    Functions Used: 
        paint_facelet_onto_shiftcube()
    External Types Used: 
        shiftCube_s
        facelet_pos_s
        face_e
    */
    face_e solved_cubieDef[3] = {
        cubieDefinitions[solved_cubie][0],
        cubieDefinitions[solved_cubie][1],
        cubieDefinitions[solved_cubie][2],
    };
    facelet_pos_s pos1 = cubie_to_orderedPositions[cubie][0];
    facelet_pos_s pos2 = cubie_to_orderedPositions[cubie][1];
    facelet_pos_s pos3 = cubie_to_orderedPositions[cubie][2];
    paint_facelet_onto_shiftCube(shiftcube, pos1, solved_cubieDef[0]);
    paint_facelet_onto_shiftCube(shiftcube, pos2, solved_cubieDef[1]);
    if (!facelet_pos_eq(pos3, NULL_POS)) {
        paint_facelet_onto_shiftCube(shiftcube, pos1, solved_cubieDef[2]);
    }
}

shift_cube_s shiftCube_from_cube18B(const cube18B_s* cube18B) {
    /*
    Tables Used:
        cubieDefinitions[]
        cubie_to_orderedPositions[]
        SOLVED_CUBE18B <- cube18B_s used as a table
    Functions Used: 
        paint_facelet_onto_shiftcube()
    External Types Used: 
        shiftCube_s
    */
    // those two cubies we omitted will be null on shiftcube.
    shift_cube_s shiftcube = NULL_CUBE; // must be completely 0s
    for (uint8_t i = 0; i < 18; i++) {
        paint_cubie_onto_shiftCube(&shiftcube, cube18B->cubies[i], SOLVED_CUBE18B.cubies[i]);
    } return shiftcube;
}

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
void print_cube18B(const cube18B_s* cube) {
    for (int i = 0; i < 18; i++) {
        printf("%d ", cube->cubies[i]);
    } printf("\n");
}

void cube18B_apply_move(cube18B_s* cube, move_s move) {
    /*
    Tables Used: 
        cubieAfterMove[]
    Functions Used: None
    External Types Used: 
        move_s
        face_e
    */
    uint8_t turns = move.turns & 3;
    face_e face = move.face;
    cube->cubies[0]  = cubieAfterMove[face][turns][cube->cubies[0]];
    cube->cubies[1]  = cubieAfterMove[face][turns][cube->cubies[1]];
    cube->cubies[2]  = cubieAfterMove[face][turns][cube->cubies[2]];
    cube->cubies[3]  = cubieAfterMove[face][turns][cube->cubies[3]];
    cube->cubies[4]  = cubieAfterMove[face][turns][cube->cubies[4]];
    cube->cubies[5]  = cubieAfterMove[face][turns][cube->cubies[5]];
    cube->cubies[6]  = cubieAfterMove[face][turns][cube->cubies[6]];
    cube->cubies[7]  = cubieAfterMove[face][turns][cube->cubies[7]];
    cube->cubies[8]  = cubieAfterMove[face][turns][cube->cubies[8]];
    cube->cubies[9]  = cubieAfterMove[face][turns][cube->cubies[9]];
    cube->cubies[10] = cubieAfterMove[face][turns][cube->cubies[10]];
    cube->cubies[11] = cubieAfterMove[face][turns][cube->cubies[11]];
    cube->cubies[12] = cubieAfterMove[face][turns][cube->cubies[12]];
    cube->cubies[13] = cubieAfterMove[face][turns][cube->cubies[13]];
    cube->cubies[14] = cubieAfterMove[face][turns][cube->cubies[14]];
    cube->cubies[15] = cubieAfterMove[face][turns][cube->cubies[15]];
    cube->cubies[16] = cubieAfterMove[face][turns][cube->cubies[16]];
    cube->cubies[17] = cubieAfterMove[face][turns][cube->cubies[17]];
}
void cube18B_xcross_apply_move(cube18B_xcross_s* cube, move_s move) {
    /*
    Tables Used: 
        cubieAfterMove[]
    Functions Used: None
    External Types Used: 
        move_s
        face_e
    */
    uint8_t turns = move.turns & 3;
    face_e face = move.face;
    cube->cubies[0]  = cubieAfterMove[face][turns][cube->cubies[0]];
    cube->cubies[1]  = cubieAfterMove[face][turns][cube->cubies[1]];
    cube->cubies[2]  = cubieAfterMove[face][turns][cube->cubies[2]];
    cube->cubies[3]  = cubieAfterMove[face][turns][cube->cubies[3]];
    cube->cubies[4]  = cubieAfterMove[face][turns][cube->cubies[4]];
    cube->cubies[5]  = cubieAfterMove[face][turns][cube->cubies[5]];
    cube->cubies[6]  = cubieAfterMove[face][turns][cube->cubies[6]];
    cube->cubies[7]  = cubieAfterMove[face][turns][cube->cubies[7]];
    cube->cubies[8]  = cubieAfterMove[face][turns][cube->cubies[8]];
    cube->cubies[9]  = cubieAfterMove[face][turns][cube->cubies[9]];
    cube->cubies[10] = cubieAfterMove[face][turns][cube->cubies[10]];
    cube->cubies[11] = cubieAfterMove[face][turns][cube->cubies[11]];
}
void cube18B_1LLL_apply_move(cube18B_1LLL_s* cube, move_s move) {
    /*
    Tables Used: 
        cubieAfterMove[]
    Functions Used: None
    External Types Used: 
        move_s
        face_e
    */
    uint8_t turns = move.turns & 3;
    face_e face = move.face;
    cube->cubies[0] = cubieAfterMove[face][turns][cube->cubies[0]];
    cube->cubies[1] = cubieAfterMove[face][turns][cube->cubies[1]];
    cube->cubies[2] = cubieAfterMove[face][turns][cube->cubies[2]];
    cube->cubies[3] = cubieAfterMove[face][turns][cube->cubies[3]];
    cube->cubies[4] = cubieAfterMove[face][turns][cube->cubies[4]];
    cube->cubies[5] = cubieAfterMove[face][turns][cube->cubies[5]];
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