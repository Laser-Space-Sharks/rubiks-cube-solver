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

face_e facelet_at_facelet_pos(const shiftCube_s* cube, facelet_pos_s pos) {
    if (pos == NULL_POS) return FACE_NULL;
    return (((cube->state[pos.face])>>(4*pos.index))&15);
}

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
    CUBIE_FRU = 24,
    CUBIE_UFR = 25,
    CUBIE_RUF = 26,
    CUBIE_LFU = 27,
    CUBIE_ULF = 28,
    CUBIE_FUL = 29,
    CUBIE_BLU = 30,
    CUBIE_UBL = 31,
    CUBIE_LUB = 32,
    CUBIE_RBU = 33,
    CUBIE_URB = 34,
    CUBIE_BUR = 35,
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
    CUBIE_BDR = 47,
    CUBIE_FUR = 48, // This cubie and beyond are not possible on the cube. We keep them in order to encode color sequences.
    CUBIE_URF = 49,
    CUBIE_RFU = 50,
    CUBIE_LUF = 51,
    CUBIE_UFL = 52,
    CUBIE_FLU = 53,
    CUBIE_BUL = 54,
    CUBIE_ULB = 55,
    CUBIE_LBU = 56,
    CUBIE_RUB = 57,
    CUBIE_UBR = 58,
    CUBIE_BRU = 59,
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
    CUBIE_NULL = 73
} cubie_e;


