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



static const facelet_pos_s NULL_POS = {
    FACE_NULL, 8
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
int8_t find_face_in_2(face_e a1, face_e a2, face_e b) {
    uint8_t ind;
    if (b == a1) ind = 0;
    else if (b == a2) ind = 1;
    else ind = -1;
    return ind;
}
int8_t find_face_in_3(face_e a1, face_e a2, face_e a3, face_e b) {
    uint8_t ind;
    if (b == a1) ind = 0;
    else if (b == a2) ind = 1;
    else if (b == a3) ind = 2;
    else ind = -1;
    return ind;
}
bool facelet_pos_eq(facelet_pos_s pos1, facelet_pos_s pos2) {
    return (pos1.index == pos2.index && pos1.face == pos2.face);
}

face_e facelet_at_facelet_pos(const shift_cube_s* cube, facelet_pos_s pos) {
    if (facelet_pos_eq(pos, NULL_POS)) return FACE_NULL;
    return (((cube->state[pos.face])>>(4*pos.index))&15);
}
static const char facePrints[7] = {'U', 'R', 'F', 'L', 'B', 'D', 'N'};
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
    "CUBIE_NULL"
};

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
void print_cubieDefinitions() {
    printf("cubieDefinitions[]:\n");
    for (cubie_e cubie = 0; cubie < NUM_SEQUENCES; cubie++) {
        face_e cubieDef[3] = {
            cubieDefinitions[cubie][0],
            cubieDefinitions[cubie][1],
            cubieDefinitions[cubie][2],
        };
        printf("\t%s : (%c, %c, %c)\n", 
            cubiePrints[cubie], 
            facePrints[cubieDef[0]], 
            facePrints[cubieDef[1]], 
            facePrints[cubieDef[2]]
        );
    }
}

static const face_e faceAfterMove[NUM_FACES+1][NUM_FACES][4] = {
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
    { // N
        {FACE_NULL, FACE_NULL, FACE_NULL, FACE_NULL}, // U
        {FACE_NULL, FACE_NULL, FACE_NULL, FACE_NULL}, // R
        {FACE_NULL, FACE_NULL, FACE_NULL, FACE_NULL}, // F
        {FACE_NULL, FACE_NULL, FACE_NULL, FACE_NULL}, // L
        {FACE_NULL, FACE_NULL, FACE_NULL, FACE_NULL}, // B
        {FACE_NULL, FACE_NULL, FACE_NULL, FACE_NULL}, // D
    }
};

//                                                                                Initialization ordering:
static cubie_e cubieDefinition_to_cubie[NUM_FACES][NUM_FACES][NUM_FACES+1];             // 1
static uint8_t colorSequence_to_solvedCubieInd[NUM_FACES+1][NUM_FACES+1][NUM_FACES+1];  // 2
static facelet_pos_s cubie_to_orderedPositions[NUM_SEQUENCES][3];                       // 3
static cubie_e cubieAfterMove[NUM_FACES][NUM_SIDES][NUM_CUBIES];                        // 4: depends on 1
static uint8_t colorsAtEdgePosInd_to_cubieAndSolvedCubie[24][NUM_EDGES][2];             // 5: depends on 1 and 2
static uint8_t colorsAtCornerPosInd_to_cubieAndSolvedCubie[48][NUM_CORNERS][2];         // 6: depends on 1 and 2
static uint8_t cubieAndSolvedCubieInd_to_colorsAtPosInd[48][18][3];                     // 7: depends on 5 and 6

void init_all_tables_in_cube18Bc() {
    init_cubieDefinition_to_cubie();                        // i1
//    print_cubieDefinition_to_cubie();                       // p1

    init_colorSequence_to_solvedCubieInd();                 // i2
//    print_colorSequence_to_solvedCubieInd();                // p2

    init_cubieToOrderedPositions();                         // i3
//    print_cubie_to_orderedPositions();                      // p3

    init_cubieAfterMove();                                  // i4
//    print_cubieAfterMove();                                 // p4

    init_colorsAtEdgePosInd_to_cubieAndSolvedCubie();       // i5
//    print_colorsAtEdgePosInd_to_cubieAndSolvedCubie();      // p5

    init_colorsAtCornerPosInd_to_cubieAndSolvedCubie();     // i6
//    print_colorsAtCornerPosInd_to_cubieAndSolvedCubie();    // p6

    init_cubieAndSolvedCubieInd_to_colorsAtPosInd();        // i7
//    print_cubieAndSolvedCubieInd_to_colorsAtPosInd();       // p7
}

