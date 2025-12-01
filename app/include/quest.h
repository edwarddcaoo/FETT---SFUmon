#ifndef QUEST_H
#define QUEST_H

typedef enum {
    QUEST_BEAR_5,
    QUEST_RACCOON_3,
    QUEST_DEER_4,
    QUEST_BIGDEER_2,
    QUEST_COUNT
} QuestID;

// Start a quest with total needed + description
void quest_start(QuestID q, int needed, const char* desc);

// Increment progress by 1
void quest_progress(QuestID q);

// Returns 1 if completed, 0 otherwise
int quest_is_complete(QuestID q);

// Returns 1 if active (started but not fully turned in)
int quest_is_active(QuestID q);

// Mark quest complete if ready
void quest_complete_if_ready(QuestID q);

// Fill an array with all active quests
int quest_get_active_list(QuestID out[], int max);

// Info getters
const char* quest_get_desc(QuestID q);
int quest_get_needed(QuestID q);
int quest_get_progress(QuestID q);

// Any active quests?
int quest_any_active(void);

// Progress but not completed
int quest_is_in_progress(QuestID q);

// Completed but not turned in
int quest_is_ready_to_turn_in(QuestID q);

#endif
