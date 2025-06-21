#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include "isautils.h"
#include <string>
#define sizeofA(__aVar)  ((int)(sizeof(__aVar)/sizeof(__aVar[0])))

MYMODCFG(chzbrgr.mapmenutweaks, MapMenuTweaks, 1.0, cheeseburger)
NEEDGAME(com.rockstargames.gtasa)
BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.2)
    ADD_DEPENDENCY_VER(net.rusjj.gtasa.utils, 1.4)
END_DEPLIST()

ISAUtils* sautils = nullptr;
uintptr_t pGame;

bool bisAdvSetExists = false;

const char* stringOptions[] =
{
	"FEM_OFF",
	"FEM_ON",
};

void HideLegend()
{
    ConfigEntry* entry = cfg->Bind("HideLegend", 1, "Main");
    entry->SetInt(1);
    cfg->Save();
    aml->WriteFloat(pGame + 0x2AB66C, 0.0);
    aml->WriteFloat(pGame + 0x2AB728, -999999.9);
}

void UnhideLegend()
{
    ConfigEntry* entry = cfg->Bind("HideLegend", 0, "Main");
    entry->SetInt(0);
    cfg->Save();
    aml->WriteFloat(pGame + 0x2AB66C, 255.0);
    aml->WriteFloat(pGame + 0x2AB728, 0.96205);
}

void LegendSet(int oldValue, int legendVal, void* mapLegend)
{
	switch(legendVal)
	{
		case 0:
			UnhideLegend();
			break;

		case 1:
			HideLegend();
			break;
	}
}

void TouchMoveSensitivitySet(int oldValue, int TsensVal, void* mapTSens)
{
    ConfigEntry* entry = cfg->Bind("TMoveSensitivity", TsensVal, "Main");
    entry->SetInt(TsensVal);
    cfg->Save();
    aml->WriteFloat(pGame + 0x2AA7B8, TsensVal);  
}

void CtrlMoveSensitivitySet(int oldValue, int CsensVal, void* mapCSens)
{
    ConfigEntry* entry = cfg->Bind("CMoveSensitivity", CsensVal, "Main");
    entry->SetInt(CsensVal);
    cfg->Save();
    aml->WriteFloat(pGame + 0x2AA324, CsensVal);  
}

void CtrlZoomSensitivitySet(int oldValue, int CzoomVal, void* mapCZSens)
{
    ConfigEntry* entry = cfg->Bind("CZoomSensitivity", CzoomVal, "Main");
    entry->SetInt(CzoomVal);
    cfg->Save();
    aml->WriteFloat(pGame + 0x2A9E70, CzoomVal);  
}

extern "C" void OnModLoad()
{
    logger->SetTag("Map Menu Tweaks");

    bisAdvSetExists = aml->HasMod("advancedsettings.sa");

    cfg->Bind("Author", "", "About")->SetString("cheeseburger"); cfg->ClearLast();
    pGame = aml->GetLib("libGTASA.so");
    
    // Apply controller bug fixes 
    aml->WriteFloat(pGame + 0x2A9E30, 390.0); // Fixes move stuck
    aml->WriteFloat(pGame + 0x2A9E48, -130.0); // Reduces zoom out limit
    aml->WriteFloat(pGame + 0x2A9E3C, 60.0); // Adjusts horizontal boundary
    aml->WriteFloat(pGame + 0x2A9E50, 350.0); // Fixes right side boundary

    sautils = (ISAUtils*)GetInterface("SAUtils");
    if(sautils != nullptr) { 

        eTypeOfSettings mapTweaksTab = sautils->AddSettingsTab("Map Menu Tweaks");

        if (!bisAdvSetExists)
        {
            int legend = cfg->GetInt("HideLegend", 1, "Main");

            switch (legend)
            {
            case 0:
                UnhideLegend();
                break;
            case 1:
                HideLegend();
                break;
            }

            sautils->AddClickableItem(mapTweaksTab, "Hide Legend", legend, 0, sizeofA(stringOptions)-1, stringOptions, LegendSet);
        }

        int TmoveSens = cfg->GetInt("TMoveSensitivity", 448, "Main");
        aml->WriteFloat(pGame + 0x2AA7B8, TmoveSens);

        sautils->AddSliderItem(mapTweaksTab, "Touch Move Sensitivity", TmoveSens, 150, 1000, TouchMoveSensitivitySet, NULL);

        int CmoveSens = cfg->GetInt("CMoveSensitivity", 128, "Main");
        aml->WriteFloat(pGame + 0x2AA324, CmoveSens);

        sautils->AddSliderItem(mapTweaksTab, "Controller Move Sensitivity", CmoveSens, 100, 700, CtrlMoveSensitivitySet, NULL);

        int CzoomSens = cfg->GetInt("CZoomSensitivity", 100, "Main");
        aml->WriteFloat(pGame + 0x2A9E70, CzoomSens);
        
        sautils->AddSliderItem(mapTweaksTab, "Controller Zoom Sensitivity", CzoomSens, 50, 200, CtrlZoomSensitivitySet, NULL);
    }
}

