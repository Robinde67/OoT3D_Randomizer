#include <stdlib.h>
#include <time.h>
#include <3ds.h>
#include "menu.hpp"
#include "patch.hpp"
#include "settings.hpp"

<<<<<<< HEAD
bool seedChanged;
=======
namespace {
>>>>>>> 060d40cfd0bc166cbe95791131596c6123119700
u8 mode;
u8 settingIdx;
u8 menuIdx;
u8 settingBound;
u8 menuBound;
Menu* currentMenu;
Option* currentSetting;
PrintConsole topScreen, bottomScreen;
}

void PrintTopScreen() {
  consoleSelect(&topScreen);
  consoleClear();
  printf("\x1b[1;10HOoT3D Randomizer testing!\n");
  printf("\x1b[3;1HA/B/D-pad: Navigate Menu\n");
  printf("   Select: Exit to Homebrew Menu\n");
  printf("    Start: Create Patch\n");
  printf("        Y: New Random Seed\n");
  printf("        X: Input Custom Seed\n");
  printf("\x1b[10;5HCurrent Seed: %s", Settings::seed.c_str());
}

void MenuInit() {
<<<<<<< HEAD
  srand(time(NULL));
  consoleInit(GFX_TOP,    &topScreen);
  consoleInit(GFX_BOTTOM, &bottomScreen);
  consoleSelect(&topScreen);

  PrintTopScreen();

  seedChanged = false;
  mode = MAIN_MENU;
  menuIdx = 0;
  menuBound = 0;
  settingIdx = 0;
  settingBound = 0;
  currentMenu = Settings::mainMenu[menuIdx];
  currentSetting = Settings::mainMenu[menuIdx]->settingsList[settingIdx];
}

void MenuUpdate(u32 kDown) {
  //clear the bottom console if a button was pressed
  if (kDown & KEY_DUP || kDown & KEY_DDOWN || kDown & KEY_DLEFT || kDown & KEY_DRIGHT || kDown & KEY_A || kDown & KEY_B) {
			consoleSelect(&bottomScreen);
			consoleClear();
	}

	if (kDown & KEY_START) {
    GenerateRandomizer();
    return;
  }

	//Check for a menu change
	if (kDown & KEY_A && mode == MAIN_MENU) {
		mode = SUB_MENU;
		settingIdx = 0;
		currentSetting = currentMenu->settingsList[settingIdx];
	} else if (kDown & KEY_B && mode == SUB_MENU) {
		mode = MAIN_MENU;
		currentMenu = Settings::mainMenu[menuIdx];
	}

  //Check for seed change
  if (kDown & KEY_Y) {
    Settings::seed = std::to_string(rand());
    seedChanged = true;
  }

  if (kDown & KEY_X) {
    GetInputSeed();
    seedChanged = true;
  }

  //reprint top console if the seed changed
  if (seedChanged) {
    PrintTopScreen();
    seedChanged = false;
  }

	//Print current menu
	consoleSelect(&bottomScreen);
	if (mode == MAIN_MENU) {
		UpdateMainMenu(kDown);
		PrintMainMenu();
	} else if (mode == SUB_MENU) {
		UpdateSubMenu(kDown);
		PrintSubMenu();
	}
}

void UpdateMainMenu(u32 kDown) {
	if (kDown & KEY_DUP)
      menuIdx--;

	if (kDown & KEY_DDOWN)
			menuIdx++;

  //bounds checking
  if (menuIdx == 0xFF) menuIdx = Settings::mainMenu.size() - 1;
  if (menuIdx == Settings::mainMenu.size()) menuIdx = 0;

	currentMenu = Settings::mainMenu[menuIdx];
}

void UpdateSubMenu(u32 kDown) {
	if (kDown & KEY_DUP)
    settingIdx--;

	if (kDown & KEY_DDOWN)
		settingIdx++;

  //bounds checking
  if (settingIdx == 0xFF) settingIdx = currentMenu->settingsList.size() - 1;
  if (settingIdx == currentMenu->settingsList.size()) settingIdx = 0;

	currentSetting = currentMenu->settingsList[settingIdx];

	if (kDown & KEY_DRIGHT) {
		currentSetting->selectedOption++;
	}
	if (kDown & KEY_DLEFT) {
		currentSetting->selectedOption--;
	}

  //bounds checking
  if (currentSetting->selectedOption == 0xFF)
    currentSetting->selectedOption = currentSetting->options.size() - 1;
  if (currentSetting->selectedOption == currentSetting->options.size())
    currentSetting->selectedOption = 0;
}

