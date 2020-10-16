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
  sprintf_s(buffer, "%0.2f", hitDiff);
  std::string text = buffer;
  Vector2 screenSize = gameWrapper->GetScreenSize();
  Vector2F stringSize = canvas.GetStringSize(text);
  Vector2 drawLoc = { screenSize.X/2 - (int)stringSize.X/2, screenSize.Y - ((int)stringSize.Y/2)*10 };
  canvas.SetPosition(drawLoc);
  if (hitDiff < 2.0) {
      canvas.SetColor(0, 230, 64, 255);
  }
  else if (hitDiff < 3.0) {
      canvas.SetColor(240, 255, 0, 255);
  }
  else {
      canvas.SetColor(207, 0, 15, 255);
  }
  canvas.DrawString(text, 3, 3);
}