//####################################################################################################################################################
// The cube can be defined in 18 bytes, where each byte is a cubie.
// |           CROSS              |                               F2L                             |                        1LLL                   |
// | eFD     eRD     eBD     eLD  |  eFR     dFR     eRB     dRB     eBL     dBL     eLF     dLF  |  eFU     eRU     eBU     uFR     uRB     uBL  |
// [[Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte]]
// Experimentally tested via the 1LLL table hashing that we don't need the last 2 1LLL cubies. That's why cube18B is not cube20B.
//####################################################################################################################################################
typedef struct {
    cubie_e cubies[18]
} cube18B_s;
typedef struct {
    cubie_e cubies[12]
} cube18B_xcross_s;
typedef struct {
    cubie_e cubies[6]
} cube18B_1LLL_s;
cube18B_s SOLVED_CUBE18B = {
    .cubies = {
        CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
        CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD, 
        CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_FR, CUBIE_RBU, CUBIE_BLU
    }
};
cubie_e SOLVED_CUBIES[20] = {
    CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
    CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD, 
    CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_FR, CUBIE_RBU, CUBIE_BLU, CUBIE_LU, CUBIE_LFU
};
cube18B_xcross_s SOLVED_CUBE18B_XCROSS = {
    .cubies = {
        CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
        CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD
    }
};
cube18B_1LLL_s SOLVED_CUBE18B_1LLL = {
    .cubies = {
        CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_FR, CUBIE_RBU, CUBIE_BLU
    }
};
face_e cubieDefinitions[NUM_SEQUENCES][3] = {
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
    {FACE_F, FACE_R, FACE_U},    //FRU
    {FACE_U, FACE_R, FACE_F},    //URF
    {FACE_F, FACE_U, FACE_R},    //FUR
    {FACE_L, FACE_F, FACE_U},    //LFU
    {FACE_U, FACE_L, FACE_F},    //ULF
    {FACE_F, FACE_L, FACE_U},    //FLU
    {FACE_B, FACE_L, FACE_U},    //BLU
    {FACE_U, FACE_B, FACE_L},    //UBL
    {FACE_L, FACE_U, FACE_B},    //LUB
    {FACE_R, FACE_B, FACE_U},    //RBU
    {FACE_U, FACE_R, FACE_B},    //URB
    {FACE_B, FACE_U, FACE_R},    //BUR
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
face_e faceAfterMove[NUM_FACES][NUM_FACES][4] = {
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
//                                                                        Initialization ordering:
cubie_e cubieDefinition_to_cubie[NUM_FACES][NUM_FACES][NUM_FACES+1];     // 1
facelet_pos_s cubie_to_orderedPositions[NUM_SEQUENCES][3];               // 2
cubie_e cubieAfterMove[NUM_FACES][NUM_SIDES][NUM_CUBIES];                // 3: depends on 1
uint8_t colorsAtEdgePosInd_to_cubieAndSolvedCubie[24][NUM_EDGES][2];     // 4: depends on 1 and 2
uint8_t colorsAtCornerPosInd_to_cubieAndSolvedCubie[48][NUM_CORNERS][2]; // 5: depends on 1 and 2

void init_cubieDefinition_to_cubie();
void init_cubieToOrderedPositions();
void init_cubieAfterMove();
void init_colorsAtEdgePosInd_to_cubieAndSolvedCubie();
void init_colorsAtCornerPosInd_to_cubieAndSolvedCubie();
cubie_e cubie_from_cubieDefinition(const uint8_t* cubieDef);
cube18B_s cube18B_from_shiftCube(const shiftCube_s* shiftcube);
void paint_facelet_onto_shiftCube(shiftCube_s* shiftcube, facelet_pos_s pos, face_e color);
void paint_cubie_onto_shiftCube(shiftCube_s* shiftcube, cube_e cubie, cubie_e solved_cubie);
shiftCube_s shiftCube_from_cube18B(const cube18B_s* cube18B);
cube18B_xcross_s cube18B_xcross_from_cube18B(const cube18B_s* cube);
cube18B_1LLL_s cube18B_1LLL_from_cube18B(const cube18B_s* cube);
cube18B_s cube18B_from_xcross_and_1LLL(const cube18B_xcross_s* xcross, const cube18B_1LLL_s* LL);
void cube18B_apply_move(cube18B_s* cube, move_s move);
void cube18B_xcross_apply_move(cube18B_xcross_s* cube, move_s move);
void cube18B_1LLL_apply_move(cube18B_1LLL_s* cube, move_s move);


void init_cubieDefinition_to_cubie() {
    for (cubie_e cubie = CUBIE_UR; cubie < NUM_SEQUENCES; cubie++) {
        face_e cubieDef[3] = cubeDefinitions[cubie];
        cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2] - (cubieDef[2] == FACE_NULL)] = cubie;
    }
}
void init_cubieToOrderedPositions() {
    for (cubie_e cubie = CUBIE_UR; cubie < NUM_SEUENCES; cubie++) {
        uint8_t cubieDef[3] = cubieDefinitions[cubie];
        if (cubieDef[2] == FACE_NULL) {
            for (int i = 0; i < NUM_EDGES; i++) {
                facelet_pos_s edge[2] = edge_pieces[i];
                if (unordered_match_faces_2x2(
                        edge[0].face, edge[1].face, 
                        cubieDef[1], cubieDef[2],
                    )) {
                    facelet_pos_s edge_pos1 = edge[find_face_in_2(cubieDef[0], cubieDef[1], edge[0].face)];
                    facelet_pos_s edge_pos2 = edge[find_face_in_2(cubieDef[0], cubieDef[1], edge[1].face)];
                    break;
                }
            }
            cubie_to_orderedPositions[cubie][0] = edge_pos1;
            cubie_to_orderedPositions[cubie][1] = edge_pos2;
            cubie_to_orderedPositions[cubie][2] = NULL_POS;
        } else {
            for (int i = 0; i < NUM_CORNERS; i++) {
                facelet_pos_s corner[3] = corner_pieces[i];
                if (unordered_match_faces_3x3(
                        corner[0].face, corner[1].face, corner[2].face,
                        cubieDef[1], cubieDef[2], cubieDef[3]
                    )) {
                    facelet_pos_s corner_pos1 = corner[find_face_in_3(cubieDef[0], cubieDef[1], cubieDef[2], corner[0].face)];
                    facelet_pos_s corner_pos1 = corner[find_face_in_3(cubieDef[0], cubieDef[1], cubieDef[2], corner[1].face)];
                    facelet_pos_s corner_pos1 = corner[find_face_in_3(cubieDef[0], cubieDef[1], cubieDef[2], corner[2].face)];
                    break;
                }
            }
            cubie_to_orderedPositions[cubie][0] = corner_pos1;
            cubie_to_orderedPositions[cubie][1] = corner_pos3;
            cubie_to_orderedPositions[cubie][2] = corner_pos3;
        }
    }
}
void init_cubieAfterMove() {
    for (cubie_e cubie0 = CUBIE_UR; cubie0 < NUM_CUBIES; cubie0++) {
        for (face_e move_face = FACE_U; move_face < NUM_FACES; move_face++) {
            for (uint8_t move_count = 0; move_count < 4; move_count++) {
                int prefix = cubieDefinitions[cubie0][0];
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
    
                    uint8_t cubie1_def[4] = {prefix, cubie1_face1, cubie1_face2, cubie1_face3};
                    cubieAfterMove[move_face][move_count][cubie0] = cubie_from_cubieDefinition(cubie1_def);
                }
            }
        }
    }
}
void init_colorsAtEdgePosInd_to_cubieAndSolvedCubie() {
    for (cubie_e cubie = CUBIE_UR; cubie < NUM_CUBIES; cubie++) {
        uint8_t cubieDef[3] = cubieDefinitions[cubie];
        if (cubieDef[2] != FACE_NULL) continue;
        facelet_pos_s orderedPosition[3] = cubie_to_orderedPositions[cubie];
        int EdgePosInd;
        for (int i = 0; i < NUM_EDGES; i++) {
            if (unordered_match_faces_2x2(
                    edge_pieces[i][0], edge_pieces[i][1], 
                    orderedPosition[0], orderedPosition[1]
                )) {
                EdgePosInd = i;
                break;
            }
        }
        facelet_pos_s edge[2] = edge_pieces[EdgePosInd];
        for (uint8_t solved_cubieInd = 0; solved_cubieInd < 20; solved_cubieInd++) {
            cubie_e solved_cubie = SOLVED_CUBIES[solved_cubieInd];
            uint8_t solved_cubieDef[3] = cubieDefinitions[solved_cubie];
            if (solved_cubieDef[2] != FACE_NULL) continue;
            face_e facelet_colors[3] = {
                solved_cubieDef[find_face_in_2(orderedPosition[0], orderedPosition[1], edge[0])],
                solved_cubieDef[find_face_in_2(orderedPosition[0], orderedPosition[1], edge[1])],
                FACE_NULL
            };
            cubie_e colorsequence = cubie_from_cubieDefinition(facelet_colors);
            colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorsequence][EdgePosInd][0] = cubie;
            colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorsequence][EdgePosInd][0] = solved_cubieInd;
        }
    }
}
void init_colorsAtCornerPosInd_to_cubieAndSolvedCubie() {
    for (cubie_e cubie = CUBIE_UR; cubie < NUM_CUBIES; cubie++) {
        uint8_t cubieDef[3] = cubieDefinitions[cubie];
        if (cubieDef[2] == FACE_NULL) continue;
        facelet_pos_s orderedPosition[3] = cubie_to_orderedPositions[cubie];
        int CornerPosInd;
        for (int i = 0; i < NUM_CORNERS; i++) {
            if (unordered_match_faces_3x3(
                    corner_pieces[i][0], corner_pieces[i][1], corner_pieces[i][2],
                    orderedPosition[0], orderedPosition[1], orderedPosition[2]
                )) {
                CornerPosInd = i;
                break;
            }
        }
        facelet_pos_s corner[3] = corner_pieces[CornerPosInd];
        for (uint8_t solved_cubieInd = 0; solved_cubieInd < 20; solved_cubieInd++) {
            cubie_e solved_cubie = SOLVED_CUBIES[solved_cubieInd];
            uint8_t solved_cubieDef[3] = cubieDefinitions[solved_cubie];
            if (solved_cubieDef[2] == FACE_NULL) continue;
            face_e facelet_colors[3] = {
                solved_cubieDef[find_face_in_3(orderedPosition[0], orderedPosition[1], orderedPosition[2], corner[0])],
                solved_cubieDef[find_face_in_3(orderedPosition[0], orderedPosition[1], orderedPosition[2], corner[1])],
                solved_cubieDef[find_face_in_3(orderedPosition[0], orderedPosition[1], orderedPosition[2], corner[2])],
            };
            cubie_e colorsequence = cubie_from_cubieDefinition(facelet_colors);
            colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorsequence-24][CornerPosInd][0] = cubie;
            colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorsequence-24][CornerPosInd][0] = solved_cubieInd;
        }
    }
}


