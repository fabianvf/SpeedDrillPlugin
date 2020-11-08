#include "SpeedDrillPlugin.h"
#include "bakkesmod/wrappers/includes.h"
#include "utils/parser.h"
#include <stdio.h>

BAKKESMOD_PLUGIN(SpeedDrillPlugin, "Speed Drill plugin", "0.6", PLUGINTYPE_FREEPLAY | PLUGINTYPE_CUSTOM_TRAINING)

void SpeedDrillPlugin::onLoad() {
  std::stringstream ss;
  ss << exports.pluginName << " version: " << exports.pluginVersion;
  cvarManager->log(ss.str());
  cvarManager->registerCvar("cl_speeddrill_display_ballhit_timer", "1", "Display time since last ball hit", true, true, 0, true, 1, true);
  cvarManager->registerCvar("cl_speeddrill_display_ballhit_timer_x", "-1", "X position of the ball hit timer display", true, true, -1, true, 3840, true);
  cvarManager->registerCvar("cl_speeddrill_display_ballhit_timer_y", "-1", "Y position of the ball hit timer display", true, true, -1, true, 2160, true);
  cvarManager->registerCvar("cl_speeddrill_display_ballhit_timer_size", "3", "Scale of the ball hit timer display", true, true, 1, true, 10, true);

  cvarManager->registerCvar("cl_speeddrill_display_session_timer", "1", "Display running time of freeplay session", true, true, 0, true, 1, true);
  cvarManager->registerCvar("cl_speeddrill_display_session_timer_x", "-1", "X position of the session timer display", true, true, -1, true, 3840, true);
  cvarManager->registerCvar("cl_speeddrill_display_session_timer_y", "-1", "Y position of the session timer display", true, true, -1, true, 2160, true);
  cvarManager->registerCvar("cl_speeddrill_display_session_timer_size", "3", "Scale of the session timer display", true, true, 1, true, 10, true);

  cvarManager->registerCvar("cl_speeddrill_yellow_threshold", "2", "Threshold at which the timer turns yellow", true, true, 0, false, 0, true);
  cvarManager->registerCvar("cl_speeddrill_red_threshold", "3", "Threshold at which the timer turns red", true, true, 0, false, 0, true);
  cvarManager->registerCvar("cl_speeddrill_minimum_hit_time", "0", "Minimum time between hits", true, true, 0, false, 0, true);

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
      auto prevHitTime = hits.lastTouchTime;
      auto curHitTime = tutorial.GetSecondsElapsed();
      auto hitDiff = curHitTime - prevHitTime;
      float minHitTime = cvarManager->getCvar("cl_speeddrill_minimum_hit_time").getFloatValue();

      if (hitDiff < minHitTime) {
          hits.lastTouchTime = curHitTime;
      }
      else {
          hits.lastTouchTime = curHitTime;
          hits.lastHitTime = curHitTime;
          hits.numHits++;
          hits.avgHitTime = ((hits.avgHitTime * (hits.numHits - 1)) + hitDiff) / hits.numHits;
      }
    
  }, 0.2f);
}

void SpeedDrillPlugin::Render(CanvasWrapper canvas) {
    if (!gameWrapper->IsInGame())
        return;
    auto tutorial = gameWrapper->GetGameEventAsServer();
    if (tutorial.GetCars().Count() == 0)
        return;
    bool renderBallTimer = cvarManager->getCvar("cl_speeddrill_display_ballhit_timer").getBoolValue();
    bool renderSessionTimer = cvarManager->getCvar("cl_speeddrill_display_session_timer").getBoolValue();

    Vector2 screenSize = gameWrapper->GetScreenSize();
    auto currentTime = tutorial.GetSecondsElapsed();
    if (renderBallTimer) {
        
        auto hitDiff = currentTime - hits.lastHitTime;
        if (hitDiff < 0) {
            hitDiff = 0;
        }
        char buffer[50];
        sprintf_s(buffer, "%0.2f", hitDiff);
        std::string text = buffer;
        auto scale = cvarManager->getCvar("cl_speeddrill_display_ballhit_timer_size").getFloatValue();

        auto x = cvarManager->getCvar("cl_speeddrill_display_ballhit_timer_x").getIntValue();
        auto y = cvarManager->getCvar("cl_speeddrill_display_ballhit_timer_y").getIntValue();
        if (x < 0) {
            Vector2F stringSize = canvas.GetStringSize(text, scale);
            x = screenSize.X / 2 - (int)stringSize.X / 2;
        }
        if (y < 0) {
            Vector2F stringSize = canvas.GetStringSize(text, scale);
            y = screenSize.Y - ((int)stringSize.Y / 2) * 10;
        }
        Vector2 drawLoc = { x, y };
        canvas.SetPosition(drawLoc);
        float yellowThreshold = cvarManager->getCvar("cl_speeddrill_yellow_threshold").getFloatValue();
        float redThreshold = cvarManager->getCvar("cl_speeddrill_red_threshold").getFloatValue();

        if (hitDiff < yellowThreshold) {
            canvas.SetColor(0, 230, 64, 255);
        }
        else if (hitDiff < redThreshold) {
            canvas.SetColor(240, 255, 0, 255);
        }
        else {
            canvas.SetColor(207, 0, 15, 255);
        }
        canvas.DrawString(text, scale, scale);
    }
    if (renderSessionTimer) {
        canvas.SetColor(255, 255, 255, 255);
        int n = (int)currentTime;
        n %= 3600;
        int minutes = n / 60;

        n %= 60;
        int seconds = n;

        char buffer2[50];
        sprintf_s(buffer2, "%dm%ds", minutes, seconds);
        std::string text2 = buffer2;
        auto x2 = cvarManager->getCvar("cl_speeddrill_display_session_timer_x").getIntValue();
        auto y2 = cvarManager->getCvar("cl_speeddrill_display_session_timer_y").getIntValue();
        auto scale2 = cvarManager->getCvar("cl_speeddrill_display_session_timer_size").getFloatValue();
        if (x2 < 0) {
            Vector2F stringSize = canvas.GetStringSize(text2, scale2);
            x2 = screenSize.X / 2 - (int)stringSize.X / 2;
        }
        if (y2 < 0) {
            Vector2F stringSize = canvas.GetStringSize(text2, scale2);
            y2 = ((int)stringSize.Y / 2) * 10;
        }
        Vector2 drawLoc2 = { x2, y2 };
        
        Vector2F stringSize2 = canvas.GetStringSize(text2);
        canvas.SetPosition(drawLoc2);
        canvas.DrawString(text2, scale2, scale2);
    }
}