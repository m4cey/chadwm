/* See LICENSE file for copyright and license details. */

#include <X11/X.h>
#include <X11/XF86keysym.h>

/* layout */
static float mfact                  = 0.50; /* factor of master area size [0.05..0.95] */
static int nmaster                  = 1;    /* number of clients in master area */
static int resizehints              = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen     = 0; /* 1 will force focus on the fullscreen window */

/* appearance */
static unsigned int borderpx        = 2;        /* border pixel of windows */
static const unsigned int default_border = 2;   /* to switch back to default border after dynamic border resizing via keybinds */
static unsigned int snap            = 16;       /* snap pixel */
static const unsigned int gappih    = 16;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 16;       /* vert inner gap between windows */
static const unsigned int gappoh    = 16;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 16;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int unsigned textsystray    = 1;
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails,display systray on the 1st monitor,False: display systray on last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static int showbar                  = 1;        /* 0 means no bar */
static const int showtab            = 0; //showtab_auto;
static int toptab                   = 0;        /* 0 means bottom tab */
static const int floatbar           = 1;/* 1 means the bar will float(don't have padding),0 means the bar have padding */
static int topbar                   = 0;        /* 0 means bottom bar */
static const int horizpadbar        = 0;
static const int vertpadbar         = 0;
static const int vertpadtab         = 0;
static const int horizpadtabi       = 16;
static const int horizpadtabo       = 16;
static const int scalepreview       = 4;
static const int tag_preview        = 0;        /* 1 means enable, 0 is off */
static const int colorfultag        = 1;        /* 0 means use SchemeSel for selected non vacant tag */
static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };
static const char *light_up[] = {"xbacklight", "-inc", "5", NULL};
static const char *light_down[] = {"xbacklight", "-dec", "5", NULL};

#define TERMINAL "st"
static const char *terminal[] = {TERMINAL, NULL};
static const char *browser[] = {"firefox", NULL};
static const char *files[] = {TERMINAL, "-e", "ranger", NULL};
static const char *top[] = {TERMINAL, "-e", "btop", NULL};
static const int new_window_attach_on_end = 1; /*  1 means the new window will attach on the end; 0 means the new window will attach on the front,default is front */
#define ICONSIZE 12   /* icon size */
#define ICONSPACING 4 /* space between icon and title */

static char primary_font[] = "JetBrainsMono Nerd Font Mono:style:medium:size=12";
static const char *fonts[]          = { primary_font, "Iosevka:style:medium:size=12" };

// theme

/*
 * Xresources preferences to load at startup
 */

static char black[]       = "#1e222a";
static char white[]       = "#abb2bf";
static char gray2[]       = "#2e323a"; // unfocused window border
static char gray3[]       = "#545862";
static char gray4[]       = "#6d8dad";
static char blue[]        = "#61afef";  // focused window border
static char green[]       = "#7EC7A2";
static char red[]         = "#e06c75";
static char orange[]      = "#caaa6a";
static char yellow[]      = "#EBCB8B";
static char pink[]        = "#c678dd";
static char col_borderbar[]  = "#1e222a"; // inner border

ResourcePref resources[] = {
	{ "font",               STRING,  &primary_font },
	{ "background",         STRING,  &black },
	{ "background",         STRING,  &col_borderbar },
	{ "foreground",         STRING,  &white },
	{ "color2",             STRING,  &blue },
	{ "color3",             STRING,  &green },
	{ "color4",             STRING,  &red },
	{ "color5",             STRING,  &orange },
	{ "color6",             STRING,  &yellow },
	{ "color7",             STRING,  &pink },
	{ "color8",             STRING,  &gray2 },
	{ "color9",             STRING,  &gray3 },
	{ "color10",            STRING,  &gray4 },
	{ "normbgcolor",        STRING,  &black },
	{ "normbordercolor",    STRING,  &gray2 },
	{ "normfgcolor",        STRING,  &gray3 },
	{ "selbgcolor",         STRING,  &blue },
	{ "selbordercolor",     STRING,  &blue },
	{ "selfgcolor",         STRING,  &gray4 },
	{ "borderpx",          	INTEGER, &borderpx },
	{ "snap",          		INTEGER, &snap },
	{ "showbar",          	INTEGER, &showbar },
	{ "topbar",          	INTEGER, &topbar },
	{ "nmaster",          	INTEGER, &nmaster },
	{ "resizehints",       	INTEGER, &resizehints },
	{ "mfact",      	 	FLOAT,   &mfact },
};