cubie_e cubie_from_cubieDefinition(const uint8_t* cubieDef) {
    return cubieDefinition_to_cubie[cubieDef[0]][cubieDef[1]][cubieDef[2] - (cubieDef[2] == FACE_NULL)];
}

cube18B_s cube18B_from_shiftCube(const shiftCube_s* shiftcube) {
    cube18B_s cube18B;
    for (int i = 0; i < NUM_EDGES; i++) {
        facelet_pos_s edge[2] = edge_pieces[i];
        face_e facelet_colors[3] = {
            facelet_at_facelet_pos(shiftcube, edge[0]),
            facelet_at_facelet_pos(shiftcube, edge[1]),
            FACE_NULL
        };
        cubie_e colorseqeunce = cubie_from_cubieDefinition(facelet_colors);
        cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorseqeunce][i][0];
        uint8_t solved_cubieInd = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorseqeunce][i][1];
        if (solved_cubieInd < 18) cube18B.cubies[solved_cubieInd] = cubie;
    } for (int i = 0; i < NUM_CORNERS; i++) {
        facelet_pos_s corner[3] = corner_pieces[i];
        face_e facelet_colors[3] = {
            facelet_at_facelet_pos(shiftcube, corner[0]),
            facelet_at_facelet_pos(shiftcube, corner[1]),
            facelet_at_facelet_pos(shiftcube, corner[2]),
        };
        cubie_e colorseqeunce = cubie_from_cubieDefinition(facelet_colors);
        cubie_e cubie = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorseqeunce-24][i][0];
        uint8_t solved_cubieInd = colorsAtEdgePosInd_to_cubieAndSolvedCubie[colorseqeunce-24][i][1];
        if (solved_cubieInd < 18) cube18B.cubies[solved_cubieInd] = cubie;
    } return cube18B;
}

