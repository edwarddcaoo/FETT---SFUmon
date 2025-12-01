#ifndef QUEST_H
#define QUEST_H

#include <stdbool.h>
#include "catch.h" // For PetType enum

#define MAX_QUESTS 3

typedef struct
{
    char npc_name[64];     // Which NPC gave this quest
    PetType target_animal; // What type to catch
    int required_count;    // How many needed
    int current_count;     // How many caught so far
    bool is_active;        // Quest is active?
    bool is_completed;     // Quest finished?
} Quest;

typedef struct
{
    Quest quests[MAX_QUESTS];
    int active_count; // How many quests are active
} QuestManager;

// Initialize quest manager
void quest_manager_init(QuestManager *manager);

// Start a new quest
void quest_start(QuestManager *manager, const char *npc_name, PetType animal, int count);

// Update quest progress when catching an animal
void quest_on_catch(QuestManager *manager, PetType animal_type);

// Check if a specific quest is completed
Quest *quest_get_by_npc(QuestManager *manager, const char *npc_name);

// Check if any quest just completed this frame
Quest *quest_check_just_completed(QuestManager *manager);

// Get quest progress string (e.g., "Bears: 2/5")
void quest_get_progress_string(Quest *quest, char *buffer, int buffer_size);

#endif