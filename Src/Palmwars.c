/************************************************************************ 
 * File:     palmwars.c
 * Purpose:  Mabwars for the Palm
 * Author:   Richard Leggett
 * Language: C
 * History:  04-Jul-01: RML: Created
 *
 * (c)2001 Richard Leggett
 *
 * Still to do:
 *   - It hung when playing a one player vs Mabs game at one point
 ************************************************************************/

#include <PalmOS.h>
#include "PalmwarsRsc.h"
#include "palmwars.h"
#include "palmwarsmaps.h"


/************************************************************************ 
 *
 * Globals
 *
 ************************************************************************/

// General globals
IndexedColorType  playerColoursB[MAX_PLAYERS+2];
IndexedColorType  playerColoursF[MAX_PLAYERS+2];
IndexedColorType  littleStarColour;
IndexedColorType  littleStarColourB;
IndexedColorType  backgroundColour;
IndexedColorType  starOutlineColour;
IndexedColorType  starTextColour;
IndexedColorType  starSelectedColour;
IndexedColorType  bottomBackgroundColour;
IndexedColorType  bottomTextColour;
IndexedColorType  joiningLinesColour;
UInt8             sx[MAX_STARS];
UInt8             sy[MAX_STARS];
char              cs[MAX_STARS][MAX_LINKS+1];
char              listItemsText[50][10];
UInt32            gOldDepth = 0;
Boolean           useColour = false;
PWGAMESTATE       gd;
PWMAPDRAW         md;


/************************************************************************ 
 *
 * Function prototypes
 *
 ************************************************************************/
static double squareroot(UInt8 n);
static Boolean PalmwarsStartFormHandleEvent(EventPtr event);
static Boolean PalmwarsGameFormHandleEvent(EventPtr event);
static Boolean PalmwarsSectorFormHandleEvent(EventPtr event);
static Boolean ApplicationHandleEvent(EventPtr event);
static void SystemListDraw(UInt16 itemNum, RectangleType *bounds, Char **itemsText);
static void PalmwarsSetStartFormUsable(FormPtr frm);
static UInt16 RandomNum(UInt16 n);
static UInt16 NonZeroRandomNum(UInt16 n);
static UInt8 MakeColourIndex(UInt8 r, UInt8 g, UInt8 b);
static void PalmwarsDrawStar(UInt8 star);
static Err StartApplication(void);
static void StopApplication(void);
static void EventLoop(void);
static void PalmwarsInitialise(void);
static void PalmwarsNewGameInitialise(void);
static void PalmwarsInitialiseBlackAndWhite(void);
static void PalmwarsInitialiseColour(void);
static void PalmwarsSetScreenMode(void);
static void PalmwarsDrawLine(UInt8 startStar, UInt8 endStar);
static void PalmwarsWriteStatusText(char *string, UInt8 x, UInt8 y);
static void PalmwarsWriteText(char *string, UInt8 x, UInt8 y, IndexedColorType foregroundColour, IndexedColorType backgroundColour, FontID font);
static void PalmwarsCentreText(char *string, UInt8 y, IndexedColorType foregroundColour, IndexedColorType backgroundColour, FontID font);
static void PalmwarsWriteInStar(UInt8 star, char* string);
static void PalmwarsDrawMap(void);
static void PalmwarsStartStarRequest(void);
static UInt8 PalmwarsReturnStarTap(UInt8 x, UInt8 y);
static void PalmwarsMarkStartStar(UInt8 star);
static void PalmwarsMabsInit(void);
static void PalmwarsStartTurn(void);
static void PalmwarsGameOverPrompt(void);
static void PalmwarsProcessTurn(UInt8 tappedStar);
static void PalmwarsTurnPrompt(void);
static void PalmwarsNextStage(void);
static void PalmwarsClearTextArea(void);
static void PalmwarsMabsTurn(void);
static void PalmwarsMabAttack(void);
static void PalmwarsMabCombat(int f, int t);
static UInt8 PalmwarsShowArmiesForm(UInt8 fromStar);
static void PalmwarsReinforce(UInt8 player);
static void PalmwarsCombat(UInt8 from, UInt8 to, UInt8 numArmies);
static void PalmwarsCheckForWinner(void);
static void PalmwarsSaveState(void);
static void PalmwarsLoadState(void);
static void PalmwarsInitialiseAppPrefs(PWGAMESTATE* state);
static void PalmwarsResetState(void);
static void PalmwarsSetupGameDrawParameters(void);
static void PalmwarsSetupPreviewDrawParameters(void);


/************************************************************************ 
 *
 * Square root code
 *
 ************************************************************************/

double root[82] = {0.000000, 1.000000, 1.414214, 1.732051, 2.000000,
                  2.236068, 2.449490, 2.645751, 2.828427, 3.000000,
                  3.162278, 3.316625, 3.464102, 3.605551, 3.741657,
                  3.872983, 4.000000, 4.123106, 4.242641, 4.358899,
                  4.472136, 4.582576, 4.690416, 4.795832, 4.898979,
                  5.000000, 5.099020, 5.196152, 5.291503, 5.385165,
                  5.477226, 5.567764, 5.656854, 5.744563, 5.830952,
                  5.916080, 6.000000, 6.082763, 6.164414, 6.244998,
                  6.324555, 6.403124, 6.480741, 6.557439, 6.633250,
                  6.708204, 6.782330, 6.855655, 6.928203, 7.000000,
                  7.071068, 7.141428, 7.211103, 7.280110, 7.348469,
                  7.416198, 7.483315, 7.549834, 7.615773, 7.681146,
                  7.745967, 7.810250, 7.874008, 7.937254, 8.000000,
                  8.062258, 8.124038, 8.185353, 8.246211, 8.306624,
                  8.366600, 8.426150, 8.485281, 8.544004, 8.602325,
                  8.660254, 8.717798, 8.774964, 8.831761, 8.888194,
                  8.944272, 9.000000};

double squareroot(UInt8 n)
{
    if (n > 81) n = 81;
    return root[n];
}