void init_cubieDefinition_to_cubie() {
    /*
    Tables Used: 
        cubeDefinitions[]
    Functions Used: None
    External Types Used: 
        face_e
    */
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
        cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2]] = cubie;
    }//print_cubieDefinition_to_cubie();
}
void print_cubieDefinition_to_cubie() {
    printf("cubieDefinition_to_cubie[][][]:\n");
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 7; k++) {
                cubie_e a = cubieDefinition_to_cubie[i][j][k];
                printf("\t(%c, %c, %c): %s\n", facePrints[i], facePrints[j], facePrints[k], cubiePrints[a]);
            }
        }
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
    for (int ind1 = 0; ind1 < 7; ind1++) {
        for (int ind2 = 0; ind2 < 7; ind2++) {
            for (int ind3 = 0; ind3 < 7; ind3++) {
                colorSequence_to_solvedCubieInd[ind1][ind2][ind3] = 21;
            }
        }
    }
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
            colorSequence_to_solvedCubieInd[sequences[row][0]][sequences[row][1]][sequences[row][2]] = solved;
        }
    }//print_colorSequence_to_solvedCubieInd();
}
void print_colorSequence_to_solvedCubieInd() {
    printf("colorSequence_to_solvedCubieInd[]:\n");
    for (int ind1 = 0; ind1 < 7; ind1++) {
        for (int ind2 = 0; ind2 < 7; ind2++) {
            for (int ind3 = 0; ind3 < 7; ind3++) {
                uint8_t solvedCubieInd = colorSequence_to_solvedCubieInd[ind1][ind2][ind3];
                if (solvedCubieInd == 21) {
                    printf("\t(%c, %c, %c) : 21 \n", 
                        facePrints[ind1], 
                        facePrints[ind2], 
                        facePrints[ind3]
                    );
                } else {
                    cubie_e solved_cubie = SOLVED_CUBIES[solvedCubieInd];
                    printf("\t(%c, %c, %c) : %s****\n", 
                        facePrints[ind1], 
                        facePrints[ind2], 
                        facePrints[ind3], 
                        cubiePrints[solved_cubie]
                    );
                }
            }
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
    for (cubie_e cubie = 0; cubie < NUM_SEQUENCES; cubie++) {
        cubie_to_orderedPositions[cubie][0] = NULL_POS;
        cubie_to_orderedPositions[cubie][1] = NULL_POS;
        cubie_to_orderedPositions[cubie][2] = NULL_POS;
    }
    //printf("NUM_SEQUENCES = %hhu\n", NUM_SEQUENCES);
    //printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    
    for (cubie_e cubie = 0; cubie < NUM_SEQUENCES; cubie++) {
        face_e cubieDef[3] = {
            cubieDefinitions[cubie][0],
            cubieDefinitions[cubie][1],
            cubieDefinitions[cubie][2],
        };
        if (cubieDef[2] == FACE_NULL) {
            facelet_pos_s edgePos[2] = {
                NULL_POS, NULL_POS
            };
            for (int i = 0; i < NUM_EDGES; i++) {
                facelet_pos_s edge[2] = {
                    edge_pieces[i][0],
                    edge_pieces[i][1],
                };
                if (unordered_match_faces_2x2(edge[0].face, edge[1].face, cubieDef[0], cubieDef[1])) {
                    edgePos[find_face_in_2(cubieDef[0], cubieDef[1], edge[0].face)] = edge[0];
                    edgePos[find_face_in_2(cubieDef[0], cubieDef[1], edge[1].face)] = edge[1];
                    break;
                }
            }
            cubie_to_orderedPositions[cubie][0] = edgePos[0];
            cubie_to_orderedPositions[cubie][1] = edgePos[1];
            cubie_to_orderedPositions[cubie][2] = NULL_POS;
        } else {
            facelet_pos_s cornerPos[3] = {
                NULL_POS, NULL_POS, NULL_POS
            };
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
                    cornerPos[find_face_in_3(cubieDef[0], cubieDef[1], cubieDef[2], corner[0].face)] = corner[0];
                    cornerPos[find_face_in_3(cubieDef[0], cubieDef[1], cubieDef[2], corner[1].face)] = corner[1];
                    cornerPos[find_face_in_3(cubieDef[0], cubieDef[1], cubieDef[2], corner[2].face)] = corner[2];
                    break;
                }
            }
            cubie_to_orderedPositions[cubie][0] = cornerPos[0];
            cubie_to_orderedPositions[cubie][1] = cornerPos[1];
            cubie_to_orderedPositions[cubie][2] = cornerPos[2];
        }
    }
    //print_cubie_to_orderedPositions();
}
void print_cubie_to_orderedPositions() {
    printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    printf("cubie_to_orderedPositions[][]:\n");
    for (cubie_e cubie = 0; cubie < NUM_SEQUENCES; cubie++) {
        facelet_pos_s Pos[3] = {
            cubie_to_orderedPositions[cubie][0],
            cubie_to_orderedPositions[cubie][1],
            cubie_to_orderedPositions[cubie][2],
        };
        if (facelet_pos_eq(Pos[2], NULL_POS)) {
            printf("\t%s : ((%c, %hhu), (%c, %hhu), NULL_POS)\n", 
                cubiePrints[cubie], 
                facePrints[Pos[0].face], 
                Pos[0].index, 
                facePrints[Pos[1].face], 
                Pos[1].index
            );
        } else {
            printf("\t%s : ((%c, %hhu), (%c, %hhu), (%c, %hhu))\n", 
                cubiePrints[cubie], 
                facePrints[Pos[0].face], 
                Pos[0].index, 
                facePrints[Pos[1].face], 
                Pos[1].index, 
                facePrints[Pos[2].face], 
                Pos[2].index
            );
        }
    }
}
// mainly for apply_move
void init_cubieAfterMove() {
    /*
    Tables Used:
        cubieDefinitions[]
        faceAfterMove[]
        cubieDefinition_to_cubie[]
    Functions Used:
        find_face_in_3
    External Types Used:
        face_e
    */
    //printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    for (cubie_e cubie0 = 0; cubie0 < NUM_CUBIES; cubie0++) {
        for (face_e move_face = FACE_U; move_face < NUM_FACES; move_face++) {
            for (uint8_t move_count = 0; move_count < 4; move_count++) {
                face_e cubie0def[3] = {
                    cubieDefinitions[cubie0][0],
                    cubieDefinitions[cubie0][1],
                    cubieDefinitions[cubie0][2],
                };
                if (find_face_in_3(cubie0def[0], cubie0def[1], cubie0def[2], move_face) == -1) {
                    cubieAfterMove[move_face][move_count][cubie0] = cubie0;
                } else {
                    face_e cubie1def[3] = {
                        faceAfterMove[cubie0def[0]][move_face][move_count],
                        faceAfterMove[cubie0def[1]][move_face][move_count],
                        faceAfterMove[cubie0def[2]][move_face][move_count],
                    };
                    cubieAfterMove[move_face][move_count][cubie0] = cubieDefinition_to_cubie[cubie1def[0]][cubie1def[1]][cubie1def[2]];
                }
            }
        }
    } 
    //printf("cubieAfterMove[U][3][UR] = %s\n", cubiePrints[cubieAfterMove[FACE_U][3][CUBIE_UR]]);
    //print_cubieAfterMove();
}
void print_cubieAfterMove() {
    printf("cubieAfterMove[][][]:\n");
    printf("\t      U1 , U2 , U3 , R1 , R2 , R3 , F1 , F2 , F3 , L1 , L2 , L3 , B1 , B2 , B3 , D1 , D2 , D3\n");
    for (cubie_e cubie = 0; cubie < 48; cubie++) {
        printf("\t%s : ", cubiePrints[cubie]);
        for (face_e face = 0; face < NUM_FACES; face++) {
            for (uint8_t count = 1; count < 4; count++) {
                cubie_e nextcubie = cubieAfterMove[face][count][cubie];
                printf("%s, ", cubiePrints[nextcubie]);
            }
        } printf("\n");
    }
}
// mainly for cube18B_from_shiftcube()
void init_colorsAtEdgePosInd_to_cubieAndSolvedCubie() {
    /*
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
    */
    //printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    for (int row = 0; row < NUM_EDGES; row++) { //*********
        for (cubie_e colors = 0; colors < 24; colors++) { //*********
            face_e colorsArr[3] = {
                cubieDefinitions[colors][0],
                cubieDefinitions[colors][1],
                cubieDefinitions[colors][2],
            };
            uint8_t solved_cubieInd = colorSequence_to_solvedCubieInd[colorsArr[0]][colorsArr[1]][colorsArr[2]]; //*********
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
            cubie_e cubie = cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2]]; //*********

            colorsAtEdgePosInd_to_cubieAndSolvedCubie[colors][row][0] = cubie;
            colorsAtEdgePosInd_to_cubieAndSolvedCubie[colors][row][1] = solved_cubieInd;
        }
    }
    //print_colorsAtEdgePosInd_to_cubieAndSolvedCubie();
}
void print_colorsAtEdgePosInd_to_cubieAndSolvedCubie() {
    printf("colorsAtEdgePosInd_to_cubieAndSolvedCubie[][][]\n");
    for (cubie_e color = 0; color < 24; color++) {
        for (uint8_t edgePosInd = 0; edgePosInd < NUM_EDGES; edgePosInd++) {
            cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[color][edgePosInd][0];
            uint8_t solvedCubieInd  = colorsAtEdgePosInd_to_cubieAndSolvedCubie[color][edgePosInd][1];
            cubie_e solved_cubie = SOLVED_CUBIES[solvedCubieInd];
            facelet_pos_s edge[2] = {
                edge_pieces[edgePosInd][0],
                edge_pieces[edgePosInd][1],
            };
            face_e colorDef[3] = {
                cubieDefinitions[color][0],
                cubieDefinitions[color][1],
                FACE_NULL,
            };
            printf("\t<color>(%c, %c, N), <Pos>(%c, %c, N) -> <solved>%s at <cubie>%s\n", 
                facePrints[colorDef[0]], 
                facePrints[colorDef[1]], 
                facePrints[edge[0].face], 
                facePrints[edge[1].face], 
                cubiePrints[solved_cubie], 
                cubiePrints[cubie]
            );
        }
    }
}
// mainly for cube18B_from_shiftcube()
void init_colorsAtCornerPosInd_to_cubieAndSolvedCubie() {
    /*
    Tables Used:
        cubieDefinitions[]
        colorSequence_to_solvedCubieInd[]
        SOLVED_CUBIES[]
        corner_pieces[]
        cubieDefinition_to_cubie[]
    Functions Used:
        find_face_in_3()
    External Types Used:
        face_e
        facelet_pos_s
    */
    //printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    //<color>(U, F, R), <Pos>(U, R, F) -> <cubie>UFR of <solved>URF

    for (cubie_e colors = 24; colors < 72; colors++) {
        face_e colorsArr[3] = {
            cubieDefinitions[colors][0],
            cubieDefinitions[colors][1],
            cubieDefinitions[colors][2],
        };
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
            cubie_e cubie = cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2]];

            colorsAtCornerPosInd_to_cubieAndSolvedCubie[colors-24][row][0] = cubie;
            colorsAtCornerPosInd_to_cubieAndSolvedCubie[colors-24][row][1] = solved_cubieInd;
        }
    }
    //printf("<color>(U, R, F) for <pos>(U, R, F) -> <cubie>%s\n", cubiePrints[colorsAtCornerPosInd_to_cubieAndSolvedCubie[CUBIE_URF-24][2][0]]);
    //printf("cubie_to_orderedPositions[UR][1].index = %hhu\n", cubie_to_orderedPositions[CUBIE_UR][1].index);
    //print_colorsAtCornerPosInd_to_cubieAndSolvedCubie();
}
void print_colorsAtCornerPosInd_to_cubieAndSolvedCubie() {
    printf("colorsAtCornerPosInd_to_cubieAndSolvedCubie[][][]\n");
    for (cubie_e color = 24; color < 72; color++) {
        for (uint8_t CornerPosInd = 0; CornerPosInd < NUM_CORNERS; CornerPosInd++) {
            cubie_e cubie = colorsAtCornerPosInd_to_cubieAndSolvedCubie[color-24][CornerPosInd][0];
            uint8_t solvedCubieInd  = colorsAtCornerPosInd_to_cubieAndSolvedCubie[color-24][CornerPosInd][1];
            cubie_e solved_cubie = SOLVED_CUBIES[solvedCubieInd];
            facelet_pos_s corner[3] = {
                corner_pieces[CornerPosInd][0],
                corner_pieces[CornerPosInd][1],
                corner_pieces[CornerPosInd][2],
            };
            face_e colorDef[3] = {
                cubieDefinitions[color][0],
                cubieDefinitions[color][1],
                cubieDefinitions[color][2],
            };
            printf("\t<color>(%c, %c, %c), <Pos>(%c, %c, %c) -> <cubie>%s of <solved>%s\n", 
                facePrints[colorDef[0]], 
                facePrints[colorDef[1]], 
                facePrints[colorDef[2]], 
                facePrints[corner[0].face], 
                facePrints[corner[1].face], 
                facePrints[corner[2].face], 
                cubiePrints[cubie], 
                cubiePrints[solved_cubie]
            );
        }
    }
}
// mainly for shiftCube_from_cube18B()
void init_cubieAndSolvedCubieInd_to_colorsAtPosInd() {
    /*
    Tables Used:
        colorsAtCornerPosInd_to_cubieAndSolvedCubie[]
        colorsAtCornerPosInd_to_cubieAndSolvedCubie[]
    Functions Used:
    External Types Used:
        face_e
    */
    // <cubie>UR  of <solved>FRD -> <color>(R, D, F), <Pos>(U, R, F), at 2
    cubie_e color = CUBIE_URF;
    uint8_t pos_ind = 2;
    cubie_e nine = colorsAtCornerPosInd_to_cubieAndSolvedCubie[color-24][pos_ind][0];
    uint8_t solve_ind_nine = colorsAtCornerPosInd_to_cubieAndSolvedCubie[color-24][pos_ind][1];
    //printf("Color URF on pos URF -> <cubie>%s of <solved>%s\n", cubiePrints[nine], cubiePrints[SOLVED_CUBIES[solve_ind_nine]]);
    // edges
    for (cubie_e colors = 0; colors < 24; colors++) {
        for (uint8_t edgePosInd = 0; edgePosInd < NUM_EDGES; edgePosInd++) {
            cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colors][edgePosInd][0];
            uint8_t solvedCubieInd = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colors][edgePosInd][1];
            cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][0] = colors;
            cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][1] = edgePosInd;
            cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][2] = 0;
        }
    }
    // corners
    for (cubie_e colors = 24; colors < 72; colors++) {
        for (uint8_t cornerPosInd = 0; cornerPosInd < NUM_CORNERS; cornerPosInd++) {
            cubie_e cubie = colorsAtCornerPosInd_to_cubieAndSolvedCubie[colors-24][cornerPosInd][0];
            uint8_t solvedCubieInd = colorsAtCornerPosInd_to_cubieAndSolvedCubie[colors-24][cornerPosInd][1];
            //printf("Cubie: %s, solved cubie index %hhu\n", cubiePrints[cubie], solvedCubieInd);
            cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][0] = colors;
            cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][1] = cornerPosInd;
            cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][2] = 1;
        }
    }
}
void print_cubieAndSolvedCubieInd_to_colorsAtPosInd() {
    printf("cubieAndSolvedCubieInd_to_colorsAtPosInd[][][]\n");
    bool isEdge[20] = {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0};
    for (cubie_e cubie = 0; cubie < 72; cubie++) {
        for (uint8_t solvedCubieInd = 0; solvedCubieInd < 18; solvedCubieInd++) {
            if ((cubie < 24 && !isEdge[solvedCubieInd]) || (cubie >= 24 && isEdge[solvedCubieInd])) {
                continue;
            }
            cubie_e solved_cubie = SOLVED_CUBIES[solvedCubieInd];

            cubie_e color = cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][0];
            uint8_t PosInd = cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][1];
            uint8_t isCorner = cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][2];

            face_e colorDef[3] = {
                cubieDefinitions[color][0],
                cubieDefinitions[color][1],
                cubieDefinitions[color][2],
            };
            if (isCorner) {
                printf("\t<cubie>%s of <solved>%s -> <color>(%c, %c, %c), <Pos>(%c, %c, %c) at PosInd %hhu\n", 
                    cubiePrints[cubie], 
                    cubiePrints[solved_cubie], 
                    facePrints[colorDef[0]], 
                    facePrints[colorDef[1]], 
                    facePrints[colorDef[2]], 
                    facePrints[corner_pieces[PosInd][0].face], 
                    facePrints[corner_pieces[PosInd][1].face], 
                    facePrints[corner_pieces[PosInd][2].face],
                    PosInd
                );
            } else {
                printf("\t<cubie>%s of <solved>%s -> <color>(%c, %c, N), <Pos>(%c, %c, N) at PosInd %hhu\n", 
                    cubiePrints[cubie], 
                    cubiePrints[solved_cubie], 
                    facePrints[colorDef[0]], 
                    facePrints[colorDef[1]], 
                    facePrints[edge_pieces[PosInd][0].face], 
                    facePrints[edge_pieces[PosInd][1].face],
                    PosInd
                );
            }
        }
    }
}

