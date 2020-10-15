#include "SpeedDrillPlugin.h"
#include "bakkesmod/wrappers/includes.h"
#include "utils/parser.h"
#include <stdio.h>

BAKKESMOD_PLUGIN(SpeedDrillPlugin, "Speed Drill plugin", "0.1", PLUGINTYPE_FREEPLAY | PLUGINTYPE_CUSTOM_TRAINING)

void SpeedDrillPlugin::onLoad() {
  gameWrapper->HookEventPost("Function TAGame.Car_TA.EventHitBall", std::bind(&SpeedDrillPlugin::OnHitBall, this, std::placeholders::_1));
  gameWrapper->RegisterDrawable(std::bind(&SpeedDrillPlugin::Render, this, std::placeholders::_1));
}

void SpeedDrillPlugin::onUnload() {
}

void SpeedDrillPlugin::OnHitBall(std::string eventName) {
  if (!gameWrapper->IsInGame())
    return;
  gameWrapper->SetTimeout([this](GameWrapper* gw) {
    if (!gameWrapper->IsInGame())
      return;
    auto tutorial = gameWrapper->GetGameEventAsServer();
    auto prevHitTime = hits.lastHitTime;
    hits.lastHitTime = tutorial.GetSecondsElapsed();
    auto hitDiff = hits.lastHitTime - prevHitTime;
    hits.numHits++;
    hits.avgHitTime = ((hits.avgHitTime * (hits.numHits - 1)) + hitDiff) / hits.numHits;
  }, 0.2f);
}

void SpeedDrillPlugin::Render(CanvasWrapper canvas) {
  if (!gameWrapper->IsInGame() || hits.lastHitTime == 0)
    return;
  auto tutorial = gameWrapper->GetGameEventAsServer();
  if (tutorial.GetCars().Count() == 0)
    return;

  auto currentTime = tutorial.GetSecondsElapsed();
  auto hitDiff = currentTime - hits.lastHitTime;
  char buffer [50];
  sprintf(buffer, "%0.2f (Average: %0.2f)", hitDiff, hits.avgHitTime);
  std::string text = buffer;
  Vector2 drawLoc = { 0, 0 };
  canvas.SetPosition(drawLoc);
  canvas.SetColor(255, 255, 255, 255);
  canvas.DrawString(text, 3, 3);
}