static const char *colors[][3]      = {
    /*                     fg       bg      border */
    [SchemeNorm]       = { gray3,   black,  gray2 },
    [SchemeSel]        = { gray4,   blue,   blue  },
    [SchemeTitle]      = { white,   black,  black  }, // active window title
    [TabSel]           = { blue,    gray2,  black },
    [TabNorm]          = { gray3,   black,  black },
    [SchemeTag]        = { pink,   black,  black },
    [SchemeTag1]       = { blue,    black,  black },
    [SchemeTag2]       = { green,     black,  black },
    [SchemeTag3]       = { red,  black,  black },
    [SchemeTag4]       = { orange,   black,  black },
    [SchemeTag5]       = { yellow,    black,  black },
    [SchemeLayout]     = { blue,   black,  black },
    [SchemeBtnPrev]    = { green,   black,  black },
    [SchemeBtnNext]    = { yellow,  black,  black },
    [SchemeBtnClose]   = { red,     black,  black },
};


/* tagging */
static char *tags[] = {"1", "2", "3", "4", "5"};

static const int tagschemes[] = {
    SchemeTag1, SchemeTag2, SchemeTag3, SchemeTag4, SchemeTag5
};

static const unsigned int ulinepad      = 6; /* horizontal padding between the underline and tag */
static const unsigned int ulinestroke   = 1; /* thickness / height of the underline */
static const unsigned int ulinevoffset  = 0; /* how far above the bottom of the bar the line should appear */
static const int ulineall               = 0; /* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     iscentered   isfloating   monitor */
    { "Gimp",     NULL,       NULL,       1 << 4,       0,           0,           -1 },
    { "firefox",  NULL,       NULL,       1 << 2,       0,           0,           -1 },
    { "discord",  NULL,       NULL,       1 << 0,       0,           0,           -1 },
};

/* layout(s) */
#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "functions.h"