cube18B_s cube18B_from_shiftCube(const shift_cube_s* shiftcube) {
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
        cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorsequence][i][0];
        uint8_t solved_cubieInd = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorsequence][i][1];
        if (solved_cubieInd < 18) cube18B.cubies[solved_cubieInd] = cubie;
    } for (int i = 0; i < NUM_CORNERS; i++) {
        face_e facelet_colors[3] = {
            facelet_at_facelet_pos(shiftcube, corner_pieces[i][0]),
            facelet_at_facelet_pos(shiftcube, corner_pieces[i][1]),
            facelet_at_facelet_pos(shiftcube, corner_pieces[i][2]),
        };
        if (facelet_colors[0] == 0 && facelet_colors[1] == 0 && facelet_colors[2] == 0) continue;
        cubie_e colorsequence = cubieDefinition_to_cubie[facelet_colors[0]][facelet_colors[1]][facelet_colors[2]];
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
void paint_cubie_onto_shiftCube(shift_cube_s* shiftcube, cubie_e cubie, uint8_t solvedCubieInd) {
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
    cubie_e colors = cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][0];
    face_e colorsDef[3] = {
        cubieDefinitions[colors][0],
        cubieDefinitions[colors][1],
        cubieDefinitions[colors][2],
    };
    uint8_t isCorner = cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][2];
    facelet_pos_s positions[3];
    if (isCorner) {
        uint8_t cornerPosInd = cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][1];
        positions[0] = corner_pieces[cornerPosInd][0];
        positions[1] = corner_pieces[cornerPosInd][1];
        positions[2] = corner_pieces[cornerPosInd][2];
    } else {
        uint8_t edgePosInd = cubieAndSolvedCubieInd_to_colorsAtPosInd[cubie][solvedCubieInd][1];
        positions[0] = edge_pieces[edgePosInd][0];
        positions[1] = edge_pieces[edgePosInd][1];
        positions[2] = NULL_POS;
    }

    paint_facelet_onto_shiftCube(shiftcube, positions[0], colorsDef[0]);
    paint_facelet_onto_shiftCube(shiftcube, positions[1], colorsDef[1]);
    if (!facelet_pos_eq(positions[2], NULL_POS)) {
        paint_facelet_onto_shiftCube(shiftcube, positions[2], colorsDef[2]);
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
shift_cube_s shiftCube_from_cube18B(const cube18B_s* cube18B) {
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