/************************************************************************ 
 *
 * FUNCTION:    PilotMain
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
    Err err = 0;        
    
    if (cmd == sysAppLaunchCmdNormalLaunch)
    {
        if ((err = StartApplication()) == 0)
        {
            EventLoop();
            
            StopApplication();
        }
    }
    else if (cmd == sysAppLaunchCmdSystemReset)
    {
        PalmwarsResetState();
    }

    return err;
}


/************************************************************************ 
 *
 * FUNCTION:    EventLoop
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
static void EventLoop(void)
{
    EventType event;
    UInt16    error;
    
    do
    {
        EvtGetEvent(&event, evtWaitForever);
        if (! SysHandleEvent(&event))
            if (! MenuHandleEvent(0, &event, &error))
                if (! ApplicationHandleEvent(&event))
                    FrmDispatchEvent(&event);
    }
    while (event.eType != appStopEvent);
}


/************************************************************************ 
 *
 * FUNCTION:    ApplicationHandleEvent
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
static Boolean ApplicationHandleEvent(EventPtr event)
{
    FormPtr frm;
    Int16   formId;
    Boolean handled = false;

    if (event->eType == frmLoadEvent)
    {
        // Load the form resource specified in the event then activate the form
        formId = event->data.frmLoad.formID;
        frm = FrmInitForm(formId);
        FrmSetActiveForm(frm);

        // Set the event handler for the form. The handler of the currently
        // active form is called by FrmDispatchEvent each time it receives an event
        switch (formId)
        {
            case PalmwarsStartForm:
                FrmSetEventHandler(frm, PalmwarsStartFormHandleEvent);
                break;
            case PalmwarsGameForm:
                FrmSetEventHandler(frm, PalmwarsGameFormHandleEvent);
                break;
            case PalmwarsSectorForm:
                FrmSetEventHandler(frm, PalmwarsSectorFormHandleEvent);
                break;
        }
        handled = true;
    }
    
    return handled;
}


/************************************************************************ 
 *
 * FUNCTION:    StartApplication
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
static Err StartApplication(void)
{
    SysRandom(TimGetTicks());

    PalmwarsLoadState();

    if (gd.gameOn)
        FrmGotoForm(PalmwarsGameForm);
    else
        FrmGotoForm(PalmwarsStartForm);
    
    return 0;
}


/************************************************************************
 *
 * FUNCTION:    StopApplication
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
static void StopApplication(void)
{
    // Save game state
    PalmwarsSaveState();

	// Restore the original screen depth
	if (gOldDepth)
	{
    	WinScreenMode(winScreenModeSet, NULL, NULL, &gOldDepth, NULL);
    	gOldDepth = 0;
    }
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsStartFormHandleEvent
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
static Boolean PalmwarsStartFormHandleEvent(EventPtr event)
{
    FormPtr      frm = FrmGetActiveForm();
    FormType    *form;
    ControlType *ctl;
    FieldPtr     fld;
    Boolean      handled;
    Boolean      playerNameNotFilled = false;
    Boolean      notEnoughPlayers = false;
    Char        *string;
    UInt16       objID;
    UInt8        i;

    handled = false;

    switch (event->eType)
    {
        case menuEvent:     
            switch(event->data.menu.itemID)
            {
                case About:
                    form = FrmInitForm(PalmwarsAboutForm);
                    FrmDoDialog(form);
                    FrmDeleteForm(form);
                    handled = true;
                    break;
            }
            break;

        case ctlSelectEvent:
            switch(event->data.ctlSelect.controlID)
            {
                case NumPlayers1:
                case NumPlayers2:
                case NumPlayers3:
                case NumPlayers4:
                {
                    // Read number of players
                    objID = FrmGetObjectId(frm, FrmGetControlGroupSelection(frm, 1));
                    gd.numPlayers = objID - NumPlayers1 + 1;

                    PalmwarsSetStartFormUsable(frm);
                    
                    for (i=gd.numPlayers + 1; i<=MAX_PLAYERS; i++)
                        StrPrintF(gd.playerNames[i], "Player%d", i);
                            			         
                    break;
                }
                    
                case BeginButton:
                {
                    // Is Mabs playing?
                    ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, MabsCheckbox));
                    gd.withMabs = CtlGetValue(ctl);

                    // Read number of players
                    objID = FrmGetObjectId(frm, FrmGetControlGroupSelection(frm, 1));
                    gd.numPlayers = objID - NumPlayers1 + 1;

                    for (i=1; i<=gd.numPlayers; i++)
                    {
                        // Read player name field
                        fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, Player1Name + i - 1));

                        // Check player name is filled
                        string = FldGetTextPtr(fld);
                        if (string)
                        {
                            StrCopy(gd.playerNames[i], string);
                        }
                        else
                        {
                            playerNameNotFilled = true;
                            break;
                        }
                    }

                    if ((gd.numPlayers == 1) && (!gd.withMabs))
                        notEnoughPlayers = true;

                    // Pop up any alerts, if relevant, or go to main game
                    if (notEnoughPlayers)
                    {
                        FrmAlert(InsufficientPlayersAlert);
                    }
                    else if (playerNameNotFilled)
                    {
                        FrmAlert(PlayerNamesAlert);
                    }
                    else
                    {
                        FrmGotoForm(PalmwarsSectorForm);
                    }

                    handled = true;
                }
            }
            break;
    
        case frmOpenEvent:
            // Initialise
            PalmwarsInitialise();
            PalmwarsNewGameInitialise();

            // Set number of players
            ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, NumPlayers1 + gd.numPlayers - 1));
            CtlSetValue(ctl, 1);
            
            // Set player names
			for (i=1; i<=gd.numPlayers; i++)
			{
	            fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, Player1Name + i - 1));
	            FldInsert(fld, gd.playerNames[i], StrLen(gd.playerNames[i]));
			}

            PalmwarsSetStartFormUsable(frm);

            // Set Mabs
            ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, MabsCheckbox));
            CtlSetValue(ctl, gd.withMabs);

            // Draw the form
            FrmDrawForm(FrmGetActiveForm());

            // Mark this event as handled
            handled = true;
            break;
    }

    return(handled);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsGameFormHandleEvent
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
static Boolean PalmwarsGameFormHandleEvent(EventPtr event)
{
    FormType *form;
    Boolean   handled;
    UInt8     tappedStar;

    handled = false;

    switch (event->eType)
    {    
        case frmOpenEvent:
            PalmwarsSetScreenMode();
            FrmDrawForm(FrmGetActiveForm());
            PalmwarsSetupGameDrawParameters();
            PalmwarsDrawMap();
            if (gd.gameOn)
            {
                switch(gd.gameState)
                {
                    case GAMESTATE_FINISHED:
                        PalmwarsGameOverPrompt();
                        break;
                    case GAMESTATE_PLAYERINIT:
                        PalmwarsStartStarRequest();
                        break;
                    default:
                        PalmwarsTurnPrompt();
                        break;
                }
            }
            else
            {
                gd.gameState = GAMESTATE_PLAYERINIT;
                gd.currentPlayer = 1;
                PalmwarsStartStarRequest();
                gd.gameOn = true;
            }

            handled = true;
            break;
        
        case menuEvent:     
            switch(event->data.menu.itemID)
            {
                case NewGame:
                    FrmGotoForm(PalmwarsStartForm);
                    handled = true;
                    break;
                case About:
                    form = FrmInitForm(PalmwarsAboutForm);
                    FrmDoDialog(form);
                    FrmDeleteForm(form);
                    handled = true;
                    break;
            }
            break;
        
        case penDownEvent:
            tappedStar = PalmwarsReturnStarTap(event->screenX, event->screenY);
            if (tappedStar > 0)
            {
                tappedStar--;

                // We have a small pen tap state machine
                switch(gd.gameState)
                {
                    case GAMESTATE_PLAYERINIT:
                        if (gd.occupier[tappedStar] == (PLAYER_NONE))
                        {
                            PalmwarsMarkStartStar(tappedStar);
                            gd.currentPlayer++;
                            if (gd.currentPlayer <= gd.numPlayers)
                            {
                                PalmwarsStartStarRequest();
                            }
                            else
                            {
                                if (gd.withMabs) PalmwarsMabsInit();
                                PalmwarsStartTurn();
                                gd.gameState = GAMESTATE_TURN;
                            }
                        }
                        break;

                    case GAMESTATE_TURN:
                        PalmwarsProcessTurn(tappedStar);
                        break;
                }
                handled = true;
            }
            break;
    }

    return(handled);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsSectorFormHandleEvent
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
static Boolean PalmwarsSectorFormHandleEvent(EventPtr event)
{
    FormPtr      frm = FrmGetActiveForm();
    FormType*    form;
    Boolean      handled = false;
    Char         string[32];

    switch (event->eType)
    {    
        case frmOpenEvent:
            PalmwarsSetScreenMode();
           
            // Set up scroll bar
            StrPrintF(string, "Sector %c", gd.currentSystem + 'A');
            FrmCopyLabel(frm, SectorName, string);
           
            // Draw the form
            FrmDrawForm(FrmGetActiveForm());
        
            PalmwarsSetupPreviewDrawParameters();
            PalmwarsDrawMap();
        
            // Mark this event as handled
            handled = true;
            break;
            
        case menuEvent:     
            switch(event->data.menu.itemID)
            {
                case About:
                    form = FrmInitForm(PalmwarsAboutForm);
                    FrmDoDialog(form);
                    FrmDeleteForm(form);
                    handled = true;
                    break;
            }
            break;
            
        case ctlSelectEvent:
            switch(event->data.ctlSelect.controlID)
            {
                case BeginButton:
                    FrmGotoForm(PalmwarsGameForm);
                    handled = true;                                   
                    break;
                    
                case LeftArrowButton:
                    if (gd.currentSystem > 0)
                        gd.currentSystem--;
                    else
                        gd.currentSystem = gd.numSystems - 1;
                    
                    StrPrintF(string, "Sector %c", gd.currentSystem + 'A');
                    FrmCopyLabel(frm, SectorName, string);
                    PalmwarsDrawMap();
                    handled = true;
                    break;
                    
                case RightArrowButton:
                    gd.currentSystem++;
                    
                    if (gd.currentSystem == gd.numSystems)
                        gd.currentSystem = 0;
                    
                    StrPrintF(string, "Sector %c", gd.currentSystem + 'A');
                    FrmCopyLabel(frm, SectorName, string);
                    PalmwarsDrawMap();                
                    handled = true;                  
                    break;
            }
            break;
    }
        
    return(handled);
}


/************************************************************************
 *
 * FUNCTION:    SystemListDraw
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
static void SystemListDraw(UInt16 itemNum, RectangleType *bounds, Char **itemsText)
{
    WinDrawChars(listItemsText[itemNum], StrLen(listItemsText[itemNum]),
                 bounds->topLeft.x, bounds->topLeft.y);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsSetStartFormUsable
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsSetStartFormUsable(FormPtr frm)
{
    int i;

    // Set fields usable
	for (i=1; i<=MAX_PLAYERS; i++)
	{
        if (i <= gd.numPlayers)
        {
		    FrmShowObject(frm, FrmGetObjectIndex(frm, Player1Name + i - 1));
		    FrmShowObject(frm, FrmGetObjectIndex(frm, Player1Label + i - 1));
		}
		else
		{
		    FrmHideObject(frm, FrmGetObjectIndex(frm, Player1Name + i - 1));
		    FrmHideObject(frm, FrmGetObjectIndex(frm, Player1Label + i - 1));
		}		    
	} 
}


/************************************************************************
 *
 * FUNCTION:    RandomNum
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
UInt16 RandomNum(UInt16 n)
{
    if (n <= 0) return 0;

    return (SysRandom(0) % n);
}


/************************************************************************
 *
 * FUNCTION:    NonZeroRandomNum
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
UInt16 NonZeroRandomNum(UInt16 n)
{
    UInt16 r;

    if (n <= 0) return 0;

    r = RandomNum(n) + 1;

    return r;
}


/************************************************************************
 *
 * FUNCTION:    MakeColourIndex
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
UInt8 MakeColourIndex(UInt8 r, UInt8 g, UInt8 b)
{
    RGBColorType rgb;

    rgb.r = r;
    rgb.g = g;
    rgb.b = b;

    return WinRGBToIndex(&rgb);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsInitialise
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsInitialise(void)
{
    PWMAPHEADER *mapHeader;
    MemHandle    resourceHandle;
    UInt8        i;

    // Locate map data
    resourceHandle = DmGetResource('maps', MapData);
    mapHeader = (PWMAPHEADER*)MemHandleLock(resourceHandle);
    gd.numSystems = mapHeader->numSystems;
    MemHandleUnlock(resourceHandle);
    DmReleaseResource(resourceHandle);

    // Make sector menu items
    for (i=0; i<26; i++)
        StrPrintF(listItemsText[i], "%c", 65 + i);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsNewGameInitialise
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsNewGameInitialise(void)
{
    UInt8 i;

    for (i=0; i<MAX_STARS; i++)
    {
        gd.armies[i] = 0;
        gd.occupier[i] = PLAYER_NONE;
    }

    gd.gameOn = false;
    gd.turn = 1;
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsInitialiseBlackAndWhite
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsInitialiseBlackAndWhite(void)
{
    // First, the player colours, noting that...
    //     0 = Mabs
    //     5 = Empty star

    playerColoursB[0] = 12;
    playerColoursF[0] = 0;
    playerColoursB[1] = 2;
    playerColoursF[1] = 15;
    playerColoursB[2] = 7;
    playerColoursF[2] = 15;
    playerColoursB[3] = 4;
    playerColoursF[3] = 0;
    playerColoursB[4] = 10;
    playerColoursF[4] = 0;
    playerColoursB[5] = 0;
    playerColoursF[5] = 0;

    // Now other colours

    backgroundColour = 15;
    littleStarColour = 0;
    littleStarColourB = 3;
    starOutlineColour = 0;
    joiningLinesColour = 0;
    starTextColour = 0;
    starSelectedColour = 0;
    bottomTextColour = 0;
    bottomBackgroundColour = backgroundColour;
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsInitialiseColour
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsInitialiseColour(void)
{
    // First, the player colours, noting that...
    //     0 = Mabs
    //     5 = Empty star

    playerColoursB[0] = MakeColourIndex(255, 0, 255); //200, 0, 100);
    playerColoursF[0] = MakeColourIndex(0, 0, 0);
    playerColoursB[1] = MakeColourIndex(255, 0, 0);
    playerColoursF[1] = MakeColourIndex(0, 0, 0);
    playerColoursB[2] = MakeColourIndex(0, 255, 0);
    playerColoursF[2] = MakeColourIndex(0, 0, 0);
    playerColoursB[3] = MakeColourIndex(255, 255, 0);
    playerColoursF[3] = MakeColourIndex(0, 0, 0);
    playerColoursB[4] = MakeColourIndex(0, 255, 255);
    playerColoursF[4] = MakeColourIndex(0, 0, 0);
    playerColoursB[5] = MakeColourIndex(180, 180, 180);
    playerColoursF[5] = MakeColourIndex(0, 0, 0);

    // Now other colours

    littleStarColour = MakeColourIndex(255, 255, 255);
    littleStarColourB = MakeColourIndex(128, 128, 128);
    backgroundColour = MakeColourIndex(0, 0, 0);
    starOutlineColour = MakeColourIndex(180, 180, 180);
    joiningLinesColour = MakeColourIndex(200, 200, 200);
    starTextColour = MakeColourIndex(0, 0, 0);
    starSelectedColour = MakeColourIndex(255, 255, 255);
    bottomTextColour = MakeColourIndex(255, 255, 255);
    bottomBackgroundColour = backgroundColour;
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsSetScreenMode
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsSetScreenMode(void)
{
    UInt32 gSupportedDepths;
    UInt32 depth = 4;

    // Store current screen mode, to restore on exit
    if (gOldDepth == 0)
    	WinScreenMode(winScreenModeGet, NULL, NULL, &gOldDepth, NULL);

    // Get support colour depths
    WinScreenMode(winScreenModeGetSupportedDepths, NULL, NULL, &gSupportedDepths, NULL);
    
    // Set palette based on best colour depth
    if (gSupportedDepths & 0x80) // 8bpp colour
    {
        useColour = true;
        depth = 8;
        PalmwarsInitialiseColour();
        WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL);
    }
    else if (gSupportedDepths & 0x08) // 4bpp greyscale
    {
        useColour = false;
        depth = 4;
        PalmwarsInitialiseBlackAndWhite();
        WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL);
    }
    else // Unplayable in lower than 4 bpp
    {
        useColour = false;
        PalmwarsInitialiseBlackAndWhite();
    }
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsDrawLine
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsDrawLine(UInt8 startStar, UInt8 endStar)
{
    WinDrawLine(md.mapXOff + (sx[startStar] * md.mapXMult), md.mapYOff + (sy[startStar] * md.mapXMult),
                md.mapXOff + (sx[endStar] * md.mapYMult), md.mapYOff + (sy[endStar] * md.mapYMult));
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsWriteStatusText
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsWriteStatusText(char *string, UInt8 x, UInt8 y)
{
    FontID           oldFont;
    IndexedColorType oldForeColour;
    IndexedColorType oldBackColour;

    // Select pen, remembering current one
    oldFont= FntSetFont(stdFont);
    oldBackColour = WinSetBackColor(bottomBackgroundColour);
    oldForeColour = WinSetTextColor(bottomTextColour);

    WinDrawChars(string, StrLen(string), x, y);
 
    // Set pen back to what it was before we started
    WinSetTextColor(oldForeColour);
    WinSetBackColor(oldBackColour);
    FntSetFont(oldFont);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsWriteText
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsWriteText(char *string, UInt8 x, UInt8 y,
                       IndexedColorType foregroundColour, 
                       IndexedColorType backgroundColour,
                       FontID font)
{
    FontID           oldFont;
    IndexedColorType oldForeColour;
    IndexedColorType oldBackColour;

    // Select pen, remembering current one
    oldFont= FntSetFont(font);
    oldBackColour = WinSetBackColor(backgroundColour);
    oldForeColour = WinSetTextColor(foregroundColour);

    WinDrawChars(string, StrLen(string), x, y);

    // Set pen back to what it was before we started
    WinSetTextColor(oldForeColour);
    WinSetBackColor(oldBackColour);
    FntSetFont(oldFont);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsWriteText
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsCentreText(char *string, UInt8 y,
                       IndexedColorType foregroundColour, 
                       IndexedColorType backgroundColour,
                       FontID font)
{
    FontID           oldFont;
    IndexedColorType oldForeColour;
    IndexedColorType oldBackColour;
    UInt8            x;

    // Select pen, remembering current one
    oldFont= FntSetFont(font);
    oldBackColour = WinSetBackColor(backgroundColour);
    oldForeColour = WinSetTextColor(foregroundColour);

    x = (160 - FntLineWidth(string, StrLen(string))) / 2;
    WinDrawChars(string, StrLen(string), x, y);

    // Set pen back to what it was before we started
    WinSetTextColor(oldForeColour);
    WinSetBackColor(oldBackColour);
    FntSetFont(oldFont);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsDrawStar
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsDrawStar(UInt8 star)
{
    IndexedColorType oldColour;
    RectangleType    rect;
    short            innerRadius = md.starRadius - 0;
    UInt8            x = md.mapXOff + (sx[star] * md.mapXMult);
    UInt8            y = md.mapYOff + (sy[star] * md.mapYMult);
    char             string[8];

#ifdef DEBUG
    printf("%c. %d (%d)\n", star+64, gd.occupier[star], gd.armies[star]);
#else

    // If star selected, draw an outer line
    if (gd.selectedStar == star)
    {
        // Select pen, remembering current one
        oldColour = WinSetForeColor(starSelectedColour);

        // Draw outer circle
        rect.topLeft.x = x - (innerRadius/2);
        rect.topLeft.y = y - (innerRadius/2);
        rect.extent.x = innerRadius;
        rect.extent.y = innerRadius;
        WinDrawRectangle(&rect, innerRadius/2);

        innerRadius -= 2;

        // Set pen back to what it was before we started
        WinSetForeColor(oldColour);
    }

    // Select pen, remembering current one
    oldColour = WinSetForeColor(starOutlineColour);

    // Set star colour according to occupier
    WinSetForeColor(playerColoursB[gd.occupier[star]]);

    // Draw inner circle
    rect.topLeft.x = x - (innerRadius/2);
    rect.topLeft.y = y - (innerRadius/2);
    rect.extent.x = innerRadius;
    rect.extent.y = innerRadius;
    WinDrawRectangle(&rect, innerRadius/2);

    // Set pen back to what it was before we started
    WinSetForeColor(oldColour);

    // If the star has armies in it, label it
    if (gd.armies[star] > 0)
    {
        oldColour = WinSetTextColor(playerColoursF[gd.occupier[star]]);
        StrPrintF(string, "%d", gd.armies[star]);
        PalmwarsWriteInStar(star, string);
        WinSetTextColor(oldColour);
    }
#endif
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsWriteInStar
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsWriteInStar(UInt8 star, char* string)
{
    FontID           oldFont;
    IndexedColorType oldBackColour;
    IndexedColorType oldForeColour;

    oldFont= FntSetFont(stdFont);

    oldBackColour = WinSetBackColor(playerColoursB[gd.occupier[star]]);
    oldForeColour = WinSetForeColor(starTextColour);

    WinDrawChars(string,
                 1,
                 md.mapXOff + (sx[star] * md.mapXMult) - 2,
                 md.mapYOff + (sy[star] * md.mapYMult) - (md.starRadius/2) + 1);

    FntSetFont(oldFont);
    WinSetBackColor(oldBackColour);
    WinSetForeColor(oldForeColour);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsDrawMap
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsDrawMap(void)
{
    PWMAPHEADER      *mapHeader;
    PWSYSTEMDATA     *mapData;
    MemHandle         resourceHandle;
    IndexedColorType  oldColour;
    RectangleType     rect;
    UInt8             star;
    UInt8             connection;
    UInt8             x;
    UInt8             y;
    UInt8             i;

    // Locate map data
    resourceHandle = DmGetResource('maps', MapData);
    mapHeader = (PWMAPHEADER*)MemHandleLock(resourceHandle);

    // Locate specified map
    mapData = (PWSYSTEMDATA*)( (char*)mapHeader + sizeof(PWMAPHEADER) );
    for (i=0; i<gd.currentSystem; i++)
    {
        mapData = (PWSYSTEMDATA*) ( (char*)mapData +
                                    sizeof(PWSYSTEMDATA) +
                                    ((mapData->numStars - 1) * sizeof(PWSTARPOS)) );
    }

    // Copy data to local array
    gd.numStars = mapData->numStars;
    for (i=0; i<gd.numStars; i++)
    {
        sx[i] = mapData->s[i].x;
        sy[i] = mapData->s[i].y;
        StrCopy(cs[i], mapData->s[i].linkedStars);
    }

    // Release data
    MemHandleUnlock(resourceHandle);
    DmReleaseResource(resourceHandle);

    // Select black pen, remembering current one
    oldColour = WinSetForeColor(backgroundColour);    

    // Blank screen
    rect.topLeft.x = md.backXOff;
    rect.topLeft.y = md.backYOff;
    rect.extent.x = md.mapXSize;
    rect.extent.y = md.mapYSize;
    WinDrawRectangle(&rect, 0);

    for (star = 0; star < md.numStars; star++)
    {
        x = RandomNum(md.starXSize) + 1 + md.mapXOff;
        y = RandomNum(md.starYSize) + 1 + md.mapYOff;
        WinSetForeColor(littleStarColour);
        WinDrawPixel(x, y);
        WinSetForeColor(littleStarColourB);
        WinDrawPixel(x - 1, y);
        WinDrawPixel(x + 1, y);
        WinDrawPixel(x, y - 1);
        WinDrawPixel(x, y + 1);
    }

    WinSetForeColor(joiningLinesColour);

    for (star = 0; star < gd.numStars; star++)
    {
        connection = 0;

        while (cs[star][connection] != 0)
        {
            PalmwarsDrawLine(star, (UInt8)(cs[star][connection++] - 'A'));
        }
    }

    for (star = 0; star<gd.numStars; star++)
        PalmwarsDrawStar(star);

    // Set pen back to what it was before we started
    WinSetForeColor(oldColour);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsStartStarRequest
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsStartStarRequest(void)
{
    char string[256];

    PalmwarsClearTextArea();

    StrPrintF(string," %s ", gd.playerNames[gd.currentPlayer]);

    PalmwarsCentreText(string, //gd.playerNames[gd.currentPlayer],
                       md.textYOff,
                       playerColoursF[gd.currentPlayer],
                       playerColoursB[gd.currentPlayer],
                       stdFont);    
    
    StrPrintF(string, "which start star do you want?");
    PalmwarsCentreText(string,
                       md.textYOff + 12,
                       bottomTextColour,
                       bottomBackgroundColour,
                       stdFont);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsReturnStarTap
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
UInt8 PalmwarsReturnStarTap(UInt8 x, UInt8 y)
{
    RectangleType rect;
    UInt8         tappedStar = 0;
    UInt8         star;

    for (star=0; star<gd.numStars; star++)
    {
        rect.topLeft.x = md.mapXOff + (sx[star] * md.mapXMult) - (md.starRadius / 2);
        rect.topLeft.y = md.mapYOff + (sy[star] * md.mapYMult) - (md.starRadius / 2);
        rect.extent.x = md.starRadius;
        rect.extent.y = md.starRadius;

        //WinDrawGrayRectangleFrame(simpleFrame, &rect);

        if (RctPtInRectangle(x, y, &rect))
        {
            tappedStar = star+1;
            break;
        }
    }

    return tappedStar;
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsMarkStartStar
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsMarkStartStar(UInt8 star)
{
    char string[4];

    gd.occupier[star] = gd.currentPlayer;
    gd.armies[star] = 5;

    StrPrintF(string, "%d", gd.armies[star]);
    PalmwarsDrawStar(star);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsMabsInit
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsMabsInit(void)
{
    UInt8 mabsStar;

    do
    {
        mabsStar = RandomNum(gd.numStars);
    }
    while (gd.occupier[mabsStar] != PLAYER_NONE);

    gd.occupier[mabsStar] = PLAYER_MABS;
    gd.armies[mabsStar] = 9;
    PalmwarsDrawStar(mabsStar);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsStartTurn
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsStartTurn(void)
{
    UInt8 player;
    UInt8 i;

    player = RandomNum(gd.numPlayers) + 1;
    
    for (i = 0; i<gd.numPlayers; i++)
    {
        gd.turnOrder[i] = player++;
        if (player > gd.numPlayers) player = 1;
    }

    PalmwarsMabsTurn();

    gd.currentPlayer = 0;
    gd.playerMove = 1;
    gd.subMove = 1;

    PalmwarsTurnPrompt();
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsTurnPrompt
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsTurnPrompt(void)
{
    char string[256];

    PalmwarsClearTextArea();

    StrPrintF(string, "Turn %d  ", gd.turn);
    PalmwarsCentreText(string,
                       2,
                       bottomTextColour,
                       bottomBackgroundColour,
                       boldFont);

    StrPrintF(string, "Your move");
    PalmwarsCentreText(string,
                       md.textYOff,
                       bottomTextColour,
                       bottomBackgroundColour,
                       stdFont);

    StrPrintF(string, " %s ", gd.playerNames[gd.turnOrder[gd.currentPlayer]]);
    PalmwarsCentreText(string,
                       md.textYOff + 12,
                       playerColoursF[gd.turnOrder[gd.currentPlayer]],
                       playerColoursB[gd.turnOrder[gd.currentPlayer]],
                       stdFont);

    if (gd.subMove == 1)
        StrPrintF(string, "Select star to move from...");
    else
        StrPrintF(string, "Select star to move to...");

    PalmwarsCentreText(string,
                       md.textYOff + 24,
                       bottomTextColour,
                       bottomBackgroundColour,
                       stdFont);
}


/************************************************************************
 * 
 * FUNCTION:    PalmwarsGameOverPrompt
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsGameOverPrompt(void)
{
    char string[256];
    
    PalmwarsClearTextArea();
    StrPrintF(string, "Game over - %s wins!", gd.playerNames[gd.winningPlayer]);
    PalmwarsWriteStatusText(string, 4, md.textYOff);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsProcessTurn
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsProcessTurn(UInt8 tappedStar)
{
    static  UInt8 fromStar = 0;
    static  UInt8 toStar = 0;
    static  UInt8 numArmies = 1;
    Boolean skipNextStage = false;

    // This function is called when a tap event has happened.    
    // If we're getting the from star, then assuming it is one of
    // the current player's stars, record it as the from star.     
    if (gd.subMove == 1)
    {
        if (gd.occupier[tappedStar] != gd.turnOrder[gd.currentPlayer])
            return;
        else
        {
            fromStar = tappedStar;
            gd.selectedStar = tappedStar;
            PalmwarsDrawStar(fromStar);
            gd.selectedStar = 255;
        }
    }
   
    // Otherwise, we've just been given a 'to' star. Assuming this
    // is okay, we pop up the dialog box asking how many armies to
    // move.                                                      
    else if (gd.subMove == 2)
    {
        // Check if tapped star is linked to from star
        if (!StrChr(cs[fromStar], (char)(tappedStar + 'A')))
            return;

        numArmies = PalmwarsShowArmiesForm(fromStar);

        if (numArmies == 255)
        {
            PalmwarsDrawStar(fromStar);
            gd.subMove = 1;
            skipNextStage = true;
        }
        else if (numArmies > 0)
        {
            // Is the star we're moving to occupied?
            if ((gd.occupier[tappedStar] != PLAYER_NONE) &&
                (gd.occupier[tappedStar] != gd.turnOrder[gd.currentPlayer]))
            {
                PalmwarsCombat(fromStar, tappedStar, numArmies);
            }
            else
            {
                // Not occupied - move in
                gd.occupier[tappedStar] = gd.turnOrder[gd.currentPlayer];
                gd.armies[tappedStar] += numArmies;
                gd.armies[fromStar] -= numArmies;
                if (gd.armies[fromStar] == 0) gd.occupier[fromStar] = PLAYER_NONE;
                if (gd.armies[tappedStar] > 9) gd.armies[tappedStar] = 9;
                PalmwarsDrawStar(fromStar);
                PalmwarsDrawStar(tappedStar);
            }
        }
        else
        {
            PalmwarsDrawStar(fromStar);
        }
    }

    if (!skipNextStage)
    {
        // Move to next stage of the game
        PalmwarsNextStage();

        // Is the game over yet?
        if (gd.gameState == GAMESTATE_FINISHED) return;

        // Reinforcements yet?
        if ((gd.subMove == 1) && (gd.playerMove == 1) && (gd.turn > 1))
        {
            PalmwarsReinforce(gd.turnOrder[gd.currentPlayer]);
        }
    }

    // Prompt for next turn
    PalmwarsTurnPrompt();
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsNextStage
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsNextStage(void)
{
    PalmwarsCheckForWinner();

    if (gd.gameState == GAMESTATE_FINISHED) return;

    gd.subMove++;

    if (gd.subMove > 2)
    {
        gd.subMove = 1;
        gd.playerMove++;

        if (gd.playerMove > 2)
        {
            gd.playerMove = 1;
            gd.currentPlayer++;
        
            if (gd.currentPlayer == gd.numPlayers)
            {
                gd.turn++;
                PalmwarsStartTurn();
            }
        }
    }

    if (gd.starsOccupied[gd.turnOrder[gd.currentPlayer]] == 0) PalmwarsNextStage();
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsClearTextArea
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsClearTextArea(void)
{
    RectangleType    rect;
    IndexedColorType oldColour;

    // Select black pen, remembering current one
    oldColour = WinSetForeColor(bottomBackgroundColour);    

    rect.topLeft.x = 0;
    rect.topLeft.y = md.textYOff;
    rect.extent.x = 160;
    rect.extent.y = 160-md.textYOff;
    WinDrawRectangle(&rect, 0);

    // Set pen back to what it was before we started
    WinSetForeColor(oldColour);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsMabsTurn
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsMabsTurn(void)
{
    char string[256];

    PalmwarsClearTextArea();

    StrPrintF(string, "Turn %d  ", gd.turn);
    PalmwarsWriteStatusText(string, 4, md.textYOff);

    StrPrintF(string, "Palm turn");
    PalmwarsWriteStatusText(string, 4, md.textYOff + 12);

    if (gd.turn > 2) PalmwarsReinforce(PLAYER_MABS);
    PalmwarsMabAttack();
    PalmwarsMabAttack();
    PalmwarsCheckForWinner();
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsMabAttack
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsMabAttack(void)
{
    UInt8 m;
    UInt8 n;
    UInt8 o;
    UInt8 frn;
    UInt8 ds;
    UInt8 dif;
    UInt8 target;
    UInt8 g;
    UInt8 en;
    char  ast0[16];
    char  ast1[16];
    
    StrPrintF(ast0, "!");
    StrPrintF(ast1, "!");

    // Analyse all systems
    for (m=0; m<gd.numStars; m++)
    {
        if (gd.occupier[m] == PLAYER_MABS)
        {
            // Analyse all targets from system
            for (n=0; n<StrLen(cs[m]); n++)
            {
                target = cs[m][n] - 'A';
                dif = gd.armies[m] - gd.armies[target] + 75;
                ds = 75 - gd.armies[target];
                frn = 65;
                if (gd.occupier[target] != PLAYER_MABS)
                {
                    // Determine strength of Mabs systems next to target
                    for (o=0; o<StrLen(cs[target]); o++)
                    {
                        g = cs[target][o] - 'A';
                        if (gd.occupier[g] == PLAYER_MABS) frn = frn + gd.armies[g];
                    }
                    // Determine strength of unfriendly systems next to m
                    en = 75;
                    for (o=0; o<StrLen(cs[m]); o++)
                    {
                        g = cs[m][o] - 'A';
                        if (gd.occupier[g] == PLAYER_MABS) en = en - gd.armies[g];
                    }

                    StrPrintF(ast1, "%c%c%c%cXX%c%c", dif, ds, frn, en, m+65, target+65);

                    if (StrCompare(ast0, ast1) < 0) StrCopy(ast0, ast1);
                }
            }
        }
    }

    if (StrCompare(ast0, "!") != 0)
    {
        m = ast0[6] - 'A';
        target = ast0[7] - 'A';
        PalmwarsMabCombat(m, target);
    }
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsMabCombat
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsMabCombat(int f, int t)
{
    UInt8 as;
    char  string[256];

    StrPrintF(string, "From %d to %d", f, t);
    PalmwarsWriteStatusText(string, 4, md.textYOff + 24);

    // Calculate Mab attack strength
    if ((gd.occupier[t] == PLAYER_MABS) || (gd.occupier[t] == PLAYER_NONE))
    {
        as = (gd.armies[f] - gd.armies[t]) / 2;
        //if ((as == 0) && (gd.armies[f] > 1)) as = 1;
        gd.armies[f] -= as;
        gd.armies[t] += as;
        if (gd.armies[t] > 0) gd.occupier[t] = PLAYER_MABS;
    }
    else
    {
        as = gd.armies[f] - 1;
        PalmwarsCombat(f, t, as);
    }

    if (gd.armies[f] == 0) gd.occupier[f] = PLAYER_NONE;
    PalmwarsDrawStar(f);
    PalmwarsDrawStar(t);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsShowArmiesForm
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
UInt8 PalmwarsShowArmiesForm(UInt8 fromStar)
{
    FormType    *form;
    ControlType *ctl;
    UInt16       objectID;
    UInt16       resourceID;
    UInt8        i;
    UInt8        numArmies;
    
    // Initialise the armies form
    form = FrmInitForm(PalmwarsNumArmiesForm);

    // Set 1 as the default number to move
    ctl = FrmGetObjectPtr(form, FrmGetObjectIndex(form, Armies1));
    CtlSetValue(ctl, 1);

    // Hide any invalid options
    for (i = gd.armies[fromStar]; i<9; i++)
        FrmHideObject(form, FrmGetObjectIndex(form, Armies1 + i));

    // Show form
    resourceID = FrmDoDialog(form);

    // Find number of items selected
    objectID = FrmGetObjectId(form, FrmGetControlGroupSelection(form, 2));
    numArmies = objectID - Armies0;

    if (resourceID == CancelButton)
        numArmies = 255;
    else if (resourceID == PassButton)
        numArmies = 0;

    // Delete instance from memory
    FrmDeleteForm(form);

    return numArmies;
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsReinforce
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsReinforce(UInt8 player)
{
    UInt8 star;
    UInt8 neighbour;
    UInt8 numNeighbours;
    UInt8 neighbourStar;

    // Go through all stars checking for ones owned by this player
    for (star=0; star<gd.numStars; star++)
    {
        if (gd.occupier[star] == player)
        {
            // Now count occupied neighbours
            numNeighbours = 0;
            for (neighbour=0; neighbour < StrLen(cs[star]); neighbour++)
            {
                neighbourStar = cs[star][neighbour] - 'A';
                if (gd.occupier[neighbourStar] == player) numNeighbours++;
            }

            // If neighbours greater than 1, we gain an army
            if (numNeighbours > 1)
            {
                gd.armies[star]++;
                if (gd.armies[star] > 9) gd.armies[star] = 9;
                PalmwarsDrawStar(star);
            }
        }
    }
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsCombat
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsCombat(UInt8 from, UInt8 to, UInt8 numArmies)
{
    double n = 0;
    double s_d = 0;
    double a_d = 0;
    double m_d = 0;
    double r_d = 0;
    double s1;
    double d1;
    UInt8  l;
    UInt8  s = numArmies;
    UInt16 m;
    UInt8  i;
    UInt16 r;

    if (numArmies == 0) return;

    for (i=0; i<s; i++) s_d++;              // Make a double of s
    for (i=0; i<gd.armies[to]; i++) a_d++;     // Make a double of armies[to]

    n = (s_d * s_d) / (a_d * a_d);
    m = NonZeroRandomNum((UInt16)9);        // 9 for 'true to original'

    for (i=0; i<m; i++) m_d++;              // Make a double of m

    n = n * (0.75 + m_d / 20.00);

    if (n >= 1)
    {
        s1 = squareroot(s * gd.armies[to]);
        d1 = a_d;
    }
    else
    {
        d1 = (double)squareroot((UInt8)(gd.armies[to] * s));
        s1 = s_d;
    }

    do
    {
        r = NonZeroRandomNum((UInt16)(d1 + s1));
        r_d = 0;
        for (i=0; i<r; i++) r_d++;

        if ( r_d > d1 )
        {
            l = 0;
            s1 = s1 - 1;
        }
        else
        {
            l = 1;
            d1 = d1 - 1;
        }


        if (l == 0)
        {
            gd.armies[to]--;
            a_d--;
            // Attacker won
        }
        else
        {
            s--;
            s_d--;
            gd.armies[from]--;
            // Defender won
        }

        PalmwarsDrawStar(from);
        PalmwarsDrawStar(to);
    }
    while((s > 0) && (gd.armies[to] > 0));

    if (gd.armies[to] == 0)
    {
        // Atacker won
        gd.armies[to] = s;
        gd.armies[from] -= s;
        gd.occupier[to] = gd.occupier[from];
    }
    else if (s == 0)
    {
        // Defender won
    }

    if (gd.armies[from] == 0)
        gd.occupier[from] = PLAYER_NONE;

    PalmwarsDrawStar(from);
    PalmwarsDrawStar(to);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsCheckForWinner
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsCheckForWinner(void)
{
    int  i;
    int  stillPlaying = 0;

    // Initialisation
    for (i=0; i<=MAX_PLAYERS; i++) gd.starsOccupied[i] = 0;

    // Work out how many stars each player owns
    for (i=0; i<gd.numStars; i++)
    {
        if (gd.occupier[i] <= MAX_PLAYERS)
            gd.starsOccupied[gd.occupier[i]]++;
    }

    // Count number of players still in the game
    for (i = 0; i<=gd.numPlayers; i++)
    {
        if (gd.starsOccupied[i] > 0)
        {
            stillPlaying++;
            gd.winningPlayer = i;
        }
    }

    // If only one left, declare a winner
    if (stillPlaying == 1)
    {
        gd.gameState = GAMESTATE_FINISHED;
        PalmwarsGameOverPrompt();
    }
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsSaveState
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsSaveState(void)
{
    PrefSetAppPreferences(RML_CREATOR_ID, 1, 1, &gd, sizeof(PWGAMESTATE), false);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsLoadState
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsLoadState(void)
{
    UInt16 prefsize = sizeof(PWGAMESTATE);

    if (PrefGetAppPreferences(RML_CREATOR_ID, 1, &gd, &prefsize, false) != 1)
    	PalmwarsInitialiseAppPrefs(&gd);

    if (gd.gameOn)
        PalmwarsInitialise();
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsInitialiseAppPrefs
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsInitialiseAppPrefs(PWGAMESTATE* state)
{
    int i;

	MemSet(state, sizeof(PWGAMESTATE), 0);
	state->selectedStar = 255;
	state->winningPlayer = 0;
	state->gameOn = false;
	state->numPlayers = 4;
	state->currentSystem = 0;
    StrPrintF(state->playerNames[0], "Palm");
	state->withMabs = true;

    for (i=1; i<=MAX_PLAYERS; i++)
        StrPrintF(state->playerNames[i], "Player%d", i);

}


/************************************************************************
 *
 * FUNCTION:    PalmwarsResetState
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsResetState(void)
{
    PWGAMESTATE state;

    PalmwarsInitialiseAppPrefs(&state);
    PrefSetAppPreferences(RML_CREATOR_ID, 1, 1, &state, sizeof(PWGAMESTATE), false);
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsSetupGameDrawParameters
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsSetupGameDrawParameters(void)
{
    md.mapXMult = 4;
    md.mapYMult = 4;
    md.mapXSize = 160;
    md.mapYSize = 160;
    md.mapXOff = 4;
    md.mapYOff = 20;
    md.backXOff = 0;
    md.backYOff = 0;
    md.textYOff = 110;
    md.starXSize = md.mapXSize;
    md.starYSize = md.textYOff - 2;
    md.starRadius = 13;
    md.numStars = 40;
}


/************************************************************************
 *
 * FUNCTION:    PalmwarsSetupPreviewDrawParameters
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURNED:
 *
 ************************************************************************/
void PalmwarsSetupPreviewDrawParameters(void)
{
    md.mapXMult = 2;
    md.mapYMult = 2;
    md.mapXSize = 88;
    md.mapYSize = 46;
    md.mapXOff = 40;
    md.mapYOff = 68; //84;
    md.backXOff = md.mapXOff - 6;
    md.backYOff = md.mapYOff - 4;
    md.textYOff = 60;
    md.starXSize = md.mapXSize - 8;
    md.starYSize = md.mapYSize - 8;
    md.starRadius = 6;
    md.numStars = 20;
}
