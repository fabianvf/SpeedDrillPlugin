#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

struct Hits
{
	float lastHitTime = 0.f;
  float avgHitTime = 0.f;
  int numHits = 0;
};

class SpeedDrillPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	Hits hits;
	virtual void onLoad();
	virtual void onUnload();
	/* void OnHitBallPre(CarWrapper cw, void* params, std::string eventName); */
	void OnHitBall(std::string eventName);
	void Render(CanvasWrapper canvas);
};

