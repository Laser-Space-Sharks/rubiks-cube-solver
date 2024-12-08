typedef struct {
    uint8_t state[18]
} cube18B_s;

cube18B_s SOLVED_CUBE18B = {
    .state = {eFD, eRD, eLD, eFR, dFR, eRB, dRB, eBL, dBL, eLF, dLF, eFU, eRU, eBU, eFR, uRB, uBL}
};

//First we will make 72-long Enum called cubie_e.
//It will go like this:
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
//Then, we will need a 72x4 table
uint8_t cubieDefinitions[NUM_CUBIES][4] = {
  // e    F       R
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
cubie_e cubieAfterMove[NUM_CUBIES][NUM_FACES][NUM_SIDES];

void init_cubieAfterMove() {
    for (cubie_e cubie0 = eUR; cubie0 < NUM_CUBIES; cubie0++) {
        for (face_e move_face = FACE_U; move_face < NUM_FACES; move_face++) {
            for (side_e move_count = SIDE_U; move_count < NUM_SIDES; move_count++) {
                int prefix = cubieDefinitions[cubie0][0];
                face_e cubie0_face1 = cubieDefinitions[cubie0][1];
                face_e cubie0_face2 = cubieDefinitions[cubie0][2];
                face_e cubie0_face3 = cubieDefinitions[cubie0][3];
                face_e cubie1_face1 = faceAfterMove[cubie0_face1][move_face][move_count];
                face_e cubie1_face2 = faceAfterMove[cubie0_face2][move_face][move_count];
                face_e cubie1_face3;
                if (cubie0_face3 == FACE_NULL) cubie1_face3 = FACE_NULL;
                else cubie1_face3 = faceAfterMove[cubie0_face3][move_face][move_count];

                uint8_t cubie1_def[4] = {prefix, cubie1_face1, cubie1_face2, cubie1_face3};
                cubie_e cubie1;
                for (cubie_e cubie = eUR; cubie < NUM_CUBIES; cubie++) {
                    if (cubieDefinitions[cubie][0] == cubie1_def[0] &&
                        cubieDefinitions[cubie][1] == cubie1_def[1] &&
                        cubieDefinitions[cubie][2] == cubie1_def[2] &&
                        cubieDefinitions[cubie][3] == cubie1_def[3]) {
                        cubie1 = cubie;
                        break;
                    }
                }
                cubieAfterMove[cubie0][move_face][move_count] = cubie1;
            }
        }
    }
}

void cube18B_apply_move(cube18B_s* cube, move_s move) {
    //####################################################################################################################################################
    // The cube can be defined in 18 bytes, where each byte is a cubie.
    // |           CROSS              |                               F2L                             |                        1LLL                   |
    // | eFD     eRD     eBD     eLD  |  eFR     dFR     eRB     dRB     eBL     dBL     eLF     dLF  |  eFU     eRU     eBU     uFR     uRB     uBL  |
    // [[Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte], [Byte]]
    // Experimentally tested via the 1LLL table hashing that we don't need the last 2 1LLL cubies. That's why cubie18B is not cube20B.
    //####################################################################################################################################################
    uint8_t turns = move.turns&3;
    face_e face = move.face;
    (*cube)[0]  =  cubieAfterMove[(*cube)[0]][face][turns];
    (*cube)[1]  =  cubieAfterMove[(*cube)[1]][face][turns];
    (*cube)[2]  =  cubieAfterMove[(*cube)[2]][face][turns];
    (*cube)[3]  =  cubieAfterMove[(*cube)[3]][face][turns];
    (*cube)[4]  =  cubieAfterMove[(*cube)[4]][face][turns];
    (*cube)[5]  =  cubieAfterMove[(*cube)[5]][face][turns];
    (*cube)[6]  =  cubieAfterMove[(*cube)[6]][face][turns];
    (*cube)[7]  =  cubieAfterMove[(*cube)[7]][face][turns];
    (*cube)[8]  =  cubieAfterMove[(*cube)[8]][face][turns];
    (*cube)[9]  =  cubieAfterMove[(*cube)[9]][face][turns];
    (*cube)[10] = cubieAfterMove[(*cube)[10]][face][turns];
    (*cube)[11] = cubieAfterMove[(*cube)[11]][face][turns];
    (*cube)[12] = cubieAfterMove[(*cube)[12]][face][turns];
    (*cube)[13] = cubieAfterMove[(*cube)[13]][face][turns];
    (*cube)[14] = cubieAfterMove[(*cube)[14]][face][turns];
    (*cube)[15] = cubieAfterMove[(*cube)[15]][face][turns];
    (*cube)[16] = cubieAfterMove[(*cube)[16]][face][turns];
    (*cube)[17] = cubieAfterMove[(*cube)[17]][face][turns];
}