void paint_facelet_onto_shiftCube(shiftCube_s* shiftcube, facelet_pos_s pos, face_e color) {
    shiftcube->state[pos.face] |= (color<<(4*pos.index));
}

void paint_cubie_onto_shiftCube(shiftCube_s* shiftcube, cube_e cubie, cubie_e solved_cubie) {
    uint8_t solved_cubieDef[4] = cubieDefinitions[solved_cubie];
    facelet_pos_s pos1 = cubie_to_orderedPositions[cubie][0];
    facelet_pos_s pos2 = cubie_to_orderedPositions[cubie][1];
    facelet_pos_s pos3 = cubie_to_orderedPositions[cubie][2];
    paint_facelet_onto_shiftcube(shiftcube, pos1, solved_cubieDef[1]);
    paint_facelet_onto_shiftcube(shiftcube, pos2, solved_cubieDef[2]);
    if (pos3 != NULL_POS) {
        paint_facelet_onto_shiftcube(shiftcube, pos1, solved_cubieDef[3]);
    }
}

shiftCube_s shiftCube_from_cube18B(const cube18B_s* cube18B) {
    // those two cubies we omitted will be null on shiftcube.
    shiftCube_s shiftcube = NULL_CUBE; // must be completely 0s
    for (uint8_t i = 0; i < 18; i++) {
        paint_cubie_onto_shiftCube(shiftcube, cube18B->cubies[i], SOLVED_CUBE18B.cubies[i]);
    } return shiftcube;
}

cube18B_xcross_s cube18B_xcross_from_cube18B(const cube18B_s* cube) {
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

void cube18B_apply_move(cube18B_s* cube, move_s move) {
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
    uint8_t turns = move.turns & 3;
    face_e face = move.face;
    cube->cubies[0] = cubieAfterMove[face][turns][cube->cubies[0]];
    cube->cubies[1] = cubieAfterMove[face][turns][cube->cubies[1]];
    cube->cubies[2] = cubieAfterMove[face][turns][cube->cubies[2]];
    cube->cubies[3] = cubieAfterMove[face][turns][cube->cubies[3]];
    cube->cubies[4] = cubieAfterMove[face][turns][cube->cubies[4]];
    cube->cubies[5] = cubieAfterMove[face][turns][cube->cubies[5]];
}