static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[]=",      tile },    /* first entry is default */
    { "[M]",      monocle },
    { "[@]",      spiral },
    { "[\\]",     dwindle },
    { "H[]",      deck },
    { "TTT",      bstack },
    { "===",      bstackhoriz },
    { "HHH",      grid },
    { "###",      nrowgrid },
    { "---",      horizgrid },
    { ":::",      gaplessgrid },
    { "|M|",      centeredmaster },
    { ">M>",      centeredfloatingmaster },
    { "><>",      NULL },    /* no layout function means floating behavior */
    { NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
    { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
    { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */

static const Key keys[] = {
    /* modifier                         key         function        argument */

	 // brightness and audio
    {0,                                 XF86XK_AudioLowerVolume,
		                                              spawn,          {.v = downvol} },
	 {0,                                 XF86XK_AudioMute,
		                                              spawn,          {.v = mutevol } },
	 {0,                                 XF86XK_AudioRaiseVolume,
		                                              spawn,          {.v = upvol} },

	 {MODKEY,                            XK_Page_Up, spawn,          {.v = light_up} },
	 {MODKEY,                            XK_Page_Down,
		                                              spawn,          {.v = light_down} },

    // toggle stuff
    { MODKEY,                           XK_b,       togglebar,      {0} },
    { MODKEY|ControlMask,               XK_t,       togglegaps,     {0} },
    { MODKEY|ShiftMask,                 XK_space,   togglefloating, {0} },
    { MODKEY,                           XK_f,       togglefullscrgeometry,  {0} },
    { MODKEY|ShiftMask,                 XK_f,       togglefullscr,  {0} },

    { MODKEY,                           XK_w,       tabmode,        { -1 } },
    { MODKEY,                           XK_j,       focusstack,     {.i = +1 } },
    { MODKEY,                           XK_k,       focusstack,     {.i = -1 } },
    { MODKEY|ControlMask,               XK_i,       incnmaster,     {.i = +1 } },
    { MODKEY|ControlMask,               XK_d,       incnmaster,     {.i = -1 } },

    // shift view
    { MODKEY,                           XK_h,       shiftview,      {.i = -1 } },
    { MODKEY,                           XK_l,       shiftview,      {.i = +1 } },

    // change m,cfact sizes
    { MODKEY|ShiftMask,                 XK_h,       setmfact,       {.f = -0.05} },
    { MODKEY|ShiftMask,                 XK_l,
                                                    setmfact,       {.f = +0.05} },
    { MODKEY|ControlMask,               XK_h,       setcfact,       {.f = +0.25} },
    { MODKEY|ControlMask,               XK_l,
		                                              setcfact,       {.f = -0.25} },
    { MODKEY|ShiftMask,                 XK_o,       setcfact,       {.f =  0.00} },


    { MODKEY|ShiftMask,                 XK_j,       movestack,      {.i = +1 } },
    { MODKEY|ShiftMask,                 XK_k,       movestack,      {.i = -1 } },
    { MODKEY|ControlMask,               XK_Return,  zoom,           {0} },
    { MODKEY,                           XK_Tab,     view,           {0} },

    // overall gaps
    // { MODKEY|ControlMask,               XK_i,       incrgaps,       {.i = +1 } },
    // { MODKEY|ControlMask,               XK_d,       incrgaps,       {.i = -1 } },

    // inner gaps
    // { MODKEY|ShiftMask,                 XK_i,       incrigaps,      {.i = +1 } },
    // { MODKEY|ControlMask|ShiftMask,     XK_i,       incrigaps,      {.i = -1 } },

    // outer gaps
    // { MODKEY|ControlMask,               XK_o,       incrogaps,      {.i = +1 } },
    // { MODKEY|ControlMask|ShiftMask,     XK_o,       incrogaps,      {.i = -1 } },

    // inner+outer hori, vert gaps
    // { MODKEY|ControlMask,               XK_6,       incrihgaps,     {.i = +1 } },
    // { MODKEY|ControlMask|ShiftMask,     XK_6,       incrihgaps,     {.i = -1 } },
    // { MODKEY|ControlMask,               XK_7,       incrivgaps,     {.i = +1 } },
    // { MODKEY|ControlMask|ShiftMask,     XK_7,       incrivgaps,     {.i = -1 } },
    // { MODKEY|ControlMask,               XK_8,       incrohgaps,     {.i = +1 } },
    // { MODKEY|ControlMask|ShiftMask,     XK_8,       incrohgaps,     {.i = -1 } },
    // { MODKEY|ControlMask,               XK_9,       incrovgaps,     {.i = +1 } },
    // { MODKEY|ControlMask|ShiftMask,     XK_9,       incrovgaps,     {.i = -1 } },

    // { MODKEY|ControlMask|ShiftMask,     XK_d,       defaultgaps,    {0} },

    // layout
    { MODKEY,                           XK_t,       setlayout,      {.v = &layouts[0]} },      //[]= tile
    { MODKEY|ShiftMask,                 XK_u,       setlayout,      {.v = &layouts[1]} },      //[M] monocle
    { MODKEY|ShiftMask,                 XK_m,       setlayout,      {.v = &layouts[2]} },      //[@] spiral
    { MODKEY,                           XK_u,       setlayout,      {.v = &layouts[4]} },     //::: gaplessgrid
    { MODKEY|ShiftMask,                 XK_g,       setlayout,      {.v = &layouts[10]} },     //::: gaplessgrid
    { MODKEY,                           XK_space,   setlayout,      {0} },                     //last layout
    { MODKEY|ControlMask,               XK_comma,   cyclelayout,    {.i = -1 } },
    { MODKEY|ControlMask,               XK_period,  cyclelayout,    {.i = +1 } },
    { MODKEY,                           XK_0,       view,           {.ui = ~0 } },
    { MODKEY|ShiftMask,                 XK_0,       tag,            {.ui = ~0 } },
    { MODKEY,                           XK_comma,   focusmon,       {.i = -1 } },
    { MODKEY,                           XK_period,  focusmon,       {.i = +1 } },
    { MODKEY|ShiftMask,                 XK_comma,   tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,                 XK_period,  tagmon,         {.i = +1 } },

	 // system actions

    {MODKEY,                            XK_BackSpace,
		                                              spawn,          SHCMD("sysact")},
	 // dmenu actions
	 // launchers
    {MODKEY,                            XK_d,       spawn,          SHCMD("dmenu_run -l 10 -g 5 -i")},
    {MODKEY|ShiftMask,                  XK_d,       spawn,          SHCMD("j4-dmenu-desktop --dmenu=\"dmenu -l 10 -g 5 -i\"")},
    {MODKEY,                            XK_c,       spawn,          SHCMD("netm_dmenu")},

    // screenshot and record
    {0,                                 XK_Print,   spawn,          SHCMD("maim pic-full-$(date '+%y%m%d-%H%M-%S').png")},
    {ShiftMask,                         XK_Print,   spawn,          SHCMD("maimpick")},
    {MODKEY,                            XK_Print,   spawn,          SHCMD("dmenurecord")},
    {MODKEY|ShiftMask,                  XK_Print,   spawn,          SHCMD("dmenurecord kill")},
    {MODKEY,                            XK_Delete,  spawn,          SHCMD("dmenurecord kill")},
    {MODKEY,                            XK_Scroll_Lock,
		                                              spawn,          SHCMD("killall screenkey || screenkey &")},
	 // mount and unmount
    {MODKEY,                            XK_F9,      spawn,          SHCMD("dmenumount")},
    {MODKEY,                            XK_F10,     spawn,          SHCMD("dmenuumount -l 5")},
    {MODKEY,                            XK_F11,     spawn,          SHCMD("mpv --no-cache --no-osc --no-input-default-bindings --vf=hflip "
                                                                          "--input-conf=/dev/null --title=webcam $(ls /dev/video[0,2,4,6,8] | "
                                                                          "tail -n 1)")},

	 // dunstctl
    {ControlMask,                       XK_space,   spawn,          SHCMD("dunstctl close")},
    {ControlMask|ShiftMask,             XK_space,   spawn,          SHCMD("dunstctl set-paused toggle")},
    {ControlMask,                       XK_grave,   spawn,          SHCMD("dunstctl history-pop")},
    {ControlMask|ShiftMask,             XK_grave,   spawn,          SHCMD("dunstctl context")},

	 // program shortcuts
    { MODKEY,                           XK_Return,  spawn,          {.v = terminal}},
    { MODKEY|ShiftMask,                 XK_w,       spawn,          {.v = browser}},
    { MODKEY,                           XK_r,       spawn,          {.v = files}},
    { MODKEY,                           XK_apostrophe,
		                                              spawn,          {.v = top}},

	 // playerctl
    { MODKEY,                           XK_p,       spawn,          SHCMD("playerctl play-pause")},
    { MODKEY,                           XK_comma,   spawn,          SHCMD("playerctl previous")},
    { MODKEY,                           XK_period,  spawn,          SHCMD("playerctl next")},
    { MODKEY | ShiftMask,               XK_p,       spawn,          SHCMD("playerctld shift")},
    { MODKEY,                           XK_bracketleft,
		                                              spawn,          SHCMD("playerctl position 10-")},
    { MODKEY | ShiftMask,               XK_bracketleft,
		                                              spawn,          SHCMD("playerctl position 60-")},
    { MODKEY,                           XK_bracketright,
		                                              spawn,          SHCMD("playerctl position 10-")},
    { MODKEY | ShiftMask,               XK_bracketright,
		                                              spawn,          SHCMD("playerctl position 60+")},
    { MODKEY | ShiftMask,               XK_comma,   spawn,          SHCMD("playerctl position 0")},

    // kill window
    { MODKEY,                           XK_q,       killclient,     {0} },

	 // restart dwm
	 {MODKEY|ShiftMask,                  XK_q,       restart,        {0} },

    // hide & restore windows
    { MODKEY,                           XK_e,       hidewin,        {0} },
    { MODKEY|ShiftMask,                 XK_e,       restorewin,     {0} },

    TAGKEYS(                            XK_1,                       0)
    TAGKEYS(                            XK_2,                       1)
    TAGKEYS(                            XK_3,                       2)
    TAGKEYS(                            XK_4,                       3)
    TAGKEYS(                            XK_5,                       4)
    TAGKEYS(                            XK_6,                       5)
    TAGKEYS(                            XK_7,                       6)
    TAGKEYS(                            XK_8,                       7)
    TAGKEYS(                            XK_9,                       8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
    { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
    { ClkWinTitle,          0,              Button2,        zoom,           {0} },
    { ClkStatusText,        0,              Button2,        spawn,          SHCMD("st") },

    /* Keep movemouse? */
    /* { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} }, */

    /* placemouse options, choose which feels more natural:
    *    0 - tiled position is relative to mouse cursor
    *    1 - tiled position is relative to window center
    *    2 - mouse pointer warps to window center
    *
    * The moveorplace uses movemouse or placemouse depending on the floating state
    * of the selected client. Set up individual keybindings for the two if you want
    * to control these separately (i.e. to retain the feature to move a tiled window
    * into a floating position).
    */
    { ClkClientWin,         MODKEY,         Button1,        moveorplace,    {.i = 0} },
    { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    { ClkClientWin,         ControlMask,    Button1,        dragmfact,      {0} },
    { ClkClientWin,         ControlMask,    Button3,        dragcfact,      {0} },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
    { ClkTabBar,            0,              Button1,        focuswin,       {0} },
    { ClkTabBar,            0,              Button1,        focuswin,       {0} },
    { ClkTabPrev,           0,              Button1,        movestack,      { .i = -1 } },
    { ClkTabNext,           0,              Button1,        movestack,      { .i = +1 } },
    { ClkTabClose,          0,              Button1,        killclient,     {0} },
};
