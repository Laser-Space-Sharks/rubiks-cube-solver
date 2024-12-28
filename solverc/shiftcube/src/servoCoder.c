#include "servoCoder.h"
#include "main.h"
#include "inter_move_table.h"

/*
ROT_TRAINS: dict[str, tuple[tuple[str, int]]] = {
    'U': (('B', 2), ('R', 1), ('F', 0), ('L', 3)),
    'R': (('U', 3), ('B', 0), ('D', 1), ('F', 0)),
    'F': (('U', 0), ('R', 0), ('D', 0), ('L', 0)),
    'L': (('U', 1), ('F', 0), ('D', 3), ('B', 0)),
    'B': (('U', 2), ('L', 0), ('D', 2), ('R', 0)),
    'D': (('F', 0), ('R', 3), ('B', 2), ('L', 1)),
}
*/
static const face_e ROT_TRAINS[6][4] {
    {FACE_B, FACE_R, FACE_F, FACE_L},
    {FACE_U, FACE_B, FACE_D, FACE_F},
    {FACE_U, FACE_R, FACE_D, FACE_L},
    {FACE_U, FACE_F, FACE_D, FACE_B},
    {FACE_U, FACE_L, FACE_D, FACE_R},
    {FACE_F, FACE_R, FACE_B, FACE_L},
};

Orientation_arr6_s Arr6_from_Orientation(Orientation_s O) {
    Orientation_arr6_s wheres06 = {
        .faces = { 0 }
    };
    wheres06.faces[O.face] = FACE_F;
    wheres06.faces[opposite_faces[O.face]] = FACE_B;
    wheres06.faces[ROT_TRAINS[O.face][mod4(0 + O.rot)]] = FACE_U;
    wheres06.faces[ROT_TRAINS[O.face][mod4(1 + O.rot)]] = FACE_R;
    wheres06.faces[ROT_TRAINS[O.face][mod4(2 + O.rot)]] = FACE_D;
    wheres06.faces[ROT_TRAINS[O.face][mod4(3 + O.rot)]] = FACE_L;

    return wheres06;
}
Orientation_s Orientation_from_Arr6(Orientation_arr6_s arr6) {
    Orientation_s O;
    for (face_e pos = 0; pos < 6; pos++) {
        if (arr6.faces[pos] == FACE_F) {
            O.face = pos;
            break;
        }
    }
    for (uint8_t rot = 0; rot < 4; rot++) {
        if (arr6.faces[ROT_TRAINS[O.face][rot]] == FACE_U) {
            O.rot = rot;
            break;
        }
    }
    return O;
}

Orientation_arr6_s multiply_arr6s(Orientation_arr6_s arr1, Orientation_arr6_s arr2) {
    return (Orientation_arr6_s) {
        .faces = {
            arr2.faces[arr1.faces[0]],
            arr2.faces[arr1.faces[1]],
            arr2.faces[arr1.faces[2]],
            arr2.faces[arr1.faces[3]],
            arr2.faces[arr1.faces[4]],
            arr2.faces[arr1.faces[5]],
        }
    };
}