void PrintMainMenu() {
	printf("\x1b[0;%dHMain Settings Menu", (BOTTOM_COLUMNS/2) - 9);

	for (u8 i = 0; i < MAX_SETTINGS_ON_SCREEN; i++) {
		if (i + menuBound >= Settings::mainMenu.size()) break;

		Menu* menu = Settings::mainMenu[i + menuBound];


    u8 row = 3 + i;
    //make the current menu green
		if (menuIdx == i + menuBound) {
			printf("\x1b[%d;%dH\x1b[32m>", row,  1);
			printf("\x1b[%d;%dH%s\x1b[0m", row,  2, menu->name.c_str());
		} else {
			printf("\x1b[%d;%dH%s\x1b[0m", row,  2, menu->name.c_str());
		}
	}
}

void PrintSubMenu() {
	//bounds checking incase settings go off screen
	if (settingIdx >= settingBound + MAX_SETTINGS_ON_SCREEN) {
		settingBound = settingBound + MAX_SETTINGS_ON_SCREEN;
	} else if (settingIdx < settingBound)  {
		settingBound = settingIdx;
	}

	//print menu name
	printf("\x1b[0;%dH%s", (BOTTOM_COLUMNS/2) - (currentMenu->name.length()/2), currentMenu->name.c_str());

	for (u8 i = 0; i < MAX_SETTINGS_ON_SCREEN; i++) {
    //break if there are no more settings to print
		if (i + settingBound >= currentMenu->settingsList.size()) break;

		Option* setting = currentMenu->settingsList[i + settingBound];

		u8 row = 3 + (i * 2);
    //make the current setting green
		if (settingIdx == i + settingBound) {
			printf("\x1b[%d;%dH\x1b[32m>", row,  1);
			printf("\x1b[%d;%dH%s:",       row,  2, setting->name.c_str());
			printf("\x1b[%d;%dH%s\x1b[0m", row, 26, setting->options[setting->selectedOption].c_str());
		} else {
			printf("\x1b[%d;%dH%s:",       row,  2, setting->name.c_str());
			printf("\x1b[%d;%dH%s",        row, 26, setting->options[setting->selectedOption].c_str());
		}

		setting->SetVariable();
	}
}

void GenerateRandomizer() {
  //if a blank seed was entered
  if (Settings::seed == "") {
    Settings::seed = std::to_string(rand());
  }
  unsigned int seedInt = std::hash<std::string>{}(Settings::seed);

  consoleSelect(&topScreen);
	consoleClear();

  Overrides.clear();
	int ret = Playthrough::Fill(0, seedInt, Overrides);
	if (ret < 0) {
		printf("Error %d with fill. Press Start to exit.\n", ret);
		return;
	}
	printf("about to write overrides to patch\n");
	if (WriteOverridesToPatch()) {
		printf("Created the patch. Have fun!\n");
	} else {
		printf("Error creating patch. Press Start to exit.\n");
	}
=======
    consoleInit(GFX_TOP,    &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);
    consoleSelect(&topScreen);

    printf("OoT3D Randomizer testing!\n");
    printf("Press Select to exit!\n");
    printf("Use D-pad to select settings!\n");
    printf("Press Start to create the patch!\n");

    mode = MAIN_MENU;
    menuIdx = 0;
    menuBound = 0;
    settingIdx = 0;
    settingBound = 0;
    currentMenu = Settings::mainMenu[menuIdx];
    currentSetting = Settings::mainMenu[menuIdx]->settingsList[settingIdx];
}

void MenuUpdate(u32 kDown) {
    //clear the console if a button was pressed
    if (kDown & KEY_DUP || kDown & KEY_DDOWN || kDown & KEY_DLEFT || kDown & KEY_DRIGHT || kDown & KEY_A || kDown & KEY_B) {
        consoleSelect(&bottomScreen);
        consoleClear();
    }

    if (kDown & KEY_START) {
        GenerateRandomizer();
        return;
    }


    //Check for a menu change
    if (kDown & KEY_A && mode == MAIN_MENU) {
        mode = SUB_MENU;
        settingIdx = 0;
        currentSetting = currentMenu->settingsList[settingIdx];
    } else if (kDown & KEY_B && mode == SUB_MENU) {
        mode = MAIN_MENU;
        currentMenu = Settings::mainMenu[menuIdx];
    }

    //Print current menu
    consoleSelect(&bottomScreen);
    if (mode == MAIN_MENU) {
        UpdateMainMenu(kDown);
        PrintMainMenu();
    } else if (mode == SUB_MENU) {
        UpdateSubMenu(kDown);
        PrintSubMenu();
    }
    consoleSelect(&topScreen);
}

