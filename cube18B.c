//First we will make 72-long Enum called cubie_e.
//It will go like this:
// Procedure for counting in cubies: 
// for face1 in (U, R, F, L, B, D), for face2 in (U, R, F, L, B, D) excluding face1 and opposite(face1).
// repeat for e, d, and u type cubies.
typedef enum : uint8_t {
    eUR = 0,
    eUF = 1,
    eUL = 2,
    eUB = 3,
    eRU = 4,
    eRF = 5,
    eRB = 6,
    eRD = 7,
    eFU = 8,
    eFR = 9,
    eFL = 10,
    eFD = 11,
    eLU = 12,
    eLF = 13,
    eLB = 14,
    eLD = 15,
    eBU = 16,
    eBR = 17,
    eBL = 18,
    eBD = 19,
    eDR = 20,
    eDF = 21,
    eDL = 22,
    eDB = 23,
    dUR = 24,
    dUF = 25,
    dUL = 26,
    dUB = 27,
    dRU = 28,
    dRF = 29,
    dRB = 30,
    dRD = 31,
    dFU = 32,
    dFR = 33,
    dFL = 34,
    dFD = 35,
    dLU = 36,
    dLF = 37,
    dLB = 38,
    dLD = 39,
    dBU = 40,
    dBR = 41,
    dBL = 42,
    dBD = 43,
    dDR = 44,
    dDF = 45,
    dDL = 46,
    dDB = 47,
    uUR = 48,
    uUF = 49,
    uUL = 50,
    uUB = 51,
    uRU = 52,
    uRF = 53,
    uRB = 54,
    uRD = 55,
    uFU = 56,
    uFR = 57,
    uFL = 58,
    uFD = 59,
    uLU = 60,
    uLF = 61,
    uLB = 62,
    uLD = 63,
    uBU = 64,
    uBR = 65,
    uBL = 66,
    uBD = 67,
    uDR = 68,
    uDF = 69,
    uDL = 70,
    uDB = 71,
    NUM_CUBIES = 72,
    CUBIE_NULL = 73,
} cubie_e;


//####################################################################################################################################################
// The cube can be defined in 18 bytes, where each byte is a cubie.
// |           CROSS              |                               F2L                             |                        1LLL                   |
// | eFD     eRD     eBD     eLD  |  eFR     dFR     eRB     dRB     eBL     dBL     eLF     dLF  |  eFU     eRU     eBU     uFR     uRB     uBL  |
// [[Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte]]
// Experimentally tested via the 1LLL table hashing that we don't need the last 2 1LLL cubies. That's why cube18B is not cube20B.
//####################################################################################################################################################
typedef struct {
    uint8_t cubies[18]
} cube18B_s;
cube18B_s SOLVED_CUBE18B = {
    .cubies = {eFD, eRD, eLD, eFR, dFR, eRB, dRB, eBL, dBL, eLF, dLF, eFU, eRU, eBU, eFR, uRB, uBL}
};

