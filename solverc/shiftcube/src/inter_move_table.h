#ifndef INTER_MOVE_TABLE_H
#define INTER_MOVE_TABLE_H

#include "servoCoder.h"

typedef struct {
    State_s endState;
    double weight;
    State_s* path;
    size_t size;
} RSS_sub_entry_s;
typedef struct {
    State_s startState;
    RSS_sub_entry_s* paths;
    size_t length;
    size_t size;
} RSS_entry_s;

typedef struct {
    State_s endState;
    double weight;
    RobotState_s* path;
    size_t size;
} sub_entry_s;
typedef struct {
    RobotState_s startState;
    sub_entry_s* paths;
    size_t length;
    size_t size;
} inter_move_entry_s;

typedef struct inter_move_table inter_move_table_s;

inter_move_table_s* inter_move_table_create();
const RSS_entry_s* inter_move_table_get_RSS(const inter_move_table_s *ht);
const inter_move_entry_s* inter_move_table_lookup(const inter_move_table_s *ht, const RobotState_s *key);
void inter_move_table_free(inter_move_table_s *ht);

#endif // INTER_MOVE_TABLE_H