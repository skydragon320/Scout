#pragma once
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void demo();
void standUp();
void goHome();
void stomp();
void wave();
void pseudoWalk(int steps);

void controllerBegin();
void controllerLoop();
void cleanupWebSocket(); 