#include "SpeedDrillPlugin.h"
#include "bakkesmod/wrappers/includes.h"
#include "utils/parser.h"
#include <stdio.h>

BAKKESMOD_PLUGIN(SpeedDrillPlugin, "Speed Drill plugin", "0.2", PLUGINTYPE_FREEPLAY | PLUGINTYPE_CUSTOM_TRAINING)

void SpeedDrillPlugin::onLoad() {
  std::stringstream ss;
  ss << exports.pluginName << " version: " << exports.pluginVersion;
  cvarManager->log(ss.str());
  cvarManager->registerCvar("cl_speeddrill_display_ballhit_timer", "1", "Display time since last ball hit", true, true, 0, true, 1, true);
  cvarManager->registerCvar("cl_speeddrill_display_ballhit_timer_x", "420", "X position of the ball hit timer display", true, true, 0, true, 3840, true);
  cvarManager->registerCvar("cl_speeddrill_display_ballhit_timer_y", "420", "Y position of the ball hit timer display", true, true, 0, true, 2160, true);
  cvarManager->registerCvar("cl_speeddrill_display_ballhit_timer_size", "3", "Scale of the ball hit timer display", true, true, 1, true, 10, true);

  cvarManager->registerCvar("cl_speeddrill_display_session_timer", "0", "Display running time of freeplay session", true, true, 0, true, 1, true);
  cvarManager->registerCvar("cl_speeddrill_display_session_timer_x", "420", "X position of the session timer display", true, true, 0, true, 3840, true);
  cvarManager->registerCvar("cl_speeddrill_display_session_timer_y", "0", "Y position of the session timer display", true, true, 0, true, 2160, true);
  cvarManager->registerCvar("cl_speeddrill_display_session_timer_size", "3", "Scale of the session timer display", true, true, 1, true, 10, true);



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
    bool renderBallTimer = cvarManager->getCvar("cl_speeddrill_display_ballhit_timer").getBoolValue();
    bool renderSessionTimer = cvarManager->getCvar("cl_speeddrill_display_session_timer").getBoolValue();

    auto currentTime = tutorial.GetSecondsElapsed();
    if (renderBallTimer) {
        Vector2 drawLoc = { cvarManager->getCvar("cl_speeddrill_display_ballhit_timer_x").getIntValue(), cvarManager->getCvar("cl_speeddrill_display_ballhit_timer_y").getIntValue() };
        auto hitDiff = currentTime - hits.lastHitTime;
        char buffer[50];
        sprintf_s(buffer, "%0.2f", hitDiff);
        std::string text = buffer;
        Vector2F stringSize = canvas.GetStringSize(text);
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
        auto scale = cvarManager->getCvar("cl_speeddrill_display_ballhit_timer_size").getFloatValue();
        canvas.DrawString(text, scale, scale);
    }
    if (renderSessionTimer) {
        canvas.SetColor(255, 255, 255, 255);
        Vector2 drawLoc2 = { cvarManager->getCvar("cl_speeddrill_display_session_timer_x").getIntValue(), cvarManager->getCvar("cl_speeddrill_display_session_timer_y").getIntValue() };
        int n = (int)currentTime;
        n = n % (24 * 3600);
        n %= 3600;
        int minutes = n / 60;

        n %= 60;
        int seconds = n;

        char buffer2[50];
        sprintf_s(buffer2, "%dm%ds", minutes, seconds);
        std::string text2 = buffer2;
        Vector2F stringSize2 = canvas.GetStringSize(text2);
        canvas.SetPosition(drawLoc2);
        auto scale2 = cvarManager->getCvar("cl_speeddrill_display_session_timer_size").getFloatValue();
        canvas.DrawString(text2, scale2, scale2);
    }
}