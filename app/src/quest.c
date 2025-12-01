#include "quest.h"
#include <stdio.h>
#include <string.h>

// Animal type names for display
static const char *ANIMAL_NAMES[] = {
    "Bears",
    "Raccoons",
    "Deer",
    "Big Deer"};

void quest_manager_init(QuestManager *manager)
{
    manager->active_count = 0;

    for (int i = 0; i < MAX_QUESTS; i++)
    {
        manager->quests[i].is_active = false;
        manager->quests[i].is_completed = false;
        manager->quests[i].current_count = 0;
    }

    printf("[Quest] Quest manager initialized\n");
}

void quest_start(QuestManager *manager, const char *npc_name, PetType animal, int count)
{
    // Check if quest already exists for this NPC
    for (int i = 0; i < MAX_QUESTS; i++)
    {
        if (manager->quests[i].is_active &&
            strcmp(manager->quests[i].npc_name, npc_name) == 0)
        {
            printf("[Quest] Quest from %s already active\n", npc_name);
            return;
        }
    }

    // Find empty quest slot
    for (int i = 0; i < MAX_QUESTS; i++)
    {
        if (!manager->quests[i].is_active)
        {
            Quest *quest = &manager->quests[i];

            strncpy(quest->npc_name, npc_name, sizeof(quest->npc_name) - 1);
            quest->target_animal = animal;
            quest->required_count = count;
            quest->current_count = 0;
            quest->is_active = true;
            quest->is_completed = false;

            manager->active_count++;

            printf("[Quest] Started: '%s' wants %d %s\n",
                   npc_name, count, ANIMAL_NAMES[animal]);
            return;
        }
    }

    printf("[Quest] ERROR: No available quest slots!\n");
}

void quest_on_catch(QuestManager *manager, PetType animal_type)
{
    for (int i = 0; i < MAX_QUESTS; i++)
    {
        Quest *quest = &manager->quests[i];

        // Only update active, non-completed quests that match the animal type
        if (quest->is_active && !quest->is_completed &&
            quest->target_animal == animal_type)
        {

            quest->current_count++;

            printf("[Quest] Progress: %s - %d/%d %s\n",
                   quest->npc_name,
                   quest->current_count,
                   quest->required_count,
                   ANIMAL_NAMES[animal_type]);

            // Check if quest is now complete
            if (quest->current_count >= quest->required_count)
            {
                quest->is_completed = true;
                printf("[Quest] ✓ COMPLETED: %s's quest!\n", quest->npc_name);
            }
        }
    }
}

Quest *quest_get_by_npc(QuestManager *manager, const char *npc_name)
{
    for (int i = 0; i < MAX_QUESTS; i++)
    {
        if (manager->quests[i].is_active &&
            strcmp(manager->quests[i].npc_name, npc_name) == 0)
        {
            return &manager->quests[i];
        }
    }
    return NULL;
}

Quest *quest_check_just_completed(QuestManager *manager)
{
    for (int i = 0; i < MAX_QUESTS; i++)
    {
        Quest *quest = &manager->quests[i];

        if (quest->is_active && quest->is_completed)
        {
            // Return the completed quest and mark it as shown
            quest->is_active = false; // Deactivate so it's not shown again
            manager->active_count--;
            return quest;
        }
    }
    return NULL;
}

void quest_get_progress_string(Quest *quest, char *buffer, int buffer_size)
{
    if (!quest || !quest->is_active)
    {
        buffer[0] = '\0';
        return;
    }

    snprintf(buffer, buffer_size, "%s: %d/%d",
             ANIMAL_NAMES[quest->target_animal],
             quest->current_count,
             quest->required_count);
}