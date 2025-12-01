#include "quest.h"
#include <stdio.h>

typedef struct {
    int active;             // Is quest active?
    int progress;           // How many caught so far
    int needed;             // Required amount
    const char* desc;       // Quest description text
    QuestID id;             // Which quest this is
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
        quests[q].progress,
        quests[q].needed);
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

const char* quest_get_desc(QuestID q)
{
    return quests[q].desc;
}

int quest_get_needed(QuestID q)
{
    return quests[q].needed;
}

int quest_get_progress(QuestID q)
{
    return quests[q].progress;
}
