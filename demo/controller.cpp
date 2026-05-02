#include "controller.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Scout Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      background: #1a1a2e;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh;
      font-family: sans-serif;
      color: white;
      gap: 20px;
    }
    h1 { font-size: 1.4em; letter-spacing: 2px; color: #e94560; }
    #status { font-size: 0.85em; color: #888; }
    #status.connected { color: #4caf50; }
    #status.disconnected { color: #e94560; }
    #buttons {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 15px;
      width: 300px;
    }
    .cmd-btn {
      background: #16213e;
      border: 2px solid #0f3460;
      border-radius: 12px;
      color: white;
      font-size: 1em;
      padding: 20px;
      cursor: pointer;
      transition: all 0.15s;
      text-align: center;
      touch-action: manipulation;
    }
    .cmd-btn:active {
      background: #e94560;
      border-color: #e94560;
      transform: scale(0.96);
    }
    .cmd-btn.full-width {
      grid-column: span 2;
      background: #0f3460;
      border-color: #e94560;
      font-size: 1.1em;
    }
    .cmd-btn.full-width:active {
      background: #e94560;
    }
    #log {
      width: 300px;
      height: 80px;
      background: #0d0d1a;
      border: 1px solid #0f3460;
      border-radius: 8px;
      padding: 8px;
      font-size: 0.75em;
      color: #4caf50;
      overflow-y: auto;
      font-family: monospace;
    }
  </style>
</head>
<body>
  <h1>⬡ SCOUT</h1>
  <div id="status" class="disconnected">Connecting...</div>
  <div id="buttons">
    <button class="cmd-btn full-width" onclick="send('demo')">▶ DEMO</button>
    <button class="cmd-btn" onclick="send('stand')">↑ STAND</button>
    <button class="cmd-btn" onclick="send('home')">⌂ HOME</button>
    <button class="cmd-btn" onclick="send('stomp')">⬡ STOMP</button>
    <button class="cmd-btn" onclick="send('wave')">✋ WAVE</button>
    <button class="cmd-btn" onclick="send('walk')">⬆ WALK</button>
  </div>
  <div id="log">Waiting for connection...</div>
  <script>
    const statusEl = document.getElementById('status');
    const logEl    = document.getElementById('log');
    function addLog(msg) {
      logEl.innerHTML += msg + '\n';
      logEl.scrollTop = logEl.scrollHeight;
    }
    let socket = new WebSocket('ws://' + location.hostname + '/ws');
    socket.onopen = () => {
      statusEl.textContent = 'Connected';
      statusEl.className = 'connected';
      addLog('Connected to Scout');
    };
    socket.onclose = () => {
      statusEl.textContent = 'Disconnected';
      statusEl.className = 'disconnected';
      addLog('Connection lost');
    };
    socket.onmessage = (e) => {
      addLog('← ' + e.data);
    };
    function send(cmd) {
      if (socket.readyState === WebSocket.OPEN) {
        socket.send(cmd);
        addLog('→ ' + cmd);
      } else {
        addLog('Not connected');
      }
    }
  </script>
</body>
</html>
)rawliteral";

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    String cmd = "";
    for (size_t i = 0; i < len; i++) cmd += (char)data[i];
    cmd.trim();

    Serial.print("Command received: ");
    Serial.println(cmd);

    client->text("Running: " + cmd);

    if (cmd == "demo") {
      demo();
    } else if (cmd == "stand") {
      standUp();
    } else if (cmd == "home") {
      goHome();
    } else if (cmd == "stomp") {
      stomp();
    } else if (cmd == "wave") {
      wave();
    } else {
      client->text("Unknown command: " + cmd);
    }

    client->text("Done: " + cmd);
  }
}

void controllerBegin() {
  WiFi.softAP("ScoutAP", "scout1234");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html; charset=utf-8", index_html);
  });

  server.begin();
  Serial.println("Controller started");
}

void controllerLoop() {
  ws.cleanupClients();
}
void cleanupWebSocket() {
  ws.cleanupClients();
}