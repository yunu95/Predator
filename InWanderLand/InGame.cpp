#include "InGame.h"

bool InGame::isPlaying = false;
bool InGame::IsPlaying()
{
    return isPlaying;
}
void InGame::Resume()
{
    isPlaying = true;
}
void InGame::Pause()
{
    isPlaying = false;
}
