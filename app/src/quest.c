#include "quest.h"
#include <stdio.h>
<<<<<<< HEAD
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
=======

typedef struct {
    int active;
    int progress;
    int needed;
    const char* desc;
    QuestID id;
} QuestData;

static QuestData quests[QUEST_COUNT];

void quest_start(QuestID q, int needed, const char* desc)
{
    quests[q].active = 1;
    quests[q].progress = 0;
    quests[q].needed = needed;
    quests[q].desc = desc;
    quests[q].id = q;

    printf("[Quest] STARTED: %s\n", desc);
}

void quest_progress(QuestID q)
{
    if (!quests[q].active)
        return;

    quests[q].progress++;

    printf("[Quest] %s Progress: %d/%d\n",
        quests[q].desc,
        quests[q].progress, quests[q].needed);
}

int quest_is_complete(QuestID q)
{
    return (quests[q].progress >= quests[q].needed);
}

void quest_complete_if_ready(QuestID q)
{
    if (quests[q].active && quest_is_complete(q))
    {
        quests[q].active = 0;
        printf("[Quest] COMPLETE: %s\n", quests[q].desc);
    }
}

int quest_is_active(QuestID q)
{
    return quests[q].active;
}

int quest_get_active_list(QuestID out[], int max)
{
    int count = 0;
    for (int i = 0; i < QUEST_COUNT; i++)
    {
        if (quests[i].active)
        {
            out[count++] = i;
            if (count >= max)
                break;
        }
    }
    return count;
}

int quest_any_active(void)
{
    for (int i = 0; i < QUEST_COUNT; i++)
        if (quests[i].active)
            return 1;
    return 0;
}

int quest_is_in_progress(QuestID q)
{
    return quests[q].active &&
           quests[q].progress > 0 &&
           quests[q].progress < quests[q].needed;
}

int quest_is_ready_to_turn_in(QuestID q)
{
    return quests[q].active &&
           quests[q].progress >= quests[q].needed;
}



const char* quest_get_desc(QuestID q)   { return quests[q].desc; }
int quest_get_needed(QuestID q)         { return quests[q].needed; }
int quest_get_progress(QuestID q)       { return quests[q].progress; }
>>>>>>> 009f1c5 (Morteza stuff)