//Then, we will need a 72x4 table
uint8_t cubieDefinitions[NUM_CUBIES][4] = {
  // e    F      R/U/D
    {0, FACE_U, FACE_R, FACE_NULL}, //eUR
    {0, FACE_U, FACE_F, FACE_NULL}, //eUF
    {0, FACE_U, FACE_L, FACE_NULL}, //eUL
    {0, FACE_U, FACE_B, FACE_NULL}, //eUB
    {0, FACE_R, FACE_U, FACE_NULL}, //eRU
    {0, FACE_R, FACE_F, FACE_NULL}, //eRF
    {0, FACE_R, FACE_B, FACE_NULL}, //eRB
    {0, FACE_R, FACE_D, FACE_NULL}, //eRD
    {0, FACE_F, FACE_U, FACE_NULL}, //eFU
    {0, FACE_F, FACE_R, FACE_NULL}, //eFR
    {0, FACE_F, FACE_L, FACE_NULL}, //eFL
    {0, FACE_F, FACE_D, FACE_NULL}, //eFD
    {0, FACE_L, FACE_U, FACE_NULL}, //eLU
    {0, FACE_L, FACE_F, FACE_NULL}, //eLF
    {0, FACE_L, FACE_B, FACE_NULL}, //eLB
    {0, FACE_L, FACE_D, FACE_NULL}, //eLD
    {0, FACE_B, FACE_U, FACE_NULL}, //eBU
    {0, FACE_B, FACE_R, FACE_NULL}, //eBR
    {0, FACE_B, FACE_L, FACE_NULL}, //eBL
    {0, FACE_B, FACE_D, FACE_NULL}, //eBD
    {0, FACE_D, FACE_R, FACE_NULL}, //eDR
    {0, FACE_D, FACE_F, FACE_NULL}, //eDF
    {0, FACE_D, FACE_L, FACE_NULL}, //eDL
    {0, FACE_D, FACE_B, FACE_NULL}, //eDB
  // d     F       R       D
    {1, FACE_U, FACE_R, FACE_F}, //dUR
    {1, FACE_U, FACE_F, FACE_L}, //dUF
    {1, FACE_U, FACE_L, FACE_B}, //dUL
    {1, FACE_U, FACE_B, FACE_R}, //dUB
    {1, FACE_R, FACE_U, FACE_B}, //dRU
    {1, FACE_R, FACE_F, FACE_U}, //dRF
    {1, FACE_R, FACE_B, FACE_D}, //dRB
    {1, FACE_R, FACE_D, FACE_F}, //dRD
    {1, FACE_F, FACE_U, FACE_R}, //dFU
    {1, FACE_F, FACE_R, FACE_D}, //dFR
    {1, FACE_F, FACE_L, FACE_U}, //dFL
    {1, FACE_F, FACE_D, FACE_L}, //dFD
    {1, FACE_L, FACE_U, FACE_F}, //dLU
    {1, FACE_L, FACE_F, FACE_D}, //dLF
    {1, FACE_L, FACE_B, FACE_U}, //dLB
    {1, FACE_L, FACE_D, FACE_B}, //dLD
    {1, FACE_B, FACE_U, FACE_L}, //dBU
    {1, FACE_B, FACE_R, FACE_U}, //dBR
    {1, FACE_B, FACE_L, FACE_D}, //dBL
    {1, FACE_B, FACE_D, FACE_R}, //dBD
    {1, FACE_D, FACE_R, FACE_B}, //dDR
    {1, FACE_D, FACE_F, FACE_R}, //dDF
    {1, FACE_D, FACE_L, FACE_F}, //dDL
    {1, FACE_D, FACE_B, FACE_L}, //dDB
  // u    F        R       U
    {2, FACE_U, FACE_R, FACE_B}, //uUR
    {2, FACE_U, FACE_F, FACE_R}, //uUF
    {2, FACE_U, FACE_L, FACE_F}, //uUL
    {2, FACE_U, FACE_B, FACE_L}, //uUB
    {2, FACE_R, FACE_U, FACE_F}, //uRU
    {2, FACE_R, FACE_F, FACE_D}, //uRF
    {2, FACE_R, FACE_B, FACE_U}, //uRB
    {2, FACE_R, FACE_D, FACE_B}, //uRD
    {2, FACE_F, FACE_U, FACE_L}, //uFU
    {2, FACE_F, FACE_R, FACE_U}, //uFR
    {2, FACE_F, FACE_L, FACE_D}, //uFL
    {2, FACE_F, FACE_D, FACE_R}, //uFD
    {2, FACE_L, FACE_U, FACE_B}, //uLU
    {2, FACE_L, FACE_F, FACE_U}, //uLF
    {2, FACE_L, FACE_B, FACE_D}, //uLB
    {2, FACE_L, FACE_D, FACE_F}, //uLD
    {2, FACE_B, FACE_U, FACE_R}, //uBU
    {2, FACE_B, FACE_R, FACE_D}, //uBR
    {2, FACE_B, FACE_L, FACE_U}, //uBL
    {2, FACE_B, FACE_D, FACE_L}, //uBD
    {2, FACE_D, FACE_R, FACE_F}, //uDR
    {2, FACE_D, FACE_F, FACE_L}, //uDF
    {2, FACE_D, FACE_L, FACE_B}, //uDL
    {2, FACE_D, FACE_B, FACE_R}, //uDB
};
//Then, we will need a 6x6x4 table
face_e faceAfterMove[NUM_FACES][NUM_FACES][NUM_SIDES] = {
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

//Finally, we will have a global, const, and hopefully cached 72x6x4 table
cubie_e cubieAfterMove[NUM_FACES][NUM_SIDES][NUM_CUBIES];

cubie_e cubie_from_cubieDefinition(const uint8_t* cubieDef) {
    cubie_e cubie1;
    for (cubie_e cubie = eUR; cubie < NUM_CUBIES; cubie++) {
        if (cubieDefinitions[cubie][0] == cubieDef[0] &&
            cubieDefinitions[cubie][1] == cubieDef[1] &&
            cubieDefinitions[cubie][2] == cubieDef[2] &&
            cubieDefinitions[cubie][3] == cubieDef[3]) {
            cubie1 = cubie;
            break;
        }
    } return cubie1;
}
void init_cubieAfterMove() {
    for (cubie_e cubie0 = eUR; cubie0 < NUM_CUBIES; cubie0++) {
        for (face_e move_face = FACE_U; move_face < NUM_FACES; move_face++) {
            for (side_e move_count = SIDE_U; move_count < NUM_SIDES; move_count++) {
                int prefix = cubieDefinitions[cubie0][0];
                face_e cubie0_face1 = cubieDefinitions[cubie0][1];
                face_e cubie0_face2 = cubieDefinitions[cubie0][2];
                face_e cubie0_face3 = cubieDefinitions[cubie0][3];
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

face_e facelet_at_facelet_pos(const shiftCube_s* cube, facelet_pos_s pos) {
    return (((cube->state[pos.face])>>(4*pos.index))&15);
}

cubie_e cubie_from_shiftCube(const shiftCube_s* cube, cubie_e solved_pos) {
    uint8_t solvedCubieDef[4] = cubieDefinitions[solved_pos];
    cubie_e cubie1;
    if (cubeDef[0] == 0) { // cubie is an edge
        for (int i = 0; i < NUM_EDGES; i++) {
            facelet_pos_s edge[2] = edge_pieces[i];
            face_e facelet_colors[2] = {
                facelet_at_facelet_pos(cube, edge[0]),
                facelet_at_facelet_pos(cube, edge[1]),
            };
            cubie_face1 = solvedCubieDef[1];
            cubie_face2 = solvedCubieDef[2];
            if ((cubie_face1 != facelet_colors[0] && cubie_face1 != facelet_colors[1]) ||
                (cubie_face2 != facelet_colors[0] && cubie_face2 != facelet_colors[1])) {
                continue;
            }
            uint8_t cubieDef[4];
            if (facelet_colors[0] == cubie_face2) {
                cubieDef = {0, edge[0].face, edge[1].face, FACE_NULL};
            } else {
                cubieDef = {0, edge[1].face, edge[0].face, FACE_NULL};
            }
            cubie1 = cubie_from_cubieDefinition(cubieDef);
            break;
        }
    } else {
        for (int i = 0; i < NUM_CORNERS; i++) {
            facelet_pos_s corner[3] = corner_pieces[i];
            face_e facelet_colors[3] = {
                facelet_at_facelet_pos(cube, corner[0]),
                facelet_at_facelet_pos(cube, corner[1]),
                faceles_at_facelet_pos(cube, corner[2])
            };
            cubie_face1 = solvedCubieDef[1];
            cubie_face2 = solvedCubieDef[2];
            cubie_face3 = solvedCubieDef[3];
            if ((cubie_face1 != facelet_colors[0] && cubie_face1 != facelet_colors[1] && cubie_face1 != facelet_colors[2]) ||
                (cubie_face2 != facelet_colors[0] && cubie_face2 != facelet_colors[1] && cubie_face2 != facelet_colors[2]) ||
                (cubie_face3 != facelet_colors[0] && cubie_face3 != facelet_colors[1] && cubie_face3 != facelet_colors[2])) {
                continue;
            }
            uint8_t cubieDef[4];
            if (facelet_colors[0] == cubie_face1) {
                if (facelet_colors[1] == cubie_face2) {
                    cubieDef = {solvedCubieDef[0], corner[0].face, corner[1].face, corner[2].face};
                } else {
                    cubieDef = {solvedCubieDef[0], corner[0].face, corner[2].face, corner[1].face};
                }
            } else if (facelet_colors[1] == cubie_face1) {
                if (facelet_colors[0] == cubie_face2) {
                    cubieDef = {solvedCubieDef[0], corner[1].face, corner[0].face, corner[2].face};
                } else {
                    cubieDef = {solvedCubieDef[0], corner[1].face, corner[2].face, corner[0].face};
                }
            } else if (facelet_colors[0] == cubie_face2) {
                cubieDef = {solvedCubieDef[0], corner[2].face, corner[0].face, corner[1].face};
            } else {
                cubieDef = {solvedCubieDef[0], corner[2].face, corner[1].face, corner[0].face};
            }
            cubie1 = cubie_from_cubieDefinition(cubieDef);
            break;
        }
    } return cubie;
}

cube18B_s cube18B_from_shiftCube(const shiftCube_s* shiftcube) {
    cube18B_s cube18B = {
        .cubies = {
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[0]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[1]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[2]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[3]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[4]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[5]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[6]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[7]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[8]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[9]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[10]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[11]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[12]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[13]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[14]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[15]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[16]),
            cubie_from_shiftCube(shiftcube, SOLVED_CUBE18B[18])
        }
    };
    return cube18B;
}

void paint_facelet_onto_shiftCube(shiftCube_s* shiftcube, facelet_pos_s pos, face_e color) {
    shiftcube->state[pos.face] |= (color<<(4*pos.index));
}

void paint_cubie_onto_shiftCube(shiftCube_s* shiftcube, cube_e cubie, cube_e solved_cubie) {
    uint8_t cubieDef[4] = cubieDefinitions[cubie];
    uint8_t solved_cubieDef[4] = cubieDefinitions[solved_cubie];
    if (cubieDef[0] == 0) { // if cubie is an edge
        for (int i = 0; i < NUM_EDGES; i++) {
            facelet_pos_s edge[2] = edge_pieces[i];
            if ((edge[0].face == cubieDef[1] || edge[0].face == cubieDef[2]) &&
                (edge[1].face == cubieDef[1] || edge[1].face == cubieDef[2])) {
                continue;
            }
            if (edge[0].face == cubieDef[1]) {
                paint_facelet_onto_shiftCube(shiftcube, edge[0], solved_cubieDef[1]);
                paint_facelet_onto_shiftCube(shiftcube, edge[1], solved_cubieDef[2]);
            } else {
                paint_facelet_onto_shiftCube(shiftcube, edge[0], solved_cubieDef[2]);
                paint_facelet_onto_shiftCube(shiftcube, edge[1], solved_cubieDef[1]);
            } break;
        }
    } else {
        for (int i = 0; i < NUM_CORNERS; i++) {
            facelet_pos_s corner[3] = corner_pieces[i];
            if ((corner[0].face == cubieDef[1] || corner[0].face == cubieDef[2] || corner[0].face == cubieDef[3]) &&
                (corner[1].face == cubieDef[1] || corner[1].face == cubieDef[2] || corner[1].face == cubieDef[3]) &&
                (corner[2].face == cubieDef[1] || corner[2].face == cubieDef[2] || corner[2].face == cubieDef[3])) {
                continue;
            }
            if (corner[0].face == cubieDef[1]) {
                if (corner[1].face == cubieDef[2]) {
                    paint_facelet_onto_shiftCube(shiftcube, corner[0], solved_cubieDef[1]);
                    paint_facelet_onto_shiftCube(shiftcube, corner[1], solved_cubieDef[2]);
                    paint_facelet_onto_shiftCube(shiftcube, corner[2], solved_cubieDef[3]);
                } else {
                    paint_facelet_onto_shiftCube(shiftcube, corner[0], solved_cubieDef[1]);
                    paint_facelet_onto_shiftCube(shiftcube, corner[1], solved_cubieDef[3]);
                    paint_facelet_onto_shiftCube(shiftcube, corner[2], solved_cubieDef[2]);
                }
            } else if (corner[0].face == cubieDef[2]) {
                if (corner[1].face == cubieDef[1]) {
                    paint_facelet_onto_shiftCube(shiftcube, corner[0], solved_cubieDef[2]);
                    paint_facelet_onto_shiftCube(shiftcube, corner[1], solved_cubieDef[1]);
                    paint_facelet_onto_shiftCube(shiftcube, corner[2], solved_cubieDef[3]);
                } else {
                    paint_facelet_onto_shiftCube(shiftcube, corner[0], solved_cubieDef[2]);
                    paint_facelet_onto_shiftCube(shiftcube, corner[1], solved_cubieDef[3]);
                    paint_facelet_onto_shiftCube(shiftcube, corner[2], solved_cubieDef[1]);
                }
            } else if (corner[1].face == cubieDef[1]) {
                paint_facelet_onto_shiftCube(shiftcube, corner[0], solved_cubieDef[3]);
                paint_facelet_onto_shiftCube(shiftcube, corner[1], solved_cubieDef[1]);
                paint_facelet_onto_shiftCube(shiftcube, corner[2], solved_cubieDef[2]);
            } else {
                paint_facelet_onto_shiftCube(shiftcube, corner[0], solved_cubieDef[3]);
                paint_facelet_onto_shiftCube(shiftcube, corner[1], solved_cubieDef[2]);
                paint_facelet_onto_shiftCube(shiftcube, corner[2], solved_cubieDef[1]);
            } break;
        }
    }
}

shiftCube_s shiftCube_from_cube18B(const cube18B_s* cube18B) {
    // those two cubies we omitted will be null on shiftcube.
    shiftCube_s shiftcube = NULL_CUBE; // must be completely 0s
    for (int i = 0; i < 18; i++) {
        paint_cubie_onto_shiftCube(shiftcube, cube18B->cubies[i], SOLVED_CUBE18B[i]);
    } return shiftcube;
}
