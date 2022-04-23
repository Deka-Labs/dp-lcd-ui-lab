#pragma once

typedef struct App App;

App* App_new();
void App_destroy(App* app);
void App_update(App* app);