void UpdateMainMenu(u32 kDown) {
    if (kDown & KEY_DUP && menuIdx != 0) {
        menuIdx--;
    }

    if (kDown & KEY_DDOWN && menuIdx != Settings::mainMenu.size() - 1) {
        menuIdx++;
    }

    currentMenu = Settings::mainMenu[menuIdx];
}

void UpdateSubMenu(u32 kDown) {
    if (kDown & KEY_DUP && settingIdx != 0) {
        settingIdx--;
    }

    if (kDown & KEY_DDOWN && settingIdx != currentMenu->settingsList.size() - 1) {
        settingIdx++;
    }

    currentSetting = currentMenu->settingsList[settingIdx];

    if (kDown & KEY_DRIGHT) {
        if (currentSetting->selectedOption != currentSetting->options.size() - 1) {
            currentSetting->selectedOption++;
        }
    }
    if (kDown & KEY_DLEFT) {
        if (currentSetting->selectedOption != 0) {
            currentSetting->selectedOption--;
        }
    }
}

void PrintMainMenu() {
    printf("\x1b[0;%dHMain Settings Menu", (BOTTOM_COLUMNS/2) - 9);

    for (u8 i = 0; i < MAX_SETTINGS_ON_SCREEN; i++) {
        if (i + static_cast<u32>(menuBound) >= Settings::mainMenu.size()) {
            break;
        }

        const Menu* menu = Settings::mainMenu[i + menuBound];

        // make the current menu green
        const unsigned int row = 3U + i;
        if (menuIdx == i + static_cast<u32>(menuBound)) {
            printf("\x1b[%u;%dH\x1b[32m>", row, 1);
            printf("\x1b[%u;%dH%s\x1b[0m", row, 2, menu->name.c_str());
        } else {
            printf("\x1b[%u;%dH%s\x1b[0m", row, 2, menu->name.c_str());
        }
    }
}

void PrintSubMenu() {
    //bounds checking incase settings go off screen
    if (settingIdx >= settingBound + MAX_SETTINGS_ON_SCREEN) {
        settingBound++;
    } else if (settingIdx < settingBound)  {
        settingBound = settingIdx;
    }

    //print menu name
    printf("\x1b[0;%dH%s", (BOTTOM_COLUMNS/2) - (currentMenu->name.length()/2), currentMenu->name.c_str());

    for (u8 i = 0; i < MAX_SETTINGS_ON_SCREEN; i++) {
        //break if there are no more settings
        if (i + static_cast<u32>(settingBound) >= currentMenu->settingsList.size()) {
            break;
        }

        Option* setting = currentMenu->settingsList[i + settingBound];
        const unsigned int row = 3U + (i * 2U);

        //make the current setting green
        if (settingIdx == i + static_cast<u32>(settingBound)) {
            printf("\x1b[%u;%dH\x1b[32m>", row,  1);
            printf("\x1b[%u;%dH%s:",       row,  2, setting->name.c_str());
            printf("\x1b[%u;%dH%s\x1b[0m", row, 26, setting->options[setting->selectedOption].c_str());
        } else {
            printf("\x1b[%u;%dH%s:",       row,  2, setting->name.c_str());
            printf("\x1b[%u;%dH%s",        row, 26, setting->options[setting->selectedOption].c_str());
        }

        setting->SetVariable();
    }
}

void GenerateRandomizer() {
    Overrides.clear();
    consoleClear();
    int ret = Playthrough::Fill(0, Settings::seed, Overrides);
    if (ret < 0) {
        printf("Error %d with fill. Press Start to exit.\n", ret);
        return;
    }
    printf("about to write overrides to patch\n");
    if (WriteOverridesToPatch()) {
        printf("Created the patch. Have fun!\n");
    } else {
        printf("Error creating patch. Press Start to exit.\n");
    }
>>>>>>> 060d40cfd0bc166cbe95791131596c6123119700
}

void GetInputSeed() {
  SwkbdState swkbd;
  char mybuf[60];
  SwkbdButton button = SWKBD_BUTTON_NONE;

  swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 1, -1);
	swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_AT | SWKBD_FILTER_PERCENT | SWKBD_FILTER_BACKSLASH | SWKBD_FILTER_PROFANITY, 2);
	swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);
	swkbdSetHintText(&swkbd, "Enter Seed");

  while (button != SWKBD_BUTTON_CONFIRM) {
    button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
  }

  Settings::seed = std::string(mybuf);
}
