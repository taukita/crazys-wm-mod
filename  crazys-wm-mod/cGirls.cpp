/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "cTariff.h"
#include "cGirls.h"
#include "cEvents.h"
#include "math.h"
#include <fstream>
#include "cBrothel.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "tinyxml.h"
#include "XmlMisc.h"
#include "cGangs.h"
#include "CGraphics.h"
#include <algorithm>
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cClinic.h"
#include "cHouse.h"
#include "cBrothel.h"
#include "cFarm.h"

#ifdef LINUX
#include "linux.h"
#endif
#include "DirPath.h"
#include "FileList.h"
#include "libintl.h"

using namespace std;

extern cMessageQue g_MessageQue;
extern cTraits g_Traits;
extern cEvents g_Events;
extern cInventory g_InvManager;
extern sGirl* MarketSlaveGirls[8];
extern char buffer[1000];
extern CGraphics g_Graphics;
extern cRng g_Dice;
extern bool g_GenGirls;
extern cGold g_Gold;
extern cGangManager g_Gangs;
extern int g_Building;
extern cGirls g_Girls;
extern sGirl *selected_girl;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager  g_Studios;
extern cArenaManager g_Arena;
extern cClinicManager g_Clinic;
extern cCentreManager g_Centre;
extern cHouseManager g_House;
extern cFarmManager g_Farm;
extern cFont m_Font;

/*
 * MOD: DocClox: Stuff for the XML loader code
 *
 * first: static members need declaring
 */
bool sGirl::m_maps_setup = false;
map<string,unsigned int> sGirl::stat_lookup;
map<string,unsigned int> sGirl::skill_lookup;
map<string,unsigned int> sGirl::status_lookup;

const char *sGirl::stat_names[] =
{
	"Charisma", "Happiness", "Libido", "Constitution", "Intelligence", "Confidence", "Mana", "Agility",
	"Fame", "Level", "AskPrice", "House", "Exp", "Age", "Obedience", "Spirit", "Beauty",
	"Tiredness", "Health", "PCFear", "PCLove", "PCHate", "Morality", "Refinement", "Dignity", "Lactation"
};
// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > *_names[]
const char *sGirl::skill_names[] =
{
	"Anal", "Magic", "BDSM", "NormalSex", "Beastiality", "Group", "Lesbian", "Service", "Strip", "Combat", "OralSex",
	"TittySex", "Medicine", "Performance", "Handjob", "Crafting", "Herbalism", "Farming", "Brewing", "AnimalHandling"
};
const char *sGirl::status_names[] =
{
	"None", "Poisoned", "Badly Poisoned", "Pregnant", "Pregnant By Player", "Slave", "Has Daughter", "Has Son",
	"Inseminated", "Controlled", "Catacombs", "Arena", "Your Daughter"
};
// calculate the max like this, and it's self-maintaining
const unsigned int sGirl::max_stats		= (sizeof(sGirl::stat_names)	/ sizeof(sGirl::stat_names[0]));
const unsigned int sGirl::max_skills	= (sizeof(sGirl::skill_names)	/ sizeof(sGirl::skill_names[0]));
const unsigned int sGirl::max_statuses	= (sizeof(sGirl::status_names)	/ sizeof(sGirl::status_names[0]));

// ----- Lookups
void sGirl::setup_maps()
{
		g_LogFile.os() << "[sGirl::setup_maps] Setting up Stats, Skills and Status codes."<< endl;
		m_maps_setup = true;
		stat_lookup["Charisma"]				= STAT_CHARISMA;
		stat_lookup["Happiness"]			= STAT_HAPPINESS;
		stat_lookup["Libido"]				= STAT_LIBIDO;
		stat_lookup["Constitution"]			= STAT_CONSTITUTION;
		stat_lookup["Intelligence"]			= STAT_INTELLIGENCE;
		stat_lookup["Confidence"]			= STAT_CONFIDENCE;
		stat_lookup["Mana"]					= STAT_MANA;
		stat_lookup["Agility"]				= STAT_AGILITY;
		stat_lookup["Fame"]					= STAT_FAME;
		stat_lookup["Level"]				= STAT_LEVEL;
		stat_lookup["AskPrice"]				= STAT_ASKPRICE;
		stat_lookup["House"]				= STAT_HOUSE;
		stat_lookup["Exp"]					= STAT_EXP;
		stat_lookup["Age"]					= STAT_AGE;
		stat_lookup["Obedience"]			= STAT_OBEDIENCE;
		stat_lookup["Spirit"]				= STAT_SPIRIT;
		stat_lookup["Beauty"]				= STAT_BEAUTY;
		stat_lookup["Tiredness"]			= STAT_TIREDNESS;
		stat_lookup["Health"]				= STAT_HEALTH;
		stat_lookup["PCFear"]				= STAT_PCFEAR;
		stat_lookup["PCLove"]				= STAT_PCLOVE;
		stat_lookup["PCHate"]				= STAT_PCHATE;
		stat_lookup["Morality"]				= STAT_MORALITY;
		stat_lookup["Refinement"]			= STAT_REFINMENT;
		stat_lookup["Dignity"]				= STAT_DIGNITY;
		stat_lookup["Lactation"]			= STAT_LACTATION;

// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > setup_maps

		skill_lookup["Anal"]				= SKILL_ANAL;
		skill_lookup["Magic"]				= SKILL_MAGIC;
		skill_lookup["BDSM"]				= SKILL_BDSM;
		skill_lookup["NormalSex"]			= SKILL_NORMALSEX;
		skill_lookup["OralSex"]				= SKILL_ORALSEX;
		skill_lookup["TittySex"]			= SKILL_TITTYSEX;
		skill_lookup["Handjob"]				= SKILL_HANDJOB;
		skill_lookup["Beastiality"]			= SKILL_BEASTIALITY;
		skill_lookup["Group"]				= SKILL_GROUP;
		skill_lookup["Lesbian"]				= SKILL_LESBIAN;
		skill_lookup["Service"]				= SKILL_SERVICE;
		skill_lookup["Strip"]				= SKILL_STRIP;
		skill_lookup["Combat"]				= SKILL_COMBAT;
		skill_lookup["Medicine"]			= SKILL_MEDICINE;
		skill_lookup["Performance"]			= SKILL_PERFORMANCE;
		skill_lookup["Crafting"]			= SKILL_CRAFTING;
		skill_lookup["Herbalism"]			= SKILL_HERBALISM;
		skill_lookup["Farming"]				= SKILL_FARMING;
		skill_lookup["Brewing"]				= SKILL_BREWING;
		skill_lookup["AnimalHandling"]		= SKILL_ANIMALHANDLING;

		status_lookup["None"]				= STATUS_NONE; 
		status_lookup["Poisoned"]			= STATUS_POISONED;
		status_lookup["Badly Poisoned"]		= STATUS_BADLY_POISONED;
		status_lookup["Pregnant"]			= STATUS_PREGNANT;
		status_lookup["Pregnant By Player"]	= STATUS_PREGNANT_BY_PLAYER;
		status_lookup["Slave"]				= STATUS_SLAVE;
		status_lookup["Has Daughter"]		= STATUS_HAS_DAUGHTER;
		status_lookup["Has Son"]			= STATUS_HAS_SON;
		status_lookup["Inseminated"]		= STATUS_INSEMINATED;
		status_lookup["Controlled"]			= STATUS_CONTROLLED;
		status_lookup["Catacombs"]			= STATUS_CATACOMBS;
		status_lookup["Arena"]			    = STATUS_ARENA;
		status_lookup["Your Daughter"]		= STATUS_YOURDAUGHTER;
}

int sGirl::lookup_skill_code(string s)
{
	// be useful to be able to log unrecognised type names here
	if (skill_lookup.find(s) == skill_lookup.end())
	{
		g_LogFile.os() << "[sGirl::lookup_skill_code] Error: unknown Skill: " << s << endl;
		return -1;
	}
	return skill_lookup[s];
}

int sGirl::lookup_status_code(string s)
{
	// be useful to be able to log unrecognised type names here
	if (status_lookup.find(s) == status_lookup.end())
	{
		g_LogFile.os() << "[sGirl::lookup_status_code] Error: unknown Status: " << s << endl;
		return -1;
	}
	return status_lookup[s];
}

int sGirl::lookup_stat_code(string s)
{
	// be useful to be able to log unrecognised type names here
	if (stat_lookup.find(s) == stat_lookup.end())
	{
		g_LogFile.os() << "[sGirl::lookup_stat_code] Error: unknown Stat: " << s << endl;
		return -1;
	}
	return stat_lookup[s];
}

// END MOD

class GirlPredicate_GRG : public GirlPredicate
{
	bool m_slave = false;
	bool m_catacomb = false;
	bool m_arena = false;
	bool m_yourdaughter = false;
public:
	GirlPredicate_GRG(bool slave, bool catacomb, bool arena, bool daughter)
	{
		m_slave = slave;
		m_catacomb = catacomb;
		m_arena = arena;
		m_yourdaughter = daughter;
	}
	virtual bool test(sGirl *girl) 
	{
		return  girl->is_slave() == m_slave
			&&	girl->is_monster() == m_catacomb
			&&	girl->is_arena() == m_arena
			&&	girl->is_yourdaughter() == m_yourdaughter;
	}
};

// ----- Create / destroy

cGirls::cGirls()
{
	m_DefImages = 0;
	m_Parent = 0;
	m_Last = 0;
	m_NumRandomGirls = m_NumGirls = 0;
	m_RandomGirls = 0;
	m_LastRandomGirls = 0;
	names.load(DirPath() << "Resources" << "Data" << "RandomGirlNames.txt");
}

cGirls::~cGirls()
{
	Free();
	m_ImgListManager.Free();
}

void cGirls::Free()
{
	if (m_Parent) delete m_Parent;
	m_Parent = m_Last = 0;
	m_NumGirls = 0;
	g_GenGirls = false;
	if (m_RandomGirls) delete m_RandomGirls;
	m_RandomGirls = 0;
	m_LastRandomGirls = 0;
	m_NumRandomGirls = 0;
	m_DefImages = 0;
}

// ----- Utility

static char *n_strdup(const char *s)
{
	return strcpy(new char[strlen(s) + 1], s);
}

sGirl *sRandomGirl::lookup = new sGirl();  // used to look up stat and skill IDs

// ----- Misc

// if this returns true, the girl will disobey
bool cGirls::DisobeyCheck(sGirl* girl, int action, sBrothel* brothel)
{
	int diff;
	int chance_to_obey = 0;							// high value - more likely to obey
	chance_to_obey = -GetRebelValue(girl, false);	// let's start out with the basic rebelliousness
	chance_to_obey += 100;							// make it range from 0 to 200
	chance_to_obey /= 2;							// get a conventional percentage value
	/*
	 *	OK, let's factor in having a matron: normally this is done in GetRebelValue
	 *	but matrons have shifts now, so really we want twice the effect for a matron
	 *	on each shift as we'd get from just one. //corrected:(Either that, or we need to make this
	 *	check shift dependent.)//
	 *
	 *	Anyway, the old version added 15 for a matron either shift. Let's add
	 *	10 for each shift. Full coverage gets you 20 points
	 */
	if (brothel)
	{ // `J` added building checks
		if (brothel->matron_on_shift(SHIFT_DAY, girl->m_InClinic, girl->m_InMovieStudio, girl->m_InArena, girl->m_InCentre, girl->m_InHouse, girl->m_InFarm, girl->where_is_she)) chance_to_obey += 10;
		if (brothel->matron_on_shift(SHIFT_NIGHT, girl->m_InClinic, girl->m_InMovieStudio, girl->m_InArena, girl->m_InCentre, girl->m_InHouse, girl->m_InFarm, girl->where_is_she)) chance_to_obey += 10;
	}
	/*
	 *	This is still confusing - at least it still confuses me
	 *	why not normalise the rebellion -100 to 100 value so it runs
	 *	0 to 100, and invert it so it's basically an obedience check
	 */

	switch (action)
	{
	case ACTION_COMBAT:
		// WD use best stat as many girls have only one stat high	
		diff = max(girl->combat(), girl->magic()) - 50;
		diff /= 3;
		chance_to_obey += diff;
		break;
	case ACTION_SEX:
		// Let's do the same thing here
		diff = girl->libido();
		diff /= 5;
		chance_to_obey += diff;
		break;
	default:
		break;
	}
	chance_to_obey += girl->m_Enjoyment[action];			// add in her enjoyment level
	chance_to_obey += girl->pclove() / 10;					// let's add in some mods for love, fear and hate
	chance_to_obey += girl->pcfear() / 10;
	chance_to_obey -= girl->pchate() / 10;
	chance_to_obey += 30;									// Let's add a blanket 30% to all of that
	int roll = g_Dice.d100();								// let's get a percentage roll
	diff = chance_to_obey - roll;
	bool girl_obeys = (diff >= 0);
	if (girl_obeys)											// there's a price to be paid for relying on love or fear
	{
		if (diff < (girl->pclove() / 10)) girl->pclove(-1);	// if the only reason she obeys is love it wears away that love
		if (diff < (girl->pcfear() / 10)) girl->pcfear(-1);	// just a little bit. And if she's only doing it out of fear
	}
	/*
	 *	do we need any more than this, really?
	 *	we can add in some shaping factors if desired
	 */

	return !girl_obeys;
}

void cGirls::CalculateGirlType(sGirl* girl)
{
	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> CalculateGirlType
	int BigBoobs = 0, SmallBoobs = 0, CuteGirl = 0, Dangerous = 0, Cool = 0, Nerd = 0, NonHuman = 0, Lolita = 0,
		Elegant = 0, Sexy = 0, NiceFigure = 0, NiceArse = 0,  Freak = 0;

	girl->m_FetishTypes = 0;
	// zzzzzz -  the traits with /**/ in front of the mods have not been updated


	/****** Physical Traits ******/
	// Breasts
	if (HasTrait(girl, "Flat Chest"))				{ BigBoobs -= 120;	SmallBoobs += 80;	CuteGirl += 20;	Lolita += 30;	Sexy -= 10;	NiceFigure -= 10; }
	if (HasTrait(girl, "Petite Breasts"))			{ BigBoobs -= 80;	SmallBoobs += 100;	CuteGirl += 15;	Lolita += 20;	Sexy -= 5;	NiceFigure -= 5; }
	if (HasTrait(girl, "Small Boobs"))				{ BigBoobs -= 40;	SmallBoobs += 50;	CuteGirl += 10;	Lolita += 10;	Sexy += 5;	NiceFigure -= 0; }
	if (HasTrait(girl, "Busty Boobs"))				{ BigBoobs += 20;	SmallBoobs -= 20;	CuteGirl += 5;	Lolita += 0; 	Sexy += 10;	NiceFigure += 5; }
	if (HasTrait(girl, "Big Boobs"))				{ BigBoobs += 40;	SmallBoobs -= 40;	CuteGirl -= 5;	Lolita -= 20;	Sexy += 20;	NiceFigure += 10; }
	if (HasTrait(girl, "Giant Juggs"))				{ BigBoobs += 60;	SmallBoobs -= 60;	CuteGirl -= 25;	Lolita -= 40;	Sexy += 20;	NiceFigure += 5; }
	if (HasTrait(girl, "Massive Melons"))			{ BigBoobs += 80;	SmallBoobs -= 80;	CuteGirl -= 25;	Lolita -= 60;	Sexy += 10;	NiceFigure -= 5;	Freak += 5; }
	if (HasTrait(girl, "Abnormally Large Boobs"))	{ BigBoobs += 100;	SmallBoobs -= 100;	CuteGirl -= 15;	Lolita -= 80;	Sexy += 0;	NiceFigure -= 10;	Freak += 10;	NonHuman += 5; }
	if (HasTrait(girl, "Titanic Tits"))				{ BigBoobs += 120;	SmallBoobs -= 120;	CuteGirl -= 25;	Lolita -= 100;	Sexy -= 10;	NiceFigure -= 20;	Freak += 20;	NonHuman += 10; }
	// Nipples
	if (HasTrait(girl, "Inverted Nipples"))			{ BigBoobs -= 5;	SmallBoobs += 5;	CuteGirl -= 5;	Freak += 5; }
	if (HasTrait(girl, "Puffy Nipples"))			{ BigBoobs += 10;	SmallBoobs -= 5;	CuteGirl += 5; }
	if (HasTrait(girl, "Perky Nipples"))			{ BigBoobs += 10;	SmallBoobs += 5;	CuteGirl += 5;	Lolita += 5; }
	// Milk
	if (HasTrait(girl, "Dry Milk"))					{ BigBoobs -= 10;	SmallBoobs += 10;	CuteGirl += 5;	Lolita += 10;	}
	if (HasTrait(girl, "Scarce Lactation"))			{ BigBoobs -= 5;	SmallBoobs += 5;	CuteGirl += 0;	Lolita += 5;	}
	if (HasTrait(girl, "Abundant Lactation"))		{ BigBoobs += 10;	SmallBoobs -= 10;	CuteGirl -= 0;	Lolita -= 10;	}
	if (HasTrait(girl, "Cow Tits"))					{ BigBoobs += 20;	SmallBoobs -= 20;	CuteGirl -= 5;	Lolita -= 20;	Freak += 5; }

	// Scars
	if (HasTrait(girl, "Small Scars"))				/**/{ CuteGirl -= 5; Dangerous += 5; Cool += 2; Freak += 2; }
	if (HasTrait(girl, "Cool Scars"))				/**/{ CuteGirl -= 10; Dangerous += 20; Cool += 30; Freak += 5; }
	if (HasTrait(girl, "Horrific Scars"))			/**/{ CuteGirl -= 15; Dangerous += 30; Freak += 20; }



	/****** Mental Traits ******/
	if (HasTrait(girl, "Quick Learner"))			/**/{ Cool -= 20; Nerd += 30; }
	if (HasTrait(girl, "Slow Learner"))				/**/{ CuteGirl += 10; Cool += 10; Nerd -= 20; }



	/****** Sexual Traits ******/
	if (HasTrait(girl, "Strong Gag Reflex"))		{ Elegant += 20; Sexy -= 20; Freak += 5;	Lolita += 20; }
	if (HasTrait(girl, "Gag Reflex"))				{ Elegant += 10; Sexy -= 10; Freak += 0;	Lolita += 10; }
	if (HasTrait(girl, "No Gag Reflex"))			{ Elegant -= 10; Sexy += 10; Freak += 15;	Lolita -= 5; }
	if (HasTrait(girl, "Deep Throat"))				{ Elegant -= 20; Sexy += 20; Freak += 30;	Lolita -= 10; }

	// Sexuality
	if (HasTrait(girl, "Straight"))					{}
	if (HasTrait(girl, "Bisexual"))					{}
	if (HasTrait(girl, "Lesbian"))					{}

	/****** Social Traits ******/


	/****** Species Traits ******/
	if (HasTrait(girl, "Not Human"))				/**/{ NonHuman += 60; Freak += 10; }
	if (HasTrait(girl, "Strange Eyes"))				/**/{ NonHuman += 10; Freak += 15; }
	if (HasTrait(girl, "Cat Girl"))					/**/{ CuteGirl += 20; NonHuman += 60; Freak += 5; }
	if (HasTrait(girl, "Demon"))					/**/{ Dangerous += 10; NonHuman += 60; Freak += 5; }
	if (HasTrait(girl, "Shape Shifter"))			/**/{ NonHuman += 35; Sexy += 20; NiceFigure += 40; Freak += 40; }
	if (HasTrait(girl, "Futanari"))					/**/{ CuteGirl -= 15; NonHuman += 10; Freak += 30; }
	if (HasTrait(girl, "Construct"))				/**/{ Dangerous += 10; NonHuman += 60; Freak += 20; }
	if (HasTrait(girl, "Half-Construct"))			/**/{ Dangerous += 5; NonHuman += 20; Freak += 20; }
	if (HasTrait(girl, "Incorporeal"))				/**/{ NonHuman += 60; Freak += 40; }


	/****** Unsorted Traits ****** need to sort them ******/
	if (HasTrait(girl, "Cool Person"))				/**/{ Dangerous += 5; Cool += 60; Nerd -= 10; }
	if (HasTrait(girl, "Nerd"))						/**/{ CuteGirl += 10; Dangerous -= 30; Cool -= 30; Nerd += 60; SmallBoobs += 5; }
	if (HasTrait(girl, "Clumsy"))					/**/{ CuteGirl += 10; Dangerous -= 20; Cool -= 10; Nerd += 20; Freak += 5; }

	if (HasTrait(girl, "Fake Orgasm Expert"))		/**/{ Sexy += 5; }
	if (HasTrait(girl, "Fast Orgasms"))				/**/{ Cool += 10; Sexy += 30; }
	if (HasTrait(girl, "Slow Orgasms"))				/**/{ CuteGirl -= 5; Cool -= 5; Elegant += 5; Sexy -= 10; Freak += 5; }
	if (HasTrait(girl, "Cute"))						/**/{ CuteGirl += 60; Lolita += 20; SmallBoobs += 5; }
	if (HasTrait(girl, "Strong"))					/**/{ Dangerous += 20; Cool += 20; Nerd -= 30; NiceFigure += 20; NiceArse += 20; Lolita -= 5; }
	if (HasTrait(girl, "Psychic"))					/**/{ Dangerous += 10; Nerd += 10; NonHuman += 10; Freak += 10; }
	if (HasTrait(girl, "Strong Magic"))				/**/{ Dangerous += 20; Nerd += 5; NonHuman += 5; Freak += 20; }
	if (HasTrait(girl, "Aggressive"))				/**/{ CuteGirl -= 15; Dangerous += 20; Lolita -= 5; Elegant -= 10; Freak += 10; }
	if (HasTrait(girl, "Adventurer"))				/**/{ Dangerous += 20; Cool += 10; Nerd -= 20; Elegant -= 5; }
	if (HasTrait(girl, "Assassin"))					/**/{ Dangerous += 25; Cool += 15; Nerd -= 25; Freak += 10; }
	if (HasTrait(girl, "Lolita"))					/**/{ BigBoobs -= 30; CuteGirl += 30; Dangerous -= 5; Lolita += 60; SmallBoobs += 15; }
	if (HasTrait(girl, "Nervous"))					/**/{ CuteGirl += 10; Nerd += 15; }
	if (HasTrait(girl, "Good Kisser"))				/**/{ Cool += 10; Sexy += 20; }
	if (HasTrait(girl, "Nymphomaniac"))				/**/{ Sexy += 15; Freak += 20; Elegant -= 5; }
	if (HasTrait(girl, "Elegant"))					/**/{ Dangerous -= 30; Nerd -= 20; NonHuman -= 20; Elegant += 60; Freak -= 30; }
	if (HasTrait(girl, "Sexy Air"))					/**/{ Cool += 5; Elegant -= 5; Sexy += 10; }
	if (HasTrait(girl, "Great Figure"))				/**/{ BigBoobs += 10; Sexy += 10; NiceFigure += 60; }
	if (HasTrait(girl, "Great Arse"))				/**/{ Sexy += 10; NiceArse += 60; }
	if (HasTrait(girl, "Broken Will"))				/**/{ Cool -= 40; Nerd -= 40; Elegant -= 40; Sexy -= 40; Freak += 40; }
	if (HasTrait(girl, "Masochist"))				/**/{ CuteGirl -= 10; Nerd -= 10; CuteGirl -= 15; Dangerous += 10; Elegant -= 10; Freak += 30; }
	if (HasTrait(girl, "Sadistic"))					/**/{ CuteGirl -= 20; Dangerous += 15; Nerd -= 10; Elegant -= 30; Sexy -= 10; Freak += 30; }
	if (HasTrait(girl, "Tsundere"))					/**/{ Dangerous += 5; Cool += 5; Nerd -= 5; Elegant -= 20; Freak += 10; }
	if (HasTrait(girl, "Yandere"))					/**/{ Dangerous += 5; Cool += 5; Nerd -= 5; Elegant -= 20; Freak += 10; }
	if (HasTrait(girl, "Meek"))						/**/{ CuteGirl += 15; Dangerous -= 30; Cool -= 30; Nerd += 30; Lolita += 10; }
	if (HasTrait(girl, "Manly"))					/**/{ CuteGirl -= 15; Dangerous += 5; Elegant -= 20; Sexy -= 20; NiceFigure -= 20; SmallBoobs += 10; Freak += 5; Lolita -= 10; }
	if (HasTrait(girl, "Merciless"))				/**/{ CuteGirl -= 20; Dangerous += 20; Nerd -= 10; Elegant -= 5; Lolita -= 10; }
	if (HasTrait(girl, "Fearless"))					/**/{ Dangerous += 20; Cool += 15; Nerd -= 10; Elegant -= 10; Lolita -= 5; }
	if (HasTrait(girl, "Iron Will"))				/**/{ Dangerous += 10; Cool += 10; Nerd -= 5; Elegant -= 10; }
	if (HasTrait(girl, "Twisted"))					/**/{ CuteGirl -= 40; Dangerous += 30; Elegant -= 30; Sexy -= 20; Freak += 40; }
	if (HasTrait(girl, "Optimist"))					/**/{ Elegant += 5; }
	if (HasTrait(girl, "Pessimist"))				/**/{ Elegant -= 5; }
	if (HasTrait(girl, "Dependant"))				/**/{ CuteGirl += 5; Dangerous -= 20; Cool -= 5; Nerd += 5; Elegant -= 20; Sexy -= 20; Freak += 10; }
	if (HasTrait(girl, "Fleet of Foot"))			/**/{ Dangerous += 10; Sexy += 20; }
	if (HasTrait(girl, "Tough"))					/**/{ CuteGirl -= 5; Dangerous += 10; Cool += 10; Nerd -= 5; Elegant -= 5; }
	if (HasTrait(girl, "One Eye"))					/**/{ CuteGirl -= 20; Cool += 5; Dangerous += 10; Sexy -= 20; NiceFigure -= 10; Freak += 20; }
	if (HasTrait(girl, "Eye Patch"))				/**/{ CuteGirl -= 5; Dangerous += 5; Cool += 20; Sexy -= 5; Freak += 20; }
	if (HasTrait(girl, "Fragile"))					/**/{ CuteGirl += 10; Nerd += 5; Freak += 10; Lolita += 10; }
	if (HasTrait(girl, "Mind Fucked"))				/**/{ CuteGirl -= 60; Dangerous -= 60; Cool -= 60; Nerd -= 60; Elegant -= 60; Sexy -= 60; Freak += 40; }
	if (HasTrait(girl, "Charismatic"))				/**/{ Elegant += 30; Sexy += 30; Freak -= 20; }
	if (HasTrait(girl, "Charming"))					/**/{ Elegant += 20; Sexy += 20; Freak -= 15; }
	if (HasTrait(girl, "Long Legs"))				/**/{ Sexy += 20; NiceFigure += 20; }
	if (HasTrait(girl, "Different Colored Eyes"))	/**/{ NonHuman += 5; Freak += 10; }
	if (HasTrait(girl, "MILF"))						/**/{ Freak += 15; Lolita -= 50; }
	if (HasTrait(girl, "Malformed"))				/**/{ NonHuman += 10; Freak += 50; }
	if (HasTrait(girl, "Retarded"))					/**/{ NonHuman += 2; Freak += 45; }
	if (HasTrait(girl, "Queen"))					/**/{ Elegant += 60; Sexy += 20; Freak -= 15; }
	if (HasTrait(girl, "Princess"))					/**/{ Elegant += 40; Sexy += 20; Freak -= 15; }
	if (HasTrait(girl, "Pierced Nipples"))			/**/{ Elegant -= 20; Sexy += 20; Freak += 15; }
	if (HasTrait(girl, "Pierced Tongue"))			/**/{ Elegant -= 20; Sexy += 20; Freak += 15; }
	if (HasTrait(girl, "Pierced Clit"))				/**/{ Elegant -= 20; Sexy += 20; Freak += 15; }



	// Addictions
	if (HasTrait(girl, "Shroud Addict"))             { Dangerous += 5; Cool += 15; Nerd -= 10; Elegant -= 20; Sexy -= 20; Freak += 10; }
	if (HasTrait(girl, "Fairy Dust Addict"))         { Dangerous += 10; Cool += 20; Nerd -= 15; Elegant -= 25; Sexy -= 25; Freak += 15; }
	if (HasTrait(girl, "Viras Blood Addict"))        { Dangerous += 15; Cool += 25; Nerd -= 20; Elegant -= 30; Sexy -= 30; Freak += 20; }


	// ? how would the customer know these? but keeping it in for completion
	if (HasTrait(girl, "Sterile"))					{}
	if (HasTrait(girl, "Fertile"))					{}







	if (BigBoobs > SmallBoobs)
	{
		if (BigBoobs > 50)		girl->m_FetishTypes |= (1 << FETISH_BIGBOOBS);
	}
	else
	{
		if (SmallBoobs > 50)	girl->m_FetishTypes |= (1 << FETISH_SMALLBOOBS);
	}
	if (CuteGirl > 50)			girl->m_FetishTypes |= (1 << FETISH_CUTEGIRLS);
	if (Dangerous > 50)			girl->m_FetishTypes |= (1 << FETISH_DANGEROUSGIRLS);
	if (Cool > 50)				girl->m_FetishTypes |= (1 << FETISH_COOLGIRLS);
	if (Nerd > 50)				girl->m_FetishTypes |= (1 << FETISH_NERDYGIRLS);
	if (NonHuman > 50)			girl->m_FetishTypes |= (1 << FETISH_NONHUMAN);
	if (Lolita > 50)			girl->m_FetishTypes |= (1 << FETISH_LOLITA);
	if (Elegant > 50)			girl->m_FetishTypes |= (1 << FETISH_ELEGANT);
	if (Sexy > 50)				girl->m_FetishTypes |= (1 << FETISH_SEXY);
	if (NiceFigure > 50)		girl->m_FetishTypes |= (1 << FETISH_FIGURE);
	if (NiceArse > 50)			girl->m_FetishTypes |= (1 << FETISH_ARSE);
	if (Freak > 50)				girl->m_FetishTypes |= (1 << FETISH_FREAKYGIRLS);
}

bool cGirls::CheckGirlType(sGirl* girl, int type)
{
	if (type == FETISH_TRYANYTHING || girl->m_FetishTypes&(1 << type))	return true;
	return false;
}

sGirl *sGirl::run_away()
{
	m_RunAway = 6;		// player has 6 weeks to retreive
	m_NightJob = m_DayJob = JOB_RUNAWAY;
	g_Brothels.AddGirlToRunaways(this);
	return 0;
}

void cGirls::CalculateAskPrice(sGirl* girl, bool vari)
{
	girl->m_Stats[STAT_ASKPRICE] = 0;
	SetStat(girl, STAT_ASKPRICE, 0);
	int askPrice = (int)(((GetStat(girl, STAT_BEAUTY) + GetStat(girl, STAT_CHARISMA)) / 2)*0.6f);	// Initial price
	askPrice += GetStat(girl, STAT_CONFIDENCE) / 10;		// their confidence will make them think they are worth more
	askPrice += GetStat(girl, STAT_INTELLIGENCE) / 10;		// if they are smart they know they can get away with a little more
	askPrice += GetStat(girl, STAT_FAME) / 2;				// And lastly their fame can be quite useful too
	if (GetStat(girl, STAT_LEVEL) > 0)	askPrice += GetStat(girl, STAT_LEVEL) * 10;  // MYR: Was * 1

	if (vari)
	{
		int minVariance = 0 - (g_Dice % 10) + 1;
		int maxVariance = (g_Dice % 10) + 1;
		int variance = ((g_Dice % 10) + maxVariance) - minVariance;
		askPrice += variance;
	}
	if (askPrice > 100) askPrice = 100;
	if (askPrice < 0) askPrice = 0;

	UpdateStat(girl, STAT_ASKPRICE, askPrice);
}

sRandomGirl* cGirls::random_girl_at(u_int n)
{
	u_int i;
	sRandomGirl* current = m_RandomGirls;
	/*
	 *	if we try and get a random girl template
	 *	that's numbered higher than the number of
	 *	templates... well we're not going to find it.
	 *
	 *	So let's cut to the chase
	 */
	if (n >= m_NumRandomGirls)	return 0;
	// loop through the linked list n times
	for (i = 0; i < n; i++)
	{
		current = current->m_Next;
		/*
		 *		current should only be null at the end
		 *		of the chain. Which we shouldn't get to
		 *		since we know n < m_NumRandomGirls from
		 *		the above.
		 *
		 *		so if it IS null, we have an integrity error
		 *		in the pointer chain.
		 *
		 *		is it too late to rewrite this using vector?
		 */
		if (current == 0)
		{
			g_LogFile.os() << "broken chain in cGirls::random_girl_at" << endl;
			return 0;
		}
	}
	return current;		// and there we (hopefully) are
}

sGirl* cGirls::CreateRandomGirl(int age, bool addToGGirls, bool slave, bool undead, bool NonHuman, bool childnaped, bool arena, bool daughter)
{
	cConfig cfg;
	sRandomGirl* current;
	if (m_NumRandomGirls == 0)	current = false;
	else
	{
		int i = 0;
		int random_girl_index = g_Dice%m_NumRandomGirls;	// pick a number between 0 and m_NumRandomGirls as the stating point
		while (i < (int)m_NumRandomGirls)	// loop until we find a human/non-human template as required
		{
			current = random_girl_at(random_girl_index);
			if (current != 0 && NonHuman == (current->m_Human == 0))				// test for humanity - or lack of it as the case may be
			{
				break;
			}
			//	She's either human when we wanted non-human or non-human when we wanted human
			//	Either way, try again...
			i++; random_girl_index++;	// `J` check all random girls then if not found return the last random girl checked
			if (random_girl_index > (int)m_NumRandomGirls) random_girl_index = 0;
		}
		if (!current)
		{
			g_LogFile.os() << "There was an error in CreateRandomGirl code, using hard coded random girl" << endl;
		}
	}
	if (!current)	// `J` added hard coded random girl if there are no rgirlsx files
	{
		current = new sRandomGirl();
		current->m_newRandom = false;
		current->m_Desc = "Hard Coded Random Girl\n(The game did not find any .rgirlsx files)\n(or there was an error in an .rgirlsx file)";
		current->m_Name = "Default";
		current->m_Human = (NonHuman == 0);
		current->m_Arena = arena;
		current->m_YourDaughter = daughter;
		for (int i = 0; i < NUM_STATS; i++)
		{
			if (age != 0 && i == STAT_AGE)	{ current->m_MinStats[i] = age;	current->m_MaxStats[i] = age; }

			if (arena)
			{
				if (i == STAT_CHARISMA)		{ current->m_MinStats[i] += 0;	current->m_MaxStats[i] += 10; }
				if (i == STAT_CONSTITUTION)	{ current->m_MinStats[i] += 20;	current->m_MaxStats[i] += 20; }
				if (i == STAT_INTELLIGENCE)	{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_CONFIDENCE)	{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 20; }
				if (i == STAT_MANA)			{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 40; }
				if (i == STAT_AGILITY)		{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 20; }
				if (i == STAT_OBEDIENCE)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] -= 10; }
				if (i == STAT_SPIRIT)		{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 20; }
			}
			if (NonHuman)
			{
				if (i == STAT_CHARISMA)		{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_CONSTITUTION)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 20; }
				if (i == STAT_INTELLIGENCE)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_CONFIDENCE)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 20; }
				if (i == STAT_MANA)			{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 40; }
				if (i == STAT_AGILITY)		{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_OBEDIENCE)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_SPIRIT)		{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
			}
			// normalize
			if (current->m_MinStats[i] < 0)current->m_MinStats[i] = 0;	if (current->m_MinStats[i] > 100)current->m_MinStats[i] = 100;
			if (current->m_MaxStats[i] < 0)current->m_MaxStats[i] = 0;	if (current->m_MaxStats[i] > 100)current->m_MaxStats[i] = 100;
			if (current->m_MinStats[i] > current->m_MaxStats[i])	{ int a = current->m_MinStats[i]; current->m_MinStats[i] = current->m_MaxStats[i]; current->m_MaxStats[i] = a; }
		}
		for (int i = 0; i < NUM_SKILLS; i++)
		{	// base for all skills is 0-30
			if (arena)
			{
				if (i == SKILL_MAGIC || i == SKILL_COMBAT)
				{
					current->m_MinSkills[i] += 50; current->m_MaxSkills[i] += 100;
				}
				if (i == SKILL_ANAL || i == SKILL_NORMALSEX || i == SKILL_GROUP || i == SKILL_LESBIAN ||
					i == SKILL_STRIP || i == SKILL_ORALSEX || i == SKILL_TITTYSEX || i == SKILL_HANDJOB)
				{
					current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] -= 20;
				}
				if (i == SKILL_BDSM || i == SKILL_BEASTIALITY)				current->m_MaxSkills[i] -= 10;
				if (i == SKILL_PERFORMANCE || i == SKILL_ANIMALHANDLING)	current->m_MaxSkills[i] += 20;
				if (i == SKILL_MEDICINE)									current->m_MaxSkills[i] += 10;
			}
			if (NonHuman)
			{
				if (i == SKILL_MAGIC)			{ current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] += 40; }
				if (i == SKILL_COMBAT)			{ current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] += 40; }
				if (i == SKILL_SERVICE)			{ current->m_MaxSkills[i] -= 20; current->m_MaxSkills[i] -= 10; }
				if (i == SKILL_BEASTIALITY)		{ current->m_MaxSkills[i] += 20; }
				if (i == SKILL_CRAFTING || i == SKILL_HERBALISM || i == SKILL_FARMING)
				{
					current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] += 10;
				}
				if (i == SKILL_ANIMALHANDLING)	{ current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] += 40; }
			}
			// normalize
			if (current->m_MinSkills[i] < 0)current->m_MinSkills[i] = 0; if (current->m_MinSkills[i] > 100)current->m_MinSkills[i] = 100;
			if (current->m_MaxSkills[i] < 0)current->m_MaxSkills[i] = 0; if (current->m_MaxSkills[i] > 100)current->m_MaxSkills[i] = 100;
			if (current->m_MinSkills[i] > current->m_MaxSkills[i]) { int a = current->m_MinSkills[i]; current->m_MinSkills[i] = current->m_MaxSkills[i]; current->m_MaxSkills[i] = a; }
		}

		// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> CreateRandomGirl > hardcoded rgirl
		current->m_NumTraits = 0;
		for (int i = 0; i < g_Traits.GetNumTraits() && current->m_NumTraits < 50; i++)
		{
			int c = -1;
			string test = g_Traits.GetTraitNum(i)->m_Name;
			// first check if it is a daughter or nonhuman trait
			if (test == "Your Daughter")	c = (daughter) ? 100 : 0;
			if (test == "Not Human")		c = (NonHuman) ? 100 : 0;
			if (test == "Incorporeal")		c = (NonHuman) ? 1 : 0;
			if (test == "Futanari")			c = (NonHuman) ? 5 : 0;
			if (test == "Construct")		c = (NonHuman) ? 5 : 0;
			if (test == "Half-Construct")	c = (NonHuman) ? 10 : 0;
			if (test == "Strange Eyes")		c = (NonHuman) ? 10 : 0;
			if (test == "Cat Girl")			c = (NonHuman) ? 20 : 0;
			if (test == "Demon")			c = (NonHuman) ? 20 : 0;
			if (test == "Shape Shifter")	c = (NonHuman) ? 5 : 0;

			// virgin is handled differently
			if (test == "Virgin")			c = (age < 21) ? max(min(100 - ((age - 17) * 10), 100), 0) : max(70 - ((age - 20) * 2), 0);
			// so under 18 is 100%, 18=90%, 19=80%, 20=70%, then 21=68%, 22=66% and so on until age 55 where it is 0%

			// all other traits have a 1 in 3 chance of making the cut
			if (c == -1 && g_Dice % 3 == 1)
			{
				// now, certain traits are more or less common than others
				if (test == "Shroud Addict")		c = 5;
				if (test == "Fairy Dust Addict")	c = 5;
				if (test == "Viras Blood Addict")	c = 5;
				if (test == "AIDS")					c = 2;
				if (test == "Chlamydia")			c = 3;
				if (test == "Syphilis")				c = 4;
				if (test == "Herpes")				c = 5;
				if (test == "Queen")				c = 5;
				if (test == "Princess")				c = 10;
				if (test == "Sterile")				c = 5;
				if (test == "Incest")				c = 5;

				// some traits are more or less common for arena girls
				if (test == "Small Scars")			c = (arena) ? 40 : 20;
				if (test == "Cool Scars")			c = (arena) ? 40 : 20;
				if (test == "Horrific Scars")		c = (arena) ? 30 : 10;
				if (test == "Cool Person")			c = (arena) ? 25 : 20;
				if (test == "Nerd")					c = (arena) ? 5 : 20;
				if (test == "Clumsy")				c = (arena) ? 5 : 20;
				if (test == "Cute")					c = (arena) ? 10 : 20;
				if (test == "Strong")				c = (arena) ? 40 : 20;
				if (test == "Strong Magic")			c = (arena) ? 30 : 10;
				if (test == "Aggressive")			c = (arena) ? 40 : 20;
				if (test == "Adventurer")			c = (arena) ? 50 : 20;
				if (test == "Assassin")				c = (arena) ? 30 : 20;
				if (test == "Lolita")				c = (arena) ? 10 : 20;
				if (test == "Nervous")				c = (arena) ? 10 : 20;
				if (test == "Elegant")				c = (arena) ? 10 : 20;
				if (test == "Great Figure")			c = (arena) ? 50 : 30;
				if (test == "Great Arse")			c = (arena) ? 40 : 30;
				if (test == "Broken Will")			c = (arena) ? 0 : 5;
				if (test == "Masochist")			c = (arena) ? 20 : 10;
				if (test == "Sadistic")				c = (arena) ? 30 : 10;
				if (test == "Tsundere")				c = (arena) ? 30 : 10;
				if (test == "Yandere")				c = (arena) ? 30 : 10;
				if (test == "Meek")					c = (arena) ? 0 : 10;
				if (test == "Manly")				c = (arena) ? 20 : 10;
				if (test == "Merciless")			c = (arena) ? 30 : 10;
				if (test == "Fearless")				c = (arena) ? 40 : 10;
				if (test == "Iron Will")			c = (arena) ? 40 : 10;
				if (test == "Twisted")				c = (arena) ? 20 : 10;
				if (test == "Dependant")			c = (arena) ? 0 : 10;
				if (test == "Fleet of Foot")		c = (arena) ? 40 : 20;
				if (test == "Tough")				c = (arena) ? 40 : 20;
				if (test == "One Eye")				c = (arena) ? 10 : 5;
				if (test == "Eye Patch")			c = (arena) ? 10 : 5;
				if (test == "Fragile")				c = (arena) ? 0 : 5;
				if (test == "Mind Fucked")			c = (arena) ? 0 : 5;
				if (test == "Malformed")			c = (arena) ? 5 : 5;
				if (test == "Retarded")				c = (arena) ? 2 : 5;

				if (c == -1) c = 20; // set all unlisted to base 20%
			}
			if (c > 0)	// after the checks, if c was set, add the trait to the girl
			{
				current->m_Traits[current->m_NumTraits] = g_Traits.GetTraitNum(i);
				current->m_TraitChance[current->m_NumTraits] = c;
				current->m_NumTraits++;
			}
		}
	}

	sGirl* newGirl = new sGirl();
	newGirl->m_Next = 0;
	newGirl->m_AccLevel = 1;
	newGirl->m_States = 0;
	newGirl->m_NumTraits = 0;

	newGirl->m_Desc = current->m_Desc;		// Bugfix.. was populating description with name.
	newGirl->m_Name = new char[current->m_Name.length() + 1];	// name
	strcpy(newGirl->m_Name, current->m_Name.c_str());

	// set all jobs to null
	newGirl->m_DayJob = newGirl->m_NightJob = newGirl->m_YesterDayJob = newGirl->m_YesterNightJob = newGirl->m_PrevDayJob = newGirl->m_PrevNightJob = 255;
	newGirl->m_WorkingDay = newGirl->m_PrevWorkingDay = newGirl->m_SpecialJobGoal = 0;

	newGirl->m_Money = (g_Dice % (current->m_MaxMoney - current->m_MinMoney)) + current->m_MinMoney;	// money

	// skills
	for (u_int i = 0; i < NUM_SKILLS; i++)
	{
		if (current->m_MaxSkills[i] == current->m_MinSkills[i])		newGirl->m_Skills[i] = current->m_MaxSkills[i];
		else if (current->m_MaxSkills[i] < current->m_MinSkills[i])	newGirl->m_Skills[i] = g_Dice % 101;
		else	newGirl->m_Skills[i] = (int)(g_Dice % (current->m_MaxSkills[i] - current->m_MinSkills[i])) + current->m_MinSkills[i];
	}

	// stats
	for (int i = 0; i < NUM_STATS; i++)
	{
		if (current->m_MaxStats[i] == current->m_MinStats[i])		newGirl->m_Stats[i] = current->m_MaxStats[i];
		else if (current->m_MaxStats[i] < current->m_MinStats[i])	newGirl->m_Stats[i] = g_Dice % 101;
		else	newGirl->m_Stats[i] = (g_Dice % (current->m_MaxStats[i] - current->m_MinStats[i])) + current->m_MinStats[i];
	}

	for (int i = 0; i < current->m_NumTraits; i++)	// add the traits
	{
		int chance = g_Dice % 100 + 1;
		if (g_Traits.GetTrait(g_Traits.GetTranslateName(current->m_Traits[i]->m_Name))) // `J` added translation check
		{
			if (current->m_Traits[i]->m_Name == "Virgin") newGirl->m_Virgin = (chance <= (int)current->m_TraitChance[i]) ? 1 : 0;
			if (chance <= (int)current->m_TraitChance[i])
			{
				if (!HasTrait(newGirl, current->m_Traits[i]->m_Name))
					AddTrait(newGirl, current->m_Traits[i]->m_Name);
			}
		}
		else
		{
			string message = "cGirls::CreateRandomGirl: ERROR: Trait '";
			message += current->m_Traits[i]->m_Name;
			message += "' from girl template ";
			message += current->m_Name;
			message += " doesn't exist or is spelled incorrectly.";
			g_MessageQue.AddToQue(message, COLOR_RED);
		}
	}

	if (current->m_Human == 0)			AddTrait(newGirl, "Not Human");
	if (current->m_YourDaughter == 1)	AddTrait(newGirl, "Your Daughter");

	newGirl->m_Stats[STAT_FAME] = 0;
	if (age != 0)	newGirl->m_Stats[STAT_AGE] = age;
	newGirl->m_Stats[STAT_HEALTH] = 100;
	newGirl->m_Stats[STAT_HAPPINESS] = 100;
	newGirl->m_Stats[STAT_TIREDNESS] = 0;

	if (childnaped)	// this girl has been taken against her will so make her rebelious
	{
		newGirl->m_Stats[STAT_SPIRIT] = 100;
		newGirl->m_Stats[STAT_CONFIDENCE] = 100;
		newGirl->m_Stats[STAT_OBEDIENCE] = 0;
		newGirl->m_Stats[STAT_PCHATE] = 50;
	}

	if (CheckVirginity(newGirl))	// `J` check random girl's virginity
	{
		newGirl->m_Virgin = 1;
		AddTrait(newGirl, "Virgin");
	}
	else
	{
		newGirl->m_Virgin = 0;
		RemoveTrait(newGirl, "Virgin");
	}
	if (newGirl->m_Stats[STAT_AGE] < 18) newGirl->m_Stats[STAT_AGE] = 18;

	if (g_Dice % 100 <= 3)	AddTrait(newGirl, "Shroud Addict");
	if (g_Dice % 100 <= 2)	AddTrait(newGirl, "Fairy Dust Addict");
	if (g_Dice % 100 == 1)	AddTrait(newGirl, "Viras Blood Addict");

	if (daughter)				newGirl->m_Stats[STAT_HOUSE] = 0;	// your daughter gets to keep all she gets
	else if (!slave && !arena)	newGirl->m_Stats[STAT_HOUSE] = cfg.initial.girls_house_perc();	// 60% is the norm
	else newGirl->m_Stats[STAT_HOUSE] = cfg.initial.slave_house_perc();	// 100% is the norm

	// If the girl is a slave or arena.. then make her more obedient.
	if (slave || newGirl->m_States&(1 << STATUS_SLAVE))
	{
		newGirl->m_States |= (1 << STATUS_SLAVE);
		newGirl->m_AccLevel = 0;
		newGirl->m_Money = 0;
		newGirl->m_Stats[STAT_OBEDIENCE] = min(newGirl->m_Stats[STAT_OBEDIENCE] + 20, 100);
	}
	if (arena || newGirl->m_States&(1 << STATUS_ARENA))
	{
		newGirl->m_States |= (1 << STATUS_ARENA);
		newGirl->m_AccLevel = 0;
		newGirl->m_Money = 0;
		newGirl->m_Stats[STAT_OBEDIENCE] = min(newGirl->m_Stats[STAT_OBEDIENCE] + 20, 100);
	}
	if (daughter || newGirl->m_States&(1 << STATUS_YOURDAUGHTER))	// `J` if she is your daughter...
	{
		newGirl->m_States |= (1 << STATUS_YOURDAUGHTER);
		newGirl->m_AccLevel = 5;						// pamper her
		newGirl->m_Money = 1000;
		newGirl->m_Stats[STAT_HOUSE] = 0;	// your daughter gets to keep all she gets
		AddTrait(newGirl, "Your Daughter");
		newGirl->m_Stats[STAT_OBEDIENCE] = max(newGirl->m_Stats[STAT_OBEDIENCE], 80);	// She starts out obedient
		if (g_Girls.CheckVirginity(newGirl))
		{		// you made sure she stayed pure
			// `J` needs work
		}
	}

	// Load any girl images if available
	LoadGirlImages(newGirl);

	if (current->m_newRandom && (newGirl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages > 0))
	{
		if (current->m_newRandomTable == 0)
		{
			current->m_newRandomTable = new bool[newGirl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages];
			for (int i = 0; i < newGirl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages; i++)
				current->m_newRandomTable[i] = false;
		}
		int j = 3;
		do
		{
			newGirl->m_newRandomFixed = g_Dice % newGirl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages;
			j--;
		} while ((j > 0) && current->m_newRandomTable[newGirl->m_newRandomFixed]);
		current->m_newRandomTable[newGirl->m_newRandomFixed] = true;
	}
	else	newGirl->m_newRandomFixed = -1;

	RemoveAllRememberedTraits(newGirl);	// WD: remove any rembered traits created from trait incompatibilities

	/*
	 *	Now that everything is in there, time to give her a random name
	 *
	 *	we'll try five times for a unique name
	 *	if that fails, we'll give her the last one found
	 *	this should be ok - assuming that names don't have to be unique
	 *
	 *	If they do need to be unique, the game will slow drastically as
	 *	the number of girls approaches the limit, and hang once it is surpassed.
	 *
	 *	So I'm assuming non-unique names are ok
	 */
	string name;
	for (int i = 0; i < 5; i++)
	{
		name = names.random();
		if (i>3) name = name + " " + names.random(); // `J` added second name to further reduce chance of multiple names
		if (NameExists(name)) continue;
		break;
	}
	newGirl->m_Realname = name;

	DirPath dp = DirPath() << "Resources" << "Characters" << newGirl->m_Name << "triggers.xml";
	newGirl->m_Triggers.LoadList(dp);
	newGirl->m_Triggers.SetGirlTarget(newGirl);

	// `J` more usefull log for rgirl
	g_LogFile.os() << gettext("Random girl ") << newGirl->m_Realname << gettext(" created from template ") << newGirl->m_Name << gettext(".rgirlsx") << endl;

	if (addToGGirls)	AddGirl(newGirl);

	CalculateGirlType(newGirl);

	return newGirl;
}

bool cGirls::NameExists(string name)
{
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_Realname == name)	return true;
		current = current->m_Next;
	}
	if (g_Brothels.NameExists(name))		return true;
	for (int i = 0; i < 8; i++)
	{
		if (MarketSlaveGirls[i])
		{
			if (MarketSlaveGirls[i]->m_Realname == name)	return true;
		}
	}
	return false;
}

// `J` moved exp check into levelup to reduce coding
void cGirls::LevelUp(sGirl* girl)
{
	int level = GetStat(girl, STAT_LEVEL);
	int xp = GetStat(girl, STAT_EXP);
	int xpneeded = min(32000, (level + 1) * 125);

	if (xp < xpneeded) return;

	SetStat(girl, STAT_EXP, xp-xpneeded);
	UpdateStat(girl, STAT_LEVEL, 1);

	if (GetStat(girl, STAT_LEVEL) <= 20)	LevelUpStats(girl);

	stringstream ss;
	ss << girl->m_Realname << gettext(" levelled up to ") << girl->level() << ".";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
	ss.str("");

	// add traits
	// MYR: One chance to get a new trait every five levels.
	if (GetStat(girl, STAT_LEVEL) % 5 == 0)
	{
		int addedtrait = GetStat(girl, STAT_LEVEL) + 5;
		while (addedtrait > 0)
		{
			int chance = g_Dice%12;
			string trait = "";
			switch (chance)
			{
			case 1:		trait = "Agile";				break;
			case 2:		trait = "Charismatic";			break;
			case 3:		trait = "Charming";				break;
			case 4:		trait = "Cool Person";			break;
			case 5:		trait = "Fake Orgasm Expert";	break;
			case 6:		trait = "Fleet of Foot";		break;
			case 7:		trait = "Good Kisser";			break;
			case 8:		trait = "Nimble Tongue";		break;
			case 9:		trait = "Nymphomaniac";			break;
			case 10:	trait = "Open Minded";			break;
			case 11:	trait = "Sexy Air";				break;
			default: break;
			}
			if (trait != "" && !HasTrait(girl, trait))
			{
				addedtrait = 0;
				AddTrait(girl, trait);
				ss << " She has gained the " << trait << " trait.";
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
			}
			addedtrait--;
		}
	}
}

void cGirls::LevelUpStats(sGirl* girl)
{
	int DiceSize = 3;
	if (HasTrait(girl, "Quick Learner"))		DiceSize = 4;
	else if (HasTrait(girl, "Slow Learner"))	DiceSize = 2;

	// level up stats (only first 8 advance in levelups)
	for (int i = 0; i < 8; i++)	UpdateStat(girl, i, g_Dice%DiceSize);

	// level up skills
	for (u_int i = 0; i < NUM_SKILLS; i++)	UpdateSkill(girl, i, g_Dice%DiceSize);
}

// ----- Add remove

void cGirls::AddRandomGirl(sRandomGirl* girl)
{
	girl->m_Next = 0;
	if (m_RandomGirls)	m_LastRandomGirls->m_Next = girl;
	else				m_RandomGirls = girl;
	m_LastRandomGirls = girl;
	m_NumRandomGirls++;
}

void cGirls::AddGirl(sGirl* girl)
{
	girl->m_Prev = girl->m_Next = 0;
	if (m_Parent)
	{
		girl->m_Prev = m_Last;
		m_Last->m_Next = girl;
		m_Last = girl;
	}
	else	m_Last = m_Parent = girl;
	m_NumGirls++;
}

void cGirls::RemoveGirl(sGirl* girl, bool deleteGirl)
{
	if (m_Parent == 0)	return;
	bool match = false;
	sGirl* currGirl = m_Parent;
	while (currGirl)
	{
		if (currGirl == girl)
		{
			match = true;
			break;
		}
		currGirl = currGirl->m_Next;
	}
	if (match)
	{
		if (deleteGirl)
		{
			if (girl->m_Prev)		girl->m_Prev->m_Next = girl->m_Next;
			if (girl->m_Next)		girl->m_Next->m_Prev = girl->m_Prev;
			if (girl == m_Parent)	m_Parent = girl->m_Next;
			if (girl == m_Last)		m_Last = girl->m_Prev;
			girl->m_Next = girl->m_Prev = 0;
			delete girl;
			girl = 0;
		}
		else
		{
			if (girl->m_Prev)		girl->m_Prev->m_Next = girl->m_Next;
			if (girl->m_Next)		girl->m_Next->m_Prev = girl->m_Prev;
			if (girl == m_Parent)	m_Parent = girl->m_Next;
			if (girl == m_Last)		m_Last = girl->m_Prev;
			girl->m_Next = girl->m_Prev = 0;
		}
		m_NumGirls--;
	}
}

void cGirls::AddTiredness(sGirl* girl)
{
	if (g_Girls.HasTrait(girl, "Incorporeal")) // Sanity check
	{
		g_Girls.SetStat(girl, STAT_TIREDNESS, 0);	return;
	}
	int tiredness = 10;
	if (g_Girls.GetStat(girl, STAT_CONSTITUTION) > 0) 
		tiredness -= (g_Girls.GetStat(girl, STAT_CONSTITUTION)) / 10;
	if (tiredness <= 0)	tiredness = 0;
	g_Girls.UpdateStat(girl, STAT_TIREDNESS, tiredness,false);
	if (g_Girls.GetStat(girl, STAT_TIREDNESS) == 100)
	{
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, -1, false);
		g_Girls.UpdateStat(girl, STAT_HEALTH, -1, false);
	}
}

// ----- Get

int cGirls::GetSlaveGirl(int from)
{
	sGirl* current = m_Parent;
	int num = 0; int girlnum = 0;
	bool found = false;
	while (current)
	{
		if (current->m_States&(1 << STATUS_SLAVE))
		{
			if (num == from)
			{
				found = true;
				break;
			}
			else	num++;
		}
		girlnum++;
		current = current->m_Next;
	}
	if (found == false)	return -1;
	return girlnum;
}

vector<sGirl *>  cGirls::get_girls(GirlPredicate* pred)
{
	sGirl *girl;
	vector<sGirl *> v;
	for (girl = m_Parent; girl; girl = girl->m_Next)
	{
		if (pred->test(girl))	v.push_back(girl);
	}
	return v;
}

string cGirls::GetGirlMood(sGirl* girl)
{
	string ret = girl->m_Realname;
	int variable = 0;

	int HateLove = GetStat(girl, STAT_PCLOVE) - GetStat(girl, STAT_PCHATE);
	ret += gettext(" feels the player ");

		 if (HateLove <= -80)	ret += gettext("should die ");
	else if (HateLove <= -60)	ret += gettext("is better off dead ");
	else if (HateLove <= -40)	ret += gettext("is mean ");
	else if (HateLove <= -20)	ret += gettext("isn't nice ");
	else if (HateLove <=   0)	ret += gettext("is annoying ");
	else if (HateLove <=  20)	ret += gettext("is ok ");
	else if (HateLove <=  40)	ret += gettext("is easy going ");
	else if (HateLove <=  60)	ret += gettext("is good ");
	else if (HateLove <=  80)	ret += gettext("is attractive ");
	else 						ret += gettext("is her true love ");

	if (GetStat(girl, STAT_PCFEAR) > 20)
	{
		if (HateLove > 0)	ret += gettext("but she is also ");
		else				ret += gettext("and she is ");
		if (GetStat(girl, STAT_PCFEAR) < 40)		ret += gettext("afraid of him.");
		else if (GetStat(girl, STAT_PCFEAR) < 60)	ret += gettext("fearful of him.");
		else if (GetStat(girl, STAT_PCFEAR) < 80)	ret += gettext("afraid he will hurt her.");
		else										ret += gettext("afraid he will kill her.");
	}
	else	ret += gettext("and he isn't scary.");

	variable = GetStat(girl, STAT_HAPPINESS);
	ret += gettext("\nShe is ");
	if (variable > 90)		ret += gettext("happy.");
	else if (variable > 80)	ret += gettext("joyful.");
	else if (variable > 60)	ret += gettext("reasonably happy.");
	else if (variable > 40)	ret += gettext("unhappy.");
	else		ret += gettext("showing signs of depression.");

	return ret;
}

string cGirls::GetDetailsString(sGirl* girl, bool purchase)
{
	if (girl == 0)	return string("");
	cConfig cfg; cTariff tariff;
	cFont check; int w, h, size = 0;
	check.LoadFont(cfg.fonts.normal(), cfg.fonts.detailfontsize());
	stringstream ss;
	string data = "";
	char buffer[100];
	string sper = ""; if (cfg.fonts.showpercent()) sper = " %";

	int skillnum[] = { SKILL_MAGIC, SKILL_COMBAT, SKILL_SERVICE, SKILL_MEDICINE, SKILL_PERFORMANCE, SKILL_CRAFTING, SKILL_HERBALISM, SKILL_FARMING, SKILL_BREWING, SKILL_ANIMALHANDLING, SKILL_ANAL, SKILL_BDSM, SKILL_NORMALSEX, SKILL_BEASTIALITY, SKILL_GROUP, SKILL_LESBIAN, SKILL_ORALSEX, SKILL_TITTYSEX, SKILL_HANDJOB, SKILL_STRIP };
	string basestr[] = { "Age : ", "Rebelliousness : ", "Looks : ", "Constitution : ", "Health : ", "Happiness : ", "Tiredness : ", "Gold : ", "Worth : " };
	string skillstr[] = { "Magic Ability : ", "Combat Ability : ", "Service Skills : ", "Medicine Skill : ", "Performance Skill : ", "Crafting Skill : ", "Herbalism Skill : ", "Farming Skill : ", "Brewing Skill : ", "Animal Handling : ", "Anal Sex : ", "BDSM Sex : ", "Normal Sex : ", "Bestiality Sex : ", "Group Sex : ", "Lesbian Sex : ", "Oral Sex : ", "Titty Sex : ", "Hand Job : ", "Stripping Sex : " };

	if (cfg.fonts.normal() == "segoeui.ttf" && cfg.fonts.detailfontsize() == 9) // `J` if already set to my default
	{
		string basesegoeuistr[] = { "Age :                                   ", "Rebelliousness :         ", "Looks :                              ", "Constitution :               ", "Health :                            ", "Happiness :                    ", "Tiredness :                     ", "Gold :                                ", "Worth :                             " };
		for (int i = 0; i < 9; i++) basestr[i] = basesegoeuistr[i];
		string skillsegoeuistr[] = { "Magic Ability :              ", "Combat Ability :           ", "Service Skills :              ", "Medicine Skill :            ", "Performance Skill :    ", "Crafting Skill :              ", "Herbalism Skill :         ", "Farming Skill :             ", "Brewing Skill :               ", "Animal Handling :    ", "Anal Sex :                        ", "BDSM Sex :                      ", "Normal Sex :                 ", "Bestiality Sex :              ", "Group Sex :                     ", "Lesbian Sex :                 ", "Oral Sex :                         ", "Titty Sex :                         ", "Hand Job :                      ", "Stripping Sex :               " };
		for (int i = 0; i < 20; i++) skillstr[i] = skillsegoeuistr[i];
		size = 90;
	}
	else		// `J` otherwise try to align the numbers
	{
		// get the widest
		for (int i = 0; i < 9; i++) { check.GetSize(basestr[i], w, h); if (w > size) size = w; }
		for (int i = 0; i < 20; i++) { check.GetSize(skillstr[i], w, h); if (w > size) size = w; }
		size += 5; // add a little padding
		// then add extra spaces until it is longer that the widest
		for (int i = 0; i < 9; i++)
		{
			check.GetSize(basestr[i], w, h);
			while (w < size)
			{
				basestr[i] += " ";
				check.GetSize(basestr[i], w, h);
			}
		}
		for (int i = 0; i < 20; i++)
		{
			check.GetSize(skillstr[i], w, h);
			while (w < size)
			{
				skillstr[i] += " ";
				check.GetSize(skillstr[i], w, h);
			}
		}
	}

	string levelstr[] = { "Level : ", "Exp : ", "Exp to level : ", "Needs : " };

	int level = GetStat(girl, STAT_LEVEL);
	int exp = GetStat(girl, STAT_EXP);
	int exptolv = min(32000, (level + 1) * 125);
	int expneed = exptolv - exp;

	check.GetSize(levelstr[0], w, h);
	if (!purchase)
	{
		while (w < size - 5)
		{
			levelstr[0] += " ";
			stringstream levelnumstr; levelnumstr << level;
			check.GetSize(levelstr[0] + levelnumstr.str(), w, h);
		}
	}
	else
	{
		while (w < size)
		{
			levelstr[0] += " ";
			check.GetSize(levelstr[0], w, h);
		}

	}

	// display looks
	ss << basestr[2] << (GetStat(girl, STAT_BEAUTY) + GetStat(girl, STAT_CHARISMA)) / 2 << sper;

	// display level and exp
	ss << "\n" << levelstr[0] << GetStat(girl, STAT_LEVEL);
	if (!purchase)
	{
		check.GetSize(levelstr[2], w, h);
		while (w < size - 5)
		{
			levelstr[2] += " ";
			stringstream levelnumstr; levelnumstr << exptolv;
			check.GetSize(levelstr[2] + levelnumstr.str(), w, h);
		}
		ss << "  |  " << levelstr[1] << exp;
		ss << "\n" << levelstr[2] << exptolv;
		ss << "  |  " << levelstr[3] << expneed;
	}

	// display Age
	ss << "\n" << basestr[0]; if (GetStat(girl, STAT_AGE) == 100) ss << "Unknown"; else ss << GetStat(girl, STAT_AGE);
	// display rebel
	ss << "\n" << basestr[1] << girl->rebel();
	// display Constitution
	ss << "\n" << basestr[3] << GetStat(girl, STAT_CONSTITUTION) << sper;

	// display HHT and money
	if (!purchase)
	{
		ss << "\n" << basestr[4] << GetStat(girl, STAT_HEALTH) << sper;
		ss << "\n" << basestr[5] << GetStat(girl, STAT_HAPPINESS) << sper;
		ss << "\n" << basestr[6] << GetStat(girl, STAT_TIREDNESS) << sper;
		ss << "\n" << basestr[7];
		if (g_Gangs.GetGangOnMission(MISS_SPYGIRLS))
		{
			ss << girl->m_Money;
		}
		else
		{
			ss << "Unknown";
		}
	}
	int cost = int(tariff.slave_price(girl, purchase));
	g_LogFile.ss() << gettext("slave ") << (purchase ? gettext("buy") : gettext("sell")) << gettext("price = ") << cost;
	g_LogFile.ssend();
	ss << "\n" << basestr[8] << cost << " Gold";
	CalculateAskPrice(girl, false);
	cost = g_Girls.GetStat(girl, STAT_ASKPRICE);
	ss << "\nAvg Pay per Customer : " << cost << " gold\n";
	data += ss.str();

	// display status
	if (girl->m_States&(1 << STATUS_SLAVE))			data += gettext("Is Branded a Slave\n");
	else if (cfg.debug.log_extradetails())			data += gettext("( She Is Not a Slave )\n");
	else data += gettext("\n");

	if (g_Girls.CheckVirginity(girl))				data += gettext("She is a Virgin\n");
	else if (cfg.debug.log_extradetails())			data += gettext("( She Is Not a Virgin )\n");
	else data += gettext("\n");

	if (!purchase)
	{
		int to_go = cfg.pregnancy.weeks_pregnant() - girl->m_WeeksPreg;
		if (girl->m_States&(1 << STATUS_PREGNANT))
		{
			_itoa(to_go, buffer, 10);
			data += gettext("Is pregnant, due: ");
			data += buffer;
			data += gettext(" weeks\n");
		}
		else if (girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
		{
			_itoa(to_go, buffer, 10);
			data += gettext("Is pregnant with your child, due: ");
			data += buffer;
			data += gettext(" weeks\n");
		}
		else if (girl->m_States&(1 << STATUS_INSEMINATED))
		{
			_itoa(to_go, buffer, 10);
			data += gettext("Is inseminated, due: ");
			data += buffer;
			data += gettext(" weeks\n");
		}
		else if (girl->m_PregCooldown != 0)
		{
			_itoa(((int)girl->m_PregCooldown), buffer, 10);
			data += gettext("Cannot get pregnant for: ");
			data += buffer;
			data += gettext(" weeks\n");
		}
		else if (cfg.debug.log_extradetails())			data += gettext("( She Is not Pregnant )\n");
		else data += gettext("\n");

		if (girl->m_States&(1 << STATUS_HAS_DAUGHTER))	data += gettext("Has Daughter\n");
		else if (cfg.debug.log_extradetails())			data += gettext("( She Has No Daughters )\n");
		else											data += gettext("\n");
		if (girl->m_States&(1 << STATUS_HAS_SON))		data += gettext("Has Son\n");
		else if (cfg.debug.log_extradetails())			data += gettext("( She Has No Sons )\n");
		else											data += gettext("\n");
	}

	if (girl->is_addict() && !girl->has_disease())		data += gettext("Has an addiciton\n");
	else if (!girl->is_addict() && girl->has_disease())	data += gettext("Has a disease\n");
	else if (girl->is_addict() && girl->has_disease())	data += gettext("Has an addiciton and a disease\n");
	else if (cfg.debug.log_extradetails())				data += gettext("( She Has No Addicitons or Diseases )\n");
	else												data += gettext("\n");

	if (!purchase)
	{
		if (girl->m_States&(1 << STATUS_BADLY_POISONED))data += gettext("Is badly poisoned\n");
		else if (girl->m_States&(1 << STATUS_POISONED))	data += gettext("Is poisoned\n");
		else if (cfg.debug.log_extradetails())			data += gettext("( She Is Not Poisoned )\n");
		else											data += gettext("\n");
	}

	// display Skills
	ss.str("");
	ss << "\n\nSKILLS";
	if (cfg.debug.log_extradetails() && !purchase) ss << "           ( base + mod + temp )";

	for (int i = 0; i < 20; i++)
	{
		if (i == 10)
		{
			ss << "\n\nSEX SKILLS";
			if (cfg.debug.log_extradetails() && !purchase) ss << "           ( base + mod + temp )";
		}
		ss << "\n" << skillstr[i] << GetSkill(girl, skillnum[i]) << sper;
		if (cfg.debug.log_extradetails() && !purchase) ss << "    ( " << girl->m_Skills[skillnum[i]] << " + " << girl->m_SkillMods[skillnum[i]] << " + " << girl->m_TempSkills[skillnum[i]] << " )";
	}
	data += ss.str();
	return data;
}

string cGirls::GetMoreDetailsString(sGirl* girl, bool purchase)
{
	cConfig cfg;
	if (girl == 0)		return string("");
	string data = "";
	string sper = ""; if (cfg.fonts.showpercent()) sper = " %";
	stringstream ss;
	cFont check; int w, h, size = 0;
	check.LoadFont(cfg.fonts.normal(), cfg.fonts.detailfontsize());

	ss << "STATS";
	if (cfg.debug.log_extradetails() && !purchase) ss << "           ( base + mod + temp )";
	int statnum[] = { STAT_CHARISMA, STAT_BEAUTY, STAT_LIBIDO, STAT_MANA, STAT_INTELLIGENCE, STAT_CONFIDENCE, STAT_OBEDIENCE, STAT_SPIRIT, STAT_AGILITY, STAT_FAME, STAT_PCFEAR, STAT_PCLOVE, STAT_PCHATE };
	string statstr[] = { "Charisma : ", "Beauty : ", "Libido : ", "Mana : ", "Intelligence : ", "Confidence : ", "Obedience : ", "Spirit : ", "Agility : ", "Fame : ", "PCFear : ", "PCLove : ", "PCHate : " };

	if (cfg.fonts.normal() == "segoeui.ttf" && cfg.fonts.detailfontsize() == 9) // `J` if already set to my default
	{
		string statsegoeuistr[] = { "Charisma :          ", "Beauty :                  ", "Libido :                   ", "Mana :                   ", "Intelligence :      ", "Confidence :       ", "Obedience :         ", "Spirit :                     ", "Agility :                  ", "Fame :                    ", "PCFear :                 ", "PCLove :                 ", "PCHate :                 " };
		for (int i = 0; i < 13; i++) statstr[i] = statsegoeuistr[i];
		size = 70;
	}
	else		// `J` otherwise try to align the numbers
	{
		// get the widest
		for (int i = 0; i < 13; i++) { check.GetSize(statstr[i], w, h); if (w > size) size = w; }
		size += 10; // add some padding
		// then add extra spaces to the statstr until it is longer that the widest
		for (int i = 0; i < 13; i++)
		{
			check.GetSize(statstr[i], w, h);
			while (w < size)
			{
				statstr[i] += " ";
				check.GetSize(statstr[i], w, h);
			}
		}
	}
	int show = (cfg.debug.log_extradetails() && !purchase) ? 13 : 10;
	for (int i = 0; i < show; i++)
	{
		ss << "\n" << statstr[i] << GetStat(girl, statnum[i]) << sper;
		if (cfg.debug.log_extradetails() && !purchase) ss << "    ( " << girl->m_Stats[statnum[i]] << " + " << girl->m_StatMods[statnum[i]] << " + " << girl->m_TempStats[statnum[i]] << " )";
	}
	if (!purchase)
	{
		ss << "\n\nAccommodation: ";
		if (girl->m_AccLevel == 0) ss << "Very Poor";
		if (girl->m_AccLevel == 1) ss << "Adequate";
		if (girl->m_AccLevel == 2) ss << "Nice";
		if (girl->m_AccLevel == 3) ss << "Good";
		if (girl->m_AccLevel == 4) ss << "Wonderful";
		if (girl->m_AccLevel == 5) ss << "High Class";
	}

	ss << "\n\nFETISH CATEGORIES\n";
	if (CheckGirlType(girl, FETISH_BIGBOOBS))		ss << " |Big Boobs| ";
	if (CheckGirlType(girl, FETISH_CUTEGIRLS))		ss << " |Cute Girl| ";
	if (CheckGirlType(girl, FETISH_DANGEROUSGIRLS))	ss << " |Dangerous| ";
	if (CheckGirlType(girl, FETISH_COOLGIRLS))		ss << " |Cool| ";
	if (CheckGirlType(girl, FETISH_NERDYGIRLS))		ss << " |Nerd| ";
	if (CheckGirlType(girl, FETISH_NONHUMAN))		ss << " |Non or part human| ";
	if (CheckGirlType(girl, FETISH_LOLITA))			ss << " |Lolita| ";
	if (CheckGirlType(girl, FETISH_ELEGANT))		ss << " |Elegant| ";
	if (CheckGirlType(girl, FETISH_SEXY))			ss << " |Sexy| ";
	if (CheckGirlType(girl, FETISH_FIGURE))			ss << " |Nice Figure| ";
	if (CheckGirlType(girl, FETISH_ARSE))			ss << " |Nice Arse| ";
	if (CheckGirlType(girl, FETISH_SMALLBOOBS))		ss << " |Small Boobs| ";
	if (CheckGirlType(girl, FETISH_FREAKYGIRLS))	ss << " |Freaky| ";

	data += ss.str();

	if (!purchase)
	{
		data += gettext("\n\nJOB PREFERENCES\n");
		// `J` When modifying Action types, search for "J-Change-Action-Types"  :  found in >> GetMoreDetailsString
		string jobs[] = {
			gettext("combat"),
			gettext("working as a whore"),
			gettext("working as an Escort"),
			gettext("cleaning"),
			gettext("acting as a matron"),
			gettext("working in the bar"),
			gettext("working in the gambling hall"),
			gettext("producing movies"),
			gettext("providing security"),
			gettext("doing advertising"),
			gettext("torturing people"),
			gettext("caring for beasts"),
			gettext("working as a doctor"),
			gettext("producing movies"),
			gettext("providing customer service"),
			gettext("working in the centre"),
			gettext("working in the club"),
			gettext("being in your harem"),
			gettext("being a recruiter"),
			gettext("working as a nurse"),
			gettext("fixing things"),
			gettext("counseling people"),
			gettext("performing music"),
			gettext("striping"),
			gettext("having her breasts milked"),
			gettext("working as a massusse"),
			gettext("working on the farm"),
			gettext("doing miscellaneous tasks")  // general
		};
		string base = gettext("She");
		string text;
		unsigned char count = 0;
		for (int i = 0; i < NUM_ACTIONTYPES; ++i)
		{
			if (jobs[i] == "")			continue;
			if (girl->m_Enjoyment[i] < -70)			{ text = gettext(" hates "); }
			else if (girl->m_Enjoyment[i] < -50)	{ text = gettext(" really dislikes "); }
			else if (girl->m_Enjoyment[i] < -30)	{ text = gettext(" dislikes "); }
			else if (girl->m_Enjoyment[i] < -20)	{ text = gettext(" doesn't particularly enjoy "); }
			else if (girl->m_Enjoyment[i] < 15)		{ continue; } // if she's indifferent, why specify it? Let's instead skip it.
			else if (girl->m_Enjoyment[i] < 30)		{ text = gettext(" is happy enough with "); }
			else if (girl->m_Enjoyment[i] < 50)		{ text = gettext(" likes "); }
			else if (girl->m_Enjoyment[i] < 70)		{ text = gettext(" really enjoys "); }
			else									{ text = gettext(" loves "); }
			data += base + text + jobs[i] + gettext(".\n");
			count++;
		}
		if (count > 0)	data += gettext("\nShe is indifferent to all other tasks.\n\n");
		else			data += gettext("At the moment, she is indifferent to all tasks.\n\n");
	}
	return data;
}

string cGirls::GetThirdDetailsString(sGirl* girl)
{
	// `J` zzzzzz I will come back to this when I start editing jobs

	if (girl == 0)		return string("");
	string data = "";
	// `J` instead of repeatedly calling the girl, call her once and store her stat
	int jr_cha = GetStat(girl, STAT_CHARISMA);
	int jr_bea = GetStat(girl, STAT_BEAUTY);
	int jr_cns = GetStat(girl, STAT_CONSTITUTION);
	int jr_int = GetStat(girl, STAT_INTELLIGENCE);
	int jr_agi = GetStat(girl, STAT_AGILITY);
	int jr_cnf = GetStat(girl, STAT_CONFIDENCE);
	int jr_lib = GetStat(girl, STAT_LIBIDO);
	int jr_man = GetStat(girl, STAT_MANA);
	int jr_obe = GetStat(girl, STAT_OBEDIENCE);
	int jr_spi = GetStat(girl, STAT_SPIRIT);
	int jr_lev = GetStat(girl, STAT_LEVEL);
	int jr_fam = GetStat(girl, STAT_FAME);

	int jr_mag = GetSkill(girl, SKILL_MAGIC);
	int jr_cmb = GetSkill(girl, SKILL_COMBAT);
	int jr_ser = GetSkill(girl, SKILL_SERVICE);
	
	int jr_bst = GetSkill(girl, SKILL_BEASTIALITY);
	int jr_stp = GetSkill(girl, SKILL_STRIP);
	int jr_med = GetSkill(girl, SKILL_MEDICINE);
	int jr_per = GetSkill(girl, SKILL_PERFORMANCE);
	int jr_cra = GetSkill(girl, SKILL_CRAFTING);
	int jr_her = GetSkill(girl, SKILL_HERBALISM);
	int jr_far = GetSkill(girl, SKILL_FARMING);
	int jr_bre = GetSkill(girl, SKILL_BREWING);
	int jr_anh = GetSkill(girl, SKILL_ANIMALHANDLING);
	

	//Job rating system  ///CRAZY

	int HateLove = g_Girls.GetStat(girl, STAT_PCLOVE) - g_Girls.GetStat(girl, STAT_PCHATE);
	int jr_slave = 0; if (girl->is_slave()) jr_slave = -1000;
	int combat = (jr_agi / 2 + jr_cns / 2 + jr_mag / 2 + jr_cmb);
	if (g_Girls.HasTrait(girl, "Incorporeal")) combat += 100;

	// Brothel Jobs
	int security = ((jr_mag * 2 + jr_cmb * 2 + jr_ser) / 3); // `J` estimate - needs work
	int advertising = (jr_per / 6 + jr_ser / 6 + jr_cha / 6 + jr_bea / 10 + jr_int / 6 + jr_cnf / 10 + jr_fam / 10);
	int custservice = ((jr_cha + jr_bea + jr_per) / 3 + (jr_cnf + jr_spi) / 3 + jr_ser); // `J` estimate - needs work
	int matron = ((jr_cha + jr_cnf + jr_spi) / 3 + (jr_ser + jr_int + jr_med) / 3 + jr_lev + jr_slave); // `J` estimate - needs work
	int catacombs = combat;
	int barmaid = (jr_int / 2 + jr_per / 2 + jr_ser);
	int barwait = (jr_int / 2 + jr_agi / 2 + jr_ser);
	int barsing = (jr_cnf + jr_per);
	int barpiano = (jr_cnf / 2 + jr_int / 2 + jr_per);
	int dealer = (jr_int / 2 + jr_agi / 2 + jr_ser / 2 + jr_per / 2);
	int entertainer = ((jr_cha + jr_bea + jr_cnf) / 3 + jr_per);
	int xxx = ((jr_cha + jr_bea + jr_cnf) / 3 + jr_stp / 2 + jr_per / 2);
	int clubbar = ((jr_cha + jr_bea + jr_per) / 3 + jr_ser);
	int clubwait = ((jr_cha + jr_bea + jr_per) / 3 + jr_ser);
	int clubstrip = (jr_cha / 2 + jr_bea / 2 + jr_per / 2 + jr_stp / 2);
	int massusse = (jr_cha / 2 + jr_bea / 2 + jr_ser / 2 + jr_med / 2);
	int brothelstrip = (jr_cha / 4 + jr_bea / 4 + jr_stp / 2 + jr_per);
	int peep = (jr_cha / 2 + jr_bea / 2 + jr_stp / 2 + jr_per / 2);
	int beastcare = (jr_int / 2 + jr_ser / 2 + jr_bst);
	// Studio Jobs
	int director = (((jr_int - 50) / 10 + (jr_spi - 50) / 10 + jr_ser / 10) / 3 + jr_fam / 10 + jr_lev + jr_slave);
	int promoter = (jr_ser / 3 + jr_cha / 6 + jr_bea / 10 + jr_int / 6 + jr_cnf / 10 + jr_fam / 4 + jr_lev / 2);
	int cameramage = (((jr_int - 50) / 10 + (jr_spi - 50) / 10 + jr_ser / 10) / 3 + jr_fam / 20 + jr_lev);
	int crystalpurifier = (((jr_int - 50) / 10 + (jr_spi - 50) / 10 + jr_ser / 10) / 3 + jr_fam / 20 + jr_lev);
	// Arena Jobs
	int cityguard = (jr_agi / 2 + jr_cmb / 2);
	int doctore = matron;
	int cagematch = combat;
	int fightbeast = combat;
	// Centre Jobs
	int centremanager = matron;
	int drugcounselor = matron;
	int comunityservice = ((jr_int / 2) + (jr_cha / 2) + jr_ser);
	int feedpoor = ((jr_int / 2) + (jr_cha / 2) + jr_ser);
	// Clinic Jobs
	int chairman = matron;
	int doctor = (jr_int + jr_med + jr_lev / 5 + jr_slave); // `J` needs work
	int nurse = (jr_cha / 2 + jr_int / 2 + jr_med + jr_lev / 5);
	int mechanic = (jr_ser / 2 + jr_med / 2 + jr_int);
	// House Jobs
	int headgirl = matron;
	int recruiter = (HateLove + jr_cha + jr_slave);
	// Farm Jobs
	int farmmanger = matron;
	int veterinarian = (jr_med + jr_anh);
	int marketer = (jr_int + jr_far);
	int researcher = ((jr_int / 2) + (jr_her / 2) + jr_bre);
	int farmer = (jr_int + jr_far);
	int gardener = ((jr_int / 2) + (jr_her / 2) + jr_far);
	int sheapherd = ((jr_int / 2) + (jr_far / 2) + jr_anh);
	int rancher = ((jr_bst / 2) + (jr_far / 2) + jr_anh);
	int beastcap = combat;
	int catarancher = ((jr_int / 2) + (jr_far / 2) + jr_anh);
	int milker = (jr_int + jr_anh);
	int butcher = (jr_int + jr_anh);
	int baker = (jr_int + jr_her);
	int brewer = (jr_int + jr_bre);
	int makeitem = (jr_cra + jr_ser);
	int makepot = ((jr_int / 2) + (jr_her / 2) + (jr_bre / 2) + (jr_cra / 2));
	int milk = 0;
	{
		if (girl->is_pregnant()) milk += 100; // preg rating | non-preg rating
		//zzzzzz boobs
		if (g_Girls.HasTrait(girl, "Abnormally Large Boobs")) milk += 150; // S | B
		else if (g_Girls.HasTrait(girl, "Big Boobs")) milk += 100; // A | C
		else if (g_Girls.HasTrait(girl, "Small Boobs")) milk += 25; // C | E
		else milk += 75; // B | D
	}

	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> GetThirdDetailsString > trait adjustments for jobs
	// Traits in alphabetical order
	//zzzzzz boobs
	if (g_Girls.GetStat(girl, STAT_FAME) > 85)
	{
		clubstrip += 10;
		brothelstrip += 10;
	}

	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
	{
		barwait -= 20;
		clubwait -= 20;
	}
	if (g_Girls.HasTrait(girl, "Adventurer"))
	{
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Aggressive"))
	{
		barmaid -= 20;
		barpiano -= 20;
		barsing -= 10;
		barwait -= 20;
		brothelstrip -= 20;
		clubbar -= 20;
		clubstrip -= 20;
		clubwait -= 20;
		comunityservice -= 20;
		dealer -= 20;
		doctor -= 20;
		entertainer -= 20;
		feedpoor -= 20;
		massusse -= 20;
		mechanic -= 10;
		nurse -= 20;
		peep -= 20;
		recruiter -= 20;
		security += 5;
		xxx -= 20;
		researcher -= 20;
		marketer -= 20;
		farmer -= 20;
		gardener -= 20;
		sheapherd -= 20;
		rancher -= 20;
		catarancher -= 20;
		milker -= 20;
		butcher -= 20;
		baker -= 20;
		brewer -= 20;
		makepot -= 20;
	}
	if (g_Girls.HasTrait(girl, "Alcoholic"))
	{
		barmaid -= 40;
		clubbar -= 40;
		security -= 50;
	}
	if (g_Girls.HasTrait(girl, "Assassin"))
	{
		security += 50;
	}
	if (g_Girls.HasTrait(girl, "Broken Will"))
	{
		barsing -= 50;
		barpiano -= 50;
		entertainer -= 50;
		recruiter -= 50;
		security -= 50;
	}
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
	{
		advertising += 10;
		barmaid += 15;
		barpiano += 10;
		barsing += 15;
		barwait += 15;
		brothelstrip += 15;
		clubbar += 20;
		clubstrip += 10;
		clubwait += 10;
		comunityservice += 20;
		dealer += 5;
		doctor += 20;
		entertainer += 15;
		feedpoor += 20;
		massusse += 15;
		mechanic += 5;
		nurse += 20;
		peep += 15;
		promoter += 10;
		recruiter += 20;
		xxx += 10;
		marketer += 15;
	}
	if (g_Girls.HasTrait(girl, "Charming"))
	{
		advertising += 10;
		barmaid += 15;
		barpiano += 5;
		barsing += 5;
		barwait += 20;
		brothelstrip += 10;
		clubbar += 15;
		clubstrip += 10;
		clubwait += 15;
		comunityservice += 15;
		dealer += 10;
		doctor += 15;
		entertainer += 15;
		feedpoor += 15;
		massusse += 10;
		mechanic += 5;
		nurse += 15;
		peep += 10;
		promoter += 10;
		recruiter += 10;
		security += 5;
		xxx += 5;
		marketer += 15;
	}
	if (g_Girls.HasTrait(girl, "Clumsy"))
	{
		advertising -= 5;
		barmaid -= 20;
		barsing -= 10;
		barwait -= 20;
		brothelstrip -= 20;
		clubbar -= 20;
		clubstrip -= 20;
		clubwait -= 20;
		comunityservice -= 20;
		dealer -= 10;
		doctor -= 20;
		entertainer -= 10;
		feedpoor -= 20;
		massusse -= 20;
		mechanic -= 20;
		nurse -= 20;
		peep -= 20;
		promoter -= 5;
		recruiter -= 5;
		security -= 5;
		xxx -= 10;
		researcher -= 20;
		marketer -= 20;
		farmer -= 20;
		gardener -= 20;
		sheapherd -= 20;
		rancher -= 20;
		catarancher -= 20;
		milker -= 20;
		butcher -= 20;
		baker -= 20;
		brewer -= 20;
		makepot -= 20;
	}
	if (g_Girls.HasTrait(girl, "Construct"))
	{
		barsing -= 20;
		mechanic += 10;
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Cool Person"))
	{
		advertising += 10;
		barmaid += 10;
		barpiano += 5;
		barsing += 5;
		barwait += 10;
		brothelstrip += 10;
		clubbar += 10;
		clubstrip += 10;
		clubwait += 10;
		comunityservice += 10;
		dealer += 5;
		doctor += 10;
		entertainer += 5;
		feedpoor += 10;
		massusse += 10;
		mechanic += 10;
		nurse += 10;
		peep += 10;
		promoter += 10;
		recruiter += 10;
		security += 5;
		xxx += 5;
		marketer += 10;
	}
	if (g_Girls.HasTrait(girl, "Cute"))
	{
		barmaid += 5;
		barpiano += 5;
		barsing += 5;
		barwait += 5;
		brothelstrip += 5;
		clubbar += 5;
		clubstrip += 5;
		clubwait += 5;
		comunityservice += 5;
		dealer += 5;
		doctor += 5;
		entertainer += 5;
		feedpoor += 5;
		massusse += 5;
		nurse += 5;
		peep += 5;
		xxx += 5;
		marketer += 5;
	}
	if (g_Girls.HasTrait(girl, "Demon"))  //
	{
		security += 10;
	}
	if (g_Girls.HasTrait(girl, "Dependant"))  //needs others to do the job
	{
		barmaid -= 50;
		barpiano -= 50;
		barsing -= 50;
		barwait -= 50;
		brothelstrip -= 50;
		clubbar -= 50;
		clubstrip -= 50;
		clubwait -= 50;
		comunityservice -= 50;
		dealer -= 50;
		doctor -= 50;
		entertainer -= 50;
		feedpoor -= 50;
		massusse -= 50;
		mechanic -= 40;
		nurse -= 50;
		peep -= 50;
		recruiter -= 50;
		security -= 5;
		xxx -= 50;
		researcher -= 50;
		marketer -= 50;
		farmer -= 50;
		gardener -= 50;
		sheapherd -= 50;
		rancher -= 50;
		catarancher -= 50;
		milker -= 50;
		butcher -= 50;
		baker -= 50;
		brewer -= 50;
		makepot -= 50;
	}
	if (g_Girls.HasTrait(girl, "Elegant"))
	{
		barsing += 5;
		barpiano += 15;
		mechanic -= 5;
	}
	if (g_Girls.HasTrait(girl, "Fearless"))
	{
		barsing += 5;
		entertainer += 5;
		peep += 10;
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Fleet of Foot"))
	{
		barwait += 5;
		clubwait += 5;
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Great Arse"))
	{
		brothelstrip += 5;
		clubbar += 5;
		clubstrip += 5;
		clubwait += 5;
		entertainer += 5;
		massusse += 5;
		peep += 5;
		xxx += 5;
	}
	if (g_Girls.HasTrait(girl, "Great Figure"))
	{
		brothelstrip += 5;
		clubbar += 5;
		clubstrip += 5;
		clubwait += 5;
		entertainer += 5;
		massusse += 5;
		peep += 5;
		xxx += 5;
	}
	if (g_Girls.HasTrait(girl, "Half-Construct"))
	{
		mechanic += 5;
	}
	if (g_Girls.HasTrait(girl, "Horrific Scars"))
	{
		brothelstrip -= 20;
		clubstrip -= 20;
		peep -= 20;
	}
	if (g_Girls.HasTrait(girl, "Iron Will"))
	{
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Lolita"))
	{
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Long Legs"))
	{
		clubstrip += 10;
		brothelstrip += 10;
	}
	if (g_Girls.HasTrait(girl, "Malformed"))
	{
		advertising -= 20;
		promoter -= 20;
	}
	if (g_Girls.HasTrait(girl, "Meek"))
	{
		barmaid -= 20;
		barpiano -= 20;
		barsing -= 20;
		barwait -= 20;
		brothelstrip -= 20;
		clubbar -= 20;
		clubstrip -= 20;
		clubwait -= 20;
		comunityservice -= 20;
		dealer -= 20;
		doctor -= 20;
		entertainer -= 20;
		feedpoor -= 20;
		massusse -= 20;
		mechanic -= 20;
		nurse -= 20;
		peep -= 20;
		recruiter -= 20;
		security -= 5;
		xxx -= 20;
		researcher -= 20;
		marketer -= 20;
		farmer -= 20;
		gardener -= 20;
		sheapherd -= 20;
		rancher -= 20;
		catarancher -= 20;
		milker -= 20;
		butcher -= 20;
		baker -= 20;
		brewer -= 20;
		makepot -= 20;
	}
	if (g_Girls.HasTrait(girl, "Merciless"))
	{
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Mind Fucked"))
	{
		security -= 50;
	}
	if (g_Girls.HasTrait(girl, "Mixologist"))
	{
		barmaid += 25;
		clubbar += 25;
	}
	if (g_Girls.HasTrait(girl, "Nerd"))
	{
		doctor += 30;
		nurse += 15;
		mechanic += 15;
		security -= 5;
		makepot += 10;
	}
	if (g_Girls.HasTrait(girl, "Nervous"))
	{
		advertising -= 5;
		barmaid -= 30;
		barpiano -= 30;
		barsing -= 30;
		barwait -= 30;
		brothelstrip -= 30;
		clubbar -= 30;
		clubstrip -= 30;
		clubwait -= 30;
		comunityservice -= 30;
		dealer -= 30;
		doctor -= 50;
		entertainer -= 30;
		feedpoor -= 30;
		massusse -= 30;
		mechanic -= 20;
		nurse -= 30;
		peep -= 30;
		promoter -= 5;
		recruiter -= 30;
		xxx -= 30;
		researcher -= 30;
		marketer -= 30;
		farmer -= 30;
		gardener -= 30;
		sheapherd -= 30;
		rancher -= 30;
		catarancher -= 30;
		milker -= 30;
		butcher -= 30;
		baker -= 30;
		brewer -= 30;
		makepot -= 30;
	}
	if (g_Girls.HasTrait(girl, "Not Human"))
	{
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
	{
		security -= 20;
	}
	if (g_Girls.HasTrait(girl, "One Eye"))
	{
		barwait -= 10;
		clubwait -= 10;
	}
	if (g_Girls.HasTrait(girl, "Optimist"))
	{
		comunityservice += 10;
		feedpoor += 10;
	}
	if (g_Girls.HasTrait(girl, "Princess"))
	{
		mechanic -= 10;
	}
	if (g_Girls.HasTrait(girl, "Psychic"))
	{
		advertising += 10;
		barmaid += 10;
		barpiano += 10;
		barsing += 10;
		barwait += 10;
		brothelstrip += 10;
		clubbar += 10;
		clubstrip += 10;
		clubwait += 10;
		dealer += 15;
		doctor += 20;
		entertainer += 15;
		massusse += 10;
		nurse += 10;
		peep += 10;
		promoter += 10;
		recruiter += 20;
		security += 10;
		xxx += 10;
		researcher += 10;
		marketer += 10;
		farmer += 10;
		gardener += 10;
		sheapherd += 10;
		rancher += 10;
		catarancher += 10;
		milker += 10;
		butcher += 10;
		baker += 10;
		brewer += 10;
		makepot += 10;
	}
	if (g_Girls.HasTrait(girl, "Queen"))
	{
		mechanic -= 20;
	}
	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		barmaid += 5;
		barpiano += 5;
		barsing += 5;
		barwait += 5;
		brothelstrip += 5;
		clubbar += 5;
		clubstrip += 5;
		clubwait += 5;
		dealer += 5;
		doctor += 10;
		entertainer += 5;
		massusse += 5;
		nurse += 5;
		peep += 5;
		xxx += 5;
		researcher += 5;
		marketer += 5;
		farmer += 5;
		gardener += 5;
		sheapherd += 5;
		rancher += 5;
		catarancher += 5;
		milker += 5;
		butcher += 5;
		baker += 5;
		brewer += 5;
		makepot += 5;
	}
	if (g_Girls.HasTrait(girl, "Retarded"))
	{
		advertising -= 20;
		doctor -= 100;
		nurse -= 50;
		promoter -= 20;
	}
	if (g_Girls.HasTrait(girl, "Sadistic"))
	{
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
	{
		advertising += 10;
		barmaid += 5;
		barpiano += 5;
		barsing += 5;
		barwait += 10;
		brothelstrip += 10;
		clubbar += 10;
		clubstrip += 10;
		clubwait += 10;
		comunityservice += 10;
		dealer += 5;
		doctor += 10;
		entertainer += 5;
		feedpoor += 10;
		massusse += 10;
		mechanic += 5;
		nurse += 10;
		peep += 10;
		promoter += 10;
		xxx += 10;
		marketer += 5;
	}
	if (g_Girls.HasTrait(girl, "Shy"))  //
	{
		brothelstrip -= 20;
		clubstrip -= 20;
		entertainer -= 20;
		peep -= 20;
		xxx -= 20;
	}
	if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		barmaid -= 10;
		barpiano -= 10;
		barsing -= 10;
		barwait -= 10;
		brothelstrip -= 10;
		clubbar -= 10;
		clubstrip -= 10;
		clubwait -= 10;
		dealer -= 15;
		entertainer -= 15;
		massusse -= 10;
		peep -= 10;
		xxx -= 10;
	}
	if (g_Girls.HasTrait(girl, "Small Scars"))
	{
		brothelstrip -= 5;
		clubstrip -= 5;
		peep -= 5;
	}
	if (g_Girls.HasTrait(girl, "Strange Eyes"))
	{
		security += 2;
	}
	if (g_Girls.HasTrait(girl, "Strong"))
	{
		mechanic += 10;
	}
	if (g_Girls.HasTrait(girl, "Tough"))
	{
		mechanic += 5;
	}
	if (g_Girls.HasTrait(girl, "Tsundere"))
	{
		security += 5;
	}
	if (g_Girls.HasTrait(girl, "Twisted"))
	{
		security -= 5;
	}
	if (g_Girls.HasTrait(girl, "Yandere"))
	{
		security += 5;
	}

	data += gettext("Brothel Job Ratings\n\n");
	data += girl->JobRatingLetter(security) + gettext("  -  Security\n");
	data += girl->JobRatingLetter(advertising) + gettext("  -  Advertising\n");
	data += girl->JobRatingLetter(custservice) + gettext("  -  Customer Service\n");
	data += girl->JobRatingLetter(matron) + gettext("  -  Matron\n");
	data += girl->JobRatingLetter(catacombs) + gettext("  -  Explore Catacombs\n");
	data += girl->JobRatingLetter(beastcare) + gettext("  -  Beast Care\n\n");
	data += girl->JobRatingLetter(barmaid) + gettext("  -  Barmaid\n");
	data += girl->JobRatingLetter(barwait) + gettext("  -  Bar Waitress\n");
	data += girl->JobRatingLetter(barsing) + gettext("  -  Singer\n");
	data += girl->JobRatingLetter(barpiano) + gettext("  -  Piano\n\n");
	data += girl->JobRatingLetter(dealer) + gettext("  -  Dealer\n");
	data += girl->JobRatingLetter(entertainer) + gettext("  -  Entertainer\n");
	data += girl->JobRatingLetter(xxx) + gettext("  -  XXX Entertainer\n\n");
	data += girl->JobRatingLetter(clubbar) + gettext("  -  Club Barmaid\n");
	data += girl->JobRatingLetter(clubwait) + gettext("  -  Club Waitress\n");
	data += girl->JobRatingLetter(clubstrip) + gettext("  -  Stripper\n\n");
	data += girl->JobRatingLetter(massusse) + gettext("  -  Massusse\n");
	data += girl->JobRatingLetter(brothelstrip) + gettext("  -  Brothel Stripper\n");
	data += girl->JobRatingLetter(peep) + gettext("  -  Peep Show\n\n");
	//STUDIO
	data += gettext("\nStudio Job Ratings\n");
	data += girl->JobRatingLetter(director) + gettext("  -  Director\n");
	data += girl->JobRatingLetter(promoter) + gettext("  -  Promoter\n");
	data += girl->JobRatingLetter(cameramage) + gettext("  -  Camera Mage\n");
	data += girl->JobRatingLetter(crystalpurifier) + gettext("  -  Crystal Purifier\n");
	//ARENA
	data += gettext("\nArena Job Ratings\n");
	data += girl->JobRatingLetter(doctore) + gettext("  -  Doctore\n");
	data += girl->JobRatingLetter(fightbeast) + gettext("  -  Fight Beast\n");
	data += girl->JobRatingLetter(cagematch) + gettext("  -  Cage Match\n");
	data += girl->JobRatingLetter(cityguard) + gettext("  -  City Guard\n");
	//CENTRE
	data += gettext("\nCentre Job Ratings\n");
	data += girl->JobRatingLetter(centremanager) + gettext("  -  Centre Manager\n");
	data += girl->JobRatingLetter(drugcounselor) + gettext("  -  Drug Counselor\n");
	data += girl->JobRatingLetter(feedpoor) + gettext("  -  Feed Poor\n");
	data += girl->JobRatingLetter(comunityservice) + gettext("  -  Comunity Service\n");
	//CLINIC
	data += gettext("\nClinic Job Ratings\n");
	data += girl->JobRatingLetter(chairman) + gettext("  -  Chairman\n");
	data += girl->JobRatingLetter(doctor) + gettext("  -  Doctor\n");
	data += girl->JobRatingLetter(nurse) + gettext("  -  Nurse\n");
	data += girl->JobRatingLetter(mechanic) + gettext("  -  Mechanic\n");
	//HOUSE
	data += gettext("\nHouse Job Ratings\n");
	data += girl->JobRatingLetter(headgirl) + gettext("  -  Head Girl\n");
	data += girl->JobRatingLetter(recruiter) + gettext("  -  Recruiter\n");
	//FARM
	data += gettext("\nFarm Job Ratings\n");
	data += girl->JobRatingLetter(farmmanger) + gettext("  -  Farm Manger\n");
	data += girl->JobRatingLetter(veterinarian) + gettext("  -  Veterinarian\n");
	data += girl->JobRatingLetter(marketer) + gettext("  -  Marketer\n");
	data += girl->JobRatingLetter(researcher) + gettext("  -  Researcher\n");
	data += girl->JobRatingLetter(farmer) + gettext("  -  Farmer\n");
	data += girl->JobRatingLetter(gardener) + gettext("  -  Gardener\n");
	data += girl->JobRatingLetter(sheapherd) + gettext("  -   Sheapherd\n");
	data += girl->JobRatingLetter(rancher) + gettext("  -  Rancher\n");
	data += girl->JobRatingLetter(catarancher) + gettext("  -  Catacombs Rancher\n");
	data += girl->JobRatingLetter(milker) + gettext("  -  Milker\n");
	data += girl->JobRatingLetter(butcher) + gettext("  -  Butcher\n");
	data += girl->JobRatingLetter(baker) + gettext("  -  Baker\n");
	data += girl->JobRatingLetter(brewer) + gettext("  -  Brewer\n");
	data += girl->JobRatingLetter(makeitem) + gettext("  -  Make Item\n");
	data += girl->JobRatingLetter(makepot) + gettext("  -  Make Potion\n");

	data += gettext("\n\nJob Ratings range from\n'I' The absolute best, 'S' Superior,\nThen 'A'-'E' with 'E' being the worst.\n'X' means they can not do the job.");

	return data;
}

sGirl* cGirls::GetRandomGirl(bool slave, bool catacomb, bool arena, bool daughter)
{
	int num_girls = m_NumGirls;
	if ((num_girls == GetNumSlaveGirls() + GetNumCatacombGirls() + GetNumArenaGirls() + GetNumYourDaughterGirls()) || num_girls == 0)
	{
		int r = 3;
		while (r)
		{
			CreateRandomGirl(0, true);
			r--;
		}
	}
	GirlPredicate_GRG pred(slave, catacomb, arena, daughter);
	vector<sGirl *> girls = get_girls(&pred);
	if (girls.size() == 0) return 0;
	return girls[g_Dice.random(girls.size())];
}

sGirl* cGirls::GetGirl(int girl)
{
	int count = 0;
	sGirl* current = m_Parent;
	if (girl < 0 || (unsigned int)girl >= m_NumGirls)		return 0;
	while (current)
	{
		if (count == girl)	return current;
		count++;
		current = current->m_Next;
	}
	return 0;
}

int cGirls::GetRebelValue(sGirl* girl, bool matron)
{
	/*
	 *	WD:	Added test to ingnore STAT_HOUSE value
	 *	if doing a job that the palyer is paying
	 *	only when processing Day or Night Shift
	 *
	 *	This is to make it so that the jobs that
	 *	cost the player support where the hosue take
	 *	has no effect has no impact on the chance of
	 *	refusal.
	 */

	if (HasTrait(girl, "Broken Will"))	return -100;
	int chanceNo = 0;
	int houseStat = GetStat(girl, STAT_HOUSE);
	int happyStat = GetStat(girl, STAT_HAPPINESS);
	bool girlIsSlave = girl->is_slave();

	// a matron (or torturer in dungeon) will help convince a girl to obey 
	if (matron)	chanceNo -= 15;

	chanceNo -= GetStat(girl, STAT_PCLOVE) / 5;
	chanceNo += GetStat(girl, STAT_SPIRIT) / 2;
	chanceNo -= GetStat(girl, STAT_OBEDIENCE) / 5;

	// having a guarding gang will enforce order
	sGang* gang = g_Gangs.GetGangOnMission(MISS_GUARDING);
	if (gang)	chanceNo -= 10;

	chanceNo += GetStat(girl, STAT_TIREDNESS) / 10;	// Tired girls increase Rebel

	if (happyStat < 50)								// Unhappy girls increase Rebel
	{
		chanceNo += (50 - happyStat) / 5;
		if (happyStat < 10)							// WD:	Fixed missing case Happiness < 10
			chanceNo += 10 - happyStat;				// WD:	Rebel increases by an additional point if happy < 10
	}
	else	chanceNo -= (happyStat - 50) / 10;		// happy girls are less cranky, less Rebel

	// House Take has no effect on slaves
	if (girlIsSlave)	chanceNo -= 15;				// Slave Girl lowers rebelinous of course
	else
	{
		chanceNo += 15;								// Free girls are a little more rebelious
		// WD	House take of gold has no affect on rebellion if
		//		job is paid by player. eg Matron / cleaner
		if ((g_Brothels.is_Dayshift_Processing() && g_Brothels.m_JobManager.is_job_Paid_Player(girl->m_DayJob)) ||
			(g_Brothels.is_Nightshift_Processing() && g_Brothels.m_JobManager.is_job_Paid_Player(girl->m_NightJob)))
			houseStat = 0;

		if (houseStat < 60)							// Take less money than normal lower Rebel 
			chanceNo -= (60 - houseStat) / 2;
		else
		{
			chanceNo += (houseStat - 60) / 2;		// Take more money than normal, more Rebel
			if (houseStat >= 100) chanceNo += 10;	// Take all the money, more Rebel

		}
	}

	// guarantee certain rebelliousness values for specific traits
	if (HasTrait(girl, "Retarded"))	chanceNo -= 30;
	if (HasTrait(girl, "Mind Fucked") && chanceNo > -50)		return -50;
	if (HasTrait(girl, "Dependant") && chanceNo > -40)		return -40;
	if (HasTrait(girl, "Meek") && chanceNo > 20)			return 20;

	// Normalise
	if (chanceNo < -100)		chanceNo = -100;
	else if (chanceNo > 100)	chanceNo = 100;
	return chanceNo;
}

int cGirls::GetNumCatacombGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_States&(1 << STATUS_CATACOMBS))
			number++;
		current = current->m_Next;
	}
	return number;
}

int cGirls::GetNumSlaveGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_States&(1 << STATUS_SLAVE))
			number++;
		current = current->m_Next;
	}
	return number;
}

int cGirls::GetNumArenaGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_States&(1 << STATUS_ARENA))
			number++;
		current = current->m_Next;
	}
	return number;
}

int cGirls::GetNumYourDaughterGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_States&(1 << STATUS_YOURDAUGHTER))
			number++;
		current = current->m_Next;
	}
	return number;
}

// ----- Stat

// returns the total of stat + statmod + tempstat + stattr
int cGirls::GetStat(sGirl* girl, int a_stat)
{
	u_int stat = a_stat;
	int value = 0, min = 0, max = 100;

	if (stat == STAT_AGE) return (int)girl->m_Stats[stat];
	else if (stat == STAT_EXP) max = 32000;
	else if (stat == STAT_LEVEL) max = 255;
	else if (stat == STAT_HEALTH	&& g_Girls.HasTrait(girl, "Incorporeal"))	return 100;
	else if (stat == STAT_TIREDNESS && g_Girls.HasTrait(girl, "Incorporeal"))	return 0;
	else if (stat == STAT_PCLOVE || stat == STAT_PCFEAR || stat == STAT_PCHATE || stat == STAT_MORALITY || 
		stat == STAT_REFINMENT || stat == STAT_DIGNITY || stat == STAT_LACTATION) min = -100;
	// Generic calculation
	value = girl->m_Stats[stat] + girl->m_StatMods[stat] + girl->m_TempStats[stat] + girl->m_StatTr[stat];

	if (value < min) value = min;
	else if (value > max) value = max;
	return value;
}

void cGirls::SetStat(sGirl* girl, int a_stat, int amount)
{
	u_int stat = a_stat;
	int amt = amount;       // Modifying amount directly isn't a good idea
	int min = 0, max = 100;
	switch (stat)
	{
	case STAT_AGE:		// age is a special case so we start with that
		if (girl->m_Stats[STAT_AGE] > 99)		girl->m_Stats[stat] = 100;
		else if (girl->m_Stats[stat] > 80)		girl->m_Stats[stat] = 80;
		else if (girl->m_Stats[stat] < 18)		girl->m_Stats[stat] = 18;
		else									girl->m_Stats[stat] = amt;
		return; break;	// and just return instead of going to the end
	case STAT_HEALTH:
		if (g_Girls.HasTrait(girl, "Incorporeal"))	// Health and tiredness need the incorporeal sanity check
		{
			girl->m_Stats[stat] = 100;
			return;
		}
		break;
	case STAT_TIREDNESS:
		if (g_Girls.HasTrait(girl, "Incorporeal"))	// Health and tiredness need the incorporeal sanity check
		{
			girl->m_Stats[stat] = 0;
			return;
		}
		break;
	case STAT_EXP:
		max = 32000;
		break;
	case STAT_LEVEL:
		max = 255;
		break;
	case STAT_PCFEAR:
	case STAT_PCHATE:
	case STAT_PCLOVE:
	case STAT_MORALITY:
	case STAT_REFINMENT:
	case STAT_DIGNITY:
	case STAT_LACTATION:
		min = -100;
		break;
	default:
		break;
	}
	if (amt > max) amt = max;
	else if (amt < min) amt = min;
	girl->m_Stats[stat] = amt;
}

// update the girls stat by amount, if usetraits is false, most traits will not affect the amount
void cGirls::UpdateStat(sGirl* girl, int a_stat, int amount, bool usetraits)
{
	u_int stat = a_stat;
	int value = 0, min = 0, max = 100;
	switch (stat)
	{
		// `J` first do special cases that do not follow standard stat 0 to 100 rule
	case STAT_AGE:
		if (girl->m_Stats[STAT_AGE] != 100 && amount != 0)	// WD: Dont change imortal age = 100
		{
			value = girl->m_Stats[stat] + amount;
			if (value > 80)			value = 80;
			else if (value < 18)	value = 18;
			girl->m_Stats[stat] = value;
		}
		return; break;
		// now for the stats with modifiers
	case STAT_HEALTH:
	case STAT_TIREDNESS:
		if (HasTrait(girl, "Incorporeal"))
		{
			girl->m_Stats[STAT_HEALTH] = 100;	// WD: Sanity - Incorporeal health should allways be at 100%
			girl->m_Stats[STAT_TIREDNESS] = 0;	// WD: Sanity - Incorporeal Tiredness should allways be at 0%
			return;
		}
		if (HasTrait(girl, "Fragile") && usetraits)		amount -= 3;
		if (HasTrait(girl, "Tough") && usetraits)		amount += 2;
		if (HasTrait(girl, "Construct") && usetraits)	amount = (int)ceil((float)amount*0.1); // constructs take 10% damage
		break;

	case STAT_HAPPINESS:
		if (HasTrait(girl, "Pessimist") && g_Dice % 5 == 1 && usetraits)	amount -= 1; // `J` added
		if (HasTrait(girl, "Optimist") && g_Dice % 5 == 1 && usetraits)		amount += 1; // `J` added
		break;

	case STAT_LEVEL:
		max = 255;
		break;
	case STAT_EXP:
		max = 32000;
		break;
	case STAT_PCFEAR:
	case STAT_PCLOVE:
	case STAT_PCHATE:
	case STAT_MORALITY:
	case STAT_REFINMENT:
	case STAT_DIGNITY:
	case STAT_LACTATION:
		min = -100;
		break;
		// and the rest
	default:
		break;
	}
	if (amount != 0)
	{
		value = girl->m_Stats[stat] + amount;
		if (value > max)		value = max;
		else if (value < min)	value = min;
		girl->m_Stats[stat] = value;
	}
}

void cGirls::UpdateStatMod(sGirl* girl, int stat, int amount)
{
	bool nomod = (stat == STAT_HEALTH || stat == STAT_HAPPINESS || stat == STAT_TIREDNESS
		|| stat == STAT_AGE || stat == STAT_EXP
		) ? true : false;
	if (nomod)	girl->m_StatMods[stat] = 0; // some stats should not have mod or temp so set them to 0
	if (amount >= 0)
	{
		if (nomod)	girl->m_Stats[stat] = min(100, amount + girl->m_Stats[stat]);
		else		girl->m_StatMods[stat] = min(100, amount + girl->m_StatMods[stat]);
	}
	else
	{
		if (nomod)	girl->m_Stats[stat] = max(0, amount + girl->m_Stats[stat]);
		else		girl->m_StatMods[stat] = max(-100, amount + girl->m_StatMods[stat]);
	}
}

void cGirls::updateTempStats(sGirl* girl)	// Normalise to zero by 30% each week
{
	if (girl->health() <= 0) return;		// Sanity check. Abort on dead girl
	for (int i = 0; i < NUM_STATS; i++)
	{
		if (girl->m_TempStats[i] != 0)				// normalize towards 0 by 30% each week
		{
			int newStat = (int)(float(girl->m_TempStats[i]) * 0.7);
			if (newStat != girl->m_TempStats[i]) girl->m_TempStats[i] = newStat;
			else if (girl->m_TempStats[i] > 0)	girl->m_TempStats[i]--;	// if 30% did nothing, go with 1 instead
			else if (girl->m_TempStats[i] < 0)	girl->m_TempStats[i]++;
		}
	}
}

void cGirls::UpdateTempStat(sGirl* girl, int stat, int amount)
{
	bool nomod = (stat == STAT_HEALTH || stat == STAT_HAPPINESS || stat == STAT_TIREDNESS
		|| stat == STAT_AGE || stat == STAT_EXP
		) ? true : false;
	if (nomod)	girl->m_TempStats[stat] = 0; // some stats should not have mod or temp so set them to 0
	if (amount >= 0)
	{
		if (nomod)	girl->m_Stats[stat] = min(100, amount + girl->m_Stats[stat]);
		else		girl->m_TempStats[stat] = min(100, amount + girl->m_TempStats[stat]);
	}
	else
	{
		if (nomod)	girl->m_Stats[stat] = max(0, amount + girl->m_Stats[stat]);
		else		girl->m_TempStats[stat] = max(-100, amount + girl->m_TempStats[stat]);
	}
}

// ----- Skill

// returns total of skill + mod + temp + trait
int cGirls::GetSkill(sGirl* girl, int skill)
{
	int value = (girl->m_Skills[skill]) + girl->m_SkillMods[skill] + girl->m_TempSkills[skill] + girl->m_SkillTr[skill];
	if (value < 0)			value = 0;
	else if (value > 100)	value = 100;
	return value;
}
// set the skill to amount
void cGirls::SetSkill(sGirl* girl, int skill, int amount)
{
	girl->m_Skills[skill] = amount;
}
// total of all skills
int cGirls::GetSkillWorth(sGirl* girl)
{
	int num = 0;
	for (u_int i = 0; i < NUM_SKILLS; i++) num += (int)girl->m_Skills[i];
	return num;
}
// add amount to skill
void cGirls::UpdateSkill(sGirl* girl, int skill, int amount)
{
	if (amount >= 0)
	{
		girl->m_Skills[skill] = min(100, amount + girl->m_Skills[skill]);
	}
	else
	{
		girl->m_Skills[skill] = max(0, amount + girl->m_Skills[skill]);
	}
}
// add amount to skillmod
void cGirls::UpdateSkillMod(sGirl* girl, int skill, int amount)
{
	if (amount >= 0)
	{
		girl->m_SkillMods[skill] = min(100, amount + girl->m_SkillMods[skill]);
	}
	else
	{
		girl->m_SkillMods[skill] = max(-100, amount + girl->m_SkillMods[skill]);
	}
}

// add amount to tempskill
void cGirls::UpdateTempSkill(sGirl* girl, int skill, int amount)
{
	if (amount >= 0)
	{
		girl->m_TempSkills[skill] = min(200, amount + girl->m_SkillMods[skill]);
	}
	else
	{
		girl->m_TempSkills[skill] = max(-200, amount + girl->m_SkillMods[skill]);
	}
}
// Normalise to zero by 30%
void cGirls::updateTempSkills(sGirl* girl)
{
	// Sanity check. Abort on dead girl
	if (girl->health() <= 0) return;

	for (u_int i = 0; i < NUM_SKILLS; i++)
	{
		if (girl->m_TempSkills[i] != 0)
		{											// normalize towards 0 by 30% each week
			int newSkill = (int)(float(girl->m_TempSkills[i]) * 0.7);
			if (newSkill != girl->m_TempSkills[i])
				girl->m_TempSkills[i] = newSkill;
			else
			{										// if 30% did nothing, go with 1 instead
				if (girl->m_TempSkills[i] > 0)
					girl->m_TempSkills[i]--;
				else if (girl->m_TempSkills[i] < 0)
					girl->m_TempSkills[i]++;
			}
		}
	}
}

// ----- Load save

// This load

bool sGirl::LoadGirlXML(TiXmlHandle hGirl)
{
	//this is always called after creating a new girl, so let's not init sGirl again
	TiXmlElement* pGirl = hGirl.ToElement();
	if (pGirl == 0)
	{
		return false;
	}

	int tempInt = 0;

	// load the name
	const char* pTempString = pGirl->Attribute("Name");
	if (pTempString)
	{
		m_Name = new char[strlen(pTempString) + 1];
		strcpy(m_Name, pTempString);
	}
	else
	{
		m_Name = "";
	}

	if (pGirl->Attribute("Realname"))
	{
		m_Realname = pGirl->Attribute("Realname");
	}

	cConfig cfg;
	if (cfg.debug.log_girls()) {
		std::stringstream ss;
		ss << "Loading girl: '" << m_Realname;
		g_LogFile.write(ss.str());
	}

	// get the description
	if (pGirl->Attribute("Desc"))
	{
		m_Desc = pGirl->Attribute("Desc");
	}

	// load the amount of days they are unhappy in a row
	pGirl->QueryIntAttribute("DaysUnhappy", &tempInt); m_DaysUnhappy = tempInt; tempInt = 0;

	// Load their traits
	LoadTraitsXML(hGirl.FirstChild("Traits"), m_NumTraits, m_Traits, m_TempTrait);
	if (m_NumTraits > MAXNUM_TRAITS)
		g_LogFile.write("--- ERROR - Loaded more traits than girls can have??");

	// Load their remembered traits
	LoadTraitsXML(hGirl.FirstChild("Remembered_Traits"), m_NumRememTraits, m_RememTraits);
	if (m_NumRememTraits > MAXNUM_TRAITS * 2)
		g_LogFile.write("--- ERROR - Loaded more remembered traits than girls can have??");

	// Load inventory items
	LoadInventoryXML(hGirl.FirstChild("Inventory"), m_Inventory, m_NumInventory, m_EquipedItems);

	// load their states
	pGirl->QueryValueAttribute<long>("States", &m_States);

	// load their stats
	LoadStatsXML(hGirl.FirstChild("Stats"), m_Stats, m_StatMods, m_TempStats);

	// load their skills
	LoadSkillsXML(hGirl.FirstChild("Skills"), m_Skills, m_SkillMods, m_TempSkills);

	// load virginity
	pGirl->QueryIntAttribute("Virgin", &m_Virgin);

	// load using antipreg
	pGirl->QueryValueAttribute<bool>("UseAntiPreg", &m_UseAntiPreg);

	// load withdrawals
	pGirl->QueryIntAttribute("Withdrawals", &tempInt); m_Withdrawals = tempInt; tempInt = 0;

	// load money
	pGirl->QueryIntAttribute("Money", &m_Money);

	// load working day counter
	pGirl->QueryIntAttribute("WorkingDay", &m_WorkingDay);
	pGirl->QueryIntAttribute("PrevWorkingDay", &m_PrevWorkingDay);	// `J` added
	pGirl->QueryIntAttribute("SpecialJobGoal", &m_SpecialJobGoal);	// `J` added
	if (m_WorkingDay < 0)		m_WorkingDay = 0;
	if (m_PrevWorkingDay < 0)	m_PrevWorkingDay = 0;
	if (m_SpecialJobGoal < 0)	m_SpecialJobGoal = 0;

	// load acom level
	pGirl->QueryIntAttribute("AccLevel", &tempInt); m_AccLevel = tempInt; tempInt = 0;
	// load day/night jobs
	pGirl->QueryIntAttribute("DayJob", &tempInt); m_DayJob = tempInt; tempInt = 0;
	pGirl->QueryIntAttribute("NightJob", &tempInt); m_NightJob = tempInt; tempInt = 0;

	// load prev day/night jobs
	pGirl->QueryIntAttribute("PrevDayJob", &tempInt); m_PrevDayJob = tempInt; tempInt = 0;
	pGirl->QueryIntAttribute("PrevNightJob", &tempInt); m_PrevNightJob = tempInt; tempInt = 0;

	// load yester day/night jobs
	pGirl->QueryIntAttribute("YesterDayJob", &tempInt); m_YesterDayJob = tempInt; tempInt = 0;
	pGirl->QueryIntAttribute("YesterNightJob", &tempInt); m_YesterNightJob = tempInt; tempInt = 0;
	if (m_YesterDayJob < 0)m_YesterDayJob = 255;
	if (m_YesterNightJob < 0)m_YesterNightJob = 255;

	// load runnayway value
	pGirl->QueryIntAttribute("RunAway", &tempInt); m_RunAway = tempInt; tempInt = 0;

	// load spotted
	pGirl->QueryIntAttribute("Spotted", &tempInt); m_Spotted = tempInt; tempInt = 0;

	// load newRandomFixed
	tempInt = -1;
	pGirl->QueryIntAttribute("NewRandomFixed", &tempInt); m_newRandomFixed = tempInt; tempInt = 0;

	// load weeks past, birth day, and pregant time
	pGirl->QueryValueAttribute<unsigned long>("WeeksPast", &m_WeeksPast);
	pGirl->QueryValueAttribute<unsigned int>("BDay", &m_BDay);
	pGirl->QueryIntAttribute("WeeksPreg", &tempInt); m_WeeksPreg = tempInt; tempInt = 0;

	// load number of customers slept with
	pGirl->QueryValueAttribute<unsigned long>("NumCusts", &m_NumCusts);

	// load girl flags
	TiXmlElement* pFlags = pGirl->FirstChildElement("Flags");
	if (pFlags)
	{
		std::string flagNumber;
		for (int i = 0; i < NUM_GIRLFLAGS; i++)
		{
			flagNumber = "Flag_";
			std::stringstream stream;
			stream << i;
			flagNumber.append(stream.str());
			pFlags->QueryIntAttribute(flagNumber, &tempInt); m_Flags[i] = tempInt; tempInt = 0;
		}
	}

	// load their torture value
	pGirl->QueryValueAttribute<bool>("Tort", &m_Tort);

	// Load their children
	pGirl->QueryIntAttribute("PregCooldown", &tempInt); m_PregCooldown = tempInt; tempInt = 0;

	// load number of children
	TiXmlElement* pChildren = pGirl->FirstChildElement("Children");
	if (pChildren)
	{
		for (TiXmlElement* pChild = pChildren->FirstChildElement("Child");
			pChild != 0;
			pChild = pChild->NextSiblingElement("Child"))
		{
			sChild* child = new sChild();
			bool success = child->LoadChildXML(TiXmlHandle(pChild));
			if (success == true)
			{
				m_Children.add_child(child);
			}
			else
			{
				delete child;
				continue;
			}
		}
	}

	// load enjoyment values
	LoadActionsXML(hGirl.FirstChildElement("Actions"), m_Enjoyment);

	// load their triggers
	m_Triggers.LoadTriggersXML(hGirl.FirstChildElement("Triggers"));
	m_Triggers.SetGirlTarget(this);

	if (m_Stats[STAT_AGE] < 18) m_Stats[STAT_AGE] = 18;

	// load their images
	g_Girls.LoadGirlImages(this);
	g_Girls.CalculateGirlType(this);

	return true;
}

// This save

TiXmlElement* sGirl::SaveGirlXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGirl = new TiXmlElement("Girl");
	pRoot->LinkEndChild(pGirl);
	// save the name
	pGirl->SetAttribute("Name", m_Name);

	// save the real name
	pGirl->SetAttribute("Realname", m_Realname);

	// save the description
	pGirl->SetAttribute("Desc", m_Desc);

	// save the amount of days they are unhappy
	pGirl->SetAttribute("DaysUnhappy", m_DaysUnhappy);

	// Save their traits
	if (m_NumTraits > MAXNUM_TRAITS)
		g_LogFile.write("---- ERROR - Saved more traits then girls can have");
	SaveTraitsXML(pGirl, "Traits", MAXNUM_TRAITS, m_Traits, m_TempTrait);

	// Save their remembered traits
	if (m_NumRememTraits > MAXNUM_TRAITS * 2)
		g_LogFile.write("---- ERROR - Saved more remembered traits then girls can have");
	SaveTraitsXML(pGirl, "Remembered_Traits", MAXNUM_TRAITS * 2, m_RememTraits, 0);

	// Save inventory items
	TiXmlElement* pInventory = new TiXmlElement("Inventory");
	pGirl->LinkEndChild(pInventory);
	SaveInventoryXML(pInventory, m_Inventory, 40, m_EquipedItems);

	// save their states
	pGirl->SetAttribute("States", m_States);

	// Save their stats
	SaveStatsXML(pGirl, m_Stats, m_StatMods, m_TempStats);

	// save their skills
	SaveSkillsXML(pGirl, m_Skills, m_SkillMods, m_TempSkills);

	// save virginity
	pGirl->SetAttribute("Virgin", m_Virgin);

	// save using antipreg
	pGirl->SetAttribute("UseAntiPreg", m_UseAntiPreg);

	// save withdrawals
	pGirl->SetAttribute("Withdrawals", m_Withdrawals);

	// save money
	pGirl->SetAttribute("Money", m_Money);

	// save working day counter
	pGirl->SetAttribute("WorkingDay", m_WorkingDay);
	pGirl->SetAttribute("PrevWorkingDay", m_PrevWorkingDay);	// `J` added
	pGirl->SetAttribute("SpecialJobGoal", m_SpecialJobGoal);	// `J` added

	// save acom level
	pGirl->SetAttribute("AccLevel", m_AccLevel);

	// save day/night jobs
	pGirl->SetAttribute("DayJob", m_DayJob);
	pGirl->SetAttribute("NightJob", m_NightJob);

	// save prev day/night jobs
	pGirl->SetAttribute("PrevDayJob", m_PrevDayJob);
	pGirl->SetAttribute("PrevNightJob", m_PrevNightJob);

	// save prev day/night jobs
	if (m_YesterDayJob < 0)m_YesterDayJob = 255;
	pGirl->SetAttribute("YesterDayJob", m_YesterDayJob);
	if (m_YesterNightJob < 0)m_YesterNightJob = 255;
	pGirl->SetAttribute("YesterNightJob", m_YesterNightJob);

	// save runnayway vale
	pGirl->SetAttribute("RunAway", m_RunAway);

	// save spotted
	pGirl->SetAttribute("Spotted", m_Spotted);

	if (m_newRandomFixed >= 0)
		pGirl->SetAttribute("NewRandomFixed", m_newRandomFixed);

	// save weeks past, birth day, and pregant time
	pGirl->SetAttribute("WeeksPast", m_WeeksPast);
	pGirl->SetAttribute("BDay", m_BDay);
	pGirl->SetAttribute("WeeksPreg", m_WeeksPreg);

	// number of customers slept with
	pGirl->SetAttribute("NumCusts", m_NumCusts);

	// girl flags
	TiXmlElement* pFlags = new TiXmlElement("Flags");
	pGirl->LinkEndChild(pFlags);
	std::string flagNumber;
	for (int i = 0; i < NUM_GIRLFLAGS; i++)
	{
		flagNumber = "Flag_";
		std::stringstream stream;
		stream << i;
		flagNumber.append(stream.str());
		pFlags->SetAttribute(flagNumber, m_Flags[i]);
	}

	// save their torture value
	pGirl->SetAttribute("Tort", m_Tort);

	// save their children
	pGirl->SetAttribute("PregCooldown", m_PregCooldown);
	TiXmlElement* pChildren = new TiXmlElement("Children");
	pGirl->LinkEndChild(pChildren);
	sChild* child = m_Children.m_FirstChild;
	while (child)
	{
		child->SaveChildXML(pChildren);
		child = child->m_Next;
	}

	// save their enjoyment values
	SaveActionsXML(pGirl, m_Enjoyment);

	// save their triggers
	m_Triggers.SaveTriggersXML(pGirl);
	return pGirl;
}

bool sChild::LoadChildXML(TiXmlHandle hChild)
{
	TiXmlElement* pChild = hChild.ToElement();
	if (pChild == 0)
	{
		return false;
	}

	int tempInt = 0;
	pChild->QueryIntAttribute(gettext("Age"), &tempInt); m_Age = tempInt; tempInt = 0;
	pChild->QueryIntAttribute("IsPlayers", &tempInt); m_IsPlayers = tempInt; tempInt = 0;
	pChild->QueryIntAttribute("Sex", &tempInt);
	m_Sex = sChild::Gender(tempInt); tempInt = 0;
	pChild->QueryIntAttribute("Unborn", &tempInt); m_Unborn = tempInt; tempInt = 0;

	// load their stats
	LoadStatsXML(hChild.FirstChild("Stats"), m_Stats);

	// load their skills
	LoadSkillsXML(hChild.FirstChild("Skills"), m_Skills);
	return true;
}

TiXmlElement* sChild::SaveChildXML(TiXmlElement* pRoot)
{
	TiXmlElement* pChild = new TiXmlElement("Child");
	pRoot->LinkEndChild(pChild);
	pChild->SetAttribute("Age", m_Age);
	pChild->SetAttribute("IsPlayers", m_IsPlayers);
	pChild->SetAttribute("Sex", m_Sex);
	pChild->SetAttribute("Unborn", m_Unborn);
	// Save their stats
	SaveStatsXML(pChild, m_Stats);

	// save their skills
	SaveSkillsXML(pChild, m_Skills);
	return pChild;
}

 // and an xml loader for sGirl
void sGirl::load_from_xml(TiXmlElement *el)
{
	int ival;
	const char *pt;
	// get the simple fields
	if (pt = el->Attribute("Name"))
	{
		m_Name = n_strdup(pt);
		m_Realname = pt;
		g_LogFile.os() << "Loading Girl : " << m_Realname << endl;
	}
	else
	{
		g_LogFile.os() << "Error: can't find name when loading girl." << "XML = " << (*el) << endl;
		return;
	}
	m_newRandomFixed = -1;

	if (pt = el->Attribute("Desc"))			m_Desc = n_strdup(pt);
	if (pt = el->Attribute("Gold", &ival))	m_Money = ival;
	if (pt = el->Attribute("Virgin"))		m_Virgin = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Catacombs"))	m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_CATACOMBS) : (0 << STATUS_CATACOMBS);
	if (pt = el->Attribute("Slave"))		m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_SLAVE) : (0 << STATUS_SLAVE);
	if (pt = el->Attribute("Arena"))		m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_ARENA) : (0 << STATUS_ARENA);
	if (pt = el->Attribute("YourDaughter"))	m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_YOURDAUGHTER) : (0 << STATUS_YOURDAUGHTER);

	for (int i = 0; i < NUM_STATS; i++) // loop through stats
	{
		int ival;
		const char *stat_name = sGirl::stat_names[i];
		pt = el->Attribute(stat_name, &ival);

		ostream& os = g_LogFile.os();
		if (pt == 0)
		{
			os << "Error: Can't find stat '" << stat_name << "' for girl '" << m_Realname << "' - Setting it to default." << endl;
			continue;
		}
		m_Stats[i] = ival;
		cConfig cfg;
		if (cfg.debug.log_girls())	os << "Debug: Girl='" << m_Realname << "'; Stat='" << stat_name << "'; Value='" << pt << "'; Ival = " << int(m_Stats[i]) << "'" << endl;
	}

	for (u_int i = 0; i < NUM_SKILLS; i++)	//	loop through skills
	{
		int ival;
		if (pt = el->Attribute(sGirl::skill_names[i], &ival))	m_Skills[i] = ival;
	}

	if (pt = el->Attribute("Status"))
	{
		/* */if (strcmp(pt, gettext("Catacombs")) == 0)		m_States |= (1 << STATUS_CATACOMBS);
		else if (strcmp(pt, gettext("Slave")) == 0)			m_States |= (1 << STATUS_SLAVE);
		else if (strcmp(pt, gettext("Arena")) == 0)			m_States |= (1 << STATUS_ARENA);
		else if (strcmp(pt, gettext("Your Daughter")) == 0)	m_States |= (1 << STATUS_YOURDAUGHTER);
		//		else	m_States = 0;
	}

	TiXmlElement * child;
	for (child = el->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		if (child->ValueStr() == "Trait")	//get the trait name 
		{
			pt = child->Attribute("Name");
			m_Traits[m_NumTraits] = g_Traits.GetTrait(g_Traits.GetTranslateName(n_strdup(pt))); // `J` added translation check
			m_NumTraits++;
		}
	}
	m_AccLevel = 1;
}

void sRandomGirl::load_from_xml(TiXmlElement *el)
{
	const char *pt;
	m_NumTraits = 0;
	// name and description are easy
	if (pt = el->Attribute("Name")) 		m_Name = pt;
	g_LogFile.os() << "Loading Rgirl : " << pt << endl;
	if (pt = el->Attribute("Desc"))			m_Desc = pt;

	// DQ - new random type ...
	m_newRandom = false;
	m_newRandomTable = 0;
	if (pt = el->Attribute("NewRandom"))		m_newRandom = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Human"))			m_Human = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Catacomb"))			m_Catacomb = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Arena"))			m_Arena = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Your Daughter"))	m_YourDaughter = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;

	// loop through children
	TiXmlElement *child;
	for (child = el->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		/*
		 *		now: what we do depends on the tag string
		 *		which we can get from the ValueStr() method
		 *
		 *		Let's process each tag type in its own method.
		 *		Keep things cleaner that way.
		 */
		if (child->ValueStr() == "Gold")
		{
			process_cash_xml(child);
			continue;
		}
		// OK: is it a stat?
		if (child->ValueStr() == "Stat")
		{
			process_stat_xml(child);
			continue;
		}
		// How about a skill?
		if (child->ValueStr() == "Skill")
		{
			process_skill_xml(child);
			continue;
		}
		// surely a trait then?
		if (child->ValueStr() == "Trait")
		{
			process_trait_xml(child);
			continue;
		}
		// None of the above? Better ask for help then.
		g_LogFile.os() << "Unexpected tag: " << child->ValueStr() << endl;
		g_LogFile.os() << "	don't know what do to, ignoring" << endl;
	}
}

void cGirls::LoadRandomGirl(string filename)
{
	/*
	 *	before we go any further: files that end in "x" are
	 *	in XML format. Get the last char of the filename.
	 */
	char c = filename.at(filename.length() - 1);
	/*
	 *	now decide how we want to really load the file
	 */
	if (c == 'x')
	{
		cerr << "loading " << filename << " as XML" << endl;
		LoadRandomGirlXML(filename);
	}
	else
	{
		cerr << ".06 no longer supports Legacy Girls. Use the Whore Master Editor to update '" << filename << "'" << endl;
	}
}

void cGirls::LoadRandomGirlXML(string filename)
{
	TiXmlDocument doc(filename);
	if (!doc.LoadFile())
	{
		g_LogFile.os() << "can't load random XML girls " << filename << endl;
		g_LogFile.os() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		return;
	}
	g_LogFile.os() << endl << "Loading File ::: " << filename << endl;
	TiXmlElement *el, *root_el = doc.RootElement();	// get the docuement root

	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{	// loop over the elements attached to the root
		sRandomGirl* girl = new sRandomGirl;		// walk the XML DOM to get the girl data
		girl->load_from_xml(el);					// uses sRandomGirl::load_from_xml
		AddRandomGirl(girl);						// add the girl to the list
	}
}

void cGirls::LoadGirlsDecider(string filename)
{
	/*
	 *	before we go any further: files that end in "x" are
	 *	in XML format. Get the last char of the filename.
	 */
	char c = filename.at(filename.length() - 1);
	/*
	 *	now decide how we want to really load the file
	 */
	if (c == 'x')
	{
		cerr << "loading " << filename << " as XML" << endl;
		LoadGirlsXML(filename);
	}
	else
	{
		cerr << ".06 no longer supports Legacy Girls. Use the Whore Master Editor to update '" << filename << "'" << endl;
	}
}

void cGirls::LoadGirlsXML(string filename)
{
	cConfig cfg;
	TiXmlDocument doc(filename);
	if (!doc.LoadFile())
	{
		g_LogFile.ss() << "can't load XML girls " << filename << endl;
		g_LogFile.ss() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		g_LogFile.ssend();
		return;
	}
	g_LogFile.os() << endl << "Loading File ::: " << filename << endl;
	// get the docuement root
	TiXmlElement *el, *root_el = doc.RootElement();
	// loop over the elements attached to the root
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{

		sGirl* girl = new sGirl;			// walk the XML DOM to get the girl data
		girl->load_from_xml(el);			// uses sGirl::load_from_xml
		if (cfg.debug.log_girls() && cfg.debug.log_extradetails()) g_LogFile.os() << *girl << endl;

		if (CheckVirginity(girl))			// `J` check girl's virginity
		{
			girl->m_Virgin = 1; AddTrait(girl, "Virgin");
		}
		else
		{
			girl->m_Virgin = 0;
			RemoveTrait(girl, "Virgin");
		}
		if (girl->m_Stats[STAT_AGE] < 18) girl->m_Stats[STAT_AGE] = 18;

		g_Girls.LoadGirlImages(girl);		// Load Girl Images
		
		
		MutuallyExclusiveTraits(girl, 1);	// make sure all the trait effects are applied
		ApplyTraits(girl);	// `J` this is the old method  - the trait add and remove has been removed 
		
		
		RemoveAllRememberedTraits(girl);	// WD: For new girls remove any remembered traits from trait incompatibilities
		// load triggers if the girl has any
		DirPath dp = DirPath() << "Resources" << "Characters" << girl->m_Name << "triggers.xml";
		girl->m_Triggers.LoadList(dp);
		girl->m_Triggers.SetGirlTarget(girl);
		AddGirl(girl);						// add the girl to the list
		CalculateGirlType(girl);			// Fetish list for customer happiniess
	}
}

bool cGirls::LoadGirlsXML(TiXmlHandle hGirls)
{
	TiXmlElement* pGirls = hGirls.ToElement();
	if (pGirls == 0) return false;
	sGirl* current = 0;					// load the number of girls
	for (TiXmlElement* pGirl = pGirls->FirstChildElement("Girl"); pGirl != 0; pGirl = pGirl->NextSiblingElement("Girl"))
	{
		current = new sGirl();			// load each girl and add her
		bool success = current->LoadGirlXML(TiXmlHandle(pGirl));
		if (success == true) AddGirl(current);
		else { delete current; continue; }
	}
	return true;
}

TiXmlElement* cGirls::SaveGirlsXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGirls = new TiXmlElement("Girls");
	pRoot->LinkEndChild(pGirls);
	sGirl* current = m_Parent;
	while (current)					// save the number of girls
	{
		current->SaveGirlXML(pGirls);
		current = current->m_Next;
	}
	return pGirls;
}

// ----- Tag processing

void sRandomGirl::process_trait_xml(TiXmlElement *el)
{
	int ival; const char *pt;
	sTrait *trait = new sTrait();													// we need to allocate a new sTrait scruct,
	if ((pt = el->Attribute("Name"))) trait->m_Name = n_strdup(pt);					// get the trait name
	m_Traits[m_NumTraits] = trait;													// store that in the next free index slot
	if ((pt = el->Attribute("Percent", &ival))) m_TraitChance[m_NumTraits] = ival;	// get the percentage chance
	m_NumTraits++;																	// and whack up the trait count.
}

void sRandomGirl::process_stat_xml(TiXmlElement *el)
{
	int ival, index; const char *pt;
	if ((pt = el->Attribute("Name"))) index = lookup->stat_lookup[pt];
	else
	{
		g_LogFile.os() << "can't find 'Name' attribute - can't process stat" << endl;
		return;		// do as much as we can without crashing
	}
	if ((pt = el->Attribute("Min", &ival))) m_MinStats[index] = ival;
	if ((pt = el->Attribute("Max", &ival))) m_MaxStats[index] = ival;
}

void sRandomGirl::process_skill_xml(TiXmlElement *el)
{
	int ival, index;
	const char *pt;
	/*
	 *	Strictly, I should use something that lets me
	 *	test for absence. This won't catch typos in the
	 *	XML file
	 */
	if ((pt = el->Attribute("Name"))) index = lookup->skill_lookup[pt];
	else
	{
		g_LogFile.os() << "can't find 'Name' attribute - can't process skill" << endl;
		return;		// do as much as we can without crashing
	}
	if ((pt = el->Attribute("Min", &ival))) m_MinSkills[index] = ival;
	if ((pt = el->Attribute("Max", &ival))) m_MaxSkills[index] = ival;
}

void sRandomGirl::process_cash_xml(TiXmlElement *el)
{
	cConfig cfg;
	int ival; const char *pt;
	if ((pt = el->Attribute("Min", &ival)))
	{
		if (cfg.debug.log_girls()) g_LogFile.os() << " min money = " << ival << endl;
		m_MinMoney = ival;
	}
	if ((pt = el->Attribute("Max", &ival)))
	{
		if (cfg.debug.log_girls()) g_LogFile.os() << " max money = " << ival << endl;
		m_MaxMoney = ival;
	}
}

// ----- Equipment & inventory

int cGirls::HasItem(sGirl* girl, string name)
{
	for (int i = 0; i < 40; i++)
	{
		if (girl->m_Inventory[i])
		{
			if (girl->m_Inventory[i]->m_Name == name) return i;
		}
	}
	return -1;
}

string stringtolower(string name)
{
	string s = name;
	for (u_int i = 0; i < name.length(); i++)
	{
		s[i] = tolower(name[i]);
	}
	return s;
}

string stringtolowerj(string name)
{
	string s = "", t = "";
	for (u_int i = 0; i < name.length(); i++)
	{
		if (tolower(name[i]) != tolower(" "[0]) || tolower(name[i]) != tolower("."[0]) || tolower(name[i]) != tolower(","[0]))
		{
			t[0] = tolower(name[i]);
			s += t[0];
		}
	}
	return s;
}

int cGirls::HasItemJ(sGirl* girl, string name)	// `J` added to compare item names removing spaces commas and periods
{
	string s = stringtolowerj(name);
	for (int i = 0; i < 40; i++)
	{
		if (girl->m_Inventory[i])
		{
			string t = stringtolowerj(girl->m_Inventory[i]->m_Name);
			if (t == s)	return i;
		}
	}
	return -1;
}

void cGirls::EquipCombat(sGirl* girl)
{
	cConfig cfg;									// girl makes sure best armor and weapons are equipped, ready for combat
	if (!cfg.initial.auto_combat_equip()) return;	// is this feature disabled in config?
	int refusal = 0;
	if (girl->has_trait("Retarded")) refusal += 30;	// if she's retarded, she might refuse or forget
	if (g_Dice.percent(refusal)) return;

	int Armor = -1, Weap1 = -1, Weap2 = -1;
	for (int i = 0; i < 40; i++)
	{
		if (girl->m_Inventory[i] != 0)
		{
			if (girl->m_Inventory[i]->m_Type == INVWEAPON)
			{
				g_InvManager.Unequip(girl, i);
				if (Weap1 == -1) Weap1 = i;
				else if (Weap2 == -1) Weap2 = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap1]->m_Cost)
				{
					Weap2 = Weap1;
					Weap1 = i;
				}
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap2]->m_Cost)
					Weap2 = i;
			}
			if (girl->m_Inventory[i]->m_Type == INVARMOR)
			{
				g_InvManager.Unequip(girl, i);
				if (Armor == -1) Armor = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Armor]->m_Cost) Armor = i;
			}
		}
	}
	if (Armor > -1) g_InvManager.Equip(girl, Armor, false);
	if (Weap1 > -1) g_InvManager.Equip(girl, Weap1, false);
	if (Weap2 > -1) g_InvManager.Equip(girl, Weap2, false);
}

void cGirls::UnequipCombat(sGirl* girl)
{  // girl unequips armor and weapons, ready for brothel work or other non-aggressive jobs	
	cConfig cfg;
	if (!cfg.initial.auto_combat_equip()) return; // is this feature disabled in config?
	// if she's a really rough or crazy bitch, she might just keep combat gear equipped
	int refusal = 0;
	if (girl->has_trait("Aggressive"))	refusal += 30;
	if (girl->has_trait("Yandere"))		refusal += 30;
	if (girl->has_trait("Twisted"))		refusal += 30;
	if (girl->has_trait("Retarded"))		refusal += 30;
	if (g_Dice.percent(refusal))			return;

	for (int i = 0; i < 40; i++)
	{
		if (girl->m_Inventory[i] != 0)
		{
			sInventoryItem* curItem = girl->m_Inventory[i];
			if (curItem->m_Type == INVWEAPON)
				g_InvManager.Unequip(girl, i);
			else if (curItem->m_Type == INVARMOR)
			{
#if 0  // code to only unequip armor if it has bad effects... unfinished and disabled for the time being; not sure it should be used anyway
				bool badEffects = false;
				for (u_int j = 0; j < curItem->m_Effects.size(); j++)
				{
					sEffect* curEffect = &curItem->m_Effects[j];
					if (curEffect->m_Affects == sEffect::Stat && curEffect->m_Amount < 0)
					{
					}
					else if (curEffect->m_Affects == sEffect::Skill && curEffect->m_Amount < 0)
					{
					}
				}
				if (badEffects)
#endif
					g_InvManager.Unequip(girl, i);
			}
		}
	}
}

void cGirls::UseItems(sGirl* girl)
{
	bool withdraw = false;
	// uses drugs first
	if (HasTrait(girl, "Viras Blood Addict"))
	{
		int temp = HasItem(girl, "Vira Blood");
		if (temp == -1)	// withdrawals for a week
		{
			if (girl->m_Withdrawals >= 30)
			{
				RemoveTrait(girl, "Viras Blood Addict");
				stringstream goodnews;
				goodnews << "Good News, " << girl->m_Realname << " has overcome her addiction to Viras Blood.";
				girl->m_Events.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
			else
			{
				UpdateStat(girl, STAT_HAPPINESS, -30);
				UpdateStat(girl, STAT_OBEDIENCE, -30);
				UpdateStat(girl, STAT_HEALTH, -4);
				if (!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			UpdateStat(girl, STAT_HAPPINESS, 10);
			UpdateTempStat(girl, STAT_LIBIDO, 10);
			g_InvManager.Equip(girl, temp, false);
			girl->m_Withdrawals = 0;
		}
	}
	if (HasTrait(girl, "Fairy Dust Addict"))
	{
		int temp = HasItem(girl, "Fairy Dust");
		if (temp == -1)	// withdrawals for a week
		{
			if (girl->m_Withdrawals >= 20)
			{
				RemoveTrait(girl, "Fairy Dust Addict");
				stringstream goodnews;
				goodnews << "Good News, " << girl->m_Realname << " has overcome her addiction to Fairy Dust.";
				girl->m_Events.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
			else
			{
				UpdateStat(girl, STAT_HAPPINESS, -30);
				UpdateStat(girl, STAT_OBEDIENCE, -30);
				UpdateStat(girl, STAT_HEALTH, -4);
				if (!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			UpdateStat(girl, STAT_HAPPINESS, 10);
			UpdateTempStat(girl, STAT_LIBIDO, 5);
			g_InvManager.Equip(girl, temp, false);
			girl->m_Withdrawals = 0;
		}
	}
	if (HasTrait(girl, "Shroud Addict"))
	{
		int temp = HasItem(girl, "Shroud Mushroom");
		if (temp == -1)	// withdrawals for a week
		{
			if (girl->m_Withdrawals >= 20)
			{
				RemoveTrait(girl, "Shroud Addict");
				stringstream goodnews;
				goodnews << "Good News, " << girl->m_Realname << " has overcome her addiction to Shroud Mushrooms.";
				girl->m_Events.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
			else
			{
				UpdateStat(girl, STAT_HAPPINESS, -30);
				UpdateStat(girl, STAT_OBEDIENCE, -30);
				UpdateStat(girl, STAT_HEALTH, -4);
				if (!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			UpdateStat(girl, STAT_HAPPINESS, 10);
			UpdateTempStat(girl, STAT_LIBIDO, 2);
			g_InvManager.Equip(girl, temp, false);
			girl->m_Withdrawals = 0;
		}
	}

	// sell crapy items
	for (int i = 0; i < girl->m_NumInventory; i++)	// use a food item if it is in stock, and remove any bad things if disobedient
	{
		if (girl->m_Inventory[i] != 0)
		{
			int max = 0;
			switch ((int)girl->m_Inventory[i]->m_Type)
			{
			case INVRING:				if (max == 0)	max = 8;
			case INVDRESS:				if (max == 0)	max = 1;
			case INVUNDERWEAR:			if (max == 0)	max = 1;
			case INVSHOES:				if (max == 0)	max = 1;
			case INVNECKLACE:			if (max == 0)	max = 1;
			case INVWEAPON:				if (max == 0)	max = 2;
			case INVSMWEAPON:			if (max == 0)	max = 2;
			case INVARMOR:				if (max == 0)	max = 1;
			case INVARMBAND:			if (max == 0)	max = 2;
				if (g_Girls.GetNumItemType(girl, girl->m_Inventory[i]->m_Type) > max) // MYR: Bug fix, was >=
				{
					int nicerThan = g_Girls.GetWorseItem(girl, girl->m_Inventory[i]->m_Type, girl->m_Inventory[i]->m_Cost);	// find a worse item of the same type
					if (nicerThan != -1)
						// `J` zzzzzzzz Add an option to have the girls put the item into "Store Room" instead of selling it
						g_Girls.SellInvItem(girl, nicerThan);
				}
				break;
			}
		}
	}

	int usedFood = (g_Dice % 3) + 1;
	int usedFoodCount = 0;
	for (int i = 0; i < girl->m_NumInventory; i++)	// use a food item if it is in stock, and remove any bad things if disobedient
	{
		sInventoryItem* curItem = girl->m_Inventory[i];
		if (curItem != 0)
		{
			if ((curItem->m_Type == INVFOOD || curItem->m_Type == INVMAKEUP) && usedFoodCount < usedFood)
			{
				if (!g_Dice.percent(curItem->m_GirlBuyChance)) continue;   // make sure she'd want it herself

				bool useful = false;					// make sure there's some reason for her to use it
				int checktouseit = curItem->m_Effects.size();
				for (u_int j = 0; j < curItem->m_Effects.size(); j++)
				{
					sEffect* curEffect = &curItem->m_Effects[j];
					if (curEffect->m_Affects == sEffect::Nothing)
					{  // really? it does nothing? sure, just for the hell of it
						checktouseit--;
					}
					else if (curEffect->m_Affects == sEffect::GirlStatus)
					{
						switch (curEffect->m_EffectID)
						{
							// these should not be used for items so skip them
						case STATUS_NONE:
						case STATUS_CATACOMBS:
						case STATUS_ARENA:
							break;
						case STATUS_HAS_DAUGHTER:
						case STATUS_HAS_SON:
							if (curEffect->m_Amount == 1)	// adopt a child item?
							{
								if (g_Dice.percent(25)) checktouseit--;		// 25% chance she wants adopt
							}
							break;
							// these statuses need to be tested individually
						case STATUS_YOURDAUGHTER:
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << STATUS_YOURDAUGHTER))
							{
								if (girl->pchate()>90 && girl->pclove()<10)	// she hates you and does not want to be your daughter
									checktouseit--;
							}
							break;
						case STATUS_PREGNANT:
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << STATUS_PREGNANT))
							{
								if (g_Dice.percent(5)) checktouseit--;		// 5% chance she wants to get rid of the baby
							}
							if (curEffect->m_Amount == 1 && !girl->is_pregnant())
							{
								if (g_Dice.percent(50)) checktouseit--;		// you gave it to her so she will consider using it
							}
							break;
						case STATUS_PREGNANT_BY_PLAYER:
							if (curEffect->m_Amount == 1 && !girl->is_pregnant())
							{
								if (girl->pchate()<10 && girl->pclove()>80)	// she love you and wants to have your child
									checktouseit--;
							}
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
							{
								if (girl->pchate()>90 && girl->pclove()<10)	// she hates you and doesn't want to have your child
									checktouseit--;
							}
							break;
						case STATUS_INSEMINATED:
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << curEffect->m_EffectID))
							{
								if (g_Dice.percent(50)) checktouseit--;	// she might not want give birth to a beast
							}
							if (curEffect->m_Amount == 1 && !girl->is_pregnant())
							{
								if (g_Dice.percent(50)) checktouseit--;	// she might want give birth to a beast
							}
							break;
							// if she has these statuses and the item removes it she will use it
						case STATUS_POISONED:
						case STATUS_BADLY_POISONED:
						case STATUS_SLAVE:
						case STATUS_CONTROLLED:
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << curEffect->m_EffectID))
							{
								checktouseit--;
							}
							break;
						}

					}
					else if (curEffect->m_Affects == sEffect::Trait)
					{
						if ((curEffect->m_Amount >= 1) != girl->has_trait(curEffect->m_Trait))
						{  // girl has trait and item removes it, or doesn't have trait and item adds it
							checktouseit--;
						}
					}
					else if (curEffect->m_Affects == sEffect::Stat)
					{
						unsigned int Stat = curEffect->m_EffectID;

						if ((curEffect->m_Amount > 0) &&
							(
							Stat == STAT_CHARISMA
							|| Stat == STAT_HAPPINESS
							|| Stat == STAT_FAME
							|| Stat == STAT_LEVEL
							|| Stat == STAT_ASKPRICE
							|| Stat == STAT_EXP
							|| Stat == STAT_BEAUTY
							)
							)
						{  // even if this stat can't be increased further, she still wants it (call it vanity, greed, whatever)
							checktouseit--;
						}
						if ((curEffect->m_Amount > 0) && (girl->m_Stats[Stat] < 100) &&
							(
							Stat == STAT_LIBIDO
							|| Stat == STAT_CONSTITUTION
							|| Stat == STAT_INTELLIGENCE
							|| Stat == STAT_CONFIDENCE
							|| Stat == STAT_MANA
							|| Stat == STAT_AGILITY
							|| Stat == STAT_SPIRIT
							|| Stat == STAT_HEALTH
							)
							)
						{  // this stat increase would be good
							checktouseit--;
						}
						if ((curEffect->m_Amount < 0) && (girl->m_Stats[Stat] > 0) &&
							(
							Stat == STAT_AGE
							|| Stat == STAT_TIREDNESS
							)
							)
						{  // decreasing this stat would actually be good
							checktouseit--;
						}
					}
					else if (curEffect->m_Affects == sEffect::Skill)
					{
						if ((curEffect->m_Amount > 0) && (girl->m_Stats[curEffect->m_EffectID] < 100))
						{  // skill would actually increase (wouldn't want to lose any skills)
							checktouseit--;
						}
					}
				}

				if (checktouseit < (int)curItem->m_Effects.size()/2) // if more than half of the effects are useful, use it
				{  // hey, this consumable item might actually be useful... gobble gobble gobble
					g_InvManager.Equip(girl, i, false);
					usedFoodCount++;
				}
			}

			// MYR: Girls shouldn't be able (IMHO) to take off things like control bracelets
			//else if(curItem->m_Badness > 20 && DisobeyCheck(girl, ACTION_GENERAL) && girl->m_EquipedItems[i] == 1)
			//{
			//	g_InvManager.Unequip(girl, i);
			//}
		}
	}

	// add the selling of items that are no longer needed here
}

bool cGirls::CanEquip(sGirl* girl, int num, bool force)
{
	if (force) return true;
	switch (girl->m_Inventory[num]->m_Type)
	{
	case INVRING:	// worn on fingers (max 8)
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 8) return false;
		break;
	case INVDRESS:	// Worn on body, (max 1)
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1) return false;
		break;
	case INVUNDERWEAR:	// Worn on body, (max 1)
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1) return false;
		break;
	case INVSHOES:	// worn on feet, (max 1)
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1) return false;
		break;
	case INVFOOD:	// Eaten, single use
		return true;
		break;
	case INVNECKLACE:	// worn on neck, (max 1)
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1) return false;
		break;
	case INVWEAPON:	// equiped on body, (max 2)
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 2) return false;
		break;
	case INVSMWEAPON: // hidden on body, (max 2)
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 2) return false;
		break;
	case INVMAKEUP:	// worn on face, single use
		return true;
		break;
	case INVARMOR:	// worn on body over dresses (max 1)
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1) return false;
		break;
	case INVMISC:	// these items don't usually do anything just random stuff girls might buy. The ones that do, cause a constant effect without having to be equiped
		return true;
		break;
	case INVARMBAND:	// (max 2), worn around arms
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 2) return false;
		break;
	}
	return true;
}

bool cGirls::IsItemEquipable(sGirl* girl, int num)
{
	switch (girl->m_Inventory[num]->m_Type)
	{
	case sInventoryItem::Ring:
	case sInventoryItem::Dress:
	case sInventoryItem::Underwear:
	case sInventoryItem::Shoes:
	case sInventoryItem::Necklace:
	case sInventoryItem::Weapon:
	case sInventoryItem::Armor:
	case sInventoryItem::Armband:
	case sInventoryItem::SmWeapon:
		return true;
	case sInventoryItem::Food:
	case sInventoryItem::Makeup:
	case sInventoryItem::Misc:
		return false;
	}
	return false;
}

bool cGirls::EquipItem(sGirl* girl, int num, bool force)
{
	if (CanEquip(girl, num, force))
	{
		g_InvManager.Equip(girl, num, force);
		return true;
	}
	return false;
}

bool cGirls::IsInvFull(sGirl* girl)
{
	bool full = true;
	if (girl)
	{
		for (int i = 0; i < 40; i++)
		{
			if (girl->m_Inventory[i] == 0)
			{
				full = false;
				break;
			}
		}
	}
	return full;
}

int cGirls::AddInv(sGirl* girl, sInventoryItem* item)
{
	int i;
	for (i = 0; i < 40; i++)
	{
		if (girl->m_Inventory[i] == 0)
		{
			girl->m_Inventory[i] = item;
			girl->m_NumInventory++;
			if (item->m_Type == INVMISC) EquipItem(girl, i, true);
			return i;  // MYR: return i for success, -1 for failure
		}
	}
	return -1;
}

bool cGirls::RemoveInvByNumber(sGirl* girl, int Pos)
{
	// Girl inventories don't stack items
	if (girl->m_Inventory[Pos] != 0)
	{
		g_InvManager.Unequip(girl, Pos);
		girl->m_Inventory[Pos] = 0;
		girl->m_NumInventory--;
		return true;
	}
	return false;
}

void cGirls::SellInvItem(sGirl* girl, int num)
{
	girl->m_Money += (int)((float)girl->m_Inventory[num]->m_Cost*0.5f);
	girl->m_NumInventory--;
	g_InvManager.Unequip(girl, num);
	girl->m_Inventory[num] = 0;
}

int cGirls::GetWorseItem(sGirl* girl, int type, int cost)
{
	int ret = -1;
	if (girl->m_NumInventory == 0) return -1;
	for (int i = 0; i < 40; i++)
	{
		if (girl->m_Inventory[i])
		{
			if (girl->m_Inventory[i]->m_Type == type && girl->m_Inventory[i]->m_Cost < cost)
			{
				ret = i;
				break;
			}
		}
	}
	return ret;
}

int cGirls::GetNumItemType(sGirl* girl, int Type)
{
	if (girl->m_NumInventory == 0) return 0;
	int num = 0;
	for (int i = 0; i < 40; i++)
	{
		if (girl->m_Inventory[i])
		{
			if (girl->m_Inventory[i]->m_Type == Type)
				num++;
		}
	}
	return num;
}

int cGirls::GetNumItemEquiped(sGirl* girl, int Type)
{
	if (girl->m_NumInventory == 0) return 0;
	int num = 0;
	for (int i = 0; i < 40; i++)
	{
		if (girl->m_Inventory[i])
		{
			if (girl->m_Inventory[i]->m_Type == Type && girl->m_EquipedItems[i] == 1) num++;
		}
	}
	return num;
}

// ----- Traits

// If a girl enjoys a job enough, she has a chance of gaining traits associated with it
bool cGirls::PossiblyGainNewTrait(sGirl* girl, string Trait, int Threshold, int ActionType, string Message, bool DayNight)
{
	if (girl->m_Enjoyment[ActionType] > Threshold && !girl->has_trait(Trait))
	{
		int chance = (girl->m_Enjoyment[ActionType] - Threshold);
		if (g_Dice.percent(chance))
		{
			girl->add_trait(Trait, false);
			girl->m_Events.AddMessage(Message, IMGTYPE_PROFILE, EVENT_WARNING);
			return true;
		}
	}
	return false;
}

// If a girl enjoys a job enough, she has a chance of losing bad traits associated with it
bool cGirls::PossiblyLoseExistingTrait(sGirl* girl, string Trait, int Threshold, int ActionType, string Message, bool DayNight)
{
	if (girl->m_Enjoyment[ActionType] > Threshold && girl->has_trait(Trait))
	{
		int chance = (girl->m_Enjoyment[ActionType] - Threshold);
		if (g_Dice.percent(chance))
		{
			girl->remove_trait(Trait);
			girl->m_Events.AddMessage(Message, IMGTYPE_PROFILE, EVENT_WARNING);
			return true;
		}
	}
	return false;
}

void cGirls::ApplyTraits(sGirl* girl, sTrait* trait, bool rememberflag)
{
	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in > ApplyTraits
	/* WD:
	*	Added doOnce = false; to end of fn
	*	else the fn will allways abort
	*/
	bool doOnce = false;
	if (trait)		doOnce = true;
	for(int i=0; i<girl->m_NumTraits || doOnce; i++)
	{
		sTrait* tr = 0;
		if(doOnce)
			tr = trait;
		else
			tr = girl->m_Traits[i];
		if(tr == 0)
			continue;
			
		//zzzzzz boobs
		if(strcmp(tr->m_Name, "Big Boobs") == 0)
		{

			UpdateStat(girl,STAT_BEAUTY,10);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_AGILITY,-5);
			UpdateStat(girl,STAT_CHARISMA,2);
			UpdateSkill(girl,SKILL_TITTYSEX,15);
		}

		else if(strcmp(tr->m_Name, "Abnormally Large Boobs") == 0)
		{
			
			UpdateStat(girl,STAT_BEAUTY,10);
			UpdateStat(girl,STAT_CONSTITUTION,10);
			UpdateStat(girl,STAT_AGILITY,-10);
			UpdateSkill(girl,SKILL_TITTYSEX,20);
		}
		
		else if(strcmp(tr->m_Name, "Small Boobs") == 0)
		{
			
			UpdateStat(girl,STAT_AGILITY,5);
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateSkill(girl,SKILL_TITTYSEX,-15);
		}

		else if (strcmp(tr->m_Name, "Fast Orgasms") == 0)
		{

			UpdateStat(girl, STAT_LIBIDO, 10);
			UpdateSkill(girl, SKILL_ANAL, 10);
			UpdateSkill(girl, SKILL_BDSM, 10);
			UpdateSkill(girl, SKILL_NORMALSEX, 10);
			UpdateSkill(girl, SKILL_ORALSEX, 10);
			UpdateSkill(girl, SKILL_BEASTIALITY, 10);
			UpdateSkill(girl, SKILL_GROUP, 10);
			UpdateSkill(girl, SKILL_LESBIAN, 10);
			UpdateStat(girl, STAT_CONFIDENCE, 10);
			UpdateEnjoyment(girl, ACTION_SEX, +10, true);
		}

		else if (strcmp(tr->m_Name, "Fake Orgasm Expert") == 0)
		{

			UpdateSkill(girl, SKILL_ANAL, 2);
			UpdateSkill(girl, SKILL_BDSM, 2);
			UpdateSkill(girl, SKILL_NORMALSEX, 2);
			UpdateSkill(girl, SKILL_BEASTIALITY, 2);
			UpdateSkill(girl, SKILL_ORALSEX, 2);
			UpdateSkill(girl, SKILL_GROUP, 2);
			UpdateSkill(girl, SKILL_LESBIAN, 2);
		}

		else if (strcmp(tr->m_Name, "Slow Orgasms") == 0)
		{

			UpdateSkill(girl,SKILL_ANAL,-2);
			UpdateSkill(girl,SKILL_BDSM,-2);
			UpdateSkill(girl,SKILL_NORMALSEX,-2);
			UpdateSkill(girl,SKILL_BEASTIALITY,-2);
			UpdateSkill(girl,SKILL_ORALSEX,-2);
			UpdateSkill(girl,SKILL_GROUP,-2);
			UpdateSkill(girl,SKILL_LESBIAN,-2);
			UpdateStat(girl,STAT_CONFIDENCE,-2);
			UpdateEnjoyment(girl, ACTION_SEX, -10, true);
		}

		else if(strcmp(tr->m_Name, "Eye Patch") == 0)
		{

			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateStat(girl,STAT_CONSTITUTION,5);
		}

		else if(strcmp(tr->m_Name, "One Eye") == 0)
		{

			UpdateStat(girl,STAT_BEAUTY,-20);
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_SPIRIT,-10);
		}

		else if(strcmp(tr->m_Name, "Fearless") == 0)
		{
			
			UpdateStat(girl,STAT_PCFEAR,-200);
			UpdateStat(girl, STAT_SPIRIT, 30);
			UpdateSkill(girl,SKILL_PERFORMANCE, 5);
			UpdateEnjoyment(girl, ACTION_COMBAT, +20, true);
		}
	
		else if(strcmp(tr->m_Name, "Meek") == 0)
		{
			
			UpdateStat(girl, STAT_CONFIDENCE, -20);
			UpdateStat(girl, STAT_OBEDIENCE, 20);
			UpdateStat(girl, STAT_SPIRIT, -20);
			UpdateSkill(girl,SKILL_PERFORMANCE, -10);
			UpdateEnjoyment(girl, ACTION_COMBAT, -20, true);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
		}
	
			
		/////////////////////////////////////////////////////////////
		else if(strcmp(tr->m_Name, "Good Kisser") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,2);
			UpdateStat(girl,STAT_CONFIDENCE,2);
			UpdateStat(girl,STAT_CHARISMA,2);
			UpdateSkill(girl,SKILL_SERVICE,5);
			UpdateEnjoyment(girl, ACTION_SEX, +5, true);
		}

		else if(strcmp(tr->m_Name, "Retarded") == 0)
		{
//	WD:		UpdateStat(girl,STAT_CONFIDENCE,-20);
			UpdateStat(girl,STAT_SPIRIT,-20);
			UpdateStat(girl,STAT_INTELLIGENCE,-50);
//	WD:		UpdateStat(girl,STAT_CONFIDENCE,-40);
			UpdateStat(girl,STAT_CONFIDENCE,-60);
		}

		else if(strcmp(tr->m_Name, "Malformed") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-20);
			UpdateStat(girl,STAT_SPIRIT,-20);
			UpdateStat(girl,STAT_INTELLIGENCE,-10);
			UpdateStat(girl,STAT_BEAUTY,-20);
		}

		else if(strcmp(tr->m_Name, "Long Legs") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,10);
		}

		else if(strcmp(tr->m_Name, "Perky Nipples") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateSkill(girl,SKILL_TITTYSEX,5);
		}

		else if(strcmp(tr->m_Name, "Puffy Nipples") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateSkill(girl,SKILL_TITTYSEX,5);
		}

		else if(strcmp(tr->m_Name, "Charismatic") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,50);
			UpdateStat(girl,STAT_BEAUTY,10);
		}

		else if(strcmp(tr->m_Name, "Charming") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,30);
			UpdateStat(girl,STAT_BEAUTY,15);
			UpdateEnjoyment(girl, ACTION_WORKBAR, +20, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, +20, true);
		}


		else if(strcmp(tr->m_Name, "Half-Construct") == 0)
		{
			UpdateStat(girl, STAT_CONSTITUTION, 20);
			UpdateSkill(girl, SKILL_COMBAT, 20);
		}


		else if (strcmp(tr->m_Name, "Fleet Of Foot") == 0)
		{
			UpdateStat(girl, STAT_AGILITY, 50);
		}

		else if(strcmp(tr->m_Name, "Dependant") == 0)
		{
			UpdateStat(girl, STAT_SPIRIT, -30);
			UpdateStat(girl, STAT_OBEDIENCE, 50);
			UpdateSkill(girl,SKILL_PERFORMANCE, -10);
			UpdateEnjoyment(girl, ACTION_COMBAT, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKBAR, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKCLUB, -20, true);
		}

		else if(strcmp(tr->m_Name, "Pessimist") == 0)
		{
			UpdateStat(girl, STAT_SPIRIT, -60);
			UpdateEnjoyment(girl, ACTION_WORKADVERTISING, -20, true);
		}

		else if(strcmp(tr->m_Name, "Optimist") == 0)
		{
			
			UpdateEnjoyment(girl, ACTION_WORKADVERTISING, +20, true);
		}

		else if(strcmp(tr->m_Name, "Iron Will") == 0)
		{
			
			UpdateStat(girl, STAT_PCFEAR, -60);
			UpdateStat(girl, STAT_SPIRIT, 60);
			UpdateStat(girl, STAT_OBEDIENCE, -90);
		}

		else if(strcmp(tr->m_Name, "Merciless") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, 20);
			UpdateEnjoyment(girl, ACTION_COMBAT, +20, true);
		}

		else if(strcmp(tr->m_Name, "Manly") == 0)
		{
			
			UpdateStat(girl, STAT_CONSTITUTION, 10);
			UpdateStat(girl, STAT_CONFIDENCE, 10);
			UpdateStat(girl, STAT_OBEDIENCE, -10);
			UpdateStat(girl, STAT_SPIRIT, 10);
		}

		else if(strcmp(tr->m_Name, "Mind Fucked") == 0)
		{
			UpdateStat(girl, STAT_CONFIDENCE, -50);
			UpdateStat(girl, STAT_OBEDIENCE, 100);
			UpdateStat(girl, STAT_SPIRIT, -50);
		}

		else if(strcmp(tr->m_Name, "Tsundere") == 0)
		{
			UpdateStat(girl, STAT_CONFIDENCE, 20);
			UpdateStat(girl, STAT_OBEDIENCE, -20);
		}

		else if(strcmp(tr->m_Name, "Yandere") == 0)
		{
			UpdateStat(girl, STAT_SPIRIT, 20);
		}

		else if(strcmp(tr->m_Name, "Lesbian") == 0)
		{
			UpdateSkill(girl, SKILL_LESBIAN, 40);
		}

		else if(strcmp(tr->m_Name, "Masochist") == 0)
		{
			UpdateSkill(girl,SKILL_BDSM, 50);
			UpdateStat(girl,STAT_CONSTITUTION, 50);
			UpdateStat(girl,STAT_OBEDIENCE,30);
		}

		else if(strcmp(tr->m_Name, "Sadistic") == 0)
		{
			UpdateSkill(girl,SKILL_BDSM, 20);
		}

		else if(strcmp(tr->m_Name, "Broken Will") == 0)
		{
			
			UpdateStat(girl,STAT_SPIRIT,-100);
			UpdateStat(girl,STAT_OBEDIENCE,100);
		}

		else if(strcmp(tr->m_Name, "Nymphomaniac") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,20);
			UpdateStat(girl,STAT_CONFIDENCE,5);
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateEnjoyment(girl, ACTION_SEX, +25, true);
		}

		else if(strcmp(tr->m_Name, "Elegant") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,10);
			UpdateStat(girl,STAT_CONFIDENCE,5);
			UpdateSkill(girl,SKILL_PERFORMANCE, 5);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKMATRON, +20, true);
		}

		else if(strcmp(tr->m_Name, "Sexy Air") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_BEAUTY,10);
			UpdateStat(girl,STAT_CONFIDENCE,2);
		}

		else if(strcmp(tr->m_Name, "Great Arse") == 0)
		{
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateSkill(girl,SKILL_ANAL,10);
		}

		else if(strcmp(tr->m_Name, "Small Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,-2);
			UpdateStat(girl,STAT_CONSTITUTION,2);
			UpdateStat(girl,STAT_SPIRIT,-2);
		}

		else if(strcmp(tr->m_Name, "Cool Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,2);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_SPIRIT,5);
			UpdateStat(girl,STAT_CHARISMA,2);
		}

		else if(strcmp(tr->m_Name, "Horrific Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateStat(girl,STAT_CONSTITUTION,10);
			UpdateStat(girl,STAT_SPIRIT,-10);
		}

		else if(strcmp(tr->m_Name, "Cool Person") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_SPIRIT,10);
			UpdateStat(girl,STAT_CONFIDENCE,10);
		}

		else if(strcmp(tr->m_Name, "Nerd") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-10);
			UpdateStat(girl,STAT_INTELLIGENCE,10);
			UpdateSkill(girl,SKILL_MEDICINE, 10);
		}

		else if(strcmp(tr->m_Name, "Clumsy") == 0)
		{
			UpdateSkill(girl,SKILL_ANAL,-10);
			UpdateSkill(girl,SKILL_BDSM,-10);
			UpdateSkill(girl,SKILL_NORMALSEX,-10);
			UpdateSkill(girl,SKILL_BEASTIALITY,-10);
			UpdateSkill(girl,SKILL_ORALSEX,-10);
			UpdateSkill(girl,SKILL_GROUP,-10);
			UpdateSkill(girl,SKILL_LESBIAN,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
			UpdateEnjoyment(girl, ACTION_WORKCLEANING, -20, true);
		}

		else if(strcmp(tr->m_Name, "Cute") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,10);
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateEnjoyment(girl, ACTION_WORKCLUB, +10, true);
			UpdateEnjoyment(girl, ACTION_WORKBAR, +20, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, +15, true);
		}

		else if(strcmp(tr->m_Name, "Strong") == 0)
		{
			UpdateSkill(girl,SKILL_COMBAT,10);
			UpdateStat(girl,STAT_CONFIDENCE,10);
		}

		else if(strcmp(tr->m_Name, "Strong Magic") == 0)
		{
			UpdateSkill(girl,SKILL_MAGIC,20);
			UpdateStat(girl,STAT_MANA,20);
		}

		else if(strcmp(tr->m_Name, "Shroud Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-5);
			UpdateStat(girl,STAT_OBEDIENCE,5);
		}

		else if(strcmp(tr->m_Name, "Fairy Dust Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-10);
			UpdateStat(girl,STAT_OBEDIENCE,5);
		}

		else if(strcmp(tr->m_Name, "Viras Blood Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,-15);
			UpdateStat(girl,STAT_OBEDIENCE,20);
			UpdateStat(girl,STAT_CHARISMA,-10);
		}

		else if(strcmp(tr->m_Name, "Aggressive") == 0)
		{
			
			UpdateSkill(girl, SKILL_COMBAT, 10);
			UpdateStat(girl, STAT_SPIRIT, 10);
			UpdateStat(girl, STAT_CONFIDENCE, 5);
			UpdateEnjoyment(girl, ACTION_COMBAT, +10, true);
			UpdateEnjoyment(girl, ACTION_WORKSECURITY, +20, true);
		}

		else if(strcmp(tr->m_Name, "Adventurer") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, 10);
			UpdateEnjoyment(girl, ACTION_COMBAT, +10, true);
		}

		else if(strcmp(tr->m_Name, "Assassin") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, 15);
			UpdateEnjoyment(girl, ACTION_COMBAT, +15, true);
			UpdateEnjoyment(girl, ACTION_WORKSECURITY, +20, true);
		}

		else if(strcmp(tr->m_Name, "Lolita") == 0)
		{
			
			UpdateStat(girl,STAT_OBEDIENCE,5);
			UpdateStat(girl,STAT_CHARISMA,10);
			UpdateStat(girl,STAT_BEAUTY,20);
		}


		else if(strcmp(tr->m_Name, "Nervous") == 0)
		{
			
			UpdateStat(girl,STAT_OBEDIENCE,10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
			UpdateSkill(girl,SKILL_PERFORMANCE, -20);
			UpdateEnjoyment(girl, ACTION_WORKCLUB, -10, true);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
			UpdateEnjoyment(girl, ACTION_WORKBAR, -5, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, -5, true);
		}

		else if(strcmp(tr->m_Name, "Great Figure") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,10);
			UpdateStat(girl,STAT_BEAUTY,20);
			UpdateEnjoyment(girl, ACTION_WORKCLUB, +20, true);
			UpdateEnjoyment(girl, ACTION_WORKBAR, +20, true);
			UpdateEnjoyment(girl, ACTION_WORKHALL, +20, true);
		}

		else if (strcmp(tr->m_Name, "Incorporeal") == 0)
		{
			UpdateEnjoyment(girl, ACTION_COMBAT, +20, true);
		}


		else if(strcmp(tr->m_Name, "Shape Shifter") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,20);
			UpdateStat(girl,STAT_BEAUTY,20);
		}

 		else if(strcmp(tr->m_Name, "Queen") == 0)
 		{
 			UpdateStat(girl,STAT_CHARISMA,20);
 			UpdateStat(girl,STAT_BEAUTY,20);
 			UpdateStat(girl,STAT_CONFIDENCE,10);
 			UpdateStat(girl,STAT_OBEDIENCE,-15);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
 		}
 
 		else if(strcmp(tr->m_Name, "Princess") == 0)
 		{
 			UpdateStat(girl,STAT_CHARISMA,10);
 			UpdateStat(girl,STAT_BEAUTY,10);
 			UpdateStat(girl,STAT_CONFIDENCE,10);
 			UpdateStat(girl,STAT_OBEDIENCE,-5);
			UpdateEnjoyment(girl, ACTION_SEX, -20, true);
 		}

		else if(strcmp(tr->m_Name, "Pierced Nipples") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,10);
			UpdateSkill(girl, SKILL_TITTYSEX, 10);
		}

 		else if(strcmp(tr->m_Name, "Pierced Tongue") == 0)
 		{
 			UpdateSkill(girl, SKILL_ORALSEX, 10);
 		}
 
 		else if(strcmp(tr->m_Name, "Pierced Clit") == 0)
 		{
 			UpdateStat(girl,STAT_LIBIDO,20);
			UpdateSkill(girl, SKILL_NORMALSEX, 10);
			UpdateEnjoyment(girl, ACTION_SEX, 10, true);
 		}
		else if(strcmp(tr->m_Name, "Gag Reflex") == 0)
		{
			UpdateSkill(girl, SKILL_ORALSEX, -50);
		}
		else if (strcmp(tr->m_Name, "No Gag Reflex") == 0)
		{
			UpdateSkill(girl, SKILL_ORALSEX, 25);
		}
		else if (strcmp(tr->m_Name, "Deep Throat") == 0)
		{
			UpdateSkill(girl, SKILL_ORALSEX, 50);
		}

		if(doOnce)
		{
			// WD: 	Added to stop fn from aborting
			// doOnce = false;
			break;
		}
	}
}

void cGirls::UnapplyTraits(sGirl* girl, sTrait* trait)
{
	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in > UnapplyTraits
	/* WD:
	*	Added doOnce = false; to end of fn
	*	else the fn will allways abort
	*/
	
	bool doOnce = (trait) ? true : false;
	for(int i=0; i<girl->m_NumTraits || doOnce; i++)
	{
		sTrait* tr = 0;
		tr = (doOnce) ? trait : girl->m_Traits[i];
		if (tr == 0) continue;

		//zzzzzz boobs
		if(strcmp(tr->m_Name, "Big Boobs") == 0)
		{

				UpdateStatMod(girl, STAT_BEAUTY, -10);
				UpdateStatMod(girl, STAT_CONSTITUTION, -5);
				UpdateStatMod(girl, STAT_AGILITY, 5);
				UpdateStatMod(girl, STAT_CHARISMA, -2);
				UpdateSkillMod(girl, SKILL_TITTYSEX, -15);
		}

		else if(strcmp(tr->m_Name, "Abnormally Large Boobs") == 0)
		{

			UpdateStat(girl,STAT_BEAUTY,-10);
			UpdateStat(girl,STAT_CONSTITUTION,-10);
			UpdateStat(girl,STAT_AGILITY,10);
			UpdateSkill(girl,SKILL_TITTYSEX,-20);
		}

		else if(strcmp(tr->m_Name, "Small Boobs") == 0)
		{

			UpdateStat(girl,STAT_AGILITY,-5);
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateSkill(girl,SKILL_TITTYSEX,10);
		}

		else if (strcmp(tr->m_Name, "Fast Orgasms") == 0)
		{

			UpdateStat(girl,STAT_LIBIDO,-10);
			UpdateSkill(girl,SKILL_ANAL,-10);
			UpdateSkill(girl,SKILL_BDSM,-10);
			UpdateSkill(girl,SKILL_NORMALSEX,-10);
			UpdateSkill(girl,SKILL_ORALSEX, -10);
			UpdateSkill(girl,SKILL_BEASTIALITY,-10);
			UpdateSkill(girl,SKILL_GROUP,-10);
			UpdateSkill(girl,SKILL_LESBIAN,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
		}

		else if (strcmp(tr->m_Name, "Fake Orgasm Expert") == 0)
		{

			UpdateSkill(girl,SKILL_ANAL,-2);
			UpdateSkill(girl,SKILL_BDSM,-2);
			UpdateSkill(girl,SKILL_NORMALSEX,-2);
			UpdateSkill(girl,SKILL_BEASTIALITY,-2);
			UpdateSkill(girl,SKILL_ORALSEX,-2);
			UpdateSkill(girl,SKILL_GROUP,-2);
			UpdateSkill(girl,SKILL_LESBIAN,-2);
		}

		else if (strcmp(tr->m_Name, "Slow Orgasms") == 0)
		{

			UpdateSkill(girl,SKILL_ANAL,2);
			UpdateSkill(girl,SKILL_BDSM,2);
			UpdateSkill(girl,SKILL_NORMALSEX,2);
			UpdateSkill(girl,SKILL_BEASTIALITY,2);
			UpdateSkill(girl,SKILL_ORALSEX,2);
			UpdateSkill(girl,SKILL_GROUP,2);
			UpdateSkill(girl,SKILL_LESBIAN,2);
			UpdateStat(girl,STAT_CONFIDENCE,2);
		}


		else if(strcmp(tr->m_Name, "Eye Patch") == 0)
		{

			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateStat(girl,STAT_CONSTITUTION,-5);
		}

		else if(strcmp(tr->m_Name, "One Eye") == 0)
		{
			
			UpdateStat(girl,STAT_BEAUTY,20);
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_CONSTITUTION,-5);
			UpdateStat(girl,STAT_SPIRIT,10);
		}

		else if(strcmp(tr->m_Name, "Fearless") == 0)
		{
			
			UpdateStat(girl,STAT_PCFEAR,-200);
			UpdateStat(girl, STAT_SPIRIT, 30);
			UpdateSkill(girl,SKILL_PERFORMANCE, -5);
		}

		else if(strcmp(tr->m_Name, "Meek") == 0)
		{
			
			UpdateStat(girl, STAT_CONFIDENCE, 20);
			UpdateStat(girl, STAT_OBEDIENCE, -20);
			UpdateStat(girl, STAT_SPIRIT, 20);
			UpdateSkill(girl,SKILL_PERFORMANCE, 10);
		}

		/////////////////////////////////////////////////////////////
		else if(strcmp(tr->m_Name, "Good Kisser") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,-2);
			UpdateStat(girl,STAT_CONFIDENCE,-2);
			UpdateStat(girl,STAT_CHARISMA,-2);
			UpdateSkill(girl,SKILL_SERVICE,-5);
		}

		else if(strcmp(tr->m_Name, "Long Legs") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,-10);
		}

		else if(strcmp(tr->m_Name, "Perky Nipples") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateSkill(girl,SKILL_TITTYSEX,-5);
		}

		else if(strcmp(tr->m_Name, "Retarded") == 0)
		{
			UpdateStat(girl,STAT_SPIRIT,20);
			UpdateStat(girl,STAT_INTELLIGENCE,50);
			UpdateStat(girl,STAT_CONFIDENCE,60);
		}

		else if(strcmp(tr->m_Name, "Malformed") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,20);
			UpdateStat(girl,STAT_SPIRIT,20);
			UpdateStat(girl,STAT_INTELLIGENCE,10);
			UpdateStat(girl,STAT_BEAUTY,20);
		}

		else if(strcmp(tr->m_Name, "Puffy Nipples") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateSkill(girl,SKILL_TITTYSEX,-5);
		}

		else if(strcmp(tr->m_Name, "Charming") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-30);
			UpdateStat(girl,STAT_BEAUTY,-15);
		}

		else if(strcmp(tr->m_Name, "Charismatic") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-50);
			UpdateStat(girl,STAT_BEAUTY,-10);
		}


		else if(strcmp(tr->m_Name, "Half-Construct") == 0)
		{
			UpdateStat(girl, STAT_CONSTITUTION, -20);
			UpdateSkill(girl, SKILL_COMBAT, -20);
		}


		else if(strcmp(tr->m_Name, "Mind Fucked") == 0)
		{
			UpdateStat(girl, STAT_CONFIDENCE, 50);
			UpdateStat(girl, STAT_OBEDIENCE, -100);
			UpdateStat(girl, STAT_SPIRIT, 50);
		}

		else if (strcmp(tr->m_Name, "Fleet of Foot") == 0 || strcmp(tr->m_Name, "Fleet Of Foot") == 0)
		{
			UpdateStat(girl, STAT_AGILITY, -50);
		}

		else if(strcmp(tr->m_Name, "Dependant") == 0)
		{
			UpdateStat(girl, STAT_SPIRIT, 30);
			UpdateStat(girl, STAT_OBEDIENCE, -50);
			UpdateSkill(girl,SKILL_PERFORMANCE, 10);
		}

		else if(strcmp(tr->m_Name, "Pessimist") == 0)
		{
			
			UpdateStat(girl, STAT_SPIRIT, 60);
		}


		else if(strcmp(tr->m_Name, "Iron Will") == 0)
		{
			
			UpdateStat(girl, STAT_PCFEAR, 60);
			UpdateStat(girl, STAT_SPIRIT, -60);
			UpdateStat(girl, STAT_OBEDIENCE, 90);
		}

		else if(strcmp(tr->m_Name, "Merciless") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, -20);
		}

		else if(strcmp(tr->m_Name, "Manly") == 0)
		{
			
			UpdateStat(girl, STAT_CONSTITUTION, -10);
			UpdateStat(girl, STAT_CONFIDENCE, -10);
			UpdateStat(girl, STAT_OBEDIENCE, 10);
			UpdateStat(girl, STAT_SPIRIT, -10);
		}

		else if(strcmp(tr->m_Name, "Tsundere") == 0)
		{
			UpdateStat(girl, STAT_CONFIDENCE, -20);
			UpdateStat(girl, STAT_OBEDIENCE, 20);
		}

		else if(strcmp(tr->m_Name, "Yandere") == 0)
		{
			UpdateStat(girl, STAT_SPIRIT, -20);
		}

		else if(strcmp(tr->m_Name, gettext("Lesbian")) == 0)
		{
			UpdateSkill(girl, SKILL_LESBIAN, -40);
		}

		else if(strcmp(tr->m_Name, "Masochist") == 0)
		{
			UpdateSkill(girl,SKILL_BDSM, -50);
			UpdateStat(girl,STAT_CONSTITUTION, -50);
			UpdateStat(girl,STAT_OBEDIENCE,-30);
		}

		else if(strcmp(tr->m_Name, "Sadistic") == 0)
		{
			UpdateSkill(girl,SKILL_BDSM, -20);
		}

		else if(strcmp(tr->m_Name, "Broken Will") == 0)
		{
			
			UpdateStat(girl,STAT_SPIRIT, 20);
			UpdateStat(girl,STAT_OBEDIENCE, 10);
		}

		else if(strcmp(tr->m_Name, "Nymphomaniac") == 0)
		{
			UpdateStat(girl,STAT_LIBIDO,-20);
			UpdateStat(girl,STAT_CONFIDENCE,-5);
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateStat(girl,STAT_CONSTITUTION,-5);
			UpdateEnjoyment(girl, ACTION_SEX, -21, true);
		}

		else if(strcmp(tr->m_Name, "Elegant") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-5);
			UpdateSkill(girl,SKILL_PERFORMANCE, -5);
		}

		else if(strcmp(tr->m_Name, "Sexy Air") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,5);
			UpdateStat(girl,STAT_BEAUTY,10);
			UpdateStat(girl,STAT_CONFIDENCE,2);
		}

		else if(strcmp(tr->m_Name, "Great Arse") == 0)
		{
			UpdateStat(girl,STAT_CONSTITUTION,-5);
			UpdateStat(girl,STAT_BEAUTY,-5);
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateSkill(girl,SKILL_ANAL,-10);
		}

		else if(strcmp(tr->m_Name, "Small Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,2);
			UpdateStat(girl,STAT_CONSTITUTION,-2);
			UpdateStat(girl,STAT_SPIRIT,2);
		}

		else if(strcmp(tr->m_Name, "Cool Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,2);
			UpdateStat(girl,STAT_CONSTITUTION,5);
			UpdateStat(girl,STAT_SPIRIT,5);
			UpdateStat(girl,STAT_CHARISMA,2);
		}

		else if(strcmp(tr->m_Name, "Horrific Scars") == 0)
		{
			UpdateStat(girl,STAT_BEAUTY,5);
			UpdateStat(girl,STAT_CONSTITUTION,-10);
			UpdateStat(girl,STAT_SPIRIT,10);
		}

		else if(strcmp(tr->m_Name, "Cool Person") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-5);
			UpdateStat(girl,STAT_SPIRIT,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
		}

		else if(strcmp(tr->m_Name, "Nerd") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,10);
			UpdateSkill(girl,SKILL_MEDICINE, -10);
		}

		else if(strcmp(tr->m_Name, "Clumsy") == 0)
		{
			UpdateSkill(girl,SKILL_ANAL,10);
			UpdateSkill(girl,SKILL_BDSM,10);
			UpdateSkill(girl,SKILL_NORMALSEX,10);
			UpdateSkill(girl,SKILL_ORALSEX,10);
			UpdateSkill(girl,SKILL_BEASTIALITY,10);
			UpdateSkill(girl,SKILL_GROUP,10);
			UpdateSkill(girl,SKILL_LESBIAN,10);
			UpdateStat(girl,STAT_CONFIDENCE,10);
		}

		else if(strcmp(tr->m_Name, "Cute") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-10);
			UpdateStat(girl,STAT_BEAUTY,-5);
		}

		else if(strcmp(tr->m_Name, "Strong") == 0)
		{
			UpdateSkill(girl,SKILL_COMBAT,-10);
			UpdateStat(girl,STAT_CONFIDENCE,-10);
		}

		else if(strcmp(tr->m_Name, "Strong Magic") == 0)
		{
			UpdateSkill(girl,SKILL_MAGIC,-20);
			UpdateStat(girl,STAT_MANA,-20);
		}

		else if(strcmp(tr->m_Name, "Shroud Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,5);
			UpdateStat(girl,STAT_CONFIDENCE,-5);
		}

		else if(strcmp(tr->m_Name, "Fairy Dust Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,10);
			UpdateStat(girl,STAT_OBEDIENCE,-5);
		}

		else if(strcmp(tr->m_Name, "Viras Blood Addict") == 0)
		{
			UpdateStat(girl,STAT_CONFIDENCE,15);
			UpdateStat(girl,STAT_OBEDIENCE,-20);
			UpdateStat(girl,STAT_CHARISMA,10);
		}

		else if(strcmp(tr->m_Name, "Aggressive") == 0)
		{
			
			UpdateSkill(girl, SKILL_COMBAT, -10);
			UpdateStat(girl, STAT_SPIRIT, -10);
			UpdateStat(girl, STAT_CONFIDENCE, -5);
		}

		else if(strcmp(tr->m_Name, "Adventurer") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, -10);
		}

		else if(strcmp(tr->m_Name, "Assassin") == 0)
		{
			UpdateSkill(girl, SKILL_COMBAT, -15);
		}

		else if(strcmp(tr->m_Name, "Lolita") == 0)
		{
			
			UpdateStat(girl,STAT_OBEDIENCE,-5);
			UpdateStat(girl,STAT_CHARISMA,-10);
			UpdateStat(girl,STAT_BEAUTY,-20);
		}


		else if(strcmp(tr->m_Name, "Nervous") == 0)
		{
			
			UpdateStat(girl,STAT_OBEDIENCE,-10);
			UpdateStat(girl,STAT_CONFIDENCE,10);
			UpdateSkill(girl,SKILL_PERFORMANCE, 20);
		}

		else if(strcmp(tr->m_Name, "Great Figure") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-10);
			UpdateStat(girl,STAT_BEAUTY,-20);
		}

		else if (strcmp(tr->m_Name, "Incorporeal") == 0)
		{
		}


		else if(strcmp(tr->m_Name, "Shape Shifter") == 0)
		{
			UpdateStat(girl,STAT_CHARISMA,-20);
			UpdateStat(girl,STAT_BEAUTY,-20);
		}

 		else if(strcmp(tr->m_Name, "Queen") == 0)
 		{
 			UpdateStat(girl,STAT_CHARISMA,-20);
 			UpdateStat(girl,STAT_BEAUTY,-20);
 			UpdateStat(girl,STAT_CONFIDENCE,-10);
 			UpdateStat(girl,STAT_OBEDIENCE,15);
 		}
 
 		else if(strcmp(tr->m_Name, "Princess") == 0)
 		{
 			UpdateStat(girl,STAT_CHARISMA,-10);
 			UpdateStat(girl,STAT_BEAUTY,-10);
 			UpdateStat(girl,STAT_CONFIDENCE,-10);
 			UpdateStat(girl,STAT_OBEDIENCE,5);
 		}

		else if(strcmp(tr->m_Name, "Pierced Nipples") == 0)
 		{
 			UpdateStat(girl,STAT_LIBIDO,-10);
			UpdateSkill(girl, SKILL_TITTYSEX, -10);
 		}
 
 		else if(strcmp(tr->m_Name, "Pierced Tongue") == 0)
 		{
 			UpdateSkill(girl, SKILL_ORALSEX, -10);
 		}

		else if(strcmp(tr->m_Name, "Pierced Clit") == 0)
 		{
 			UpdateStat(girl,STAT_LIBIDO,-20);
			UpdateSkill(girl, SKILL_NORMALSEX, -10);
 		}
		else if(strcmp(tr->m_Name, "Gag Reflex") == 0)
		{
			UpdateSkill(girl, SKILL_ORALSEX, 50);
		}
		else if (strcmp(tr->m_Name, "No Gag Reflex") == 0)
		{
			UpdateSkill(girl, SKILL_ORALSEX, -25);
		}
		else if (strcmp(tr->m_Name, "Deep Throat") == 0)
		{
			UpdateSkill(girl, SKILL_ORALSEX, -50);
		}

		if(doOnce)
		{
			// WD: 	Added to stop fn from aborting
			//doOnce = false;
			//return i;
			break;
		}
	}
//	return -1;
}

void cGirls::MutuallyExclusiveTraits(sGirl* girl, bool apply, sTrait* trait, bool rememberflag)
{
	bool doOnce = (trait) ? true : false;
	for (int i = 0; i < girl->m_NumTraits || doOnce; i++)
	{
		sTrait* tr = 0;
		tr = (doOnce) ? trait : girl->m_Traits[i];
		if (tr == 0) continue;
		string name = tr->m_Name;
		if (name == "") continue;

		// `J` base for adding new mutually exclusive traits

		// no need to comment this out because it will always fail to be true because of the first if check

		else if (	// Check _type_ Traits
			name == "" ||
			name == "" ||
			name == "" ||
			name == "")
		{
			if (apply)
			{
				if (name != "")			RemoveTrait(girl, "", rememberflag, true);
				if (name != "")			RemoveTrait(girl, "", rememberflag, true);
				if (name != "")			RemoveTrait(girl, "", rememberflag, true);
				if (name != "")			RemoveTrait(girl, "", rememberflag, true);
			}
			else
			{
				/* */if (name != "" && HasRememberedTrait(girl, ""))	AddTrait(girl, "", false, false, true);
				else if (name != "" && HasRememberedTrait(girl, ""))	AddTrait(girl, "", false, false, true);
				else if (name != "" && HasRememberedTrait(girl, ""))	AddTrait(girl, "", false, false, true);
				else if (name != "" && HasRememberedTrait(girl, ""))	AddTrait(girl, "", false, false, true);
			}
		}

		//	`J` end of base for adding new mutually exclusive traits

#if 1	// Start of Breast traits

		else if (	// Check Breast size traits
			name == "Flat Chest" ||
			name == "Petite Breasts" ||
			name == "Small Boobs" ||
			name == "Busty Boobs" ||
			name == "Big Boobs" ||
			name == "Giant Juggs" ||
			name == "Massive Melons" ||
			name == "Abnormally Large Boobs" ||
			name == "Titanic Tits")
		{
			if (apply)
			{
				if (name != "Flat Chest")				RemoveTrait(girl, "Flat Chest", rememberflag, true);
				if (name != "Petite Breasts")			RemoveTrait(girl, "Petite Breasts", rememberflag, true);
				if (name != "Small Boobs")				RemoveTrait(girl, "Small Boobs", rememberflag, true);
				if (name != "Busty Boobs")				RemoveTrait(girl, "Busty Boobs", rememberflag, true);
				if (name != "Big Boobs")				RemoveTrait(girl, "Big Boobs", rememberflag, true);
				if (name != "Giant Juggs")				RemoveTrait(girl, "Giant Juggs", rememberflag, true);
				if (name != "Massive Melons")			RemoveTrait(girl, "Massive Melons", rememberflag, true);
				if (name != "Abnormally Large Boobs")	RemoveTrait(girl, "Abnormally Large Boobs", rememberflag, true);
				if (name != "Titanic Tits")				RemoveTrait(girl, "Titanic Tits", rememberflag, true);
			}
			else
			{
				/* */if (name != "Flat Chest" && HasRememberedTrait(girl, "Flat Chest"))							AddTrait(girl, "Flat Chest", false, false, true);
				else if (name != "Petite Breasts" && HasRememberedTrait(girl, "Petite Breasts"))					AddTrait(girl, "Petite Breasts", false, false, true);
				else if (name != "Small Boobs" && HasRememberedTrait(girl, "Small Boobs"))							AddTrait(girl, "Small Boobs", false, false, true);
				else if (name != "Busty Boobs" && HasRememberedTrait(girl, "Busty Boobs"))							AddTrait(girl, "Busty Boobs", false, false, true);
				else if (name != "Big Boobs" && HasRememberedTrait(girl, "Big Boobs"))								AddTrait(girl, "Big Boobs", false, false, true);
				else if (name != "Giant Juggs" && HasRememberedTrait(girl, "Giant Juggs"))							AddTrait(girl, "Giant Juggs", false, false, true);
				else if (name != "Massive Melons" && HasRememberedTrait(girl, "Massive Melons"))					AddTrait(girl, "Massive Melons", false, false, true);
				else if (name != "Abnormally Large Boobs" && HasRememberedTrait(girl, "Abnormally Large Boobs"))	AddTrait(girl, "Abnormally Large Boobs", false, false, true);
				else if (name != "Titanic Tits" && HasRememberedTrait(girl, "Titanic Tits"))						AddTrait(girl, "Titanic Tits", false, false, true);
			}
		}
		else if (	// Check Lactation Traits
			name == "Dry Milk" ||
			name == "Scarce Lactation" ||
			name == "Abundant Lactation" ||
			name == "Cow Tits")
		{

			if (apply)
			{
				if (HasTrait(girl, "No Nipples"))
				{
					// if she has no nipples she can not produce milk, but remember that is has changed in case she grows nipples
					RemoveTrait(girl, "Dry Milk", false, true, name == "Dry Milk");
					RemoveTrait(girl, "Scarce Lactation", false, true, name == "Scarce Lactation");
					RemoveTrait(girl, "Abundant Lactation", false, true, name == "Abundant Lactation");
					RemoveTrait(girl, "Cow Tits", false, true, name == "Cow Tits");
				}
				else
				{
					if (name != "Dry Milk")				RemoveTrait(girl, "Dry Milk", rememberflag, true);
					if (name != "Scarce Lactation")		RemoveTrait(girl, "Scarce Lactation", rememberflag, true);
					if (name != "Abundant Lactation")	RemoveTrait(girl, "Abundant Lactation", rememberflag, true);
					if (name != "Cow Tits")				RemoveTrait(girl, "Cow Tits", rememberflag, true);
				}
			}
			else
			{
				/* */if (name != "Dry Milk" && HasRememberedTrait(girl, "Dry Milk"))						AddTrait(girl, "Dry Milk", false, false, true);
				else if (name != "Scarce Lactation" && HasRememberedTrait(girl, "Scarce Lactation"))		AddTrait(girl, "Scarce Lactation", false, false, true);
				else if (name != "Abundant Lactation" && HasRememberedTrait(girl, "Abundant Lactation"))	AddTrait(girl, "Abundant Lactation", false, false, true);
				else if (name != "Cow Tits" && HasRememberedTrait(girl, "Cow Tits"))						AddTrait(girl, "Cow Tits", false, false, true);
				
				// make it easy by adding the trait as usual and then move it to remembered if she has no nipples
				if (HasTrait(girl, "No Nipples"))
				{
					// if she has no nipples she can not produce milk, but remember that is has changed in case she grows nipples
					RemoveTrait(girl, "Dry Milk", true, true, HasTrait(girl, "Dry Milk"));
					RemoveTrait(girl, "Scarce Lactation", true, true, HasTrait(girl, "Scarce Lactation"));
					RemoveTrait(girl, "Abundant Lactation", true, true, HasTrait(girl, "Abundant Lactation"));
					RemoveTrait(girl, "Cow Tits", true, true, HasTrait(girl, "Cow Tits"));
				}
			}
		}
		else if (	// Check Nipple Traits
			name == "No Nipples" ||
			name == "Missing Nipple" ||
			name == "Inverted Nipples" ||
			name == "Perky Nipples" ||
			name == "Puffy Nipples")
		{
			if (apply)
			{
				// if adding "No Nipples" remove all other nipple traits but remember them
				if (name == "No Nipples" || HasTrait(girl, "No Nipples"))
				{
					if (HasTrait(girl, "Dry Milk"))				RemoveTrait(girl, "Dry Milk", true, true, true);
					if (HasTrait(girl, "Scarce Lactation"))		RemoveTrait(girl, "Scarce Lactation", true, true, true);
					if (HasTrait(girl, "Abundant Lactation"))	RemoveTrait(girl, "Abundant Lactation", true, true, true);
					if (HasTrait(girl, "Cow Tits"))				RemoveTrait(girl, "Cow Tits", true, true, true);

					if (name == "Missing Nipple" || HasTrait(girl, "Missing Nipple"))	RemoveTrait(girl, "Missing Nipple", rememberflag, true, true);
					if (name == "Puffy Nipples" || HasTrait(girl, "Puffy Nipples"))		RemoveTrait(girl, "Puffy Nipples", rememberflag, true, true);
					if (name == "Inverted Nipples" || HasTrait(girl, "Perky Nipples"))	RemoveTrait(girl, "Perky Nipples", rememberflag, true, true);
					if (name == "Perky Nipples" || HasTrait(girl, "Inverted Nipples"))	RemoveTrait(girl, "Inverted Nipples", rememberflag, true, true);
				}
				else 
				{
					// she can not have both Inverted and Perky but Puffy can go with either
					if (name == "Inverted Nipples")		RemoveTrait(girl, "Perky Nipples", rememberflag, true);
					if (name == "Perky Nipples")		RemoveTrait(girl, "Inverted Nipples", rememberflag, true);
				}
			}
			else
			{
				// if removing "No Nipples" try adding back the others
				/* */if (name == "No Nipples")
				{
					/* */if (HasRememberedTrait(girl, "Dry Milk"))				AddTrait(girl, "Dry Milk", false, false, true);
					else if (HasRememberedTrait(girl, "Scarce Lactation"))		AddTrait(girl, "Scarce Lactation", false, false, true);
					else if (HasRememberedTrait(girl, "Abundant Lactation"))	AddTrait(girl, "Abundant Lactation", false, false, true);
					else if (HasRememberedTrait(girl, "Cow Tits"))				AddTrait(girl, "Cow Tits", false, false, true);
					/* */if (HasRememberedTrait(girl, "Puffy Nipples"))			AddTrait(girl, "Puffy Nipples", false, false, true);
				}
				/* */if ((name == "No Nipples" || (!HasTrait(girl, "No Nipples") && name == "Inverted Nipples"))
					&& HasRememberedTrait(girl, "Perky Nipples"))	
					AddTrait(girl, "Perky Nipples", false, false, true);
				else if ((name == "No Nipples" || (!HasTrait(girl, "No Nipples") && name == "Perky Nipples"))
					&& HasRememberedTrait(girl, "Inverted Nipples"))	
					AddTrait(girl, "Inverted Nipples", false, false, true);
			}
		}

#endif	// End of Breast traits

#if 1	// Start of Traits that affect sex

		else if (	// Check Fertility Traits
			name == "Sterile" ||
			name == "Broodmother" ||
			name == "Fertile")
		{
			if (apply)
			{
				if (name != "Sterile")		RemoveTrait(girl, "Sterile", rememberflag, true);
				if (name != "Broodmother")	RemoveTrait(girl, "Broodmother", rememberflag, true);
				if (name != "Fertile")		RemoveTrait(girl, "Fertile", rememberflag, true);
			}
			else
			{
				/* */if (name != "Sterile" && HasRememberedTrait(girl, "Sterile"))			AddTrait(girl, "Sterile", false, false, true);
				else if (name != "Broodmother" && HasRememberedTrait(girl, "Broodmother"))	AddTrait(girl, "Broodmother", false, false, true);
				else if (name != "Fertile" && HasRememberedTrait(girl, "Fertile"))			AddTrait(girl, "Fertile", false, false, true);
			}
		}
		else if (	// Check Sexuality Traits
			name == "Bisexual" ||
			name == "Lesbian" ||
			name == "Straight")
		{
			if (apply)
			{
				if (name != "Bisexual")		RemoveTrait(girl, "Bisexual", rememberflag, true);
				if (name != "Lesbian")		RemoveTrait(girl, "Lesbian", rememberflag, true);
				if (name != "Straight")		RemoveTrait(girl, "Straight", rememberflag, true);
			}
			else
			{
				/* */if (name != "Bisexual" && HasRememberedTrait(girl, "Bisexual"))	AddTrait(girl, "Bisexual", false, false, true);
				else if (name != "Lesbian" && HasRememberedTrait(girl, "Lesbian"))		AddTrait(girl, "Lesbian", false, false, true);
				else if (name != "Straight" && HasRememberedTrait(girl, "Straight"))	AddTrait(girl, "Straight", false, false, true);
			}
		}
		else if (	// Check Gag Reflex Traits
			name == "Strong Gag Reflex" ||
			name == "Gag Reflex" ||
			name == "No Gag Reflex" ||
			name == "Deep Throat")
		{
			if (apply)
			{
				if (name != "Strong Gag Reflex")		RemoveTrait(girl, "Strong Gag Reflex", rememberflag, true);
				if (name != "Gag Reflex")				RemoveTrait(girl, "Gag Reflex", rememberflag, true);
				if (name != "No Gag Reflex")			RemoveTrait(girl, "No Gag Reflex", rememberflag, true);
				if (name != "Deep Throat")				RemoveTrait(girl, "Deep Throat", rememberflag, true);
			}
			else
			{
				/* */if (name != "Strong Gag Reflex" && HasRememberedTrait(girl, "Strong Gag Reflex"))	AddTrait(girl, "Strong Gag Reflex", false, false, true);
				else if (name != "Gag Reflex" && HasRememberedTrait(girl, "Gag Reflex"))				AddTrait(girl, "Gag Reflex", false, false, true);
				else if (name != "No Gag Reflex" && HasRememberedTrait(girl, "No Gag Reflex"))			AddTrait(girl, "No Gag Reflex", false, false, true);
				else if (name != "Deep Throat" && HasRememberedTrait(girl, "Deep Throat"))				AddTrait(girl, "Deep Throat", false, false, true);
			}
		}
		else if (	// Check Orgasm Traits
			name == "Fake Orgasm Expert" ||
			name == "Fast Orgasms" ||
			name == "Slow Orgasms")
		{
			if (apply)
			{
				// "Fake Orgasm Expert" overrides the other 2 so if adding it, remove the others but if adding the others don't remove it 
				if (name != "Fast Orgasms")			RemoveTrait(girl, "Fast Orgasms", rememberflag, true);
				if (name != "Slow Orgasms")			RemoveTrait(girl, "Slow Orgasms", rememberflag, true);
			}
			else
			{
				/* */if (name != "Fake Orgasm Expert" && HasRememberedTrait(girl, "Fake Orgasm Expert"))	AddTrait(girl, "Fake Orgasm Expert", false, false, true);
				else if (name != "Fast Orgasms" && HasRememberedTrait(girl, "Fast Orgasms"))				AddTrait(girl, "Fast Orgasms", false, false, true);
				else if (name != "Slow Orgasms" && HasRememberedTrait(girl, "Slow Orgasms"))				AddTrait(girl, "Slow Orgasms", false, false, true);
			}
		}

#endif	// End of Traits that affect sex

#if 1	// Start of Physical traits

		else if (	// Check Fragile/Tough Traits
			name == "Fragile" ||
			name == "Tough")
		{
			if (apply)
			{
				if (name != "Fragile")			RemoveTrait(girl, "Fragile", rememberflag, true);
				if (name != "Tough")			RemoveTrait(girl, "Tough", rememberflag, true);
			}
			else
			{
				/* */if (name != "Fragile" && HasRememberedTrait(girl, "Fragile"))	AddTrait(girl, "Fragile", false, false, true);
				else if (name != "Tough" && HasRememberedTrait(girl, "Tough"))		AddTrait(girl, "Tough", false, false, true);
			}
		}
		else if (	// Check Construct Traits
			name == "Construct" ||
			name == "Half-Construct")
		{
			if (apply)
			{
				if (name != "Construct")			RemoveTrait(girl, "Construct", rememberflag, true);
				if (name != "Half-Construct")		RemoveTrait(girl, "Half-Construct", rememberflag, true);
			}
			else
			{
				/* */if (name != "Construct" && HasRememberedTrait(girl, "Construct"))				AddTrait(girl, "Construct", false, false, true);
				else if (name != "Half-Construct" && HasRememberedTrait(girl, "Half-Construct"))	AddTrait(girl, "Half-Construct", false, false, true);
			}
		}
		else if (	// Check Ass Traits
			name == "Flat Ass" ||
			name == "Tight Butt" ||
			name == "Plump Tush" ||
			name == "Great Arse" ||
			name == "Phat Booty" ||
			name == "Deluxe Derriere")
		{
			if (apply)
			{
				if (name != "Flat Ass")				RemoveTrait(girl, "Flat Ass", rememberflag, true);
				if (name != "Tight Butt")			RemoveTrait(girl, "Tight Butt", rememberflag, true);
				if (name != "Plump Tush")			RemoveTrait(girl, "Plump Tush", rememberflag, true);
				if (name != "Great Arse")			RemoveTrait(girl, "Great Arse", rememberflag, true);
				if (name != "Phat Booty")			RemoveTrait(girl, "Phat Booty", rememberflag, true);
				if (name != "Deluxe Derriere")		RemoveTrait(girl, "Deluxe Derriere", rememberflag, true);
			}
			else
			{
				/* */if (name != "Flat Ass" && HasRememberedTrait(girl, "Flat Ass"))				AddTrait(girl, "Flat Ass", false, false, true);
				else if (name != "Tight Butt" && HasRememberedTrait(girl, "Tight Butt"))			AddTrait(girl, "Tight Butt", false, false, true);
				else if (name != "Plump Tush" && HasRememberedTrait(girl, "Plump Tush"))			AddTrait(girl, "Plump Tush", false, false, true);
				else if (name != "Great Arse" && HasRememberedTrait(girl, "Great Arse"))			AddTrait(girl, "Great Arse", false, false, true);
				else if (name != "Phat Booty" && HasRememberedTrait(girl, "Phat Booty"))			AddTrait(girl, "Phat Booty", false, false, true);
				else if (name != "Deluxe Derriere" && HasRememberedTrait(girl, "Deluxe Derriere"))	AddTrait(girl, "Deluxe Derriere", false, false, true);
			}
		}
		else if (	// Check Teeth Traits
			name == "Missing Teeth" ||
			name == "No Teeth")
		{
			if (apply)
			{
				if (name != "Missing Teeth")	RemoveTrait(girl, "Missing Teeth", rememberflag, true);
				if (name != "No Teeth")			RemoveTrait(girl, "No Teeth", rememberflag, true);
			}
			else
			{
				/* */if (name != "Missing Teeth" && HasRememberedTrait(girl, "Missing Teeth"))	AddTrait(girl, "Missing Teeth", false, false, true);
				else if (name != "No Teeth" && HasRememberedTrait(girl, "No Teeth"))			AddTrait(girl, "No Teeth", false, false, true);
			}
		}


#endif	// End of Physical traits

#if 1	// Start of Appearance traits

		else if (	// Check Tattoo Traits
			name == "Small Tattoos" ||
			name == "Tattooed" ||
			name == "Heavily Tattooed")
		{
			if (apply)
			{
				if (name != "Small Tattoos")		RemoveTrait(girl, "Small Tattoos", rememberflag, true);
				if (name != "Tattooed")				RemoveTrait(girl, "Tattooed", rememberflag, true);
				if (name != "Heavily Tattooed")		RemoveTrait(girl, "Heavily Tattooed", rememberflag, true);
			}
			else
			{
				/* */if (name != "Small Tattoos" && HasRememberedTrait(girl, "Small Tattoos"))			AddTrait(girl, "Small Tattoos", false, false, true);
				else if (name != "Tattooed" && HasRememberedTrait(girl, "Tattooed"))					AddTrait(girl, "Tattooed", false, false, true);
				else if (name != "Heavily Tattooed" && HasRememberedTrait(girl, "Heavily Tattooed"))	AddTrait(girl, "Heavily Tattooed", false, false, true);
			}
		}

#endif	// End of Appearance traits

#if 1	// Start of Magical traits

		else if (	// Check Magic Strength Traits
			name == "Muggle" ||
			name == "Weak Magic" ||
			name == "Strong Magic" ||
			name == "Powerful Magic")
		{
			if (apply)
			{
				if (name != "Muggle")			RemoveTrait(girl, "Muggle", rememberflag, true);
				if (name != "Weak Magic")		RemoveTrait(girl, "Weak Magic", rememberflag, true);
				if (name != "Strong Magic")		RemoveTrait(girl, "Strong Magic", rememberflag, true);
				if (name != "Powerful Magic")	RemoveTrait(girl, "Powerful Magic", rememberflag, true);
			}
			else
			{
				/* */if (name != "Muggle" && HasRememberedTrait(girl, "Muggle"))					AddTrait(girl, "Muggle", false, false, true);
				else if (name != "Weak Magic" && HasRememberedTrait(girl, "Weak Magic"))			AddTrait(girl, "Weak Magic", false, false, true);
				else if (name != "Strong Magic" && HasRememberedTrait(girl, "Strong Magic"))		AddTrait(girl, "Strong Magic", false, false, true);
				else if (name != "Powerful Magic" && HasRememberedTrait(girl, "Powerful Magic"))	AddTrait(girl, "Powerful Magic", false, false, true);
			}
		}

#endif	// End of Magical traits

#if 1	// Start of Social Traits

		else if (	// Check Princess/Queen Traits
			name == "Princess" ||
			name == "Queen")
		{
			if (apply)
			{
				if (name != "Princess")			RemoveTrait(girl, "Princess", rememberflag, true);
				if (name != "Queen")			RemoveTrait(girl, "Queen", rememberflag, true);
			}
			else
			{
				/* */if (name != "Princess" && HasRememberedTrait(girl, "Princess"))	AddTrait(girl, "Princess", false, false, true);
				else if (name != "Queen" && HasRememberedTrait(girl, "Queen"))			AddTrait(girl, "Queen", false, false, true);
			}
		}
		else if (	// Check Lolita/MILF Traits
			name == "Lolita" ||
			name == "MILF" ||
			name == "Old")
		{
			if (apply)
			{
				// If adding Lolita remove the others but if adding the others only remove Lolita
				if (name == "Lolita")
				{
					RemoveTrait(girl, "MILF", rememberflag, true);
					RemoveTrait(girl, "Old", rememberflag, true);
				}
				if (name == "MILF")			RemoveTrait(girl, "Lolita", rememberflag, true);
				if (name == "Old")			RemoveTrait(girl, "Lolita", rememberflag, true);
			}
			else
			{
				// if removing Lolita add back both Old and MILF
				if (name == "Lolita")
				{
					AddTrait(girl, "Old", false, false, true);
					AddTrait(girl, "MILF", false, false, true);
				}
				// if removing Old or MILF try to add the other one and if neither are there then try adding Lolita
				else
				{
					if (name != "Old" && HasRememberedTrait(girl, "Old"))	AddTrait(girl, "Old", false, false, true);
					if (name != "MILF" && HasRememberedTrait(girl, "MILF"))	AddTrait(girl, "MILF", false, false, true);
					if (!HasTrait(girl, "Old") && !HasTrait(girl, "MILF"))			AddTrait(girl, "Lolita", false, false, true);
				}
			}
		}
		else if (	// Check Shy/Exhibitionist Traits
			name == "Shy" ||
			name == "Exhibitionist")
		{
			if (apply)
			{
				if (name != "Shy")				RemoveTrait(girl, "Shy", rememberflag, true);
				if (name != "Exhibitionist")	RemoveTrait(girl, "Exhibitionist", rememberflag, true);
			}
			else
			{
				/* */if (name != "Shy" && HasRememberedTrait(girl, "Shy"))						AddTrait(girl, "Shy", false, false, true);
				else if (name != "Exhibitionist" && HasRememberedTrait(girl, "Exhibitionist"))	AddTrait(girl, "Exhibitionist", false, false, true);
			}
		}

#endif	// End of Social Traits

#if 1	// Start of Mental Traits

		else if (	// Check Optimist/Pessimist Traits
			name == "Optimist" ||
			name == "Pessimist")
		{
			if (apply)
			{
				if (name != "Optimist")			RemoveTrait(girl, "Optimist", rememberflag, true);
				if (name != "Pessimist")		RemoveTrait(girl, "Pessimist", rememberflag, true);
			}
			else
			{
				/* */if (name != "Optimist" && HasRememberedTrait(girl, "Optimist"))	AddTrait(girl, "Optimist", false, false, true);
				else if (name != "Pessimist" && HasRememberedTrait(girl, "Pessimist"))	AddTrait(girl, "Pessimist", false, false, true);
			}
		}
		else if (	// Check Willpower Traits
			name == "Broken Will" ||
			name == "Iron Will")
		{
			if (apply)
			{
				if (name != "Broken Will")			RemoveTrait(girl, "Broken Will", rememberflag, true);
				if (name != "Iron Will")			RemoveTrait(girl, "Iron Will", rememberflag, true);
			}
			else
			{
				/* */if (name != "Broken Will" && HasRememberedTrait(girl, "Broken Will"))	AddTrait(girl, "Broken Will", false, false, true);
				else if (name != "Iron Will" && HasRememberedTrait(girl, "Iron Will"))		AddTrait(girl, "Iron Will", false, false, true);
			}
		}
		else if (	// Check Learning Traits
			name == "Slow Learner" ||
			name == "Quick Learner")
		{
			if (apply)
			{
				if (name != "Slow Learner")			RemoveTrait(girl, "Slow Learner", rememberflag, true);
				if (name != "Quick Learner")		RemoveTrait(girl, "Quick Learner", rememberflag, true);
			}
			else
			{
				/* */if (name != "Slow Learner" && HasRememberedTrait(girl, "Slow Learner"))	AddTrait(girl, "Slow Learner", false, false, true);
				else if (name != "Quick Learner" && HasRememberedTrait(girl, "Quick Learner"))	AddTrait(girl, "Quick Learner", false, false, true);
			}
		}
		else if (	// Check Social Force Traits
			name == "Audacity" ||
			name == "Fearless" ||
			name == "Aggressive" ||
			name == "Meek" ||
			name == "Dependant" ||
			name == "Nervous")
		{
			if (apply)
			{
				if (name == "Audacity")
				{
					RemoveTrait(girl, "Meek", rememberflag, true);
					RemoveTrait(girl, "Nervous", rememberflag, true);
					RemoveTrait(girl, "Dependant", rememberflag, true);
				}
				if (name == "Fearless")
				{
					RemoveTrait(girl, "Meek", rememberflag, true);
					RemoveTrait(girl, "Nervous", rememberflag, true);
				}
				if (name == "Aggressive")
				{
					RemoveTrait(girl, "Meek", rememberflag, true);
				}
				if (name == "Meek")
				{
					RemoveTrait(girl, "Aggressive", rememberflag, true);
					RemoveTrait(girl, "Fearless", rememberflag, true);
					RemoveTrait(girl, "Audacity", rememberflag, true);
				}
				if (name == "Nervous")
				{
					RemoveTrait(girl, "Fearless", rememberflag, true);
					RemoveTrait(girl, "Audacity", rememberflag, true);
				}
				if (name == "Dependant")
				{
					RemoveTrait(girl, "Audacity", rememberflag, true);
				}
			}
			else
			{
				// if removing a trait from group a try to add back the others from that
				if ((name == "Audacity" || name == "Fearless" || name == "Aggressive") &&
					!HasTrait(girl, "Audacity") && !HasTrait(girl, "Fearless") && !HasTrait(girl, "Aggressive"))
				{
					AddTrait(girl, "Meek", false, false, true);
				}
				if ((name == "Audacity" || name == "Fearless") &&
					!HasTrait(girl, "Audacity") && !HasTrait(girl, "Fearless"))
				{
					AddTrait(girl, "Nervous", false, false, true);
				}
				if (name == "Audacity")
				{
					AddTrait(girl, "Dependant", false, false, true);
				}
				if ((name == "Meek" || name == "Nervous" || name == "Dependant") &&
					!HasTrait(girl, "Meek") && !HasTrait(girl, "Nervous") && !HasTrait(girl, "Dependant"))
				{
					AddTrait(girl, "Audacity", false, false, true);
				}
				if ((name == "Meek" || name == "Nervous") &&
					!HasTrait(girl, "Meek") && !HasTrait(girl, "Nervous"))
				{
					AddTrait(girl, "Fearless", false, false, true);
				}
				if (name == "Meek")
				{
					AddTrait(girl, "Aggressive", false, false, true);
				}
			}
		}

#endif	// End of Mental Traits

#if 1	// Start of Action Traits

		else if (	// Check Eyesight Traits
			name == "Blind" ||
			name == "Bad Eyesight" ||
			name == "Sharp-Eyed")
		{
			if (apply)
			{
				if (name != "Blind")			RemoveTrait(girl, "Blind", rememberflag, true);
				if (name != "Bad Eyesight")		RemoveTrait(girl, "Bad Eyesight", rememberflag, true);
				if (name != "Sharp-Eyed")		RemoveTrait(girl, "Sharp-Eyed", rememberflag, true);
			}
			else
			{
				/* */if (name != "Blind" && HasRememberedTrait(girl, "Blind"))					AddTrait(girl, "Blind", false, false, true);
				else if (name != "Bad Eyesight" && HasRememberedTrait(girl, "Bad Eyesight"))	AddTrait(girl, "Bad Eyesight", false, false, true);
				else if (name != "Sharp-Eyed" && HasRememberedTrait(girl, "Sharp-Eyed"))		AddTrait(girl, "Sharp-Eyed", false, false, true);
			}
		}

#endif	// End of Action Traits

		// These Trait groups will be handled differently
#if 1	// Start of Special Group Traits

		
		else if (	// Check _type_ Traits
			name == "Different Colored Eyes" ||
			name == "Eye Patch" ||
			name == "One Eye" ||
			name == "Cyclops")
		{
			if (apply)
			{
				// If she becomes a Cyclops, remove all the other traits and forget them, if she changes back later we assume the new eyes are good and the same color
				if (name == "Cyclops")
				{
					RemoveTrait(girl, "Different Colored Eyes", false, true);
					RemoveTrait(girl, "Eye Patch", false, true);
					RemoveTrait(girl, "One Eye", false, true);
				}
				else if (name == "Different Colored Eyes")	// If something tries to give a girl DCE and she only has 1 eye...
				{
					// if she only has 1 eye because she is a Cyclops (naturally only has space on her face for 1 eye)...
					if (HasTrait(girl, "Cyclops"))
						RemoveTrait(girl, "Different Colored Eyes", false, true);	// Forget having tried to get it.
					// but if she had 2 eyes but lost 1...
					else if (HasTrait(girl, "Eye Patch") || HasTrait(girl, "One Eye"))
						RemoveTrait(girl, "Different Colored Eyes", true, true);	// Remember having tried to get it.
				}
				// If something tries to give her the "One Eye" trait but she is a Cyclops, don't give her "One Eye"
				if (name == "One Eye" && HasTrait(girl, "Cyclops"))
				{
					RemoveTrait(girl, "One Eye", false, true);
				}
				// If she loses an eye or covers it up with an eye patch and she is not a Cyclops, remove DCE but remember it
				if ((name == "Eye Patch" || name == "One Eye") &&
					(HasTrait(girl, "Eye Patch") || HasTrait(girl, "One Eye")) && !HasTrait(girl, "Cyclops"))
				{
					RemoveTrait(girl, "Different Colored Eyes", true, true);
				}
			}
			else
			{
				// If she removes Cyclops, basically growing a second eye, we assume the new eye is good and the same color as the original one

				// If removing "Different Colored Eyes", we assume this means her eyes have become the same color

				// If removing "Eye Patch" or "One Eye", this may give back DCE if she does not have the others
				if ((name == "Eye Patch" || name == "One Eye") &&
					!HasTrait(girl, "Eye Patch") && !HasTrait(girl, "One Eye") && !HasTrait(girl, "Cyclops"))
				{
					AddTrait(girl, "Different Colored Eyes", false, false, true);
				}
			}
		}

#endif	// End of Special Group Traits

#if 1	// Start of Unsorted Traits

















#endif	// End of Unsorted Traits

		if (doOnce)
		{
			break;
		}
	}
}


// When traits change, update stat and skill modifiers
void cGirls::UpdateSSTraits(sGirl* girl)
{

}

bool cGirls::HasRememberedTrait(sGirl* girl, string trait)
{
	for (int i = 0; i < MAXNUM_TRAITS * 2; i++)
	{
		if (girl->m_RememTraits[i])
		{
			if (trait.compare(girl->m_RememTraits[i]->m_Name) == 0) return true;
		}
	}
	return false;
}

bool cGirls::HasTrait(sGirl* girl, string trait)
{
	for (int i = 0; i < MAXNUM_TRAITS; i++)
	{
		if (girl->m_Traits[i])
		{
			if (trait.compare(girl->m_Traits[i]->m_Name) == 0) return true;
		}
	}
	return false;
}

void cGirls::RemoveRememberedTrait(sGirl* girl, string name)
{
	sTrait* trait = g_Traits.GetTrait(name);
	for (int i = 0; i < MAXNUM_TRAITS * 2; i++)	// remove the traits
	{
		if (girl->m_RememTraits[i])
		{
			if (girl->m_RememTraits[i] == trait)
			{
				girl->m_NumRememTraits--;
				girl->m_RememTraits[i] = 0;
				return;
			}
		}
	}
}

void cGirls::RemoveAllRememberedTraits(sGirl* girl)
{
	/*
	 *	WD: Remove all traits to for new girl creation
	 *		is required after using ApplyTraits() & UnApplyTraits()
	 *		can create remembered traits especially random girls
	 */
	for (int i = 0; i < MAXNUM_TRAITS * 2; i++)
	{
		girl->m_RememTraits[i] = 0;
	}
	girl->m_NumRememTraits = 0;
}

bool cGirls::RemoveTrait(sGirl* girl, string name, bool addrememberlist, bool force, bool keepinrememberlist)
{
	/*
	 *	WD: Added logic for remembered trait
	 *
	 *		addrememberlist = true = will move the trait from active to the remember list
	 *		addrememberlist = false = will remove the trait from active but not add it to the remember list
	 *
	 *		force = false = will ignore this check
	 *		force = true = will remove the trait entirely
	 *
	 *		keepinrememberlist = false = will ignore this check
	 *		keepinrememberlist = true will add it to the remember list even if it is completely removed
	 *		`J` added - this is for when an item tries to add a trait that the girl can not possible get because she has a trait that precludes it
	 *			ie. adding "Perky Nipples" to a girl who has "No Nipples", if "No Nipples" gets removed the "Perky Nipples" will show themselves
	 *
	 *		Returns true if there was a active trait or remember list entry removed
	 *
	 *		This should fix items with duplicate traits and loss of original trait if
	 *		overwritten by a trait from an item that is later removed
	 */

	bool hasRemTrait = HasRememberedTrait(girl, name);

	if (!HasTrait(girl, name))							// WD:	no active trait to remove
	{
		if (hasRemTrait && !keepinrememberlist)
		{	// WD:	try remembered trait // `J` only if we want to remove it
			RemoveRememberedTrait(girl, name);
			return true;
			// `J` explain - she had the trait removed before and it is getting removed again so remove it for good
		}
		if (!hasRemTrait && keepinrememberlist)	
		{	// `J` if she does not have it at all but we want her to remember trying to get it
			AddRememberedTrait(girl, name);
		}
		return false;	// otherwise just return false
	}
	// `J` - so she has the trait active at this point...

	if (!force && hasRemTrait)	//	WD:	has remembered trait so don't touch active trait unless we are forcing removal of active trait
	{
		RemoveRememberedTrait(girl, name);
		return true;
		// `J` explain - she had the trait in both active and remembered so instead of removing active and replacing with remembered, just remove remembered
	}

	//	WD:	save trait to remember list before removing
	if (addrememberlist || keepinrememberlist) AddRememberedTrait(girl, name);

	//	WD: Remove trait
	sTrait* trait = g_Traits.GetTrait(name);
	for (int i = 0; i < MAXNUM_TRAITS; i++)			// remove the traits
	{
		if (girl->m_Traits[i] && girl->m_Traits[i] == trait)
		{
			girl->m_NumTraits--;

			MutuallyExclusiveTraits(girl, 0, girl->m_Traits[i]);
			UnapplyTraits(girl, girl->m_Traits[i]);	// `J` this is the old method  - the trait add and remove has been removed 

			if (girl->m_TempTrait[i] > 0) girl->m_TempTrait[i] = 0;
			girl->m_Traits[i] = 0;
			return true;
		}
	}
	return false;
}

//	Usually called as just g_Girls.LoseVirginity(girl) with implied no-remember, force=true
bool cGirls::LoseVirginity(sGirl* girl, bool addrememberlist, bool force)
{
	/*  Very similar to (and uses) RemoveTrait(). Added since trait "Virgin" created 04/14/2013.
	 *	This includes capability for items, magic or other processes
	 *	to have a "remove but remember" effect, like a "Belt of False Defloration"
	 *	that provides a magical substitute vagina, preserving the original while worn.
	 *	Well, why not?		DustyDan
	 */

	bool traitOpSuccess = false;
	girl->m_Virgin = 0;
	traitOpSuccess = RemoveTrait(girl, "Virgin", addrememberlist, force);
	return traitOpSuccess;
}

//	Usually called as just g_Girls.RegainVirginity(girl) with implied temp=false, removeitem=false, inrememberlist=falsee
bool cGirls::RegainVirginity(sGirl* girl, bool temp, bool removeitem, bool inrememberlist)
{
	/*  Very similar to (and uses) AddTrait(). Added since trait "Virgin" created 04/14/2013.
	 *	This includes capability for items, magic or other processes
	 *	to have a "remove but remember" effect, like a "Belt of False Defloration"
	 *	that provides a magical substitute vagina, preserving the original while worn.
	 *	Well, why not?		DustyDan
	 */

	bool traitOpSuccess = false;
	girl->m_Virgin = 1;
	//	Let's avoid re-inventing the wheel
	traitOpSuccess = AddTrait(girl, "Virgin", temp, removeitem, inrememberlist);
	return traitOpSuccess;
}

bool cGirls::CheckVirginity(sGirl* girl)
{
	if (HasTrait(girl, "Virgin") && girl->m_Virgin == 1) // `J` if already correct settings then return true
	{
		return true;
	}
	else if (HasTrait(girl, "Virgin"))	// `J` if not set correctly, set it correctly and return true
	{
		girl->m_Virgin = 1;
		return true;
	}
	else if (girl->m_Virgin == 1)	// `J` if not set correctly, set it correctly and return true
	{
		AddTrait(girl, "Virgin");
		return true;
	}
	else if (girl->m_Virgin == 0) // `J` if already correct settings then return false
	{
		return false;
	}
	else if (girl->m_Stats[STAT_AGE] < 18)	// `J` If she just turned 18 she should not legally have had sex yet
	{
		girl->m_Stats[STAT_AGE] = 18;
		girl->m_Virgin = 1;
		AddTrait(girl, "Virgin");
		RemoveTrait(girl, "MILF");
		return true;
	}
	else	// `J` average all sex skills plus age
	{
		int totalsex = girl->m_Stats[STAT_AGE];
		int div = 1;
		for (u_int i = 0; i < NUM_SKILLS; i++)
		{
			// `J` removed nonsex from virginity check
			if (i != SKILL_SERVICE && i != SKILL_MAGIC && i != SKILL_COMBAT && i != SKILL_MEDICINE && i != SKILL_PERFORMANCE &&
				i != SKILL_CRAFTING && i != SKILL_HERBALISM && i != SKILL_FARMING && i != SKILL_BREWING && i != SKILL_ANIMALHANDLING)
			{
				totalsex += girl->m_Skills[i];
				div++;	// `J` added to allow new skills
			}
		}
		int avg = totalsex / div;	// `J` fixed to allow new skills
		if (avg < 20)
		{
			girl->m_Virgin = 1;
			AddTrait(girl, "Virgin");
			return true;
		}
		else
		{
			girl->m_Virgin = 0;
			return false;
		}
	}
	if (girl->m_Virgin == 1) AddTrait(girl, "Virgin");
	else { girl->m_Virgin = 0; RemoveTrait(girl, "Virgin"); }
	return (girl->m_Virgin == 1);
}

void cGirls::AddRememberedTrait(sGirl* girl, string name)
{
	for (int i = 0; i < MAXNUM_TRAITS * 2; i++)	// add the traits
	{
		if (girl->m_RememTraits[i] == 0)
		{
			girl->m_NumRememTraits++;
			girl->m_RememTraits[i] = g_Traits.GetTrait(g_Traits.GetTranslateName(name)); // `J` added translation check
			return;
		}
	}
}

bool cGirls::AddTrait(sGirl* girl, string name, bool temp, bool removeitem, bool inrememberlist)
{
	/*
	 *	WD: Added logic for remembered trait
	 *
	 *		removeitem = true Will add to Remember
	 *		trait list if the trait is allready active
	 *		Used with items / efects may be removed
	 *		later eg items - rings
	 *
	 *		inrememberlist = true only add trait if
	 *		exists in the remember list and remove
	 *		from the list. Use mainly with unequiping
	 *		items and Trait overiding in ApplyTraits()
	 *
	 *		Returns true if trait made active or added
	 *		trait remember list.
	 *
	 *		This should fix items with duplicate
	 *		traits and loss of original trait if
	 *		overwritten by a trait from an item
	 *		that is later removed
	 *
	 */

	if (HasTrait(girl, name))
	{
		if (removeitem)								//	WD: Overwriting existing trait with removable item / effect
			AddRememberedTrait(girl, name);			//	WD:	Save trait for when item is removed
		return true;
	}

	if (inrememberlist)								// WD: Add trait only if it is in the Remember List
	{
		if (HasRememberedTrait(girl, name)) RemoveRememberedTrait(girl, name);
		else return false;							//	WD:	No trait to add
	}

	for (int i = 0; i < MAXNUM_TRAITS; i++)				// add the trait
	{
		if (girl->m_Traits[i] == 0)
		{
			if (temp) girl->m_TempTrait[i] = 20;
			girl->m_NumTraits++;
			girl->m_Traits[i] = g_Traits.GetTrait(g_Traits.GetTranslateName(name)); // `J` added translation check

			MutuallyExclusiveTraits(girl, 1, girl->m_Traits[i], removeitem);
			ApplyTraits(girl, girl->m_Traits[i], removeitem);	// `J` this is the old method  - the trait add and remove has been removed 

			return true;
		}
	}
	return false;
}

// Update temp traits and remove expired traits
void cGirls::updateTempTraits(sGirl* girl)
{
	if (girl->health() <= 0) return;		// Sanity check. Abort on dead girl
	for (int i = 0; i < MAXNUM_TRAITS; i++)
	{
		if (girl->m_Traits[i] && girl->m_TempTrait[i] > 0)
		{
			girl->m_TempTrait[i]--;
			if (girl->m_TempTrait[i] == 0)
				g_Girls.RemoveTrait(girl, girl->m_Traits[i]->m_Name);
		}
	}
}

// Update happiness for trait affects
void cGirls::updateHappyTraits(sGirl* girl)
{
	if (girl->health() <= 0) return;	// Sanity check. Abort on dead girl
	if (girl->has_trait("Optimist")) girl->happiness(5);

	if (girl->has_trait("Pessimist"))
	{
		girl->happiness(-5);
		if (girl->happiness() <= 0 && g_Dice.percent(50))
		{
			stringstream ss;
			string stopper = "";
			if (girl->m_InArena && g_Arena.GetNumGirlsOnJob(0, JOB_DOCTORE, 0) > 0)
				stopper = "the Doctore";
			else if (girl->m_InMovieStudio && g_Studios.GetNumGirlsOnJob(0, JOB_DIRECTOR, 1) > 0)
				stopper = "the Director";
			else if (girl->m_InClinic && g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, 0) > 0)
				stopper = "the Chairman";
			else if (girl->m_InCentre && (girl->m_DayJob == JOB_REHAB || girl->m_PrevDayJob == JOB_REHAB) && g_Centre.GetNumGirlsOnJob(0, JOB_DRUGCOUNSELOR, 0) > 0)
				stopper = "her Counselor";
			else if (girl->m_InCentre && g_Centre.GetNumGirlsOnJob(0, JOB_CENTREMANAGER, 0) > 0)
				stopper = "the Centre Manager";
			else if (girl->m_InHouse && g_Dice.percent(50))
				stopper = "You";
			else if (girl->m_InHouse && g_House.GetNumGirlsOnJob(0, JOB_HEADGIRL, 0) > 0)
				stopper = "your Head Girl";
			else if (g_Brothels.GetNumGirlsOnJob(girl->where_is_she, JOB_MATRON, 0) > 0)
				stopper = "the Matron";
			else if (girl->m_InFarm && g_Farm.GetNumGirlsOnJob(0, JOB_FARMMANGER, 0) > 0)
				stopper = "the Farm Manger";

			int Schance = g_Dice % 100;
			if (stopper != "")
			{
				ss << girl->m_Realname << " tried to killed herself but " << stopper;
				if (Schance < 50)		{ ss << " talked her out of it."; }
				else if (Schance < 90)	{ ss << " stopped her."; }
				else	{ girl->m_Stats[STAT_HEALTH] = 1;	ss << " revived her."; }
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
			}
			else
			{
				string msg = girl->m_Realname + gettext(" has killed herself since she was unhappy and depressed.");
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
				g_MessageQue.AddToQue(ss.str(), COLOR_RED);
				girl->health(-1000);
			}
		}
	}
}

// ----- Sex

void cGirls::GirlFucks(sGirl* girl, int DayNight, sCustomer* customer, bool group, string& message, u_int& SexType)
{
	bool good = false;
	bool contraception = false;
	int STDchance = 0;
	int happymod = 0;	// Start the customers unhappiness/happiness bad sex decreases, good sex inceases
	if (HasTrait(girl, "Fake Orgasm Expert"))		happymod += 20;
	else if (HasTrait(girl, "Fast Orgasms"))		happymod += 10;
	else if (HasTrait(girl, "Slow Orgasms"))		happymod -= 10;
	if (HasTrait(girl, "Psychic"))					happymod += 10;	// she knows what the customer wants

	if (customer->m_Fetish == FETISH_BIGBOOBS)
	{
		/* */if (HasTrait(girl, "Flat Chest"))				happymod -= 15;
		else if (HasTrait(girl, "Petite Breasts"))			happymod -= 10;
		else if (HasTrait(girl, "Small Boobs"))				happymod -= 5;
		else if (HasTrait(girl, "Busty Boobs"))				happymod += 4;
		else if (HasTrait(girl, "Big Boobs"))				happymod += 8;
		else if (HasTrait(girl, "Giant Juggs"))				happymod += 12;
		else if (HasTrait(girl, "Massive Melons"))			happymod += 16;
		else if (HasTrait(girl, "Abnormally Large Boobs"))	happymod += 20;
		else if (HasTrait(girl, "Titanic Tits"))			happymod += 25;
	}
	else if (customer->m_Fetish == FETISH_SMALLBOOBS)
	{
		/* */if (HasTrait(girl, "Flat Chest"))				happymod += 15;
		else if (HasTrait(girl, "Petite Breasts"))			happymod += 20;
		else if (HasTrait(girl, "Small Boobs"))				happymod += 10;
		else if (HasTrait(girl, "Busty Boobs"))				happymod -= 2;
		else if (HasTrait(girl, "Big Boobs"))				happymod -= 5;
		else if (HasTrait(girl, "Giant Juggs"))				happymod -= 10;
		else if (HasTrait(girl, "Massive Melons"))			happymod -= 15;
		else if (HasTrait(girl, "Abnormally Large Boobs"))	happymod -= 20;
		else if (HasTrait(girl, "Titanic Tits"))			happymod -= 30;
	}
	else
	{
		/* */if (HasTrait(girl, "Flat Chest"))				happymod -= 2;
		else if (HasTrait(girl, "Petite Breasts"))			happymod -= 1;
		else if (HasTrait(girl, "Small Boobs"))				happymod += 0;
		else if (HasTrait(girl, "Busty Boobs"))				happymod += 1;
		else if (HasTrait(girl, "Big Boobs"))				happymod += 2;
		else if (HasTrait(girl, "Giant Juggs"))				happymod += 1;
		else if (HasTrait(girl, "Massive Melons"))			happymod += 0;
		else if (HasTrait(girl, "Abnormally Large Boobs"))	happymod -= 1;
		else if (HasTrait(girl, "Titanic Tits"))			happymod -= 2;

	}

	girl->m_NumCusts += (int)customer->m_Amount;

	if (group)
	{
		// the customer will be an average in all skills for the customers involved in the sex act
		SexType = SKILL_GROUP;
	}
	else	// Any other sex acts
		SexType = customer->m_SexPref;

	// If the girls skill < 50 then it will be unsatisfying otherwise it will be satisfying
	happymod = (GetSkill(girl, SexType) - 50) / 5;
	/* `J` replaced with single check
	if (GetSkill(girl, SexType) < 50)	happymod -= (100 - GetSkill(girl, SexType)) / 5;
	else happymod += GetSkill(girl, SexType) / 5;
	//*/

	// If the girl is famous then he will be slightly happier
	happymod += GetStat(girl, STAT_FAME) / 5;

	// her service ability will also make him happier (I.e. does she help clean him well)
	happymod += GetSkill(girl, SKILL_SERVICE) / 10;

	int value = customer->m_Stats[STAT_HAPPINESS] + happymod;			// `J` now set customers happiness
	if (value > 100)	{ customer->m_Stats[STAT_HAPPINESS] = 100; }
	else if (value < 0)	{ customer->m_Stats[STAT_HAPPINESS] = 0; }
	else				{ customer->m_Stats[STAT_HAPPINESS] += happymod; }

	// her magic ability can make him think he enjoyed it more if she has mana

	int happycost = 3 - int(GetSkill(girl, SKILL_MAGIC) / 40);	// `J` how many mana will each point of happy cost her
	if (happycost < 1) happycost = 1;		// so [magic:cost] [<10:can't] [10-39:3] [40-79:2] [80+:1] (probably, I hate math)
	if (customer->m_Stats[STAT_HAPPINESS] < 100 &&			// If they are not fully happy
		GetStat(girl, STAT_MANA) >= happycost &&		// If she has enough mana to actually try
		GetSkill(girl, SKILL_MAGIC) > 9)				// If she has at least 10 magic
	{
		int happymana = GetStat(girl, STAT_MANA);					// check her mana
		if (happymana > 20) happymana = 20;							// and only max of 20 will be used
		int happygain = happymana / happycost;						// check how much she can increase happiness with 20 mana
		if (happygain > 10) happygain = 10;							// can only increase happy by 10
		int lesshappy = 100 - customer->m_Stats[STAT_HAPPINESS];	// how much can she charm him before it is wasted?
		if (happygain > lesshappy) happygain = lesshappy;			// can only increase happy by 10
		happymana = happygain * happycost;							// check how much mana she actually spends
		if (happymana > 20) happymana = 20;							// correct incase more than 20
		if (happymana < 0) happymana = 1;							// will spend at least 1 mana just for trying

		customer->m_Stats[STAT_HAPPINESS] += happygain;				// now apply happy
		UpdateStat(girl, STAT_MANA, -happymana);					// and apply mana
	}

	message += girl->m_Realname;
	switch (SexType)
	{
	case SKILL_ANAL:
		/* */if (GetSkill(girl, SexType) < 20)	message += gettext(" found it difficult to get it in but painfully allowed the customer to fuck her in her tight ass.");
		else if (GetSkill(girl, SexType) < 40)	message += gettext(" had to relax somewhat but had the customer fucking her in her ass.");
		else if (GetSkill(girl, SexType) < 60)	message += gettext(" found it easier going with the customer fucking her in her ass.");
		else if (GetSkill(girl, SexType) < 80)	message += gettext(" had the customer's cock go in easy. She found his cock in her ass a very pleasurable experience.");
		else /*                             */	message += gettext(" came hard as the customer fucked her ass.");
		break;

	case SKILL_BDSM:
		/* */if (GetSkill(girl, SexType) < 40)	message += gettext(" was frightened by being tied up and having pain inflicted on her.");
		else if (GetSkill(girl, SexType) < 60)	message += gettext(" was a little turned on by being tied up and having the customer hurting her.");
		else if (GetSkill(girl, SexType) < 80)	message += gettext(" was highly aroused by the pain and bondage, even more so when fucking at the same time.");
		else /*                             */	message += GetRandomBDSMString();
		break;

	case SKILL_NORMALSEX:
		/* */if (GetSkill(girl, SexType) < 20)	message += gettext(" didn't do much as she allowed the customer to fuck her pussy.");
		else if (GetSkill(girl, SexType) < 40)	message += gettext(" fucked the customer back while their cock was embedded in her cunt.");
		else if (GetSkill(girl, SexType) < 60)	message += gettext(" liked the feeling of having a cock buried in her cunt and fucked back as much as she got.");
		else if (GetSkill(girl, SexType) < 80)	message += gettext(" fucked like a wild animal, cumming several times and ending with her and the customer covered in sweat.");
		else /*                             */	message += GetRandomSexString();
		break;

	case SKILL_ORALSEX:
		/* */if (GetSkill(girl, SexType) < 20)	message += " awkwardly licked the customer's cock, and recoiled when he came.";
		else if (GetSkill(girl, SexType) < 60)	message += " licked and sucked the customer's cock.";
		else if (GetSkill(girl, SexType) < 80)	message += " loved sucking the customer's cock, and let him cum all over her.";
		else /*                             */	message += " wouldn't stop licking and sucking the customer's cock until she had swallowed his entire load.";
		break;

	case SKILL_TITTYSEX:
		/* */if (GetSkill(girl, SexType) < 20)	message += " awkwardly let the customer's cock fuck her tits, and recoiled when he came.";
		else if (GetSkill(girl, SexType) < 60)	message += " used her breasts on the customer's cock.";
		else if (GetSkill(girl, SexType) < 80)	message += " loved using her breasts on the customer's cock, and let him cum all over her.";
		else /*                             */	message += " wouldn't stop using her breasts to massage the customer's cock until she had made him spill his entire load.";
		break;

	case SKILL_HANDJOB:
		/* */if (GetSkill(girl, SexType) < 20)	message += " awkwardly worked the customer's cock with her hand, and recoiled when he came.";
		else if (GetSkill(girl, SexType) < 60)	message += " used her hand on the customer's cock.";
		else if (GetSkill(girl, SexType) < 80)	message += " loved using her hand on the customer's cock, and let him cum all over her.";
		else /*                             */	message += " wouldn't stop using her hand to massage the customer's cock until she had made him spill his entire load.";
		break;

	case SKILL_BEASTIALITY:
		if (g_Brothels.GetNumBeasts() == 0)
		{
			message += gettext(" found that there were no beasts available, so some fake ones were used. This disapointed the customer somewhat.");
			customer->m_Stats[STAT_HAPPINESS] -= 10;
		}
		else
		{
			int harmchance = -(GetSkill(girl, SKILL_BEASTIALITY) + GetSkill(girl, SKILL_ANIMALHANDLING) - 50);  // 50% chance at 0 skill, 1% chance at 49 skill
			if (g_Dice.percent(harmchance))
			{
				message += gettext(" accidentally harmed some beasts during the act and she");
				g_Brothels.add_to_beasts(-((g_Dice % 3) + 1));
			}
			else if (g_Dice.percent(1 +
				(HasTrait(girl, "Aggressive") ? 3 : 0) +
				(HasTrait(girl, "Assassin") ? 1 : 0) +
				(HasTrait(girl, "Merciless") ? 1 : 0) +
				(HasTrait(girl, "Sadistic") ? 2 : 0) +
				(HasTrait(girl, "Twisted") ? 1 : 0)))
			{
				message += gettext(" 'accidentally' harmed a beast during the act and she");
				g_Brothels.add_to_beasts(-1);
			}

			/* */if (GetSkill(girl, SexType) < 20)	message += " was disgusted by the idea but still allowed the customer to watch as she was fucked by some animals.";
			else if (GetSkill(girl, SexType) < 40)	message += " was a only little put off by the idea but still allowed the customer to watch and help as she was fucked by animals.";
			else if (GetSkill(girl, SexType) < 60)	message += " took a large animal's cock deep inside her and enjoyed being fucked by it, her cries of pleasure being muffled by the customer's cock in her mouth.";
			else if (GetSkill(girl, SexType) < 80)	message += " fucked some exotic beasts covered with massive cocks and tentacles, she came over and over alongside the customer.";
			else /*                             */	message += GetRandomBeastString();
		}
		break;

	case SKILL_GROUP:
		/* */if (GetSkill(girl, SexType) < 20)	message += gettext(" struggled to service everyone in the group that came to fuck her.");
		else if (GetSkill(girl, SexType) < 40)	message += gettext(" managed to keep the group of customers fucking her satisfied.");
		else if (GetSkill(girl, SexType) < 60)	message += gettext(" serviced all of the group of customers that fucked her.");
		else if (GetSkill(girl, SexType) < 80)	message += gettext(" fucked and came many times with everyone in the group of customers.");
		else /*                             */	message += GetRandomGroupString();
		break;

	case SKILL_LESBIAN:
		/* */if (GetSkill(girl, SexType) < 20)	message += gettext(" licked her female customer's cunt until she came. She didn't want any herself.");
		else if (GetSkill(girl, SexType) < 40)	message += gettext(" was aroused as she made her female customer cum.");
		else if (GetSkill(girl, SexType) < 60)	message += gettext(" fucked and was fucked by her female customer.");
		else if (GetSkill(girl, SexType) < 80)	message += gettext(" and her female customer's cumming could be heard thoughout the building.");
		else /*                             */	message += GetRandomLesString();
		break;

	case SKILL_STRIP:
	default:
		/* */if (GetSkill(girl, SexType) < 20)	message += gettext(" shyly took her clothes off infront of the customer.");
		else if (GetSkill(girl, SexType) < 40)	message += gettext(" coyly took her clothes off infront of the customer.");
		else if (GetSkill(girl, SexType) < 60)	message += gettext(" hotly took her clothes off infront of the customer.");
		else if (GetSkill(girl, SexType) < 80)	message += gettext(" proudly took her clothes off infront of the customer.");
		else /*                             */	message += gettext(" joyously took her clothes off infront of the customer.");
		break;
	}

	// WD:	customer HAPPINESS changes complete now cap the stat to 100
	customer->m_Stats[STAT_HAPPINESS] = min(100, (int)customer->m_Stats[STAT_HAPPINESS]);

	message += (SexType == SKILL_GROUP) ? "\nThe customers " : "\nThe customer ";
	/* */if (customer->m_Stats[STAT_HAPPINESS] > 80)	message += gettext("swore they would come back.");
	else if (customer->m_Stats[STAT_HAPPINESS] > 50)	message += gettext("enjoyed the experience.");
	else if (customer->m_Stats[STAT_HAPPINESS] > 30)	message += gettext("didn't enjoy it.");
	else /*                                       */	message += gettext("thought it was crap.");

	// WD: update Fame based on Customer HAPPINESS
	UpdateStat(girl, STAT_FAME, (customer->m_Stats[STAT_HAPPINESS] - 1) / 33);

	// The girls STAT_CONSTITUTION and STAT_AGILITY modify how tired she gets
	AddTiredness(girl);

	// if the girl likes sex and the sex type then increase her happiness otherwise decrease it
	if (GetStat(girl, STAT_LIBIDO) > 5)
	{
		/* */if (GetSkill(girl, SexType) < 20)	message += gettext("\nThough she had a tough time with it, she was horny and still managed to gain some little enjoyment.");
		else if (GetSkill(girl, SexType) < 40)	message += gettext("\nShe considered it a learning experience and enjoyed it a bit.");
		else if (GetSkill(girl, SexType) < 60)	message += gettext("\nShe enjoyed it a lot and wanted more.");
		else if (GetSkill(girl, SexType) < 80)	message += gettext("\nIt was nothing new for her, but she really does appreciate such work.");
		else /*                             */	message += gettext("\nIt seems that she lives for this sort of thing.");
		UpdateStat(girl, STAT_HAPPINESS, GetStat(girl, STAT_LIBIDO) / 5);
	}
	else
	{
		message += gettext("\nShe wasn't really in the mood.");
		UpdateStat(girl, STAT_HAPPINESS, -1);
	}

	// special cases for certain sex types
	switch (SexType)
	{
	case SKILL_ANAL:
		if (GetSkill(girl, SexType) <= 20)	// if unexperienced then will get hurt
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_HEALTH, -3);
		}
		UpdateStat(girl, STAT_LIBIDO, -40);
		UpdateStat(girl, STAT_SPIRIT, -1);
		STDchance += 40;
		break;

	case SKILL_BDSM:
		if (GetSkill(girl, SexType) <= 30)	// if unexperienced then will get hurt
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_HEALTH, -3);
		}
		contraception = girl->calc_pregnancy(customer, false, 0.75);
		STDchance += (contraception ?3:30);

		UpdateStat(girl, STAT_LIBIDO, -20);
		UpdateStat(girl, STAT_SPIRIT, -1);
		break;

	case SKILL_NORMALSEX:
		if (GetSkill(girl, SexType) < 15)
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_HEALTH, -3);
		}
		// if they're both happy afterward, it's good sex which modifies the chance of pregnancy
		good = (customer->happiness() >= 60 && girl->happiness() >= 60);
		contraception = girl->calc_pregnancy(customer, good);
		STDchance += (contraception ? 5 : 50);
		UpdateStat(girl, STAT_LIBIDO, -50);
		break;

	case SKILL_ORALSEX:
		if (GetSkill(girl, SexType) <= 20)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience caused her some embarrassment.";	// Changed... being new at oral doesn't hurt, but can be embarrasing. --PP
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			//	UpdateStat(girl, STAT_HEALTH, -3);				// Removed... oral doesn't hurt unless you get herpes or something. --PP
		}
		STDchance += 20;
		UpdateStat(girl, STAT_LIBIDO, -10);
		break;

	case SKILL_TITTYSEX:
		if (GetSkill(girl, SexType) <= 20)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience caused her some embarrassment.";	// Changed... being new at oral doesn't hurt, but can be embarrasing. --PP
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
		}
		STDchance += 5;
		UpdateStat(girl, STAT_LIBIDO, -10);
		break;

	case SKILL_HANDJOB:
		if (GetSkill(girl, SexType) <= 20)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience caused her some embarrassment.";	// Changed... being new at handjob doesn't hurt, but can be embarrasing. --PP
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
		}
		STDchance += 5;
		UpdateStat(girl, STAT_LIBIDO, -10);
		break;

	case SKILL_BEASTIALITY:
		if (GetSkill(girl, SexType) <= 30)	// if unexperienced then will get hurt
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_HEALTH, -3);
		}
		UpdateStat(girl, STAT_SPIRIT, -1);	// is pretty degrading
		// if they're both happy afterward, it's good sex which modifies the chance of pregnancy
		good = (customer->happiness() >= 60 && girl->happiness() >= 60);
		// mod: added check for number of beasts owned; otherwise, fake beasts could somehow inseminate the girl
		if (g_Brothels.GetNumBeasts() > 0)
		{
			contraception = girl->calc_insemination(customer, good);
			STDchance += (contraception ? 4 : 40);
		}
		UpdateStat(girl, STAT_LIBIDO, -40);
		break;

	case SKILL_GROUP:
		if (GetSkill(girl, SexType) <= 30)	// if unexperienced then will get hurt
		{
			message += gettext("\nHer inexperience hurt her a little.");
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_HEALTH, -3);
		}
		// if they're both happy afterward, it's good sex which modifies the chance of pregnancy
		good = (customer->happiness() >= 60 && girl->happiness() >= 60);
		// adding a 50% bonus to the chance of pregnancy since there's more than one partner involved
		contraception = girl->calc_pregnancy(customer, good, 1.5);
		STDchance += ((4 + customer->m_Amount) * (contraception ? 1 : 10));
		UpdateStat(girl, STAT_LIBIDO, -80);
		break;
	case SKILL_LESBIAN:
		STDchance += 10;
		UpdateStat(girl, STAT_LIBIDO, -50);
		break;

	case SKILL_STRIP:
	default:
		if (GetSkill(girl, SexType) <= 30)	// if inexperienced then will get hurt
		{
			message += gettext("\nShe got tangled in her clothes and fell on her face.");
			UpdateStat(girl, STAT_HAPPINESS, -2);
			UpdateStat(girl, STAT_SPIRIT, -3);
			UpdateStat(girl, STAT_CONFIDENCE, -1);
			UpdateStat(girl, STAT_HEALTH, -3);
		}
		STDchance += 1;
		UpdateStat(girl, STAT_LIBIDO, -5);
		break;
	}

	// lose virginity unless it was anal sex -- or lesbian, or Oral also customer is happy no matter what. -PP
	if (g_Girls.CheckVirginity(girl))
	{
		/*zzzzzz - adjust customer happiness for virgins by the sex job
		SexType == SKILL_ANAL			||
		SexType == SKILL_BDSM			||
		SexType == SKILL_NORMALSEX		||
		SexType == SKILL_BEASTIALITY	||
		SexType == SKILL_GROUP			||
		SexType == SKILL_LESBIAN		||
		SexType == SKILL_STRIP			||
		SexType == SKILL_ORALSEX		||
		SexType == SKILL_TITTYSEX		||
		SexType == SKILL_HANDJOB		||
		*/
		message += gettext(" The customer was overjoyed that she was a virgin.");
		customer->m_Stats[STAT_HAPPINESS] = 100;
		if (SexType == SKILL_BDSM || SexType == SKILL_NORMALSEX || SexType == SKILL_BEASTIALITY || SexType == SKILL_GROUP)
			g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
	}

	// Now calculate other skill increases
	int skillgain = 4;	int exp = 5;
	if (HasTrait(girl, "Quick Learner"))		{ skillgain = 5; exp = 7; }
	else if (HasTrait(girl, "Slow Learner"))	{ skillgain = 3; exp = 3; }
	if (SexType == SKILL_GROUP)
	{
		UpdateSkill(girl, SKILL_ANAL, max(0, g_Dice%skillgain + 1));
		UpdateSkill(girl, SKILL_BDSM, max(0, g_Dice%skillgain - 1));
		UpdateSkill(girl, SKILL_NORMALSEX, max(0, g_Dice%skillgain + 1));
		UpdateSkill(girl, SKILL_BEASTIALITY, max(0, g_Dice%skillgain - 3));
		UpdateSkill(girl, SKILL_GROUP, max(0, g_Dice%skillgain + 1));
		UpdateSkill(girl, SKILL_LESBIAN, max(0, g_Dice%skillgain - 1));
		UpdateSkill(girl, SKILL_STRIP, max(0, g_Dice%skillgain - 1));
		UpdateSkill(girl, SKILL_ORALSEX, max(0, g_Dice%skillgain - 1));
		UpdateSkill(girl, SKILL_TITTYSEX, max(0, g_Dice%skillgain - 1));
		UpdateSkill(girl, SKILL_HANDJOB, max(0, g_Dice%skillgain - 1));
	}
	else	UpdateSkill(girl, SexType, g_Dice%skillgain + 1);
	UpdateSkill(girl, SKILL_SERVICE, max(0, g_Dice % skillgain - 1));
	UpdateStat(girl, STAT_EXP, (g_Dice % (exp * 3)));

	int enjoy = 1;
	if (HasTrait(girl, "Nymphomaniac"))
	{
		switch (SexType)
		{
		case SKILL_GROUP:			enjoy += 3; break;
		case SKILL_ANAL:			enjoy += 2; break;
		case SKILL_NORMALSEX:		enjoy += 2; break;
		case SKILL_BDSM:
		case SKILL_BEASTIALITY:
		case SKILL_LESBIAN:			enjoy += 1; break;
			// Nymphomaniac would rather have something inside her so if she can't, she does not enjoy it as much
		case SKILL_STRIP:			enjoy -= 2; break;
		case SKILL_TITTYSEX:
		case SKILL_HANDJOB:			enjoy -= 1; break;
		case SKILL_ORALSEX:
		default:
			break;
		}
	}
	if (HasTrait(girl, "Lesbian"))
	{
		switch (SexType)
		{
		case SKILL_LESBIAN:			enjoy += 3; break;
		case SKILL_STRIP:			enjoy += 1; break;
			// Lesbian would rather not have sex with a male
		case SKILL_NORMALSEX:
		case SKILL_TITTYSEX:
		case SKILL_ORALSEX:
		case SKILL_HANDJOB:			enjoy -= 1; break;
		case SKILL_ANAL:
		case SKILL_GROUP:			enjoy -= 2; break;
		default:
			break;
		}
	}
	if (HasTrait(girl, "Straight"))
	{
		switch (SexType)
		{
		case SKILL_NORMALSEX:		enjoy += 1; break;
		case SKILL_LESBIAN:			enjoy -= 1; break;
		default:
			break;
		}
	}

	if (GetStat(girl, STAT_HAPPINESS) > 50)			enjoy += 2;
	else if (GetStat(girl, STAT_HAPPINESS) <= 5)	enjoy -= 2;

	int health = GetStat(girl, STAT_HEALTH);
	/* */if (health <= 10)	STDchance *= 3;
	else if (health <= 20)	STDchance *= 2;
	else if (health >= 80)	STDchance /= 4;
	else if (health >= 90)	STDchance /= 7;
	else if (health == 100)	STDchance /= 10;
	if (STDchance < 1)		STDchance = 1;

	if (HasTrait(girl, "AIDS") && !customer->m_HasAIDS && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girl->m_Realname + " gave the customer AIDS! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
		customer->m_HasAIDS = true;
		customer->m_Stats[STAT_HAPPINESS] -= 100;
		enjoy -= 3;
	}
	else if (!HasTrait(girl, "AIDS") && customer->m_HasAIDS && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girl->m_Realname + " has caught the disease AIDS! She will likely die, but a rare cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
		AddTrait(girl, "AIDS");
		girl->happiness(-50);
		enjoy -= 30;
	}
	if (HasTrait(girl, "Chlamydia") && !customer->m_HasChlamydia && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girl->m_Realname + " gave the customer Chlamydia! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
		customer->m_HasChlamydia = true;
		customer->m_Stats[STAT_HAPPINESS] -= 40;
		enjoy -= 3;
	}
	else if (!HasTrait(girl, "Chlamydia") && customer->m_HasChlamydia && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girl->m_Realname + " has caught the disease Chlamydia! A cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
		AddTrait(girl, "Chlamydia");
		girl->happiness(-30);
		enjoy -= 30;
	}

	if (HasTrait(girl, "Syphilis") && !customer->m_HasSyphilis && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girl->m_Realname + " gave the customer Syphilis! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
		customer->m_HasSyphilis = true;
		customer->m_Stats[STAT_HAPPINESS] -= 50;
		enjoy -= 3;
	}
	else if (!HasTrait(girl, "Syphilis") && customer->m_HasSyphilis && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girl->m_Realname + " has caught the disease Syphilis! This can be deadly, but a cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
		AddTrait(girl, "Syphilis");
		girl->happiness(-30);
		enjoy -= 30;
	}

	if (HasTrait(girl, "Herpes") && !customer->m_HasHerpes && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girl->m_Realname + " gave the customer Herpes! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
		customer->m_HasHerpes = true;
		customer->m_Stats[STAT_HAPPINESS] -= 30;
		enjoy -= 3;
	}
	else if (!HasTrait(girl, "Herpes") && customer->m_HasHerpes && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girl->m_Realname + " has caught the disease Herpes! A cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
		AddTrait(girl, "Herpes");
		girl->happiness(-30);
		enjoy -= 30;
	}

	g_Girls.UpdateEnjoyment(girl, ACTION_SEX, enjoy, true);

	cConfig cfg;
	if (cfg.debug.log_debug())
	{
		g_LogFile.os() << "STD Debug ::: Sex Type : " << sGirl::skill_names[SexType]
			<< " :: Contraception: " << (contraception ? "True" : "False") 
			<< " :: Health: " << health
			<< " :: STD Chance: " << STDchance
			<< endl;
	}
}

string cGirls::GetRandomSexString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	string OutStr;
	// MYR: Can't resist a little cheeky chaos
	random = g_Dice % 500;
	if (random == 345)
	{
		OutStr += " (phrase 1). (phrase 2) (phrase 3).";
		return OutStr;
	}
	OutStr += " ";  // Consistency
	// Roll #1
# pragma region sex1
	roll1 = g_Dice % 8 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1:
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("straddled");
		else if (random <= 4)	OutStr += gettext("cow-girled");
		else if (random <= 6)	OutStr += gettext("wrapped her legs around");
		else if (random <= 8)	OutStr += gettext("contorted her legs behind her head for");
		else /*            */	OutStr += gettext("scissored");
		OutStr += gettext(" the client, because it ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("turned him on");
		else if (random <= 4)	OutStr += gettext("made him crazy");
		else if (random <= 6)	OutStr += gettext("gave him a massive boner");
		else if (random <= 8)	OutStr += gettext("was more fun than talking");
		else /*            */	OutStr += gettext("made him turn red");
		break;
	case 2:
		OutStr += gettext("was told to grab ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("her ankles");
		else if (random <= 4)	OutStr += gettext("the chair");
		else if (random <= 6)	OutStr += gettext("her knees");
		else if (random <= 8)	OutStr += gettext("the table");
		else /*            */	OutStr += gettext("the railing");
		OutStr += gettext(" and ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("shook her hips");
		else if (random <= 4)	OutStr += gettext("spread her legs");
		else if (random <= 6)	OutStr += gettext("close her eyes");
		else if (random <= 8)	OutStr += gettext("look away");
		else /*            */	OutStr += gettext("bend waaaaayyy over");
		break;
	case 3:
		OutStr += gettext("had fun with his ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("foot");
		else if (random <= 4)	OutStr += gettext("stocking");
		else if (random <= 6)	OutStr += gettext("hair");
		else if (random <= 8)	OutStr += gettext("lace");
		else if (random <= 10)	OutStr += gettext("butt");
		else /*            */	OutStr += gettext("food");
		OutStr += gettext(" fetish and gave him an extended ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("foot");
		else if (random <= 4)	OutStr += gettext("hand");
		else /*            */	OutStr += gettext("oral");
		OutStr += gettext(" surprise");
		break;
	case 4:
		OutStr += gettext("dressed as ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("a school girl");
		else if (random <= 4)	OutStr += gettext("a nurse");
		else if (random <= 6)	OutStr += gettext("a nun");
		else if (random <= 8)	OutStr += gettext("an adventurer");
		else /*            */	OutStr += gettext("a dominatrix");
		OutStr += gettext(" to grease ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("the little man");
		else if (random <= 4)	OutStr += gettext("his pole");
		else if (random <= 6)	OutStr += gettext("his tool");
		else if (random <= 8)	OutStr += gettext("his fingers");
		else /*            */	OutStr += gettext("his toes");
		break;
	case 5:
		OutStr += gettext("decided to skip ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("the bed");
		else if (random <= 4)	OutStr += gettext("foreplay");
		else if (random <= 6)	OutStr += gettext("niceties");
		else /*            */	OutStr += gettext("greetings");
		OutStr += gettext(" and assumed position ");
		random = g_Dice % 9999 + 1;
		char buffer[10];
		_itoa(random, buffer, 10);
		OutStr += buffer;
		break;
	case 6:
		OutStr += gettext("gazed in awe at ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("how well hung he was");
		else if (random <= 4)	OutStr += gettext("the time");
		else if (random <= 6)	OutStr += gettext("his muscles");
		else if (random <= 8)	OutStr += gettext("his handsome face");
		else /*            */	OutStr += gettext("his collection of sexual magic items");
		OutStr += gettext(" and ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("felt inspired");
		else if (random <= 4)	OutStr += gettext("played hard to get");
		else if (random <= 6)	OutStr += gettext("squealed like a little girl");
		else /*            */	OutStr += gettext("prepared for action");
		break;
	case 7: OutStr += gettext("bent into ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("a delightful");
		else if (random <= 4)	OutStr += gettext("an awkward");
		else if (random <= 6)	OutStr += gettext("a difficult");
		else /*            */	OutStr += gettext("a crazy");
		OutStr += gettext(" position and ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("squealed");
		else if (random <= 4)	OutStr += gettext("moaned");
		else 	OutStr += gettext("grew hot");
		OutStr += gettext(" as he ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("touched");
		else if (random <= 4)	OutStr += gettext("caressed");
		else /*            */	OutStr += gettext("probed");
		OutStr += gettext(" her defenseless body");
		break;
	case 8:
		OutStr += gettext("lay on the ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("floor");
		else if (random <= 4)	OutStr += gettext("bed");
		else /*            */	OutStr += gettext("couch");
		OutStr += gettext(" and ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("had him take off all her clothes");
		else if (random <= 4)	OutStr += gettext("told him exactly what turned her on");
		else /*            */	OutStr += gettext("encouraged him to take off her bra and panties with his teeth");
		break;
	}
# pragma endregion sex1
	// Roll #2
# pragma region sex2
	OutStr += ". ";
	roll2 = g_Dice % 11 + 1;
	switch (roll2)
	{
	case 1:
		OutStr += gettext("She ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("rode him all the way to the next town");
		else if (random <= 4)	OutStr += gettext("massaged his balls and sucked him dry");
		else if (random <= 6)	OutStr += gettext("titty fucked and sucked the well dry");
		else /*            */	OutStr += gettext("fucked him blind");
		OutStr += gettext(". He was a trooper though and rallied: She ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("was deeply penetrated");
		else if (random <= 4)	OutStr += gettext("was paralyzed with stunning sensations");
		else if (random <= 6)	OutStr += gettext("bucked like a bronko");
		else /*            */	OutStr += gettext("shook with pleasure");
		OutStr += gettext(" and ");
		random = g_Dice % 4 + 1;
		/* */if (random <= 2)	OutStr += gettext("came like a fire hose from");
		else /*            */	OutStr += gettext("repeatedly shook in orgasm with");
		break;
	case 2:
		OutStr += gettext("It took a lot of effort to stay ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 3)	OutStr += gettext("interested in");
		else if (random <= 7)	OutStr += gettext("awake for");
		else 	OutStr += gettext("conscious for");
		break;
	case 3:
		OutStr += gettext("She was fucked ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("blind");
		else if (random <= 4)	OutStr += gettext("silly twice over");
		else if (random <= 6)	OutStr += gettext("all crazy like");
		else if (random <= 8)	OutStr += gettext("for hours");
		else /*            */	OutStr += gettext("for minutes");
		OutStr += gettext(" by"); break;
	case 4:
		OutStr += gettext("She performed ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext("uninspired ");
		else /*            */	OutStr += gettext("inspired ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("missionary ");
		else if (random <= 4)	OutStr += gettext("oral ");
		else if (random <= 6)	OutStr += gettext("foot ");
		else /*            */	OutStr += gettext("hand ");
		OutStr += gettext("sex for"); break;
	case 5:
		//OutStr += ""; 
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("Semen");
		else if (random <= 4)	OutStr += gettext("Praise");
		else if (random <= 6)	OutStr += gettext("Flesh");
		else if (random <= 8)	OutStr += gettext("Drool");
		else /*            */	OutStr += gettext("Chocolate sauce");
		OutStr += gettext(" rained down on her from");
		break;
	case 6:
		OutStr += gettext("She couldn't ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("stand");
		else if (random <= 4)	OutStr += gettext("walk");
		else if (random <= 6)	OutStr += gettext("swallow");
		else if (random <= 8)	OutStr += gettext("feel her legs");
		else /*            */	OutStr += gettext("move");
		OutStr += gettext(" after screwing");
		break;
	case 7:
		OutStr += gettext("It took a great deal of effort to look ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("interested in");
		else if (random <= 4)	OutStr += gettext("awake for");
		else if (random <= 6)	OutStr += gettext("alive for");
		else if (random <= 8)	OutStr += gettext("enthusiastic for");
		else /*            */	OutStr += gettext("hurt for");
		break;
	case 8:
		OutStr += gettext("She played 'clean up the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("tools");
		else if (random <= 4)	OutStr += gettext("customer");
		else if (random <= 6)	OutStr += gettext("sword");
		else 	OutStr += gettext("sugar frosting");
		OutStr += gettext("' with");
		break;
	case 9:
		OutStr += gettext("Hopefully her ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("cervix");
		else if (random <= 4)	OutStr += gettext("pride");
		else if (random <= 6)	OutStr += gettext("reputation");
		else if (random <= 8)	OutStr += gettext("ego");
		else /*            */	OutStr += gettext("stomach");
		OutStr += gettext(" wasn't bruised by");
		break;
	case 10:
		OutStr += gettext("She called in ");
		random = g_Dice % 3 + 2;
		char buffer[10];
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" reinforcements to tame");
		break;
	case 11:
		OutStr += gettext("She orgasmed ");
		random = g_Dice % 100 + 30;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" times with"); break;
	}
# pragma endregion sex2
	// Roll #3
# pragma region sex3
	OutStr += " ";	// Consistency
	roll3 = g_Dice % 20 + 1;
	switch (roll3)
	{
	case 1:
		OutStr += gettext("the guy ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("wearing three amulets of the sex elemental.");
		else if (random <= 4)	OutStr += gettext("wearing eight rings of the horndog.");
		else if (random <= 6)	OutStr += gettext("wearing a band of invulnerability.");
		else if (random <= 8)	OutStr += gettext("carrying a waffle iron.");
		else /*            */	OutStr += gettext("carrying a body probe of irresistable sensations.");
		break;
	case 2: OutStr += gettext("Thor, God of Thunderfucking!!!!"); break;
	case 3:
		OutStr += gettext("the frustrated ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("astronomer.");
		else if (random <= 4)	OutStr += gettext("physicist.");
		else if (random <= 6)	OutStr += gettext("chemist.");
		else if (random <= 8)	OutStr += gettext("biologist.");
		else /*            */	OutStr += gettext("engineer.");
		break;
	case 4: OutStr += gettext("the invisible something or other????"); break;
	case 5: OutStr += gettext("the butler. (He always did it.)"); break;
	case 6:
		OutStr += gettext("the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += "sentient apple tree.";
		else if (random <= 4)	OutStr += gettext("sentient sex toy.");
		else if (random <= 6)	OutStr += gettext("pan-dimensional toothbrush.");
		else if (random <= 8)	OutStr += gettext("magic motorcycle.");
		else /*            */	OutStr += gettext("regular bloke.");
		break;
	case 7:
		OutStr += gettext("the unbelievably well behaved ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("Pink Petal forum member.");
		else if (random <= 4)	OutStr += gettext("tentacle.");
		else if (random <= 6)	OutStr += gettext("pirate.");
		else 	OutStr += gettext("sentient bottle.");
		break;
	case 8:
		random = g_Dice % 20 + 1;
		/* */if (random <= 2)	OutStr += gettext("Cousin");
		else if (random <= 4)	OutStr += gettext("Brother");
		else if (random <= 6)	OutStr += gettext("Saint");
		else if (random <= 8)	OutStr += gettext("Lieutenant");
		else if (random <= 10)	OutStr += gettext("Master");
		else if (random <= 12)	OutStr += gettext("Doctor");
		else if (random <= 14)	OutStr += gettext("Mr.");
		else if (random <= 16)	OutStr += gettext("Smith");
		else if (random <= 18)	OutStr += gettext("DockMaster");
		else /*            */	OutStr += gettext("Perfect");
		OutStr += gettext(" Parkins from down the street.");
		break;
	case 9: OutStr += gettext("the master of the hidden dick technique. (Where is it? Nobody knows.)"); break;
	case 10: OutStr += gettext("cake. It isn't a lie!"); break;
	case 11:
		OutStr += gettext("the really, really macho ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("Titan.");
		else if (random <= 4)	OutStr += gettext("Storm Giant.");
		else if (random <= 6)	OutStr += gettext("small moon.");
		else if (random <= 8)	OutStr += gettext("kobold.");
		else /*            */	OutStr += gettext("madness.");
		break;
	case 12:
		OutStr += gettext("the clockwork man!");
		OutStr += gettext(" (With no sensation in his clockwork ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("tool");
		else if (random <= 4)	OutStr += gettext("head");
		else if (random <= 6)	OutStr += gettext("fingers");
		else if (random <= 8)	OutStr += gettext("attachment");
		else /*            */	OutStr += gettext("clock");
		OutStr += gettext(" and no sense to ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("stop");
		else if (random <= 4)	OutStr += gettext("slow down");
		else if (random <= 6)	OutStr += gettext("moderate");
		else if (random <= 8)	OutStr += gettext("be gentle");
		else
		{
			OutStr += gettext("stop at ");
			random = g_Dice % 50 + 30;
			_itoa(random, buffer, 10);
			OutStr += buffer;
			OutStr += gettext(" orgasms");
		}
		OutStr += gettext(".)");
		break;
	case 13:
		// MYR: This one gives useful advice to the players.  A gift from us to them.
		OutStr += gettext("the Brothel Master developer. ");
		random = g_Dice % 20 + 1;
		/* */if (random <= 2)	OutStr += gettext("(Quick learner is a great talent to have.)");
		else if (random <= 4)	OutStr += gettext("(Don't ignore the practice skills option for your girls.)");
		else if (random <= 6)	OutStr += gettext("(Train your gangs.)");
		else if (random <= 8)	OutStr += gettext("(Every time you restart the game, the shop inventory is reset.)");
		else if (random <= 10)	OutStr += gettext("(Invulnerable (insubstantial) characters should be exploring the catacombs.)");
		else if (random <= 12)	OutStr += gettext("(High dodge gear is great for characters exploring the catacombs.)");
		else if (random <= 14)	OutStr += gettext("(For a character with a high constitution, experiment with working on both shifts.)");
		else if (random <= 16)	OutStr += gettext("(Matrons need high service skills.)");
		else if (random <= 18)	OutStr += gettext("(Girls see a max of 3 people for high reputations, 3 for high appearance and 3 for high skills.)");
		else /*            */	OutStr += gettext("(Don't overlook the bribery option in the town hall and the bank.)");
		break;
	case 14: OutStr += gettext("grandmaster piledriver the 17th."); break;
	case 15:
		OutStr += gettext("the evolved sexual entity from ");
		random = g_Dice % 8 + 1;
		if (random <= 2)
		{
			random = g_Dice % 200000 + 100000;
			_itoa(random, buffer, 10);
			OutStr += buffer;
			OutStr += gettext(" years in the future.");
		}
		else if (random <= 4)	OutStr += gettext("the closet.");
		else if (random <= 6)	OutStr += gettext("the suburbs.");
		else 	OutStr += gettext("somewhere in deep space.");
		break;
	case 16:
		OutStr += gettext("the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("mayor");
		else if (random <= 4)	OutStr += gettext("bishop");
		else if (random <= 6)	OutStr += gettext("town treasurer");
		else 	OutStr += gettext("school principle");
		OutStr += gettext(", on one of his regular health checkups.");
		break;
	case 17: OutStr += gettext("the letter H."); break;
	case 18: OutStr += gettext("a completely regular and unspectacular guy."); break;
	case 19:
		OutStr += gettext("the ");
		random = g_Dice % 20 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" dick, ");
		random = g_Dice % 20 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext("-armed ");
		OutStr += gettext("(Each wearing ");
		random = g_Dice % 2 + 4;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("rings of the Schwarzenegger");
		else if (random <= 4)	OutStr += gettext("rings of the horndog");
		else if (random <= 6)	OutStr += gettext("rings of beauty");
		else 	OutStr += gettext("rings of potent sexual stamina");
		OutStr += gettext(") ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("neighbor");
		else if (random <= 4)	OutStr += gettext("yugoloth");
		else if (random <= 6)	OutStr += gettext("abberation");
		else 	OutStr += gettext("ancient one");
		OutStr += gettext(".");
		break;
	case 20:
		OutStr += gettext("the number 69."); break;
	}
# pragma endregion sex3
	OutStr += gettext("\n");
	return OutStr;
}
string cGirls::GetRandomGroupString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	string OutStr;
	char buffer[10];
	// Part 1
# pragma region group1
	OutStr += gettext(" ");
	roll1 = g_Dice % 4 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1:
		OutStr += gettext("counted the number of customers: ");
		random = g_Dice % 20 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += ". ";
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OutStr += gettext("This was going to be rough");
		else if (random <= 4)	OutStr += gettext("Sweet");
		else if (random <= 6)	OutStr += gettext("It could be worse");
		else if (random <= 8)	OutStr += gettext("A smile formed on her lips. This was going to be fun");
		else if (random <= 10)	OutStr += gettext("Boring");
		else if (random <= 12)	OutStr += gettext("Not enough");
		else /*            */	OutStr += gettext("'Could you get more?' she wondered");
		break;
	case 2: OutStr += gettext("was lost in ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("a sea");
		else if (random <= 4)	OutStr += gettext("a storm");
		else if (random <= 6)	OutStr += gettext("an ocean");
		else /*            */	OutStr += gettext("a jungle");
		OutStr += gettext(" of hot bodies");
		break;
	case 3:
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("sat");
		else if (random <= 4)	OutStr += gettext("lay");
		else if (random <= 6)	OutStr += gettext("stood");
		else /*            */	OutStr += gettext("crouched");
		OutStr += " ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext("blindfolded and ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("handcuffed");
		else if (random <= 4)	OutStr += gettext("tied up");
		else if (random <= 6)	OutStr += gettext("wrists bound in rope");
		else /*            */	OutStr += gettext("wrists in chains hanging from the ceiling");
		OutStr += gettext(" in the middle of a ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("circle");
		else if (random <= 4)	OutStr += gettext("smouldering pile");
		else if (random <= 6)	OutStr += gettext("phalanx");
		else /*            */	OutStr += gettext("wall");
		OutStr += gettext(" of flesh");
		break;
	case 4:
		OutStr += gettext("was ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("amazed by");
		else if (random <= 4)	OutStr += gettext("disappointed by");
		else if (random <= 6)	OutStr += gettext("overjoyed with");
		else /*            */	OutStr += gettext("ecstatically happy with");
		OutStr += gettext(" the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("brigade");
		else if (random <= 4)	OutStr += gettext("army group");
		else if (random <= 6)	OutStr += gettext("squad");
		else /*            */	OutStr += gettext("batallion");
		OutStr += gettext(" of ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("man meat");
		else if (random <= 4)	OutStr += gettext("cock");
		else if (random <= 6)	OutStr += gettext("muscle");
		else /*            */	OutStr += gettext("horny, brainless thugs");
		OutStr += gettext(" around her");
		break;
	}
# pragma endregion group1
	// Part 2
# pragma region group2
	OutStr += gettext(". ");
	roll2 = g_Dice % 8 + 1;
	switch (roll2)
	{
	case 1:
		OutStr += gettext("She was thoroughly ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("banged");
		else if (random <= 4)	OutStr += gettext("fucked");
		else if (random <= 6)	OutStr += gettext("disappointed");
		else /*            */	OutStr += gettext("penetrated");
		OutStr += gettext(" by");
		break;
	case 2:
		OutStr += gettext("They handled her like ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext("an expensive");
		else /*            */	OutStr += gettext("a cheap");
		OutStr += gettext(" ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("hooker");
		else if (random <= 4)	OutStr += gettext("street worker");
		else if (random <= 6)	OutStr += gettext("violin");
		else /*            */	OutStr += gettext("wine");
		OutStr += gettext(" for");
		break;
	case 3:
		OutStr += gettext("Her ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext("holes were");
		else /*            */	OutStr += gettext("love canal was");
		OutStr += gettext(" plugged by");
		break;
	case 4:
		OutStr += gettext("She ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("was bukkaked by");
		else if (random <= 4)	OutStr += gettext("was given pearl necklaces by");
		else 	OutStr += gettext("received a thorough face/hair job from");
		break;
	case 5:
		OutStr += gettext("They demanded simultaneous hand, foot and mouth ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("jobs");
		else if (random <= 4)	OutStr += gettext("action");
		else 	OutStr += gettext("combat");
		OutStr += gettext(" for");
		break;
	case 6:
		OutStr += gettext("There was a positive side: 'So much ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("meat");
		else if (random <= 4)	OutStr += gettext("cock");
		else if (random <= 6)	OutStr += gettext("testosterone");
		else /*            */	OutStr += gettext("to do");
		OutStr += gettext(", so little time' she said to");
		break;
	case 7:
		OutStr += gettext("They made sure she had a nutritious meal of ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("man meat");
		else if (random <= 4)	OutStr += gettext("cock");
		else if (random <= 6)	OutStr += gettext("penis");
		else /*            */	OutStr += gettext("meat rods");
		OutStr += gettext(" and drinks of delicious ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("semen");
		else if (random <= 4)	OutStr += gettext("man mucus");
		else if (random <= 6)	OutStr += gettext("man-love");
		else /*            */	OutStr += gettext("man-cream");
		OutStr += gettext(" from");
		break;
	case 8:
		OutStr += gettext("She was ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("fucked");
		else if (random <= 4)	OutStr += gettext("banged");
		else if (random <= 6)	OutStr += gettext("humped");
		else /*            */	OutStr += gettext("sucked");
		OutStr += gettext(" silly ");
		/* */if (random <= 2)	OutStr += gettext("twice over");
		else if (random <= 4)	OutStr += gettext("three times over");
		else 	OutStr += gettext("so many times");
		OutStr += gettext(" by");
		break;
	}
# pragma endregion group2
	// Part 3
# pragma region group3
	OutStr += gettext(" ");
	roll3 = g_Dice % 11 + 1;
	switch (roll3)
	{
	case 1:
		OutStr += gettext("every member of the Crossgate ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("roads crew.");
		else if (random <= 4)	OutStr += gettext("administrative staff.");
		else if (random <= 6)	OutStr += gettext("interleague volleyball team.");
		else /*            */	OutStr += gettext("short persons defense league.");
		; break;
	case 2:
		OutStr += gettext("all the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("lieutenants");
		else if (random <= 4)	OutStr += gettext("sergeants");
		else if (random <= 6)	OutStr += gettext("captains");
		else /*            */	OutStr += gettext("junior officers");
		OutStr += gettext(" in the Mundigan ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("army.");
		else if (random <= 4)	OutStr += gettext("navy.");
		else if (random <= 6)	OutStr += gettext("elite forces.");
		else /*            */	OutStr += gettext("foreign legion.");
		break;
	case 3:
		OutStr += gettext("the visiting ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("half-giants. (Ouch!)");
		else if (random <= 4)	OutStr += gettext("storm giants.");
		else if (random <= 6)	OutStr += gettext("titans.");
		else /*            */	OutStr += gettext("ogres.");
		break;
	case 4:
		OutStr += gettext("the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("Hentai Research");
		else if (random <= 4)	OutStr += gettext("Women's Rights");
		else if (random <= 6)	OutStr += gettext("Prostitution Studies");
		else /*            */	OutStr += gettext("Celibacy");
		OutStr += gettext(" club of the University of Cunning Linguists.");
		break;
	case 5:
		OutStr += gettext("the squad of ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext("hard-to-find ninjas.");
		else /*            */	OutStr += gettext("racous pirates.");
		break;
	case 6: OutStr += gettext("a group of people from some place called the 'Pink Petal forums'."); break;
	case 7:
		OutStr += gettext("the seemingly endless ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("army");
		else if (random <= 4)	OutStr += gettext("horde");
		else if (random <= 6)	OutStr += gettext("number");
		else /*            */	OutStr += gettext("group");
		OutStr += gettext(" of really");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext(", really ");
		else /*            */	OutStr += gettext(" ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("fired up");
		else if (random <= 4)	OutStr += gettext("horny");
		else if (random <= 6)	OutStr += gettext("randy");
		else /*            */	OutStr += gettext("backed up");
		OutStr += gettext(" ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("gnomes.");
		else if (random <= 4)	OutStr += gettext("halflings.");
		else if (random <= 6)	OutStr += gettext("kobolds.");
		else /*            */	OutStr += gettext("office workers.");
		break;
	case 8:
		OutStr += gettext("CSI ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("New York");
		else if (random <= 4)	OutStr += gettext("Miami");
		else if (random <= 6)	OutStr += gettext("Mundigan");
		else /*            */	OutStr += gettext("Tokyo");
		OutStr += gettext(" branch.");
		break;
	case 9:
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("frosh");
		else if (random <= 4)	OutStr += gettext("seniors");
		else if (random <= 6)	OutStr += gettext("young adults");
		else /*            */	OutStr += gettext("women");
		OutStr += gettext(" on a ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext("serious ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("bender");
		else if (random <= 4)	OutStr += gettext("road trip");
		else if (random <= 6)	OutStr += gettext("medical study");
		else /*            */	OutStr += gettext("lark");
		OutStr += gettext(".");
		break;
	case 10:
		OutStr += gettext("all the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("second stringers");
		else if (random <= 4)	OutStr += gettext("has-beens");
		else if (random <= 6)	OutStr += gettext("never-weres");
		else /*            */	OutStr += gettext("victims");
		OutStr += gettext(" from the ");
		random = g_Dice % 20 + 1991;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" H anime season.");
		break;
	case 11:
		OutStr += gettext("Grandpa Parkins and his extended family of ");
		random = g_Dice % 200 + 100;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(".");
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OutStr += gettext(" (And ");
			random = g_Dice % 100 + 50;
			_itoa(random, buffer, 10);
			OutStr += buffer;
			OutStr += gettext(" guests.)");
		}
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OutStr += gettext(" (And ");
			random = g_Dice % 100 + 50;
			_itoa(random, buffer, 10);
			OutStr += buffer;
			OutStr += gettext(" more from the extended extended family.)");
		}
		break;
	}
# pragma endregion group3
	OutStr += gettext("\n");
	return OutStr;
}
string cGirls::GetRandomBDSMString()
{
	int roll2 = 0, roll3 = 0, random = 0;
	string OutStr;
	char buffer[10];
	OutStr += gettext(" was ");
	// Part 1:
# pragma region bdsm1
	// MYR: Was on a roll so I completely redid the first part
	random = g_Dice % 12 + 1;
	/* */if (random <= 2)	OutStr += gettext("dressed as a dominatrix");
	else if (random <= 4)	OutStr += gettext("stripped naked");
	else if (random <= 6)	OutStr += gettext("dressed as a (strictly legal age) school girl");
	else if (random <= 8)	OutStr += gettext("dressed as a nurse");
	else if (random <= 10)	OutStr += gettext("put in heels");
	else /*            */	OutStr += gettext("covered in oil");
	random = g_Dice % 4 + 1;
	if (random == 3)
	{
		random = g_Dice % 4 + 1;
		if (random <= 2)	OutStr += gettext(", rendered helpless by drugs");
		else 	OutStr += gettext(", restrained by magic");
	}
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OutStr += gettext(", blindfolded");
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OutStr += gettext(", gagged");
	OutStr += gettext(", and ");
	random = g_Dice % 12 + 1;
	/* */if (random <= 2)	OutStr += gettext("chained");
	else if (random <= 4)	OutStr += gettext("lashed");
	else if (random <= 6)	OutStr += gettext("tied");
	else if (random <= 8)	OutStr += gettext("bound");
	else if (random <= 10)	OutStr += gettext("cuffed");
	else /*            */	OutStr += gettext("leashed");
	random = g_Dice % 4 + 1;
	/* */if (random == 3)	OutStr += gettext(", arms behind her back");
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OutStr += gettext(", fettered");
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OutStr += gettext(", spread eagle");
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OutStr += gettext(", upside down");
	OutStr += gettext(" ");
	random = g_Dice % 16 + 1;
	/* */if (random <= 2)	OutStr += gettext("to a bed");
	else if (random <= 4)	OutStr += gettext("to a post");
	else if (random <= 6)	OutStr += gettext("to a wall");
	else if (random <= 8)	OutStr += gettext("to vertical stocks");
	else if (random <= 10)	OutStr += gettext("to a table");
	else if (random <= 12)	OutStr += gettext("on a wooden horse");
	else if (random <= 14)	OutStr += gettext("in stocks");
	else /*            */	OutStr += gettext("at the dog house");
# pragma endregion bdsm1
	// Part 2
# pragma region bdsm2
	OutStr += gettext(". ");
	roll2 = g_Dice % 8 + 1;
	switch (roll2)
	{
	case 1:
		OutStr += gettext("She was fucked ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("with a rake");
		else if (random <= 4)	OutStr += gettext("with a giant dildo");
		else if (random <= 6)	OutStr += gettext("and flogged");
		else if (random <= 8)	OutStr += gettext("and lashed");
		else if (random <= 10)	OutStr += gettext("tenderly");
		else /*            */	OutStr += gettext("like a dog");
		OutStr += gettext(" by");
		break;
	case 2:
		OutStr += gettext("Explanations were necessary before she was ");
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OutStr += gettext("screwed");
		else if (random <= 4)	OutStr += gettext("penetrated");
		else if (random <= 6)	OutStr += gettext("abused");
		else if (random <= 8)	OutStr += gettext("whipped");
		else if (random <= 10)	OutStr += gettext("yelled at");
		else if (random <= 12)	OutStr += gettext("banged repeatedly");
		else /*            */	OutStr += gettext("smacked around");
		OutStr += gettext(" by");
		break;
	case 3:
		OutStr += gettext("Her holes were filled ");
		random = g_Dice % 16 + 1;
		/* */if (random <= 2)	OutStr += gettext("with wiggly things");
		else if (random <= 4)	OutStr += gettext("with vibrating things");
		else if (random <= 6)	OutStr += gettext("with sex toys");
		else if (random <= 8)	OutStr += gettext("by things with uncomfortable edges");
		else if (random <= 10)	OutStr += gettext("with marbles");
		else if (random <= 12)	OutStr += gettext("with foreign objects");
		else if (random <= 14)	OutStr += gettext("with hopes and dreams");
		else /*            */	OutStr += gettext("with semen");
		OutStr += gettext(" by");
		break;
	case 4:
		OutStr += gettext("A massive aphrodisiac was administered before she was ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("teased");
		else if (random <= 4)	OutStr += gettext("fucked");
		else if (random <= 6)	OutStr += gettext("left alone");
		else if (random <= 8)	OutStr += gettext("repeatedly brought to the edge of orgasm, but not over");
		else 	OutStr += gettext("mercilessly tickled by a feather wielded");
		OutStr += gettext(" by");
		break;
	case 5:
		OutStr += gettext("Entertainment was demanded before she was ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("humped");
		else if (random <= 4)	OutStr += gettext("rough-housed");
		else if (random <= 6)	OutStr += gettext("pinched over and over");
		else if (random <= 8)	OutStr += gettext("probed by instruments");
		else if (random <= 10)	OutStr += gettext("fondled roughly");
		else /*            */	OutStr += gettext("sent away");
		OutStr += gettext(" by");
		break;
	case 6:
		OutStr += gettext("She was pierced repeatedly by ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("needles");
		else if (random <= 4)	OutStr += gettext("magic missiles");
		else /*            */	OutStr += gettext("evil thoughts");
		OutStr += gettext(" from");
		break;
	case 7:
		//OutStr += "She had ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("Weights");
		else if (random <= 4)	OutStr += gettext("Christmas ornaments");
		else if (random <= 6)	OutStr += gettext("Lewd signs");
		else if (random <= 6)	OutStr += gettext("Trinkets");
		else /*            */	OutStr += gettext("Abstract symbols");
		OutStr += gettext(" were hung from her unmentionables by");
		break;
	case 8:
		OutStr += gettext("She was ordered to ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("flail herself");
		else if (random <= 4)	OutStr += gettext("perform fellatio");
		else if (random <= 6)	OutStr += gettext("masturbate");
		else /*            */	OutStr += gettext("beg for it");
		OutStr += gettext(" by");
		break;
	}
# pragma endregion bdsm2
	// Part 3
# pragma region bdsm3
	OutStr += gettext(" ");
	roll3 = g_Dice % 18 + 1;
	switch (roll3)
	{
	case 1: OutStr += gettext("Iron Man."); break;
	case 2:
		OutStr += gettext("the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("wizard");
		else if (random <= 4)	OutStr += gettext("sorceress");
		else if (random <= 6)	OutStr += gettext("archmage");
		else /*            */	OutStr += gettext("warlock");
		OutStr += gettext("'s ");
		random = g_Dice % 8;
		/* */if (random <= 2)	OutStr += gettext("golem.");
		else if (random <= 4)	OutStr += gettext("familiar.");
		else if (random <= 6)	OutStr += gettext("homoculous.");
		else /*            */	OutStr += gettext("summoned monster.");
		break;
	case 3:
		OutStr += gettext("the amazingly hung ");
		random = g_Dice % 8;
		if (random <= 2)	OutStr += gettext("goblin.");
		else if (random <= 4)	OutStr += gettext("civic worker.");
		else if (random <= 6)	OutStr += gettext("geletanious cube.");
		else /*            */	OutStr += gettext("sentient shirt.");   // MYR: I love this one.
		break;
	case 4: OutStr += gettext("the pirate dressed as a ninja. (Cool things are cool.)"); break;
	case 5: OutStr += gettext("Hannibal Lecter."); break;
	case 6:
		OutStr += gettext("the stoned ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OutStr += gettext("dark elf.");
		else if (random <= 4)	OutStr += gettext("gargoyle.");
		else if (random <= 6)	OutStr += gettext("earth elemental.");
		else if (random <= 8)	OutStr += gettext("astral deva.");
		else /*            */	OutStr += gettext("college kid.");
		break;
	case 7:
		OutStr += gettext("your hyperactive ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OutStr += gettext("cousin.");
		else if (random <= 4)	OutStr += gettext("grandmother.");
		else if (random <= 6)	OutStr += gettext("grandfather.");
		else if (random <= 8)	OutStr += gettext("brother.");
		else /*            */	OutStr += gettext("sister.");
		break;
	case 8: OutStr += gettext("someone who looks exactly like you!"); break;
	case 9:
		OutStr += gettext("the horny ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OutStr += gettext("genie.");
		else if (random <= 4)	OutStr += gettext("fishmonger.");
		else if (random <= 6)	OutStr += gettext("chauffeur.");
		else if (random <= 8)	OutStr += gettext("Autobot.");
		else /*            */	OutStr += gettext("thought.");
		break;
	case 10:
		OutStr += gettext("the rampaging ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OutStr += gettext("english professor.");
		else if (random <= 4)	OutStr += gettext("peace activist.");
		else if (random <= 6)	OutStr += gettext("color red.");
		else if (random <= 8)	OutStr += gettext("special forces agent.");
		else /*            */	OutStr += gettext("chef.");
		break;
	case 11:
		random = g_Dice % 10 + 1;
		if (random <= 2)	OutStr += gettext("disloyal thugs");
		else if (random <= 4)	OutStr += gettext("girls");
		else if (random <= 6)	OutStr += gettext("dissatisfied customers");
		else if (random <= 8)	OutStr += gettext("workers");
		else /*            */	OutStr += gettext("malicious agents");
		OutStr += gettext(" from a competing brothel."); break;
	case 12: OutStr += gettext("a cruel ");
		random = g_Dice % 8 + 1;
		if (random <= 2)	OutStr += gettext("Cyberman.");
		else if (random <= 4)	OutStr += gettext("Dalek.");
		else if (random <= 6)	OutStr += gettext("Newtype.");
		else /*            */	OutStr += gettext("Gundam.");
		break;
	case 13: OutStr += gettext("Sexbot Mk-");
		random = g_Dice % 200 + 50;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(".");
		break;
	case 14:
		OutStr += gettext("underage kids ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OutStr += gettext("(Who claim to be of age.)");
		else if (random <= 4)	OutStr += gettext("(Who snuck in.)");
		else if (random <= 6)	OutStr += gettext("(Who are somehow related to the Brothel Master, so its ok.)");
		else if (random <= 8)	OutStr += gettext("(They paid, so who cares?)");
		else /*            */	OutStr += gettext("(We must corrupt them while they're still young.)");
		break;
	case 15: OutStr += gettext("Grandpa Parkins from down the street."); break;
	case 16:
		OutStr += gettext("the ... thing living ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OutStr += gettext("in the underwear drawer");
		else if (random <= 4)	OutStr += gettext("in the sex-toy box");
		else if (random <= 6)	OutStr += gettext("under the bed");
		else if (random <= 8)	OutStr += gettext("in her shadow");
		else /*            */	OutStr += gettext("in her psyche");
		OutStr += gettext(".");
		break;
	case 17: OutStr += gettext("the senior member of the cult of ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OutStr += gettext("tentacles.");
		else if (random <= 4)	OutStr += gettext("unending pain.");
		else if (random <= 6)	OutStr += gettext("joy and happiness.");
		else if (random <= 8)	OutStr += gettext("Whore Master developers.");
		else /*            */	OutStr += gettext("eunuchs.");
		break;
	case 18:
		OutStr += gettext("this wierdo who appeared out of this blue box called a ");
		random = g_Dice % 10 + 1;
		if (random <= 5)	OutStr += gettext("TARDIS.");
		else /*            */	OutStr += gettext("TURDIS"); // How many people will say I made a spelling mistake?
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OutStr += " ";
			random = g_Dice % 10 + 1;
			/* */if (random <= 2)	OutStr += gettext("His female companion was in on the action too.");
			else if (random <= 4)	OutStr += gettext("His mechanical dog was involved as well.");
			else if (random <= 6)	OutStr += gettext("His female companion and mechanical dog did lewd things to each other while they watched.");
		}
		break;
	}
# pragma endregion bdsm3
	OutStr += "\n";
	return OutStr;
}
string cGirls::GetRandomBeastString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	char buffer[10];
	string OutStr;
	bool NeedAnd = false;
	OutStr += gettext(" was ");
# pragma region beast1
	roll1 = g_Dice % 7 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1: OutStr += gettext("filled with ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("vibrating");
		else if (random <= 4)	OutStr += gettext("wiggling");
		else if (random <= 6)	OutStr += gettext("living");
		else if (random <= 8)	OutStr += gettext("energetic");
		else if (random <= 10)	OutStr += gettext("big");
		else 	OutStr += gettext("pokey");
		OutStr += gettext(" things that ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("tickled");
		else if (random <= 4)	OutStr += gettext("pleasured");
		else if (random <= 6)	OutStr += gettext("massaged");
		else /*            */	OutStr += gettext("scraped");
		OutStr += gettext(" her insides");
		break;
	case 2:
		OutStr += gettext("forced against ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("a wall");
		else if (random <= 4)	OutStr += gettext("a window");
		else if (random <= 6)	OutStr += gettext("another client");
		else /*            */	OutStr += gettext("another girl");
		OutStr += gettext(" and told to ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("spread her legs");
		else if (random <= 4)	OutStr += gettext("give up hope");
		else if (random <= 6)	OutStr += gettext("hold on tight");
		else /*            */	OutStr += gettext("smile through it");
		break;
	case 3:
		OutStr += gettext("worried by the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("huge size");
		else if (random <= 4)	OutStr += gettext("skill");
		else if (random <= 6)	OutStr += gettext("reputation");
		else /*            */	OutStr += gettext("aggressiveness");
		OutStr += gettext(" of the client");
		break;
	case 4:
		OutStr += gettext("stripped down to her ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("g-string");
		else if (random <= 4)	OutStr += gettext("panties");
		else if (random <= 6)	OutStr += gettext("bra and panties");
		else if (random <= 8)	OutStr += gettext("teddy");
		else /*            */	OutStr += gettext("skin");
		OutStr += gettext(" and covered in ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("compliments");
		else if (random <= 4)	OutStr += gettext("abuse");
		else if (random <= 6)	OutStr += gettext("peanut butter");
		else if (random <= 8)	OutStr += gettext("honey");
		else /*            */	OutStr += gettext("motor oil");
		break;
	case 5:
		OutStr += gettext("chained up in the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("dungeon");
		else if (random <= 4)	OutStr += gettext("den");
		else if (random <= 6)	OutStr += gettext("kitchen");
		else if (random <= 8)	OutStr += gettext("most public of places");
		else /*            */	OutStr += gettext("backyard");
		OutStr += gettext(" and her ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("arms");
		else if (random <= 4)	OutStr += gettext("legs");
		else /*            */	OutStr += gettext("arms and legs");
		OutStr += gettext(" were lashed to posts");
		break;
	case 6:
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("tied up");
		else if (random <= 4)	OutStr += gettext("wrapped up");
		else if (random <= 6)	OutStr += gettext("trapped");
		else if (random <= 8)	OutStr += gettext("bound");
		else /*            */	OutStr += gettext("covered");
		OutStr += gettext(" in ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("flesh");
		else if (random <= 4)	OutStr += gettext("tentacles");
		else if (random <= 6)	OutStr += gettext("cellophane");
		else if (random <= 8)	OutStr += gettext("tape");
		else /*            */	OutStr += gettext("false promises");
		OutStr += gettext(" and ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("groped");
		else if (random <= 4)	OutStr += gettext("tweaked");
		else if (random <= 6)	OutStr += gettext("licked");
		else if (random <= 8)	OutStr += gettext("spanked");
		else /*            */	OutStr += gettext("left alone");
		OutStr += gettext(" for hours");
		break;
	case 7:
		OutStr += gettext("pushed to the limits of ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("flexibility");
		else if (random <= 4)	OutStr += gettext("endurance");
		else if (random <= 6)	OutStr += gettext("patience");
		else if (random <= 8)	OutStr += gettext("consciousness");
		else /*            */	OutStr += gettext("sanity");
		OutStr += gettext(" and ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("cried out");
		else if (random <= 4)	OutStr += gettext("swooned");
		else /*            */	OutStr += gettext("spasmed");
		break;
	}
# pragma endregion beast1
	// Part 2
# pragma region beast2
	OutStr += ". ";
	roll2 = g_Dice % 9 + 1;
	switch (roll2)
	{
	case 1:
		OutStr += gettext("She ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("smoothly");
		else if (random <= 4)	OutStr += gettext("roughly");
		else if (random <= 6)	OutStr += gettext("lustily");
		else if (random <= 8)	OutStr += gettext("repeatedly");
		else /*            */	OutStr += gettext("orgasmically");
		OutStr += gettext(" ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("fucked");
		else if (random <= 4)	OutStr += gettext("railed");
		else if (random <= 6)	OutStr += gettext("banged");
		else if (random <= 8)	OutStr += gettext("screwed");
		else /*            */	OutStr += gettext("pleasured");
		break;
	case 2:
		OutStr += gettext("She was ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("teased");
		else if (random <= 4)	OutStr += gettext("taunted");
		else /*            */	OutStr += gettext("roughed up");
		OutStr += gettext(" and ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("orally");
		else if (random <= 4)	OutStr += "";         // MYR: This isn't a bug.  'physically violated' is redundant, so this just prints 'violated'
		else if (random <= 6)	OutStr += gettext("mentally");
		else if (random <= 8)	OutStr += gettext("repeatedly");
		else /*            */	OutStr += gettext("haughtily");
		OutStr += gettext(" violated by");
		break;
	case 3:
		OutStr += gettext("She was drenched in ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("cum");
		else if (random <= 4)	OutStr += gettext("sweat");
		else if (random <= 6) 	OutStr += gettext("broken hopes and dreams");
		else if (random <= 8)	OutStr += gettext("Koolaid");
		else /*            */	OutStr += gettext("sticky secretions");
		OutStr += gettext(" by");
		break;
	case 4:
		OutStr += gettext("She ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("moaned");
		else if (random <= 4)	OutStr += gettext("winced");
		else if (random <= 6) 	OutStr += gettext("swooned");
		else if (random <= 8) 	OutStr += gettext("orgasmed");
		else /*            */	OutStr += gettext("begged for more");
		OutStr += gettext(" as her stomach repeatedly poked out from ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("thrusts");
		else if (random <= 4)	OutStr += gettext("strokes");
		else if (random <= 6) 	OutStr += gettext("fistings");
		else /*            */	OutStr += gettext("a powerful fucking");
		OutStr += gettext(" by");
		break;
	case 5:
		OutStr += gettext("She used her ");
		if (g_Dice % 10 + 1 <= 5)	{ NeedAnd = true;	OutStr += gettext("hands, "); }
		if (g_Dice % 10 + 1 <= 5)	{ NeedAnd = true;	OutStr += gettext("feet, "); }
		if (g_Dice % 10 + 1 <= 5)	{ NeedAnd = true;	OutStr += gettext("mouth, "); }
		if (NeedAnd == true)		{ NeedAnd = false;	OutStr += gettext("and "); }
		if (g_Dice % 10 + 1 <= 5)	OutStr += gettext("pussy");
		else /*            */	OutStr += gettext("holes");
		OutStr += gettext(" to ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("please");
		else if (random <= 4)	OutStr += gettext("pleasure");
		else /*            */	OutStr += gettext("tame");
		break;
	case 6:
		OutStr += gettext("She shook with ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("orgasmic joy");
		else if (random <= 4)	OutStr += gettext("searing pain");
		else if (random <= 6)	OutStr += gettext("frustration");
		else if (random <= 8)	OutStr += gettext("agony");
		else /*            */	OutStr += gettext("frustrated boredom");
		OutStr += gettext(" when fondled by");
		break;
	case 7:
		OutStr += gettext("It felt like she was ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("ripping apart");
		else if (random <= 4)	OutStr += gettext("exploding");
		else if (random <= 6)	OutStr += gettext("imploding");
		else if (random <= 8)	OutStr += gettext("nothing");
		else /*            */	OutStr += gettext("absent");
		OutStr += gettext(" when handled by");
		break;
	case 8:
		OutStr += gettext("She passed out from ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("pleasure");
		else if (random <= 4)	OutStr += gettext("pain");
		else if (random <= 6)	OutStr += gettext("boredom");
		else if (random <= 8)	OutStr += gettext("rough sex");
		else /*            */	OutStr += gettext("inactivity");
		OutStr += gettext(" from");
		break;
	case 9:
		OutStr += gettext("She screamed as ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("wrenching pain");
		else if (random <= 4)	OutStr += gettext("powerful orgasms");
		else if (random <= 6)	OutStr += gettext("incredible sensations");
		else if (random <= 8)	OutStr += gettext("freight trains");
		else /*            */	OutStr += gettext("lots and lots of nothing");
		OutStr += gettext(" thundered through her from");
		break;
	}
# pragma endregion beast2
	// Part 3
# pragma region beast3
	OutStr += " ";
	roll3 = g_Dice % 12 + 1;
	switch (roll3)
	{
	case 1:
		OutStr += gettext("the ravenous ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("tentacles.");
		else if (random <= 4)	OutStr += gettext(", sex-starved essences of lust.");
		else if (random <= 6)	OutStr += gettext("Balhannoth. (Monster Manual 4, pg. 15.)");
		else if (random <= 8)	OutStr += gettext("priest.");
		else /*            */	OutStr += gettext("Yugoloth.");
		break;
	case 2:
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext("an evil");
		else /*            */	OutStr += gettext("a misunderstood");
		OutStr += gettext(" ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("tengu.");
		else if (random <= 4)	OutStr += gettext("Whore Master developer.");
		else if (random <= 6)	OutStr += gettext("school girl.");
		else if (random <= 8)	OutStr += gettext("garden hose.");
		else /*            */	OutStr += gettext("thought.");
		break;
	case 3:
		OutStr += gettext("a major ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("demon");
		else if (random <= 4)	OutStr += gettext("devil");
		else if (random <= 6)	OutStr += gettext("oni");
		else if (random <= 8)	OutStr += gettext("fire elemental");
		else if (random <= 10)	OutStr += gettext("god");
		else /*            */	OutStr += gettext("Mr. Coffee");
		OutStr += gettext(" from the outer planes.");
		break;
	case 4:
		OutStr += gettext("the angel.");
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OutStr += gettext(" ('");
			random = g_Dice % 8 + 1;
			/* */if (random <= 2)	OutStr += gettext("You're very pretty");
			else if (random <= 4)	OutStr += gettext("I was never here");
			else if (random <= 6)	OutStr += gettext("I had a great time");
			else /*            */	OutStr += gettext("I didn't know my body could do that");
			OutStr += gettext("' he said.)");
		}
		break;
	case 5:
		OutStr += gettext("the ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("demon");
		else if (random <= 4)	OutStr += gettext("major devil");
		else if (random <= 6)	OutStr += gettext("oni");
		else if (random <= 8)	OutStr += gettext("earth elemental");
		else if (random <= 10)	OutStr += gettext("raging hormome beast");
		else /*            */	OutStr += gettext("Happy Fun Ball");
		OutStr += gettext(" with an urge to exercise his ");
		random = g_Dice % 30 + 20;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" cocks and ");
		random = g_Dice % 30 + 20;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" claws.");
		break;
	case 6: OutStr += gettext("the swamp thing with (wait for it) swamp tentacles!"); break;
	case 7: OutStr += gettext("the pirnja gestalt. (The revolution is coming.)"); break;
	case 8:
		OutStr += gettext("the color ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("purple");
		else if (random <= 4)	OutStr += gettext("seven");  // MYR: Not a mistake. I meant to write 'seven'.
		else if (random <= 6)	OutStr += gettext("mauve");
		else if (random <= 8)	OutStr += gettext("silver");
		else if (random <= 10)	OutStr += gettext("ochre");
		else /*            */	OutStr += gettext("pale yellow");
		OutStr += gettext(".");
		break;
	case 9:
		random = g_Dice % 10 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" werewolves wearing ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("true");
		else if (random <= 4)	OutStr += gettext("minor artifact");
		else if (random <= 6)	OutStr += gettext("greater artifact");
		else if (random <= 10)	OutStr += gettext("godly");
		else /*            */	OutStr += gettext("near omnipitent");
		OutStr += gettext(" rings of the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("eternal");
		else if (random <= 4)	OutStr += gettext("body breaking");
		else if (random <= 6)	OutStr += gettext("vorporal");
		else if (random <= 10)	OutStr += gettext("transcendent");
		else /*            */	OutStr += gettext("incorporeal");
		OutStr += gettext(" hard-on.");
		break;
	case 10:
		random = g_Dice % 10 + 5;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" Elder Gods.");
		random = g_Dice % 10 + 1;
		if (random <= 4)
		{
			OutStr += gettext(" (She thought ");
			random = g_Dice % 12 + 1;
			/* */if (random <= 2)	OutStr += gettext("Cthulhu");
			else if (random <= 4)	OutStr += gettext("Hastur");
			else if (random <= 6)	OutStr += gettext("an Old One");
			else if (random <= 8)	OutStr += gettext("Shub-Niggurath");
			else if (random <= 10)	OutStr += gettext("Nyarlathotep");
			else /*            */	OutStr += gettext("Yog-Sothoth");
			OutStr += gettext(" was amongst them, but blacked out after a minute or so.)");
		}
		break;
	case 11:
		OutStr += gettext("the level ");
		random = g_Dice % 20 + 25;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" epic paragon ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("troll");
		else if (random <= 4)	OutStr += gettext("beholder");
		else if (random <= 6)	OutStr += gettext("displacer beast");
		else if (random <= 8)	OutStr += gettext("ettin");
		else if (random <= 10)	OutStr += gettext("gargoyle");
		else /*            */	OutStr += gettext("fire extinguisher");
		OutStr += gettext(" with ");
		random = g_Dice % 20 + 20;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" strength and ");
		random = g_Dice % 20 + 20;
		_itoa(random, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" constitution.");
		break;
	case 12:
		OutStr += gettext("the phalanx of ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OutStr += gettext("horny orcs.");
		else if (random <= 4)	OutStr += gettext("goblins.");
		else if (random <= 6)	OutStr += gettext("sentient marbles.");
		else if (random <= 8)	OutStr += gettext("living garden gnomes.");
		else /*            */	OutStr += gettext("bugbears.");
		break;
	}
# pragma endregion beast3
	OutStr += gettext("\n");
	return OutStr;
}
string cGirls::GetRandomLesString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0, plus = 0;
	string OutStr;
	char buffer[10];
	OutStr += gettext(" ");
	// Part1
# pragma region les1
	roll1 = g_Dice % 6 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1:
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("aggressively");
		else if (random <= 4)	OutStr += gettext("tenderly");
		else if (random <= 6)	OutStr += gettext("slowly");
		else if (random <= 8) 	OutStr += gettext("authoratively");
		else /*            */	OutStr += gettext("violently");
		OutStr += gettext(" ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("straddled");
		else if (random <= 4)	OutStr += gettext("scissored");
		else if (random <= 6)	OutStr += gettext("symmetrically docked with");
		else if (random <= 8) 	OutStr += gettext("cowgirled");
		else /*            */	OutStr += gettext("69ed");
		OutStr += gettext(" the woman");
		break;
	case 2:
		random = g_Dice % 10 + 1;
		if (random <= 5)	OutStr += gettext("shaved her");
		else /*            */	OutStr += gettext("was shaved");
		OutStr += gettext(" with a +");
		plus = g_Dice % 7 + 4;
		_itoa(plus, buffer, 10);
		OutStr += buffer;
		OutStr += gettext(" ");
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OutStr += gettext("vorporal broadsword");
		else if (random <= 4)
		{
			OutStr += gettext("dagger, +");
			plus = plus + g_Dice % 5 + 2;
			_itoa(plus, buffer, 10);
			OutStr += buffer;
			OutStr += gettext(" vs pubic hair");
		}
		else if (random <= 6)	OutStr += gettext("flaming sickle");
		else if (random <= 8) 	OutStr += gettext("lightning burst bo-staff");
		else if (random <= 10)	OutStr += gettext("human bane greatsword");
		else if (random <= 12)	OutStr += gettext("acid burst warhammer");
		else /*            */	OutStr += gettext("feral halfling");
		break;
	case 3:
		OutStr += gettext("had a ");
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OutStr += gettext("pleasant");
		else if (random <= 4)	OutStr += gettext("long");
		else if (random <= 6)	OutStr += gettext("heartfelt");
		else if (random <= 8)	OutStr += gettext("deeply personal");
		else if (random <= 10)	OutStr += gettext("emotional");
		else if (random <= 12)	OutStr += gettext("angry");
		else /*            */	OutStr += gettext("violent");
		OutStr += gettext(" conversation with her lady-client about ");
		random = g_Dice % 16 + 1;
		/* */if (random <= 2)	OutStr += gettext("sadism");
		else if (random <= 4)	OutStr += gettext("particle physics");
		else if (random <= 6)	OutStr += gettext("domination");
		else if (random <= 8) 	OutStr += gettext("submission");
		else if (random <= 10)	OutStr += gettext("brewing poisons");
		else if (random <= 12) 	OutStr += gettext("flower arranging");
		else if (random <= 14)	OutStr += gettext("the Brothel Master");
		else /*            */	OutStr += gettext("assassination techniques");
		break;
	case 4:
		OutStr += gettext("massaged the woman with ");
		// MYR: Ok, I know I'm being super-silly
		random = g_Dice % 20 + 1;
		/* */if (random <= 2)	OutStr += gettext("bath oil");
		else if (random <= 4)	OutStr += gettext("aloe vera");
		else if (random <= 6)	OutStr += gettext("the tears of Chuck Norris's many victims");
		else if (random <= 8)	OutStr += gettext("the blood of innocent angels");
		else if (random <= 10)	OutStr += gettext("Unicorn blood");
		else if (random <= 12)	OutStr += gettext("Unicorn's tears");
		else if (random <= 14)	OutStr += gettext("a strong aphrodisiac");
		else if (random <= 16)	OutStr += gettext("oil of greater breast growth");
		else if (random <= 18)	OutStr += gettext("potent oil of massive breast growth");
		else /*            */	OutStr += gettext("oil of camel-toe growth");
		break;
	case 5:
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext("put a ball gag and blindfolded on");
		else /*            */	OutStr += gettext("put a sensory deprivation hood on");
		OutStr += gettext(", was ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("lashed");
		else if (random <= 4)	OutStr += gettext("cuffed");
		else if (random <= 6)	OutStr += gettext("tied");
		else /*            */	OutStr += gettext("chained");
		OutStr += gettext(" to a ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("bed");
		else if (random <= 4)	OutStr += gettext("bench");
		else if (random <= 6)	OutStr += gettext("table");
		else /*            */	OutStr += gettext("post");
		OutStr += gettext(" and ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("symmetrically docked");
		else if (random <= 4)	OutStr += gettext("69ed");
		else if (random <= 6)	OutStr += gettext("straddled");
		else /*            */	OutStr += gettext("scissored");
		break;
	case 6:
		// MYR: This is like a friggin movie! The epic story of the whore and her customer.
		OutStr += gettext("looked at the woman across from her. ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("Position");
		else if (random <= 4)	OutStr += gettext("Toy");
		else if (random <= 6)	OutStr += gettext("Oil");
		else if (random <= 8)	OutStr += gettext("Bed sheet color");
		else /*            */	OutStr += gettext("Price");
		OutStr += gettext(" was to be ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("a trial");
		else if (random <= 4)	OutStr += gettext("decided");
		else if (random <= 6)	OutStr += gettext("resolved");
		else /*            */	OutStr += gettext("dictated");
		OutStr += gettext(" by combat. Both had changed into ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("string bikinis");
		else if (random <= 4)	OutStr += gettext("lingerie");
		else if (random <= 6)	OutStr += gettext("body stockings");
		else if (random <= 8)	OutStr += gettext("their old school uniforms");
		else /*            */	OutStr += gettext("dominatrix outfits");
		OutStr += gettext(" and wielded ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("whips");
		else if (random <= 4)	OutStr += gettext("staves");
		else if (random <= 6)	OutStr += gettext("boxing gloves");
		else /*            */	OutStr += gettext("cat-o-nine tails");
		OutStr += gettext(" of ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("explosive orgasms");
		else if (random <= 4)	OutStr += gettext("clothes shredding");
		else if (random <= 6)	OutStr += gettext("humiliation");
		else if (random <= 8) 	OutStr += gettext("subjugation");
		else /*            */	OutStr += gettext("brutal stunning");
		OutStr += gettext(". ");
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OutStr += gettext("They stared at each other across the ");
			random = g_Dice % 8 + 1;
			/* */if (random <= 2)	OutStr += gettext("mud");
			else if (random <= 4)	OutStr += gettext("jello");
			else if (random <= 6)	OutStr += gettext("whip cream");
			else /*            */	OutStr += gettext("clothes-eating slime");
			OutStr += gettext(" pit.");
		}
		OutStr += gettext(" A bell sounded! They charged and ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("dueled");
		else if (random <= 4)	OutStr += gettext("fought it out");
		else if (random <= 6)	OutStr += gettext("battled");
		OutStr += gettext("!\n");
		random = g_Dice % 10 + 1;
		if (random <= 6)	OutStr += gettext("The customer won");
		else /*            */	OutStr += gettext("The customer was vanquished");
		break;
	}
# pragma endregion les1
	OutStr += gettext(". ");
	// Part 2
# pragma region les2
	roll2 = g_Dice % 8 + 1;
	switch (roll2)
	{
	case 1:
		OutStr += gettext("She was ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("tormented");
		else if (random <= 4)	OutStr += gettext("teased");
		else if (random <= 6)	OutStr += gettext("massaged");
		else if (random <= 8) 	OutStr += gettext("frustrated");
		else /*            */	OutStr += gettext("satisfied");
		OutStr += gettext(" with ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("great care");
		else if (random <= 4)	OutStr += gettext("deva feathers");
		else if (random <= 6)	OutStr += gettext("drug-soaked sex toys");
		else if (random <= 8) 	OutStr += gettext("extreme skill");
		else /*            */	OutStr += gettext("wild abandon");
		OutStr += gettext(" by");
		break;
	case 2:
		// Case 1 reversed and reworded
		OutStr += gettext("She used ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("phoenix down");
		else if (random <= 4)	OutStr += gettext("deva feathers");
		else if (random <= 6)	OutStr += gettext("drug-soaked sex toys");
		else if (random <= 8) 	OutStr += gettext("restraints");
		else /*            */	OutStr += gettext("her wiles");
		OutStr += gettext(" to ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("torment");
		else if (random <= 4)	OutStr += gettext("tease");
		else if (random <= 6)	OutStr += gettext("massage");
		else if (random <= 8) 	OutStr += gettext("frustrate");
		else /*            */	OutStr += gettext("satisfy");
		break;
	case 3:
		OutStr += gettext("She ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("fingered");
		else if (random <= 4)	OutStr += gettext("teased");
		else if (random <= 6)	OutStr += gettext("caressed");
		else if (random <= 8) 	OutStr += gettext("fondled");
		else /*            */	OutStr += gettext("pinched");
		OutStr += gettext(" the client's ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("clit");
		else if (random <= 4)	OutStr += gettext("clitorus");
		else /*            */	OutStr += gettext("love bud");
		OutStr += gettext(" and expertly elicited orgasm after orgasm from");
		break;
	case 4:
		OutStr += gettext("Her ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("clit");
		else if (random <= 4)	OutStr += gettext("clitorus");
		else /*            */	OutStr += gettext("love bud");
		OutStr += gettext(" was ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("fingered");
		else if (random <= 4)	OutStr += gettext("teased");
		else if (random <= 6)	OutStr += gettext("caressed");
		else if (random <= 8) 	OutStr += gettext("fondled");
		else /*            */	OutStr += gettext("pinched");
		OutStr += gettext(" and she orgasmed repeatedly under the expert touch of");
		break;
	case 5:
		OutStr += gettext("She ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("clam wrestled");
		else if (random <= 4)	OutStr += gettext("rubbed");
		else if (random <= 6)	OutStr += gettext("attacked");
		else /*            */	OutStr += gettext("hammered");
		OutStr += gettext(" the client's ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("clit");
		else if (random <= 4)	OutStr += gettext("clitorus");
		else if (random <= 6)	OutStr += gettext("love bud");
		else /*            */	OutStr += gettext("vagina");
		OutStr += gettext(" causing waves of orgasms to thunder through");
		break;
	case 6:
		OutStr += gettext("She ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("single mindedly");
		else if (random <= 4)	OutStr += gettext("repeatedly");
		else /*            */	OutStr += gettext("roughly");
		OutStr += gettext(" ");
		random = g_Dice % 2 + 1;
		/* */if (random <= 2)	OutStr += gettext("rubbed");
		else if (random <= 4)	OutStr += gettext("fondled");
		else if (random <= 6)	OutStr += gettext("prodded");
		else if (random <= 8)	OutStr += gettext("attacked");
		else if (random <= 10)	OutStr += gettext("tongued");
		else /*            */	OutStr += gettext("licked");
		OutStr += gettext(" the client's g-spot. Wave after wave of ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("orgasms");
		else if (random <= 4)	OutStr += gettext("pleasure");
		else if (random <= 6)	OutStr += gettext("powerful sensations");
		else /*            */	OutStr += gettext("indescribable joy");
		OutStr += gettext(" ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("rushed");
		else if (random <= 4)	OutStr += gettext("thundered");
		else if (random <= 6)	OutStr += gettext("cracked");
		else /*            */	OutStr += gettext("pounded");
		OutStr += gettext(" through");
		break;
	case 7:
		OutStr += gettext("Wave after wave of ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("orgasms");
		else if (random <= 4)	OutStr += gettext("back-stretching joy");
		else if (random <= 6)	OutStr += gettext("madness");
		else /*            */	OutStr += gettext("incredible feeling");
		OutStr += gettext(" ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OutStr += gettext("throbbed");
		else if (random <= 4)	OutStr += gettext("shook");
		else if (random <= 6)	OutStr += gettext("arced");
		else /*            */	OutStr += gettext("stabbed");
		OutStr += gettext(" through her as she was ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("single mindedly");
		else if (random <= 4)	OutStr += gettext("repeatedly");
		else /*            */	OutStr += gettext("roughly");
		OutStr += gettext(" ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("rubbed");
		else if (random <= 4)	OutStr += gettext("fondled");
		else if (random <= 6)	OutStr += gettext("prodded");
		else if (random <= 8)	OutStr += gettext("attacked");
		else if (random <= 10)	OutStr += gettext("tongued");
		else /*            */	OutStr += gettext("licked");
		OutStr += gettext(" by");
		break;
	case 8:
		// MYR: I just remembered about \n
		OutStr += gettext("Work stopped ");
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OutStr += gettext("in the brothel");
		else if (random <= 4)	OutStr += gettext("on the street");
		else if (random <= 6)	OutStr += gettext("all over the block");
		else if (random <= 8)	OutStr += gettext("in the town");
		else if (random <= 10)	OutStr += gettext("within the country");
		else  if (random <= 12)	OutStr += gettext("over the whole planet");
		else  if (random <= 12)	OutStr += gettext("within the solar system");
		else /*            */	OutStr += gettext("all over the galactic sector");
		OutStr += gettext(". Everything was drowned out by:\n\n");
		OutStr += gettext("Ahhhhh!\n\n");
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			random = g_Dice % 6 + 1;
			/* */if (random <= 2)	OutStr += gettext("For the love... of aaaaahhhhh mercy.  No nnnnnnnnh more!\n\n");
			else if (random <= 4)	OutStr += gettext("oooooOOOOOO YES! ahhhhhhHHHH!\n\n");
			else /*            */	OutStr += gettext("nnnnnhhh nnnnnhhhh NNNHHHHHH!!!!\n\n");
		}
		OutStr += gettext("Annnnnhhhhhaaa!\n\n");
		OutStr += gettext("AHHHHHHHH! I'm going to ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("CCCUUUUUUMMMMMMMM!!!!!");
		else if (random <= 4)	OutStr += gettext("EEEXXXXXPLLLOOODDDDEEEE!!!");
		else if (random <= 6)	OutStr += gettext("DIEEEEEE!");
		else if (random <= 8)	OutStr += gettext("AHHHHHHHHHHH!!!!");
		else if (random <= 10)	OutStr += gettext("BRRRREEEEAAAAKKKKKK!!!!");
		else /*            */	OutStr += gettext("WAAAAHHHHHOOOOOOO!!!");
		OutStr += gettext("\nfrom ");
		break;
	}
# pragma endregion les2
	OutStr += " ";
	// Part 3
# pragma region les3
	// For case 2
	int BrothelNo = -1, NumGirlsInBroth = -1;
	sGirl * TempGPtr = 0;
	roll3 = g_Dice % 6 + 1;
	switch (roll3)
	{
	case 1:
		OutStr += gettext("the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("prostitute");
		else if (random <= 4)	OutStr += gettext("street worker");
		else if (random <= 6)	OutStr += gettext("hooker");
		else if (random <= 8)	OutStr += gettext("street walker");
		else /*            */	OutStr += gettext("working girl");
		OutStr += gettext(" from a ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("friendly");
		else if (random <= 4)	OutStr += gettext("rival");
		else if (random <= 6)	OutStr += gettext("hostile");
		else 	OutStr += gettext("feeder");
		OutStr += gettext(" brothel.");
		break;
	case 2:
		BrothelNo = g_Dice%g_Brothels.GetNumBrothels();
		NumGirlsInBroth = g_Brothels.GetNumGirls(BrothelNo);
		random = g_Dice%NumGirlsInBroth;
		TempGPtr = g_Brothels.GetGirl(BrothelNo, random);
		/* */if (TempGPtr == 0)	OutStr += "a girl";
		else /*            */	OutStr += TempGPtr->m_Realname;
		OutStr += gettext(" from ");
		OutStr += g_Brothels.GetName(BrothelNo);
		OutStr += gettext(" brothel.");
		BrothelNo = -1;        // MYR: Paranoia
		NumGirlsInBroth = -1;
		TempGPtr = 0;
		break;
	case 3:
		OutStr += gettext("the ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("sexy");
		else if (random <= 4)	OutStr += gettext("rock hard");
		else if (random <= 6)	OutStr += gettext("hot");
		else if (random <= 8)	OutStr += gettext("androgonous");
		else if (random <= 10)	OutStr += gettext("spirited");
		else /*            */	OutStr += gettext("exuberant");
		OutStr += gettext(" MILF.");
		break;
	case 4:
		OutStr += gettext("the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OutStr += gettext("senior");
		else /*            */	OutStr += gettext("junior");
		OutStr += gettext(" ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("Sorceress");
		else if (random <= 4)	OutStr += gettext("Warrioress");
		else if (random <= 6)	OutStr += gettext("Priestess");
		else if (random <= 8)	OutStr += gettext("Huntress");
		else if (random <= 10)	OutStr += gettext("Amazon");
		else /*            */	OutStr += gettext("Druidess");
		OutStr += gettext(" of the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("Hidden");
		else if (random <= 4)	OutStr += gettext("Silent");
		else if (random <= 6)	OutStr += gettext("Masters");
		else if (random <= 8)	OutStr += gettext("Scarlet");
		else /*            */	OutStr += gettext("Resolute");
		OutStr += gettext(" ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("Hand");
		else if (random <= 4)	OutStr += gettext("Dagger");
		else if (random <= 6)	OutStr += gettext("Will");
		else if (random <= 8)	OutStr += gettext("League");
		else /*            */	OutStr += gettext("Hearts");
		OutStr += gettext(".");
		break;
	case 5:
		OutStr += gettext("the ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OutStr += gettext("high-ranking");
		else if (random <= 4)	OutStr += gettext("mid-tier");
		else /*            */	OutStr += gettext("low-ranking");
		OutStr += gettext(" ");
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OutStr += gettext("elf");
		else if (random <= 4)	OutStr += gettext("woman");     // MYR: Human assumed
		else if (random <= 6)	OutStr += gettext("dryad");
		else if (random <= 8)	OutStr += gettext("succubus");
		else if (random <= 10)	OutStr += gettext("nymph");
		else if (random <= 12)	OutStr += gettext("eyrine");
		else /*            */	OutStr += gettext("cat girl");
		OutStr += gettext(" from the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("Nymphomania");
		else if (random <= 4)	OutStr += gettext("Satyriasis");
		else if (random <= 6)	OutStr += gettext("Women Who Love Sex");
		else if (random <= 8)	OutStr += gettext("Real Women Don't Marry");
		else /*            */	OutStr += gettext("Monster Sex is Best");
		OutStr += gettext(" ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("support group");
		else if (random <= 4)	OutStr += gettext("league");
		else if (random <= 6)	OutStr += gettext("club");
		else if (random <= 8)	OutStr += gettext("faction");
		else /*            */	OutStr += gettext("guild");
		OutStr += gettext(".");
		break;
	case 6:
		OutStr += gettext("the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OutStr += gettext("disguised");
		else if (random <= 4)	OutStr += gettext("hot");
		else if (random <= 6)	OutStr += gettext("sexy");
		else if (random <= 8)	OutStr += gettext("curvacious");
		else /*            */	OutStr += gettext("sultry");
		OutStr += gettext(" ");
		// MYR: Covering the big fetishes/stereotpes
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OutStr += gettext("idol singer");
		else if (random <= 4)	OutStr += gettext("princess");
		else if (random <= 6)	OutStr += gettext("school girl");
		else if (random <= 8)	OutStr += gettext("nurse");
		else if (random <= 10)	OutStr += gettext("maid");
		else /*            */	OutStr += gettext("waitress");
		OutStr += gettext(".");
		break;
	}
# pragma endregion les3
	OutStr += gettext("\n");
	return OutStr;
}
// MYR: Burned out before anal. Someone else feeling creative?
string cGirls::GetRandomAnalString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0;
	string OutStr;
	OutStr += gettext(" ");
	// Part 1
#pragma region anal1
	roll1 = g_Dice % 10 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1: OutStr += ""; break;
	case 2: OutStr += ""; break;
	case 3: OutStr += ""; break;
	case 4: OutStr += ""; break;
	case 5: OutStr += ""; break;
	case 6: OutStr += ""; break;
	case 7: OutStr += ""; break;
	case 8: OutStr += ""; break;
	case 9: OutStr += ""; break;
	case 10: OutStr += ""; break;
	}
#pragma endregion anal1
	OutStr += ". ";
	// Part 2
#pragma region anal2
	roll2 = g_Dice % 10 + 1;
	switch (roll2)
	{
	case 1: OutStr += ""; break;
	case 2: OutStr += ""; break;
	case 3: OutStr += ""; break;
	case 4: OutStr += ""; break;
	case 5: OutStr += ""; break;
	case 6: OutStr += ""; break;
	case 7: OutStr += ""; break;
	case 8: OutStr += ""; break;
	case 9: OutStr += ""; break;
	case 10: OutStr += ""; break;
	}
#pragma endregion anal2
	OutStr += " ";
	// Part 3
#pragma endregion anal3
	roll3 = g_Dice % 10 + 1;
	switch (roll3)
	{
	case 1: OutStr += ""; break;
	case 2: OutStr += ""; break;
	case 3: OutStr += ""; break;
	case 4: OutStr += ""; break;
	case 5: OutStr += ""; break;
	case 6: OutStr += ""; break;
	case 7: OutStr += ""; break;
	case 8: OutStr += ""; break;
	case 9: OutStr += ""; break;
	case 10: OutStr += ""; break;
	}
#pragma endregion anal3
	return OutStr;
}

// ----- Combat

Uint8 cGirls::girl_fights_girl(sGirl* a, sGirl* b)
{
	// NB: For girls exploring the catacombs, a is the character, b the monster
	// Return value of 1 means a (the girl) won
	//   "      "    " 2  "   b (the monster) won
	//   "      "    " 0  "   it was a draw

	CLog l;

	// MYR: Sanity checks on incorporeal. It is actually possible (but very rare) 
	//      for both girls to be incorporeal.
	if(a->has_trait("Incorporeal") && b->has_trait("Incorporeal"))
	{
		l.ss()	<< gettext("\ngirl_fights_girl: Both ") << a->m_Realname << gettext(" and ") << b->m_Realname 
			<< gettext(" are incorporeal, so the fight is a draw.\n");
		return 0;
	}
	else if (a->has_trait("Incorporeal"))
	{
		l.ss()	<< gettext("\ngirl_fights_girl: ") << a->m_Realname << gettext(" is incorporeal, so she wins.\n");
		return 1;
	}
	else if (b->has_trait("Incorporeal"))
	{
		l.ss()	<< gettext("\ngirl_fights_girl: ") << b->m_Realname << gettext(" is incorporeal, so she wins.\n");
		return 2;
	}

	int a_dodge = 0;
	int b_dodge = 0;
	u_int a_attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int b_attack = SKILL_COMBAT;

	if(a == 0 || b == 0)
		return 0;

	// first determine what skills they will fight with
	// girl a
	if(g_Girls.GetSkill(a, SKILL_COMBAT) >= g_Girls.GetSkill(a, SKILL_MAGIC))
		a_attack = SKILL_COMBAT;
	else
		a_attack = SKILL_MAGIC;

	// girl b
	if(g_Girls.GetSkill(b, SKILL_COMBAT) >= g_Girls.GetSkill(b, SKILL_MAGIC))
		b_attack = SKILL_COMBAT;
	else
		b_attack = SKILL_MAGIC;

	// determine dodge ability
	// girl a
	if((g_Girls.GetStat(a, STAT_AGILITY) - g_Girls.GetStat(a, STAT_TIREDNESS)) < 0)
		a_dodge = 0;
	else
		a_dodge = (g_Girls.GetStat(a, STAT_AGILITY) - g_Girls.GetStat(a, STAT_TIREDNESS));

	// girl b
	if((g_Girls.GetStat(b, STAT_AGILITY) - g_Girls.GetStat(b, STAT_TIREDNESS)) < 0)
		b_dodge = 0;
	else
		b_dodge = (g_Girls.GetStat(b, STAT_AGILITY) - g_Girls.GetStat(b, STAT_TIREDNESS));

	l.ss() << gettext("Girl vs. Girl: ") << a->m_Realname << gettext(" fights ") << b->m_Realname << gettext("\n");
	l.ss() << gettext("\t") << a->m_Realname << gettext(": Health ") << a->health() << gettext(", Dodge ") << a_dodge << gettext(", Mana ") << a->mana() << gettext("\n");
	l.ss() << gettext("\t") << b->m_Realname << gettext(": Health ") << b->health() << gettext(", Dodge ") << b_dodge << gettext(", Mana ") << b->mana() << gettext("\n");
	l.ssend();

	sGirl* Attacker = b;
	sGirl* Defender = a;
	unsigned int attack = 0;
	int dodge = a_dodge;
	int attack_count = 0;
	int winner = 0; // 1 for a, 2 for b
	while(1)
	{
		if(g_Girls.GetStat(a, STAT_HEALTH) <= 20)
		{
			g_Girls.UpdateEnjoyment(a, ACTION_COMBAT, -1, true);
			winner = 2;
			break;
		}
		else if(g_Girls.GetStat(b, STAT_HEALTH) <= 20)
		{
			g_Girls.UpdateEnjoyment(b, ACTION_COMBAT, -1, true);
			winner = 1;
			break;
		}

		if(attack_count > 1000)  // if the fight's not over after 1000 blows, call it a tie
		{
			l.ss()	<< gettext("The fight has gone on for too long, over 1000 (attempted) blows either way. Calling it a draw.");
			l.ssend();

			return 0;
		}
		attack_count++;

		if(Attacker == a)
		{
			Attacker = b;
			attack = b_attack;
			Defender = a;
			b_dodge = dodge;
			dodge = a_dodge;
		}
		else if(Attacker == b)
		{
			Attacker = a;
			attack = a_attack;
			Defender = b;
			a_dodge = dodge;
			dodge = b_dodge;
		}

		// Girl attacks
		l.ss() << gettext("\t\t") << Attacker->m_Realname << gettext(" attacks: ");

		if(attack == SKILL_MAGIC)
		{
			if(Attacker->mana() < 7)
				l.ss()	<< gettext("Insufficient mana: using combat");
			else
			{
				Attacker->mana(-7);
				l.ss()	<< gettext("Casts a spell (mana now ") << Attacker->mana() << gettext(")");
			}
		}
		else
			l.ss()	<< gettext("Using physical attack");

		l.ssend();

		int girl_attack_chance = g_Girls.GetSkill(Attacker, attack);
		int die_roll = g_Dice.d100();

		l.ss()	<< gettext("\t\t") << gettext("Attack chance: ") << girl_attack_chance << gettext(" Die roll: ") << die_roll;
		l.ssend();

		if(die_roll > girl_attack_chance)
			l.ss()	<< gettext("\t\t\t") << gettext("Miss!");
		else 
		{
			int damage = 5;
			l.ss()	<< gettext("\t\t\t") << gettext("Hit! base damage is ") << damage << gettext(". ");

	/*
	*		she has hit now calculate how much damage will be done
	*
	*		magic works better than conventional violence.
	*		Link this amount of mana remaining?
	*/
			int bonus = 0;
			if(attack == SKILL_MAGIC)
				bonus = g_Girls.GetSkill(Attacker, SKILL_MAGIC)/5 + 2;
			else 
				bonus = g_Girls.GetSkill(Attacker, SKILL_COMBAT)/10;

			l.ss()	<< gettext("Bonus damage is ") << bonus << gettext(".");
			l.ssend();
			damage += bonus;

			die_roll = g_Dice.d100();

			// Defender attempts Dodge
			l.ss()	<< gettext("\t\t\t\t") << Defender->m_Realname << gettext(" tries to dodge: needs ") << dodge << gettext(", gets ") << die_roll << gettext(": ");

			if(die_roll <= dodge)
			{
				l.ss()	<< gettext("Success!");
				l.ssend();
			}
			else
			{
				l.ss()	<< gettext("Failure! ");
				l.ssend();

				//int con_mod = Defender->m_Stats[STAT_CONSTITUTION] / 10;
				int con_mod = g_Girls.GetStat(Defender, STAT_CONSTITUTION)/10;
				int ActualDmg = damage - con_mod;
				if (ActualDmg <= 0)
					l.ss() << gettext("\t\t\t\t") << Defender->m_Realname << gettext(" shrugs it off.");
				else
				{
					g_Girls.UpdateStat(Defender,STAT_HEALTH,-ActualDmg);
					l.ss() << gettext("\t\t\t\t") << Defender->m_Realname << gettext(" takes ") << damage  << gettext(" damage, less ") << con_mod << gettext(" for CON\n"); 
					l.ss() << gettext("\t\t\t\t\t") << gettext("New health value = ") << Defender->health();
				}
			} // Didn't dodge
		}     // Hit
		l.ssend();

		// update girls dodge ability
		if((dodge - 1) < 0)
			dodge = 0;
		else
			dodge--;

		// Check if girl 2 (the Defender) has been defeated
		// Highest damage is 27 pts.  Checking for health between 20 and 30 is a bug as it can be stepped over.
		if(g_Girls.GetStat(Defender, STAT_HEALTH) <= 40 && g_Girls.GetStat(Defender, STAT_HEALTH) >= 10)
		{
			g_Girls.UpdateEnjoyment(Defender, ACTION_COMBAT, -1, true);
			break; 
		}  // if defeated
	}  // while (1)

	// Girls exploring catacombs: Girl is "a" - and thus wins
	if (Attacker == a)  
	{
		l.ss() << a->m_Realname << gettext(" WINS!");
		l.ssend();

		g_Girls.UpdateEnjoyment(a, ACTION_COMBAT, +1, true);

		return 1;
	}
	if (Attacker == b)  // Catacombs: Monster wins
	{
		l.ss()	<< b->m_Realname << gettext(" WINS!");
		l.ssend();

		g_Girls.UpdateEnjoyment(b, ACTION_COMBAT, +1, true);

		return 2;
	}

	l.ss()	<< gettext("ERROR: cGirls::girl_fights_girl - Shouldn't reach the function bottom.");
	l.ssend();

	return 0;
}

/*
 * passing the girl_wins boolean here to avoid confusion over
 * what returning TRUE actually means
 */

// MYR: How should incorporeal factor in to this?

void sGirl::fight_own_gang(bool &girl_wins)
{
	girl_wins = false;
	vector<sGang*> v = g_Gangs.gangs_on_mission(MISS_GUARDING);
/*
 *	we'll take goons from the top gang in the list
 */
	sGang *gang = v[0];
/*
 *	4 + 1 for each gang on guard duty
 *	that way there's a benefit to multiple gangs guarding
 */
	int max_goons = 4 + v.size();
/*
 *	to the maximum of the number in the gang
 */
	if(max_goons > gang->m_Num) {
		max_goons = gang->m_Num;
	}
/*
 *	now - sum the girl and gang stats
 *	we're not going to average the gangs.
 *	yes this gives them an unfair advantage
 *	that's the point of having 5:1 odds :)
 */
	int girl_stats = combat() + magic() + intelligence();
/*
 *	Now the gangs. I'm not factoring the girl's health
 *	because there's something dramatically satisfying
 *	about her breeaking out of the dungeon after being 
 *	tortured near unto death, and then still beating the
 *	thugs up. You'd buy into it in a Hollywood blockbuster...
 *
 *	Annnnyway....
 */
	int goon_stats = 0;
	for(int i = 0; i < max_goons; i++) {
		goon_stats +=	gang->combat() +
				gang->magic() +
				gang->intelligence()
		;
	}
/*
 *	the girl's base chance of winning is determined by the stat ratio
 */
	double odds = 1.0 * girl_stats / (goon_stats + girl_stats);
/*
 *	let's add some trait based bonuses
 *	I'm not going to do any that are already reflected in stat values
 *	(so no "Psychic" bonus, no "Tough" either)
 *	we can streamline this with the trait overhaul
 */
	if(has_trait("Clumsy"))		odds -= 0.05;
	if(has_trait("Broken Will"))	odds -= 0.10;
	if(has_trait("Meek"))		odds -= 0.05;
	if(has_trait("Dependant"))	odds -= 0.10;
	if(has_trait("Fearless"))	odds += 0.10;
	if (has_trait("Fleet of Foot"))	odds += 0.10;
/*
 *	get it back into the 0 <= N <= 1 range
 */
	if(odds < 0) odds = 0;
	if(odds > 1) odds = 1;
/*
 *	roll the dice! If it passes then the girl wins
 */
	if(g_Dice.percent(int(odds * 100))) {
		win_vs_own_gang(v, max_goons, girl_wins);
	}
	else {
		lose_vs_own_gang(v, max_goons,girl_stats,goon_stats,girl_wins);
	}
}

void sGirl::win_vs_own_gang(vector<sGang*> &v, int max_goons, bool &girl_wins)
{
	sGang *gang = v[0];
	girl_wins = true;
/*
 *	Give her some damage from the combat. She won, so don't kill her.
 *	should really modify this for traits. And agility.
 */
	int damage = g_Dice.random(10) * max_goons;
	health(-damage);
	if(health() == 0) health(1);
/*
 *	now - how many goons die today?
 *	I'm assuming the girl's priority is to escape
 *	rather than "clearing the level" so let's have a base of
 *	1-5
 */
	int casualties = g_Dice.in_range(1,6);
/*
 *	some more trait mods. Assasin adds an extra dice since
 *	she's been trained to deliver the coup-de-grace on a helpless
 *	opponent.
 */
	if(has_trait("Assassin")) {
		casualties += g_Dice.in_range(1,6);
	}
	if(has_trait("Adventurer")) {	// some level clearing instincts
		casualties += 2;
	}
	if(has_trait("Merciless")) casualties ++;
	if(has_trait("Yandere")) casualties ++;
	if(has_trait("Tsundere")) casualties ++;
	if(has_trait("Meek")) casualties --;
	if(has_trait("Dependant")) casualties -= 2;  //misspelled fixed by crazy
/*
 *	fleet of foot means she gets out by running away more than fighting
 *	so fewer fatalities
 */
	if (has_trait("Fleet of Foot")) casualties -= 2;
/*
 *	OK, apply the casualties and make sure it doesn't go negative
 */
	gang->m_Num -= casualties;
	if(gang->m_Num < 0) {
		gang->m_Num = 0;
		
	}
}

void sGirl::lose_vs_own_gang( vector<sGang*> &v, int max_goons, int girl_stats, int gang_stats, bool &girl_wins)
{
	sGang *gang = v[0];
	girl_wins = false;
/*
 *	She's going to get hurt some. Moderating this, we have the fact that
 *	the goons don't really want to damage her (lest the boss get all shouty)
 *	However, depending on how good she is, they may not have a choice
 *
 *	also, I don't want to kill her if she was a full health. I hate it when
 *	that happens. You can send a girl to the dungeons and she's dead before
 *	you've even had a chance to twirl your moustaches at her.
 *	So we need to know how much health she had in the first place
 */
	int start_health = health();
	int damage = g_Dice.random(12) * max_goons;
/*
 *	and if the gangs had the advantage, reduce the
 *	damage actually taken accordingly
 */
	if(gang_stats > girl_stats) {
		damage *= girl_stats;
		damage /= gang_stats;
	}
/*
 *	lastly, make sure this isn't going to kill her, if her health was
 *	> 90 in the first place
 */
	if(start_health >= 90 && damage >= start_health) {
		damage = start_health - 1;
	}
	health(-damage);
/*
 *	need a bit more detail here, really...
 */
	int casualties = g_Dice.in_range(1,6);
	casualties += 3;
	if(casualties < 0) casualties = 0;
	gang->m_Num -= casualties;
}

bool cGirls::GirlInjured(sGirl* girl, unsigned int unModifier)
{  // modifier: 5 = 5% chance, 10 = 10% chance
/*
 *	WD	Injury was only possible if girl is pregnant or
 *		hasn't got the required traits.
 *
 *		Now check for injury first
 *		Use usigned int so can't pass negative chance
 */
//	bool injured = false;
	string message;
	int nMod	= static_cast<int>(unModifier); 

	// Sanity check, Can't get injured
	if (girl->has_trait("Incorporeal")) return false;
	if (girl->has_trait("Fragile")) nMod += nMod;
	if (girl->has_trait("Tough"))	nMod /= 2;

	// Did the girl get injured
	if (!g_Dice.percent(nMod))
		return false;
/*
 *	INJURY PROCESSING
 *	Only injured girls continue past here
 */

	// getting hurt badly could lead to scars
	if (
		g_Dice.percent(nMod*2) &&
		!girl->has_trait("Small Scars") &&
		!girl->has_trait("Cool Scars") &&
		!girl->has_trait("Horrific Scars")
	)
	{
		//injured = true;
		int chance = g_Dice%6;
		if (chance == 0)
		{
			girl->add_trait("Horrific Scars", false);
			message = gettext("She was badly injured, and now has to deal with Horrific Scars.");
		}
		else if (chance <= 2)
		{
			girl->add_trait("Small Scars", false);
			message = gettext("She was injured and now has a couple of Small Scars.");
		}
		else
		{
			girl->add_trait("Cool Scars", false);
			message = gettext("She was injured and scarred. As scars go however, at least they are pretty Cool Scars.");
		}
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// in rare cases, she might even lose an eye
	if (
		g_Dice.percent((nMod/2)) &&
		!girl->has_trait("One Eye") &&
		!girl->has_trait("Eye Patch")
	)
	{
		//injured = true;
		int chance = g_Dice%3;
		if (chance == 0)
		{
			girl->add_trait("One Eye", false);
			message = gettext("Oh, no! She was badly injured, and now only has One Eye!");
		}
		else
		{
			girl->add_trait("Eye Patch", false);
			message = gettext("She was injured and lost an eye, but at least she has a cool Eye Patch to wear.");
		}
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// or become fragile
	if (
		g_Dice.percent((nMod/2))
		&& !girl->has_trait("Fragile")
	)
	{
		//injured = true;
		girl->add_trait("Fragile", false);
		message = gettext("Her body has become rather Fragile due to the extent of her injuries.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// and if pregnant, she might lose the baby; I'll assume inseminations can't be aborted so easily
	if (girl->carrying_human() && g_Dice.percent((nMod*2)))
	{  // unintended abortion time
		//injured = true;
		girl->clear_pregnancy();
		girl->happiness(-20);
		girl->spirit(-5);
		message = gettext("Her unborn child has been lost due to the injuries she sustained, leaving her quite distraught.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// Lose between 5 - 14 hp
	girl->health(-5 - g_Dice%10);

	return true;
}

// MYR: Centralized the damage calculation and added in traits that should apply
//      This fn and TakeCombatDamage(..) are in the attack/defense/agility style of combat
//      and will probably be made redundanty by the new style

int cGirls::GetCombatDamage(sGirl *girl, int CombatType)
{
  int damage = 5;

  // Some traits help for both kinds of combat
  // There are a number of them so I set them at one point each
  // This also has the effect that some traits actually do something in the
  // game now

  if (girl->has_trait("Psychic"))
	  damage++;

  if (girl->has_trait("Adventurer"))
	  damage++;

  if (girl->has_trait("Aggressive"))
	  damage++;

  if (girl->has_trait("Fearless"))
	  damage++;

  if (girl->has_trait("Yandere"))
	  damage++;

  if (girl->has_trait("Merciless"))
	  damage++;

  if (girl->has_trait("Sadistic"))
	  damage++;

  if (girl->has_trait("Twisted"))
	  damage++;

  if(unsigned(CombatType) == SKILL_MAGIC)
  {
	damage += g_Girls.GetSkill(girl, SKILL_MAGIC)/5 + 2;

	// Depending on how you see magic, charisma can influence how it flows
	// (Think Dungeons and Dragons sorcerer)
	if (girl->has_trait("Charismatic"))
	  damage += 1;

	// Same idea as charismatic.
	// Note that I love using brainwashing oil, so this hurts me more than
	// it hurts you
	if (girl->has_trait("Iron Will"))
	  damage += 2;
	else if (girl->has_trait("Broken Will"))
	  damage -= 2;

	if (girl->has_trait("Strong Magic"))
	  damage += 2;

	// Can Mind Fucked people even work magic?
	if (girl->has_trait("Mind Fucked"))
	  damage -= 5;
  }
  else   // SKILL_COMBAT case
  {
	damage += g_Girls.GetSkill(girl, SKILL_COMBAT)/10;

	if (girl->has_trait("Manly"))
	  damage += 2;

	if (girl->has_trait("Strong"))
	  damage += 2;
  }

  return damage;
}

// MYR: Separated out taking combat damage from taking damage from other sources
//     Combat damage can be lowered a bit by certain traits, where other kinds of
//     damage cannot
//
// Returns the new health value

int cGirls::TakeCombatDamage(sGirl* girl, int amt)
{

	if (HasTrait(girl, "Incorporeal"))
	{
		girl->m_Stats[STAT_HEALTH] = 100;	// WD: Sanity - Incorporeal health should allways be at 100%
		return 100;                         // MYR: Sanity is good. Moved to the top
	}
	if (amt == 0) return girl->m_Stats[STAT_HEALTH];

	// This function works with negative numbers, but we'll be flexible and take positive numbers as well
	int value = (amt > 0) ? amt * -1 : amt;

	// High con allows you to shrug off damage
	value = value + (GetStat(girl, STAT_CONSTITUTION) / 20);

	if (HasTrait(girl, "Fragile"))			value -= 3;		// Takes more damage
	if (HasTrait(girl, "Tough"))			value += 2;		// Takes less damage
	if (HasTrait(girl, "Adventurer"))		value += 1;
	if (HasTrait(girl, "Fleet of Foot"))	value += 1;
	if (HasTrait(girl, "Optimist"))			value += 1;
	if (HasTrait(girl, "Pessimist"))		value -= 1;
	if (HasTrait(girl, "Manly"))			value += 1;
	if (HasTrait(girl, "Maschoist"))		value += 2;
	if (HasTrait(girl, "Construct"))		value /= 10;	// `J` constructs take 10% damage
	//  Can't heal when damage is dealed and always at least 1 damage inflicted
	if (value >= 0) value = -1;

	UpdateStat(girl, STAT_HEALTH, value, false);

	return girl->m_Stats[STAT_HEALTH];
}

// ----- Update

void cGirls::UpdateEnjoyment(sGirl* girl, int whatSheEnjoys, int amount, bool wrapTo100)
{
	girl->m_Enjoyment[whatSheEnjoys] += amount;
	if (wrapTo100 == true)
	{
		if (girl->m_Enjoyment[whatSheEnjoys] > 100) 		girl->m_Enjoyment[whatSheEnjoys] = 100;
		else if (girl->m_Enjoyment[whatSheEnjoys] < -100) 	girl->m_Enjoyment[whatSheEnjoys] = -100;
	}
}

// Increment birthday counter and update Girl's age if needed
void cGirls::updateGirlAge(sGirl* girl, bool inc_inService)
{
	// Sanity check. Abort on dead girl
	if (girl->health() <= 0) return;
	if (inc_inService)
	{
		girl->m_WeeksPast++;
		girl->m_BDay++;
	}
	if (girl->m_BDay >= 52)					// Today is girl's birthday
	{
		girl->m_BDay = 0;
		girl->age(1);
		if (girl->age() > 20 && girl->has_trait("Lolita")) g_Girls.RemoveTrait(girl, "Lolita");
	}
}

// Update health and other things for STDs
void cGirls::updateSTD(sGirl* girl)
{
	// Sanity check. Abort on dead girl
	if (girl->health() <= 0) return;

	int a, b, c;
	if (girl->has_trait("AIDS"))
	{
		a = g_Dice % 15 + 5;		girl->health(-a);
		b = g_Dice % 5 + 5;			girl->happiness(-b);
		c = g_Dice % 2 + 1;			girl->tiredness(c);
	}
	if (girl->has_trait("Herpes"))
	{
		a = max(0, g_Dice % 4 - 2);	girl->health(-a);
		b = max(1, g_Dice % 5 - 2);	girl->happiness(-b);
		c = max(0, g_Dice % 4 - 1);	girl->charisma(-c);
	}
	if (girl->has_trait("Chlamydia"))
	{
		a = g_Dice % 3 + 1;			girl->health(-a);
		b = g_Dice % 3 + 1;			girl->happiness(-b);
		c = g_Dice % 2 + 1;			girl->tiredness(c);
	}
	if (girl->has_trait("Syphilis"))
	{
		a = g_Dice % 10 + 5;		girl->health(-a);
		b = g_Dice % 5 + 5;			girl->happiness(-b);
		c = max(0, g_Dice % 4 - 2);	girl->intelligence(-c);
	}

	if (girl->health() <= 0)
	{
		string msg = girl->m_Realname + gettext(" has died from STDs.");
		girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
		g_MessageQue.AddToQue(msg, COLOR_RED);
	}
}

// Stat update code that is to be run every turn
void cGirls::updateGirlTurnStats(sGirl* girl)
{
	if (girl->health() <= 0) return;		// Sanity check. Abort on dead girl

	// TIREDNESS Really tired girls get unhappy fast
	int bonus = girl->tiredness() - 90;
	int b = 0;
	if (bonus > 0)
	{
		girl->obedience(-1);					// Base loss for being tired
		girl->pclove(-1);
		b = bonus / 3 + 1;
		girl->happiness(-b);
		b = bonus / 2 + 1;
		if (girl->health() - b < 10)	// Don't kill the girl from tiredness
		{
			girl->health(10);				// Girl will hate player more if badly hurt from being tired
			girl->pclove(-1);
			girl->pchate(1);
		}
		else girl->health(-bonus);			// Really tired girls lose more health
	}

	// HEALTH hurt girls get tired fast
	bonus = 40 - girl->health();
	if (bonus > 0)
	{
		girl->pchate(1);					// Base loss for being hurt
		girl->pclove(-1);
		girl->happiness(-1);

		bonus = bonus / 8 + 1;			// bonus vs health values 1: 33-39, 2: 25-32, 3: 17-24, 4: 09-16 5: 01-08
		girl->tiredness(bonus);

		bonus = bonus / 2 + 1;			// bonus vs health values 1: 33-39, 2: 17-32, 3: 01-16
		if (girl->health() - bonus < 1)		// Don't kill the girl from low health
		{
			girl->health(1);				// Girl will hate player more for very low health
			girl->pclove(-1);
			girl->pchate(1);
		}
		else
		{
			girl->health(-bonus);
		}

		/*		These messages duplicate warning messages in the matron code
		 *
		 *		msg = "DANGER " + girlName + " health is low!";
		 *		girl->m_Events.AddMessage(msg, IMGTYPE_PROFILE, EVENT_DANGER);
		 */
	}

	// LOVE love is updated only if happiness is >= 100 or < 50
	if (girl->happiness() >= 100)
	{
		girl->pclove(2);					// Happy girls love player more
	}
	if (!girl->is_slave() && girl->happiness() < 50)
	{
		girl->pclove(-2);					// Unhappy FREE girls love player less	
	}
}

// ----- Stream operators

/*
 * While I'm here, let's defined the stream operator for the girls,
 * random and otherwise.
 *
 * This looks big and complex, but it's just printing out the random
 * girl data from the load. It's so I can say cout << *rgirl << endl;
 * and get a something sensible printed
 */

ostream& operator<<(ostream &os, sRandomGirl &g)
{
	os << g.m_Name << endl;
	os << g.m_Desc << endl;
	os << gettext("Human? ") << (g.m_Human ? gettext("Yes") : gettext("No")) << endl;
	os << gettext("Catacomb Dweller? ") << (g.m_Catacomb ? gettext("Yes") : gettext("No")) << endl;
	os << gettext("Arena Girl? ") << (g.m_Arena ? gettext("Yes") : gettext("No")) << endl;
	os << gettext("Your Daughter? ") << (g.m_YourDaughter ? gettext("Yes") : gettext("No")) << endl;
	os << gettext("Money: Min = ") << g.m_MinMoney << gettext(". Max = ") << g.m_MaxMoney << endl;
/*
 *	loop through stats
 *	setw sets a field width for the next operation,
 *	left forces left alignment. Makes the columns line up.
 */
	for(unsigned int i = 0; i < sGirl::max_stats; i++) 
	{
		os << setw(14) << left << sGirl::stat_names[i]
		   << gettext(": Min = ") << (g.m_MinStats[i])
		   << endl
		;
		os << setw(14) << ""
		   << gettext(": Max = ") << (g.m_MaxStats[i])
		   << endl
		;
	}
/*
 *	loop through skills
 */
	for(unsigned int i = 0; i < sGirl::max_skills; i++) 
	{
		os << setw(14) << left << sGirl::skill_names[i]
		   << gettext(": Min = ") << int(g.m_MinSkills[i])
		   << endl
		;
		os << setw(14) << ""
		   << gettext(": Max = ") << int(g.m_MaxSkills[i])
		   << endl
		;
	}
/*
 *	loop through traits
 */
	for(int i = 0; i < g.m_NumTraits; i++) 
	{
		string name = g.m_Traits[i]->m_Name;
		int percent = int(g.m_TraitChance[i]);
		os << gettext("Trait: ")
		   << setw(14) << left << name
		   << ": " << percent
		   << gettext("%")
		   << endl
		;
	}
/*
 *	important to return the stream, so the next 
 *	thing in the << chain has something on which to operate 
 */
	return os;
}

// * another stream operator, this time for sGirl
 
ostream& operator<<(ostream& os, sGirl &g)
{
	os << g.m_Realname << endl;
	os << g.m_Desc << endl;
	os << endl;

	for (int i = 0; i < NUM_STATS; i++)
	{
		os.width(20);
		os.flags(ios::left);
		os << g.stat_names[i] << gettext("\t: ") << int(g.m_Stats[i]) << endl;
	}
	os << endl;

	for (u_int i = 0; i < NUM_SKILLS; i++)
	{
		os.width(20);
		os.flags(ios::left);
		os << g.skill_names[i] << gettext("\t: ") << int(g.m_Skills[i]) << endl;
	}
	os << endl;

	os << endl;
	return os;
}

// ----- Pregnancy, insemination & children

// if contraception is TRUE, then she can't get pregnant which makes sense
static bool has_contraception(sGirl *girl)
{
// `J` rearranged to speed up checks
	if (girl->has_trait("Sterile"))		return true;	// If she's Sterile, she can't get pregnant
	if (girl->is_pregnant())			return true;	// If she's pregnant, she shouldn't get pregnant
	if (girl->m_PregCooldown > 0) 		return true;	// If she's in her cooldown period after giving birth
	if (girl->m_DayJob == JOB_INDUNGEON || girl->m_NightJob == JOB_INDUNGEON)	// `J`
	{
		girl->m_InMovieStudio = girl->m_InCentre = girl->m_InClinic = girl->m_InHouse = girl->m_InArena = girl->m_InFarm = false;
		girl->where_is_she = 0;
	}
	if (UseAntiPreg(girl->m_UseAntiPreg, girl->m_InClinic, girl->m_InMovieStudio, girl->m_InArena, girl->m_InCentre, girl->m_InHouse, girl->m_InFarm, girl->where_is_she))
	{
		return true;
	}
	return false;
}

bool sGirl::calc_pregnancy(int chance, cPlayer *player)
{
	return g_GirlsPtr->CalcPregnancy(this, chance, STATUS_PREGNANT_BY_PLAYER, player->m_Stats, player->m_Skills);
}

sChild::sChild(bool is_players, Gender gender)
{
	m_Unborn = 1;
	m_Age = 0;
	m_IsPlayers = is_players;
	m_Sex = gender;
	m_Next = m_Prev = 0;
	if (gender != None) return;
	m_Sex = (g_Dice.is_girl() ? Girl : Boy);
}

void cChildList::add_child(sChild * child)
{
	m_NumChildren++;
	if (m_LastChild)
	{
		m_LastChild->m_Next = child;
		child->m_Prev = m_LastChild;
		m_LastChild = child;
	}
	else
	{
		m_LastChild = child;
		m_FirstChild = child;
	}
}

sChild * cChildList::remove_child(sChild * child, sGirl * girl)
{
	m_NumChildren--;
	sChild* temp = child->m_Next;
	if (child->m_Next)	child->m_Next->m_Prev = child->m_Prev;
	if (child->m_Prev)	child->m_Prev->m_Next = child->m_Next;
	if (child == girl->m_Children.m_FirstChild)	girl->m_Children.m_FirstChild = child->m_Next;
	if (child == girl->m_Children.m_LastChild)	girl->m_Children.m_LastChild = child->m_Prev;
	child->m_Next = 0;
	delete child;
	return temp;
}

int sGirl::preg_chance(int base_pc, bool good, double factor)
{
	cConfig cfg;
	g_LogFile.ss() << "sGirl::preg_chance: " << "base %chance = " << base_pc << ", " << "good flag = " << good << ", " << "factor = " << factor;
	g_LogFile.ssend();
	double chance = base_pc;
	// factor is used to pass situational modifiers. For instance BDSM has a 25% reduction in chance
	chance *= factor;
	// if the sex was good, the chance is modded, again from the config file
	if (good) chance *= cfg.pregnancy.good_sex_factor();
	return int(chance);
}

bool sGirl::calc_pregnancy(cPlayer *player, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(cfg.pregnancy.player_chance(), good, factor);
	//	now do the calculation
	return g_GirlsPtr->CalcPregnancy(this, int(chance), STATUS_PREGNANT_BY_PLAYER, player->m_Stats, player->m_Skills);
}
bool sGirl::calc_group_pregnancy(cPlayer *player, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(cfg.pregnancy.player_chance(), good, factor);
	// player has 25% chance to be father (4 men in the group)
	int father = STATUS_PREGNANT;
	if (g_Dice.percent(25)) father = STATUS_PREGNANT_BY_PLAYER;
	// now do the calculation
	return g_GirlsPtr->CalcPregnancy(this, int(chance), father, player->m_Stats, player->m_Skills);
}
bool sGirl::calc_pregnancy(sCustomer *cust, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(cfg.pregnancy.customer_chance(), good, factor);
	return g_GirlsPtr->CalcPregnancy(this, int(chance), STATUS_PREGNANT, cust->m_Stats, cust->m_Skills);
}
bool sGirl::calc_insemination(sCustomer *cust, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(cfg.pregnancy.monster_chance(), good, factor);
	return g_GirlsPtr->CalcPregnancy(this, int(chance), STATUS_INSEMINATED, cust->m_Stats, cust->m_Skills);
}
bool sGirl::calc_insemination(cPlayer *player, bool good, double factor)
{
	cConfig cfg;
	double chance = preg_chance(cfg.pregnancy.monster_chance(), good, factor);
	return g_GirlsPtr->CalcPregnancy(this, int(chance), STATUS_INSEMINATED, player->m_Stats, player->m_Skills);
}

bool cGirls::CalcPregnancy(sGirl* girl, int chance, int type, int stats[NUM_STATS], int skills[NUM_SKILLS])
{
	string text = gettext("She has");
	/*
	 *	for reasons I do not understand, but nevertheless think
	 *	are kind of cool, virgins have a +10 to their pregnancy
	 *	chance
	 */
	if (g_Girls.CheckVirginity(girl) && chance > 0) chance += 10;
	/*
	 *	If there's a condition that would stop her getting preggers
	 *	then we get to go home early
	 *
	 *	return TRUE to indicate that pregnancy is FALSE
	 *	(actually, supposed to mean that contraception is true,
	 *	but it also applies for things like being pregnant,
	 *	or just blowing the dice roll. That gets confusing too.
	 */
	if (has_contraception(girl)) return true;
	/*
	 *	the other effective form of contraception, of course,
	 *	is failing the dice roll. Let's check the chance of
	 *	her NOT getting preggers here
	 */
	if (g_Dice.percent(100 - chance)) return true;
	/*
	 *	narrative depends on what it was that Did The Deed
	 *	specifically, was it human or not?
	 */
	switch (type)
	{
	case STATUS_INSEMINATED:
		text += gettext(" been inseminated.");
		break;
	case STATUS_PREGNANT_BY_PLAYER:
		text += gettext(" gotten pregnant with you.");
		break;
	case STATUS_PREGNANT:
	default:
		type = STATUS_PREGNANT;		// `J` rearranged and added default to make sure there are no complications
		text += gettext(" gotten pregnant.");
		break;
	}
	girl->m_States |= (1 << type);	// set the pregnant status
	girl->m_Events.AddMessage(text, IMGTYPE_PREGNANT, EVENT_DANGER);

	sChild* child = new sChild(unsigned(type) == STATUS_PREGNANT_BY_PLAYER);

	// `J` average the mother's and father's stats and skills
	for (int i = 0; i < NUM_STATS; i++)
		child->m_Stats[i] = (stats[i] + girl->m_Stats[i]) / 2;
	for (u_int i = 0; i < NUM_SKILLS; i++)
		child->m_Skills[i] = (skills[i] + girl->m_Skills[i]) / 2;

	// if there is somehow leftover pregnancy data, clear it
	girl->m_WeeksPreg = 0;
	sChild* leftover = girl->m_Children.m_FirstChild;
	while (leftover)
	{
		leftover = girl->next_child(leftover, (leftover->m_Unborn > 0));
	}

	girl->m_Children.add_child(child);
	return false;
}

int cGirls::calc_abnormal_pc(sGirl *mom, sGirl *sprog, bool is_players)
{
	if (is_players == false) 	// the non-pc-daughter case is simpler
	{
		if (mom->has_trait("Your Daughter")) return 0;		// if the mom is your daughter then any customer is a safe dad - genetically speaking, anyway
		if (g_Dice.percent(98)) return 0;					// so what are the odds that this customer fathered both mom and sprog. Let's say 2%
		sprog->add_trait("Incest", false);					// that's enough to give the sprog the incest trait
		if (mom->has_trait("Incest") == false) return 0;	// but there's only a risk of abnormality if mom is herself incestuous
		return 5;											// If we get past all that lot, there's a 5% chance of abnormality
	}
	sprog->add_trait("Your Daughter", false);				// OK. The sprog is the player's get
	if (mom->has_trait("Your Daughter") == false) return 0;	// if mom isn't the player's then there is no problem
	sprog->add_trait("Incest", false);						// she IS, so we add the incest trait
	if (mom->has_trait("Incest")) return 10;				// if mom is also incestuous, that adds 5% to the odds
	return 5;
}

bool cGirls::child_is_grown(sGirl* mom, sChild *child, string& summary, bool PlayerControlled)
{
	cConfig cfg;
	cTariff tariff;
	stringstream ss;
	// bump the age - if it's still not grown, go home
	child->m_Age++;		if (child->m_Age < cfg.pregnancy.weeks_till_grown())	return false;

	// we need a coming of age ceremony
	if (child->is_boy())
	{
		summary += gettext("A son grew of age. ");
		mom->m_States |= (1 << STATUS_HAS_SON);

		if (PlayerControlled)	// get the going rate for a male slave and sell the poor sod
		{
			int gold = tariff.male_slave_sales();
			g_Gold.slave_sales(gold);
			ss << gettext("Her son has grown of age and has been sold into slavery.\n");
			ss << gettext("You make ") << gold << gettext(" gold selling the boy.\n");
		}
		else	// or send him on his way
		{
			int roll = g_Dice % 4;
			ss << "Her son has grown of age and ";
			if (roll == 0)		ss << "moved away";
			else if (roll == 1)	ss << "joined the army";
			else 				ss << "got his own place in town";
			ss << gettext(".\n");
		}
		mom->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
		return true;
	}

	summary += gettext("A daughter grew of age. ");
	mom->m_States |= (1 << STATUS_HAS_DAUGHTER);
	bool slave = mom->is_slave();
	bool AllowNonHuman = mom->has_trait("Not Human");
	// create a new girl for the barn
	sGirl* sprog = g_Girls.CreateRandomGirl(12, false, slave, false, AllowNonHuman);
	// check for incest, get the odds on abnormality
	int abnormal_pc = calc_abnormal_pc(mom, sprog, (child->m_IsPlayers != 0));
	if (g_Dice.percent(abnormal_pc))
	{
		if (g_Dice.percent(50)) g_Girls.AddTrait(sprog, "Malformed");
		else 					g_Girls.AddTrait(sprog, "Retarded");
	}
	// loop throught the mom's traits, inheriting where appropriate
	for (int i = 0; i < mom->m_NumTraits && sprog->m_NumTraits < 30; i++)
	{
		string tname = mom->m_Traits[i]->m_Name;
		if (g_Girls.InheritTrait(mom->m_Traits[i]) && tname != "")
			g_Girls.AddTrait(sprog, mom->m_Traits[i]->m_Name);
	}

	// inherit stats
	for (int i = 0; i < NUM_STATS; i++)
	{
		int min = 0, max = 100;
		if (mom->m_Stats[i] < child->m_Stats[i]) { min = mom->m_Stats[i]; max = child->m_Stats[i]; }
		else 									 { max = mom->m_Stats[i]; min = child->m_Stats[i]; }
		sprog->m_Stats[i] = (g_Dice % (max - min)) + min;
	}

	// set age to 18, fix health
	sprog->m_Stats[STAT_AGE] = 18;
	sprog->m_Stats[STAT_HEALTH] = 100;
	sprog->m_Stats[STAT_HAPPINESS] = 100;
	sprog->m_Stats[STAT_TIREDNESS] = 0;
	sprog->m_Stats[STAT_LEVEL] = 0;
	sprog->m_Stats[STAT_EXP] = 0;

	// inherit skills
	for (u_int i = 0; i < NUM_SKILLS; i++)
	{
		int s = 0;
		if (mom->m_Skills[i] < child->m_Skills[i])	s = child->m_Skills[i];
		else										s = mom->m_Skills[i];
		sprog->m_Skills[i] = g_Dice%max(s, 20);
	}

	// make sure slave daughters have house perc. set to 100, otherwise 60
	sprog->m_Stats[STAT_HOUSE] = (slave) ? cfg.initial.slave_house_perc() : cfg.initial.girls_house_perc();

	if (PlayerControlled)
	{
		ss << "Her daughter has grown of age and has been placed in your Dungeon.";
		g_Brothels.GetDungeon()->AddGirl(sprog, DUNGEON_KID);
	}
	else
	{
		ss << "Her daughter has grown of age and has been placed in your Brothel.";
		g_Girls.AddGirl(sprog);
	}
	mom->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);

	return true;
}

void cGirls::UncontrolledPregnancies()
{
	sGirl* current = m_Parent;
	string summary;
	while (current)
	{
		HandleChildren(current, summary, false);
		current = current->m_Next;
	}
}

void cGirls::HandleChildren(sGirl* girl, string& summary, bool PlayerControlled)
{
	sChild* child;
	girl->m_JustGaveBirth = false;
	/*
	 *	start by advancing pregnancy cooldown time
	 */
	if (girl->m_PregCooldown > 0) girl->m_PregCooldown--;
	/*
	 *	now: if the girl has no children
	 *	we have nothing to do
	 *
	 *	logically this can precede the cooldown bump
	 *	since if she's on cooldown she must have
	 *	given birth
	 *
	 *	but I guess this way offers better bugproofing
	 */
	if (girl->m_Children.m_FirstChild == 0) return;
	/*
	 *	loop through the girl's children,
	 *	and divide them into those growing up
	 *	and those still to be born
	 */
	bool remove_flag;
	child = girl->m_Children.m_FirstChild;
	while (child)
	{
		/*
		 *		if the child is yet unborn
		 *		see if it is due
		 */
		if (child->m_Unborn)
		{
			/*
			 *			some births (monsters) we do not track to adulthood
			 *			these need removing from the list
			 */
			remove_flag = child_is_due(girl, child, summary, PlayerControlled);
		}
		else
		{
			/*
			 *			the child has been born already
			 *
			 *			if it comes of age we remove it from the list
			 */
			remove_flag = child_is_grown(girl, child, summary, PlayerControlled);
		}
		child = girl->next_child(child, remove_flag);
	}
}

bool cGirls::child_is_due(sGirl* girl, sChild *child, string& summary, bool PlayerControlled)
{
	cConfig cfg;
	cTariff tariff;
	stringstream ss;
	/*
	 *	clock on the count and see if she's due
	 *	if not, return false (meaning "do not remove this child yet)
	 */
	girl->m_WeeksPreg++;
	if (girl->m_WeeksPreg < cfg.pregnancy.weeks_pregnant()) return false;
	/*
	 *	OK, it's time to give birth
	 *	start with some basic bookkeeping.
	 */
	girl->m_WeeksPreg = 0;
	child->m_Unborn = 0;
	girl->m_PregCooldown = cfg.pregnancy.cool_down();

	//ADB low health is risky for pregnancy!
	//80 health will add 2 to percent chance of sterility and death, 10 health will add 9 percent!
	int healthFactor = (100 - g_Girls.GetStat(girl, STAT_HEALTH)) / 10;
	/*
	 *	the human-baby case is marginally easier than the
	 *	tentacle-beast-monstrosity one, so we do that first
	 */
	if (girl->carrying_human())
	{
		/*
		 *		first things first - clear the pregnancy bit
		 *		this is a human birth, so add the MILF trait
		 */
		girl->clear_pregnancy();
		g_Girls.AddTrait(girl, "MILF");
		/*
		 *		format a message
		 */
		//summary += "Gave birth. ";
		ss << "She has given birth to a baby " << child->boy_girl_str() << ".\n\nYou grant her the week off for maternity leave.";
		/*
		 *		check for sterility
		 */
		if (g_Dice.percent(1 + healthFactor))
		{
			ss << gettext(" It was a difficult birth and she has lost the ability to have children.");
			g_Girls.AddTrait(girl, "Sterile");
		}
		/*
		 *		queue the message and return false because we need to
		 *		see this one grow up
		 */
		if (PlayerControlled) girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
		girl->m_JustGaveBirth = true;
		return false;
	}
	/*
	 *	It's monster time!
	 *
	 *	How much does one of these suckers bring on the open market
	 *	anyway?
	 *
	 *	might as well record the transaction and clear the preggo bit
	 *	while we're at it
	 */
	if (PlayerControlled)
	{
		long gold = tariff.creature_sales();
		g_Gold.creature_sales(gold);

		//summary += gettext("Gave birth to a beast. ");
		ss << gettext("The creature within her has matured and emerged from her womb.\n");
		ss << gettext("You make ") << gold << gettext(" gold selling the creature.");
		ss << gettext("\n\nYou grant her the week off for her body to recover.");
	}
	girl->clear_pregnancy();
	/*
	 *	check for death
	 */
	if (g_Dice.percent(1 + healthFactor))
	{
		//summary += gettext("And died from it. ");
		ss << gettext("\nSadly, the girl did not survive the experience.");
		girl->m_Stats[STAT_HEALTH] = 0;
	}
	/*
	 *	and sterility
	 *	slight mod: 1% death, 5% sterility for monster births
	 *	as opposed to other way around. Seems better this way.
	 */
	else if (g_Dice.percent(5 + healthFactor))
	{
		ss << gettext(" It was a difficult birth and she has lost the ability to have children.");
		g_Girls.AddTrait(girl, "Sterile");
	}
	/*
	 *	queue the message and return TRUE
	 *	because we're not interested in watching
	 *	little tentacles grow to adulthood
	 */
	if (PlayerControlled) girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
	girl->m_JustGaveBirth = true;
	return true;
}

bool cGirls::InheritTrait(sTrait* trait)
{
	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> InheritTrait

	string name = trait->m_Name;
	if (trait)
	{
		if (name == "Malformed" ||
			name == "Retarded" ||
			name == "Sterile")
		{
			if (g_Dice.percent(10)) return true;
		}
		if (name == "Fragile" ||
			name == "Fleet of Foot" ||
			name == "Clumsy" ||
			name == "Strong" ||
			name == "Psychic" ||
			name == "Strong Magic")
		{
			if (g_Dice.percent(30)) return true;
		}
		if (name == "Tough" ||
			name == "Fast Orgasms" ||
			name == "Slow Orgasms" ||
			name == "Quick Learner" ||
			name == "Slow Learner" ||
			name == "Chlamydia" ||
			name == "Syphilis" ||
			name == "Herpes" ||

			name == "Shroud Addict" ||
			name == "Fairy Dust Addict" ||
			name == "Viras Blood Addict")
		{
			if (g_Dice.percent(50)) return true;
		}
		if (name == "Nymphomaniac")
		{
			if (g_Dice.percent(60)) return true;
		}
		if (name == "Perky Nipples" ||
			name == "Puffy Nipples" ||
			name == "Long Legs" ||
			//zzzzzz boobs

			name == "Big Boobs" ||
			name == "Abnormally Large Boobs" ||
			name == "Small Boobs" ||
			name == "Great Arse" ||
			name == "Great Figure" ||
			name == "AIDS" ||
			name == "Cute" ||
			name == "Strange Eyes" ||
			name == "Different Colored Eyes")
		{
			if (g_Dice.percent(70)) return true;
		}
		if (name == "Demon" ||
			name == "Cat Girl" ||
			name == "Not Human")
			return true;

		// WD: traits that can't be inherited or are a special case
		if (name == "Construct" ||
			name == "Half-Construct" ||
			name == "Cool Scars" ||
			name == "Small Scars" ||
			name == "Horrific Scars" ||
			name == "MILF" ||
			name == "Your Daughter" ||
			name == "Incest" ||
			name == "One Eye" ||
			name == "Eye Patch" ||
			name == "Assassin" ||
			name == "Adventurer" ||
			name == "Mind Fucked" ||
			name == "Broken Will")
			return false;

		// WD: Any unlisted traits here
		if (g_Dice.percent(30))
			return true;
	}
	return false;
}

#if 0
void cGirls::HandleChildren(sGirl* girl, string summary)
{
	cConfig cfg;
	if(girl->m_PregCooldown > 0)
		girl->m_PregCooldown--;
	if(girl->m_Children.m_FirstChild)
	{
		sChild* child = girl->m_Children.m_FirstChild;
		while(child)
		{
			if(child->m_Unborn == 0)
			else	// handle the pregnancy
			{
				girl->m_WeeksPreg++;
				if(girl->m_WeeksPreg >= cfg.pregnancy.weeks_pregnant())
				{
					girl->m_PregCooldown = cfg.pregnancy.cool_down();
					if(girl->m_States & (1<<STATUS_INSEMINATED))
						g_Girls.AddTrait(girl, "MILF");
					string message = "";
					if(girl->m_States&(1<<STATUS_PREGNANT) || girl->m_States&(1<<STATUS_PREGNANT_BY_PLAYER))
					{
						summary += gettext("Gave birth. ");
						message = gettext("She has given birth to a baby ");

						if(child->m_Sex == 1)
							message += gettext("boy.");
						else
							message += gettext("girl.");
						girl->m_States &= ~(1<<STATUS_PREGNANT);
						girl->m_States &= ~(1<<STATUS_PREGNANT_BY_PLAYER);
						if(g_Dice%100 == 0)
						{
							message += gettext(" It was a difficult birth and she has lost the ability to have children.");
							g_Girls.AddTrait(girl, "Sterile");
						}
					}
					else if(girl->m_States & (1<<STATUS_INSEMINATED))
					{
						summary += gettext("Gave birth to beast. ");
						message = gettext("The creature within her has matured and emerged from her womb.\n");
						message += gettext("You make ");
						long gold = (g_Dice%2000)+100;
						_ltoa(gold,buffer,10);
						message += buffer;
						message += gettext(" gold selling the creature.\n");
						g_Gold.creature_sales(gold);

						int death = g_Dice%101;
						if(death < 5)
						{
							summary += gettext("And died from it. ");
							message += gettext("Sadly the girl did not survive the experiance.";
							girl->m_Stats[STAT_HEALTH] = 0;
						}

						girl->m_States &= ~(1<<STATUS_INSEMINATED);
						if(g_Dice%100 == 0)
						{
							message += gettext(" It was a difficult birth and she has lost the ability to have children.");
							g_Girls.AddTrait(girl, "Sterile");
						}

						girl->m_Events.AddMessage(message, IMGTYPE_PREGNANT, 3);
						girl->m_WeeksPreg = 0;
						child->m_Unborn = 0;

						// remove from the list
						//child = girl->m_Children.remove_child(child,girl);
						continue;
					}

					girl->m_Events.AddMessage(message, IMGTYPE_PREGNANT, 3);
					girl->m_WeeksPreg = 0;
					child->m_Unborn = 0;
				}
			}
			child = child->m_Next;
		}
	}
}
#endif

// ----- Image

void cGirls::LoadGirlImages(sGirl* girl)
{
	girl->m_GirlImages = g_Girls.GetImgManager()->LoadList(girl->m_Name);
}

bool cImageList::AddImage(string filename, string path, string file)
{
	//ifstream in;
	//in.open(filename.c_str());
	//if(!in)
	//{
	//	in.close();
	//	return false;
	//}
	//in.close();

	// create image item
	cImage* newImage = new cImage();

	if(filename[filename.size()-1] == 'i')
	{
		string name = path;
		name += "\\ani\\";
		name += file;
		name.erase(name.size()-4, 4);
		name += ".jpg";
		newImage->m_Surface = new CSurface();
		newImage->m_Surface->LoadImage(name);
		newImage->m_AniSurface = new cAnimatedSurface();
		int numFrames, speed, aniwidth, aniheight;
		ifstream input;
		input.open(filename.c_str());
		if(!input)
		{
			CLog l;
			l.ss() << "Incorrect data file given for animation - " << filename;
			l.ssend();
			return false;
		}
		else
			input>>numFrames>>speed>>aniwidth>>aniheight;
		newImage->m_AniSurface->SetData(0,0,numFrames, speed, aniwidth, aniheight, newImage->m_Surface);
		input.close();
		//newImage->m_Surface->FreeResources();  //this was causing lockup in CResourceManager::CullOld
	}
	else
		newImage->m_Surface = new CSurface(filename);

	// Store image item
	if(m_Images)
	{
		m_LastImages->m_Next = newImage;
		m_LastImages = newImage;
	}
	else
		m_LastImages = m_Images = newImage;

	return true;
}

CSurface* cImageList::GetImageSurface(bool random, int& img)
{
	int count = 0;
	int ImageNum = -1;

	if (!random)
	{
		if (img == -1) return 0;
		ImageNum = img;
		cImage* current = m_Images;
		while (current)
		{
			if (count == ImageNum) break;
			count++;
			current = current->m_Next;
		}
		if (current)
		{
			img = ImageNum;
			return current->m_Surface;
		}
	}
	else
	{
		if (m_NumImages == 0) return 0;
		else if (m_NumImages == 1)
		{
			img = 0;
			return m_Images->m_Surface;
		}
		else
		{
			ImageNum = g_Dice%m_NumImages;
			cImage* current = m_Images;
			while (current)
			{
				if (count == ImageNum) break;
				count++;
				current = current->m_Next;
			}
			if (current)
			{
				img = ImageNum;
				return current->m_Surface;
			}
			else
			{
				img = ImageNum;
				return 0;
			}
		}
	}
	img = ImageNum;
	return 0;
}

cAnimatedSurface* cImageList::GetAnimatedSurface(int& img)
{
	int count = 0;
	int ImageNum = -1;
	if (img == -1) return 0;
	ImageNum = img;
	cImage* current = m_Images;
	while (current)
	{
		if (count == ImageNum) break;
		count++;
		current = current->m_Next;
	}
	if (current)
	{
		img = ImageNum;
		if (current->m_AniSurface) return current->m_AniSurface;
		else return 0;
	}
	return 0;
}

bool cImageList::IsAnimatedSurface(int& img)
{
	int count = 0;
	int ImageNum = -1;
	if (img == -1) return false;
	ImageNum = img;
	cImage* current = m_Images;
	while (current)
	{
		if (count == ImageNum) break;
		count++;
		current = current->m_Next;
	}
	if (current)
	{
		img = ImageNum;
		return (current->m_AniSurface) ? true : false;
	}
	return false;
}

int cImageList::DrawImage(int x, int y, int width, int height, bool random, int img)
{
	int count = 0;
	SDL_Rect rect;
	int ImageNum = -1;

	rect.y = rect.x = 0;
	rect.w = width;
	rect.h = height;

	if(!random)
	{
		if(img == -1)
			return -1;

		if(img > m_NumImages)
		{
			if(m_NumImages == 1)
				ImageNum = 0;
			else
				ImageNum = g_Dice%m_NumImages;
		}
		else
			ImageNum = img;
		cImage* current = m_Images;
		while(current)
		{
			if(count == ImageNum)
				break;
			count++;
			current = current->m_Next;
		}

		if(current)
		{
			if(current->m_AniSurface)
				current->m_AniSurface->DrawFrame(x,y, rect.w, rect.h, g_Graphics.GetTicks());
			else
				current->m_Surface->DrawSurface(x,y,0,&rect, true);
		}
	}
	else
	{
		if(m_NumImages == 0)
			return -1;
		else if(m_NumImages == 1)
		{
			m_Images->m_Surface->DrawSurface(x,y,0,&rect, true);
			return 0;
		}
		else
		{
			ImageNum = g_Dice%m_NumImages;
			cImage* current = m_Images;
			while(current)
			{
				if(count == ImageNum)
					break;
				count++;
				current = current->m_Next;
			}

			if(current)
			{
				if(current->m_AniSurface)
					current->m_AniSurface->DrawFrame(x,y, rect.w, rect.h, g_Graphics.GetTicks());
				else
					current->m_Surface->DrawSurface(x,y,0,&rect, true);
			}
			else
				return -1;
		}
	}

	return ImageNum;
}

string cImageList::GetName(int i)
{
	int count = 0;
	cImage* current = m_Images;
	while (current)
	{
		if (count == i) break;
		count++;
		current = current->m_Next;
	}
	if (current) return current->m_Surface->GetFilename();
	return string("");
}

cAImgList* cImgageListManager::ListExists(string name)
{
	cAImgList* current = m_First;
	while (current)
	{
		if (current->m_Name == name) break;
		current = current->m_Next;
	}
	return current;
}

cAImgList* cImgageListManager::LoadList(string name)
{
	cAImgList* current = ListExists(name);
	if (current) return current;

	current = new cAImgList();
	current->m_Name = name;
	current->m_Next = 0;
	/* mod
	uses dir path and file list to construct the girl images
	*/

	DirPath imagedir;
	imagedir << "Resources" << "Characters" << name;
	string numeric = "0123456789 ().,[]-";
	string pic_types[] =
	{
		// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> LoadList *.*g
		"Anal*.*g", "BDSM*.*g", "Sex*.*g", "Beast*.*g", "Group*.*g", "Les*.*g", "torture*.*g",
		"Death*.*g", "Profile*.*g", "Combat*.*g", "Oral*.*g", "Ecchi*.*g", "Strip*.*g", "Maid*.*g", "Sing*.*g",
		"Wait*.*g", "Card*.*g", "Bunny*.*g", "Nude*.*g", "Mast*.*g", "Titty*.*g", "Milk*.*g", "Hand*.*g",
		// added but not used currently
		"Foot*.*g", "Bed*.*g", "Farm*.*g", "Herd*.*g", "Cook*.*g", "Craft*.*g", "Swim*.*g", "Bath*.*g", "Nurse*.*g", "Formal*.*g", "Shop*.*g",

		"Preg*.*g", "PregAnal*.*g", "PregBDSM*.*g", "PregSex*.*g", "pregbeast*.*g", "preggroup*.*g", "pregles*.*g", "pregtorture*.*g", "pregdeath*.*g",
		"pregprofile*.*g", "pregcombat*.*g", "pregoral*.*g", "pregecchi*.*g", "pregstrip*.*g", "pregmaid*.*g",
		"pregsing*.*g", "pregwait*.*g", "pregcard*.*g", "pregbunny*.*g", "pregnude*.*g", "pregmast*.*g",
		"pregtitty*.*g", "pregmilk*.*g", "preghand*.*g",
		// added but not used currently
		"pregFoot*.*g", "pregBed*.*g", "pregFarm*.*g", "pregHerd*.*g", "pregCook*.*g", "pregCraft*.*g",
		"pregSwim*.*g", "pregBath*.*g", "pregNurse*.*g", "pregFormal*.*g", "pregShop*.*g"

	};
	int i = 0;

	do {
		bool to_add = true;
		FileList the_files(imagedir, pic_types[i].c_str());
		for (int k = 0; k < the_files.size(); k++)
		{
			bool test = false;
			/*
			 * `J` fixed this by changing
			 *		string numeric="123456789";
			 * to	string numeric="0123456789 ().,[]-";
			 * Check Preg*.*g filenames [leaf] and accept as non-subtype. ONLY those with number 1--9 in char 5
			 * (Allows filename like 'Preg22.jpg' BUT DOESN'T allow like 'Preg (2).jpg' or 'Preg09.jpg')
			 * MIGHT BE BETTER to just throw out sub-type filenames in this Preg*.*g section. */
			if (i == IMGTYPE_PREGNANT)
			{
				char c = the_files[k].leaf()[4];
				for (int j = 0; j < (int)numeric.size(); j++)
				{
					if (c == numeric[j])
					{
						test = true;
						break;
					}
				}
				if (!test)
				{
					k = the_files.size();
					to_add = false;
				}
			}
			if (to_add)
			{
				current->m_Images[i].AddImage(the_files[k].full());
				current->m_Images[i].m_NumImages++;
			}
		}
		i++;
	} while (i < NUM_IMGTYPES);

	// Yes this is just a hack to load animations (my bad ;) - Necro
	string pic_types2[] = {
		// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> LoadList *.ani
		"Anal*.ani", "BDSM*.ani", "Sex*.ani", "Beast*.ani", "Group*.ani", "Les*.ani", "torture*.ani",
		"Death*.ani", "Profile*.ani", "Combat*.ani", "Oral*.ani", "Ecchi*.ani", "Strip*.ani", "Maid*.ani",
		"Sing*.ani", "Wait*.ani", "Card*.ani", "Bunny*.ani", "Nude*.ani", "Mast*.ani", "Titty*.ani",
		"Milk*.ani", "Hand*.ani", "Nurse*.ani", "Formal*.ani", "Shop*.ani",

		// added but not used currently
		"Foot*.ani", "Bed*.ani", "Farm*.ani", "Herd*.ani", "Cook*.ani", "Craft*.ani", "Swim*.ani", "Bath*.ani",

		"Preg*.ani", "PregAnal*.ani", "PregBDSM*.ani", "PregSex*.ani", "pregbeast*.ani", "preggroup*.ani",
		"pregles*.ani", "pregtorture*.ani", "pregdeath*.ani", "pregprofile*.ani", "pregcombat*.ani", "pregoral*.ani",
		"pregecchi*.ani", "pregstrip*.ani", "pregmaid*.ani", "pregsing*.ani", "pregwait*.ani", "pregcard*.ani",
		"pregbunny*.ani", "pregnude*.ani", "pregmast*.ani", "pregtitty*.ani", "pregmilk*.ani", "preghand*.ani",
		"pregShop*.ani",

		// added but not used currently
		"pregFoot*.ani", "pregBed*.ani", "pregFarm*.ani", "pregHerd*.ani", "pregCook*.ani", "pregCraft*.ani",
		"pregSwim*.ani", "pregBath*.ani", "pregNurse*.ani", "pregFormal*.ani"

	};
	i = 0;
	do {
		bool to_add = true;
		FileList the_files(imagedir, pic_types2[i].c_str());
		for (int k = 0; k < the_files.size(); k++)
		{
			bool test = false;
			/* Check Preg*.*g filenames [leaf] and accept as non-subtypew ONLY those with number 1--9 in char 5
			 * (Allows filename like 'Preg22.jpg' BUT DOESN'T allow like 'Preg (2).jpg' or 'Preg09.jpg')
			 * MIGHT BE BETTER to just throw out sub-type filenames in this Preg*.*g section. */
			if (i == IMGTYPE_PREGNANT)
			{
				char c = the_files[k].leaf()[4];
				for (int j = 0; j < 9; j++) {

					if (c == numeric[j])
					{
						test = true;
						break;
					}
				}
				if (!test)
				{
					k = the_files.size();
					to_add = false;
				}
			}
			if (to_add)
			{
				current->m_Images[i].AddImage(the_files[k].full(), the_files[k].path(), the_files[k].leaf());
				current->m_Images[i].m_NumImages++;
			}
		}
		i++;
	} while (i < NUM_IMGTYPES);

	if (m_Last)
	{
		m_Last->m_Next = current;
		m_Last = current;
	}
	else
		m_First = m_Last = current;

	return current;
}

void cGirls::LoadDefaultImages()	// for now they are hard coded
{
	m_DefImages = m_ImgListManager.LoadList("Default");
}

bool cGirls::IsAnimatedSurface(sGirl* girl, int ImgType, int& img)
{
	//               Loop thru case stmt(s) for image types; if current imagetype has no images, 
	//               substitute for next loop an image type that has/may have images.
	//               Only substitute more-general image types or those with > 0 image count, 
	//               ending with girl profile or default images, avoiding endless loop. 
	//				 (Simplified 'if' logic by using success/failure 'case" statements, 4-5-2013.)

	//				Also added capability to handle passed ImgType of pregnant subtypes

	CLog l;

	while (1)
	{
		/*
		 *		if you sell a girl from the dungeon, and then hotkey back to girl management
		 *		it crashes with girl->m_GirlImages == 0
		 *
		 *		so let's test for that here
		 */
		if (!girl || !girl->m_GirlImages) {
			break;
		}

		switch (ImgType)
		{
			//				kept all cases to test for invalid Image Type.
		case IMGTYPE_TORTURE:
			if (girl->is_pregnant() && girl->m_GirlImages->m_Images[IMGTYPE_TORTURE + PREG_OFFSET].m_NumImages)
				return girl->m_GirlImages->m_Images[IMGTYPE_TORTURE + PREG_OFFSET].IsAnimatedSurface(img);
			if (girl->is_pregnant() && girl->m_GirlImages->m_Images[IMGTYPE_BDSM + PREG_OFFSET].m_NumImages)
				return girl->m_GirlImages->m_Images[IMGTYPE_BDSM + PREG_OFFSET].IsAnimatedSurface(img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_TORTURE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_TORTURE].IsAnimatedSurface(img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_BDSM].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_BDSM].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_PROFILE;		// Try this next loop
			break;

		case IMGTYPE_ANAL:
		case IMGTYPE_BDSM:
		case IMGTYPE_BEAST:
		case IMGTYPE_GROUP:
		case IMGTYPE_LESBIAN:
			//				Similar pregnant/non-pregnant 'success' condition and action; uses 'pregnancy offset'
			if (girl->is_pregnant() && girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].m_NumImages)
				return girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].IsAnimatedSurface(img);
			else if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_SEX;		// Try this next loop
			break;

		case IMGTYPE_PROFILE:
			if (girl->is_pregnant() && girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].m_NumImages)
			{
				//				if(girl->m_newRandomFixed >= 0)  // A simiar process had these 2 lines
				//					random = true;
				return girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].IsAnimatedSurface(img);
			}
			else if (girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].IsAnimatedSurface(img);
			else
				//							Use default images, avoid endless loop
				return m_DefImages->m_Images[IMGTYPE_PROFILE].IsAnimatedSurface(img);
			break;

		case IMGTYPE_PREGANAL:
		case IMGTYPE_PREGBDSM:
		case IMGTYPE_PREGBEAST:
		case IMGTYPE_PREGGROUP:
		case IMGTYPE_PREGLESBIAN:
		case IMGTYPE_PREGSEX:
			if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_SEX;		// Try this next loop
			break;

		case IMGTYPE_PREGNANT:
		case IMGTYPE_DEATH:
			if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			else
				return m_DefImages->m_Images[ImgType].IsAnimatedSurface(img);
			break;

		case IMGTYPE_SEX:
			if (girl->is_pregnant() && girl->m_GirlImages->m_Images[IMGTYPE_PREGSEX].m_NumImages)
				return m_DefImages->m_Images[IMGTYPE_SEX].IsAnimatedSurface(img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_SEX].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_SEX].IsAnimatedSurface(img);
			else
				return m_DefImages->m_Images[IMGTYPE_SEX].IsAnimatedSurface(img);
			break;

		case IMGTYPE_COMBAT:
		case IMGTYPE_MAID:
		case IMGTYPE_SING:
		case IMGTYPE_WAIT:
		case IMGTYPE_CARD:
		case IMGTYPE_BUNNY:
		case IMGTYPE_MILK:
			//				Similar'success' condition and action; 
			if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_PROFILE;

			break;

		case IMGTYPE_ORAL:
		case IMGTYPE_ECCHI:
		case IMGTYPE_STRIP:
		case IMGTYPE_NUDE:
		case IMGTYPE_MAST:
		case IMGTYPE_TITTY:
		case IMGTYPE_HAND:
			//				Similar'success' condition and action  (but no alternative ImgType set); 
			if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			break;

		default:	// `J` return the ImgType if there are any otherwise send error and return 0
			if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);

			//error!
			l.ss() << "cGirls::IsAnimatedSurface: " << "unexpected image type: " << ImgType;
			l.ssend();
			return 0;
			break;

			//				And many conditions return early
		}

		//		If not returned to calling module already, have failed to find ImgType images.
		//              If have not already, test a substitute image type that has/may have images,
		//					substitute ImgType and leave processing for nexp loop if simpler.
		//			(First switch testing success, 2nd setting replacement ImgType replaces complicated 'if's.

		switch (ImgType)
		{
		case IMGTYPE_ORAL:
			ImgType = IMGTYPE_SEX;
			break;

		case IMGTYPE_ECCHI:
			if (girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].IsAnimatedSurface(img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_STRIP:
			if (girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].IsAnimatedSurface(img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_NUDE:
			if (girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].IsAnimatedSurface(img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].IsAnimatedSurface(img);
			else
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_MAST:
			ImgType = IMGTYPE_NUDE;
			break;

		case IMGTYPE_TITTY:
			ImgType = IMGTYPE_ORAL;
			break;

		case IMGTYPE_HAND:
			ImgType = IMGTYPE_ORAL;
			break;

		default:	// `J` return the ImgType if there are any otherwise return 0
			if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].IsAnimatedSurface(img);
			return 0;
		}
	}
	return 0;
}

CSurface* cGirls::GetImageSurface(sGirl* girl, int ImgType, bool random, int& img, bool gallery)
{
	/*
	*		if you sell a girl from the dungeon, and then hotkey back to girl management
	*		it crashes with girl->m_GirlImages == 0
	*		so let's test for that here
	*	`J` hopefully this was fixed but leave the test in anyway
	*/
	if (!girl || !girl->m_GirlImages)	return 0;

	// `J` If the image is for the gallery, just return the images
	if (gallery) return girl->m_GirlImages->m_Images[ImgType].GetImageSurface(random, img);

	// `J` check for pregnant profile
	if (girl->is_pregnant() && ImgType == IMGTYPE_PROFILE &&
		!girl->m_GirlImages->m_Images[IMGTYPE_PREGPROFILE].m_NumImages &&
		girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].m_NumImages)
		return girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].GetImageSurface(random, img);

	// `J` check if she is pregnant and if there are any Preg images for the requested type
	if (girl->is_pregnant() && ImgType < PREG_OFFSET && ImgType != IMGTYPE_PREGNANT &&
		girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].m_NumImages)
		return girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].GetImageSurface(random, img);

	// `J` check if there are any images for the requested type
	if (girl->m_GirlImages->m_Images[ImgType].m_NumImages)
		return girl->m_GirlImages->m_Images[ImgType].GetImageSurface(random, img);

	// `J` if image type is preg varitant, replace it with nonpreg value before checking alts
	if (ImgType >= PREG_OFFSET && ImgType != IMGTYPE_PREGNANT)
		ImgType -= PREG_OFFSET;

	// `J` create list of alternates for testing and set null values as -1
	int alttypes[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	switch (ImgType)
	{
		// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> GetImageSurface
		// First do sex types
	case IMGTYPE_ANAL:		alttypes[0] = IMGTYPE_SEX;		break;
	case IMGTYPE_BDSM:		alttypes[0] = IMGTYPE_SEX;		break;
	case IMGTYPE_SEX:		alttypes[0] = IMGTYPE_ANAL;		alttypes[1] = IMGTYPE_GROUP;	alttypes[2] = IMGTYPE_LESBIAN;
		alttypes[3] = IMGTYPE_ORAL;		alttypes[4] = IMGTYPE_TITTY;	alttypes[5] = IMGTYPE_HAND;
		alttypes[6] = IMGTYPE_FOOT;		break;
	case IMGTYPE_BEAST:		alttypes[0] = IMGTYPE_SEX;		break;
	case IMGTYPE_GROUP:		alttypes[0] = IMGTYPE_SEX;		break;
	case IMGTYPE_LESBIAN:	alttypes[0] = IMGTYPE_NUDE;		alttypes[1] = IMGTYPE_SEX;		break;
	case IMGTYPE_ORAL:		alttypes[0] = IMGTYPE_HAND;		alttypes[1] = IMGTYPE_TITTY;	alttypes[2] = IMGTYPE_FOOT;
		alttypes[3] = IMGTYPE_SEX;		break;
	case IMGTYPE_TITTY:		alttypes[0] = IMGTYPE_HAND;		alttypes[1] = IMGTYPE_ORAL;		alttypes[2] = IMGTYPE_SEX;		break;
	case IMGTYPE_HAND:		alttypes[0] = IMGTYPE_ORAL;		alttypes[1] = IMGTYPE_TITTY;	alttypes[2] = IMGTYPE_FOOT;
		alttypes[3] = IMGTYPE_SEX;		break;
	case IMGTYPE_FOOT:		alttypes[0] = IMGTYPE_HAND;		alttypes[1] = IMGTYPE_ORAL;		alttypes[2] = IMGTYPE_TITTY;
		alttypes[3] = IMGTYPE_SEX;		break;

		// torture
	case IMGTYPE_TORTURE:	alttypes[0] = IMGTYPE_BDSM;		alttypes[1] = IMGTYPE_DEATH;	break;

		// single girl, non sex
	case IMGTYPE_NUDE:		alttypes[0] = IMGTYPE_STRIP;	alttypes[1] = IMGTYPE_ECCHI;	break;
	case IMGTYPE_MAST:		alttypes[0] = IMGTYPE_NUDE;		alttypes[1] = IMGTYPE_STRIP;	alttypes[2] = IMGTYPE_ECCHI;	break;
	case IMGTYPE_ECCHI:		alttypes[0] = IMGTYPE_STRIP;	alttypes[1] = IMGTYPE_NUDE;		break;
	case IMGTYPE_MILK:
	case IMGTYPE_STRIP:
	case IMGTYPE_BED:
	case IMGTYPE_SWIM:
	case IMGTYPE_BATH:
		alttypes[0] = IMGTYPE_ECCHI;	alttypes[1] = IMGTYPE_NUDE;		break;

		// types that alt to bunny and formal
	case IMGTYPE_WAIT:
	case IMGTYPE_MAID:
	case IMGTYPE_SING:
	case IMGTYPE_CARD:		alttypes[0] = IMGTYPE_BUNNY;	break;

		// Farm types
	case IMGTYPE_FARM:		alttypes[0] = -1;				break;
	case IMGTYPE_COOK:		alttypes[0] = IMGTYPE_WAIT;		alttypes[1] = IMGTYPE_MAID;	break;
	case IMGTYPE_HERD:		alttypes[0] = IMGTYPE_FARM;		break;
	case IMGTYPE_CRAFT:		alttypes[0] = IMGTYPE_FARM;		break;

		// these image types have no alt types
		//	case IMGTYPE_NURSE:
		//	case IMGTYPE_FORMAL:
	case IMGTYPE_BUNNY:
	case IMGTYPE_DEATH:
	case IMGTYPE_COMBAT:
	case IMGTYPE_PREGNANT:
	case IMGTYPE_PROFILE:
	case IMGTYPE_SHOP:
	default:
		alttypes[0] = -1;
		break;

	}
	// `J` first check if there are preg varients
	if (girl->is_pregnant() && ImgType != IMGTYPE_PREGNANT)
	{
		for (int i = 0; i < 10; i++)
		{
			if (alttypes[i] == -1) break;
			if (girl->m_GirlImages->m_Images[alttypes[i] + PREG_OFFSET].m_NumImages)
				return girl->m_GirlImages->m_Images[alttypes[i] + PREG_OFFSET].GetImageSurface(random, img);
		}
	}
	// `J` then check varients
	for (int i = 0; i < 10; i++)
	{
		if (alttypes[i] == -1) break;
		if (girl->m_GirlImages->m_Images[alttypes[i]].m_NumImages)
		{
			return girl->m_GirlImages->m_Images[alttypes[i]].GetImageSurface(random, img);
		}
	}
	// `J` if there are no alternate types found then try default images
	if (girl->is_pregnant() && ImgType == IMGTYPE_PROFILE && m_DefImages->m_Images[IMGTYPE_PREGNANT].m_NumImages)
	{
		return m_DefImages->m_Images[IMGTYPE_PREGNANT].GetImageSurface(random, img);
	}
	if (girl->is_pregnant() && ImgType != IMGTYPE_PREGNANT && m_DefImages->m_Images[ImgType + PREG_OFFSET].m_NumImages)
	{
		return m_DefImages->m_Images[ImgType + PREG_OFFSET].GetImageSurface(random, img);
	}
	if (m_DefImages->m_Images[ImgType].m_NumImages)
	{
		return m_DefImages->m_Images[ImgType].GetImageSurface(random, img);
	}
	// `J` if there are no alternate or default types found then try profile
	if (girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages)
	{
		return girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].GetImageSurface(random, img);
	}
	if (m_DefImages->m_Images[IMGTYPE_PROFILE].m_NumImages)
	{
		return m_DefImages->m_Images[IMGTYPE_PROFILE].GetImageSurface(random, img);
	}

	return 0;		// would be failure to find & put image on surface, but errcode not passed back
}

cAnimatedSurface* cGirls::GetAnimatedSurface(sGirl* girl, int ImgType, int& img)
{
	return girl->m_GirlImages->m_Images[ImgType].GetAnimatedSurface(img);
}

/*
 * takes a girl, and image type number, and the pregnant equivalent thereof.
 *
 * returns the pregnant number if A) girl is preggers and B) girl specific images
 * exist
 *
 * Otherwise returns the non-preggy number, if the girl has images for that action
 *
 * if not, returns -1 so the caller can sort it out
 */
// `J` This is not used anywhere so it is not getting updated at this time
int cGirls::get_modified_image_type(sGirl *girl, int image_type, int preg_type)
{
	bool preg = girl->is_pregnant();
	/*
	 *	if she's pregnant, and if there exist pregnant images for
	 *	whatever this action is - use them
	 */
	if (preg && girl->m_GirlImages->m_Images[preg_type].m_NumImages > 0) {
		return preg_type;
	}
	/*
	 *	if not, pregnant or not, try and find a non pregnant image for this sex type
	 *	the alternative would be to use a pregnant vanilla sex image - but
	 *	we're keeping the sex type in preference to the pregnancy
	 */
	if (girl->m_GirlImages->m_Images[image_type].m_NumImages > 0) {
		return image_type;
	}
	/*
	 *	rather than try for pregnant straight sex and straight sex
	 *	let's just return -1 here and let the caller re-try with
	 *	normal sex arguments
	 */
	return -1;
}

/*
 * Given an image type, this tries to draw the girls own version of that image if available. 
 * If not, it takes one from the default set
 */
// `J` This is not used anywhere so it is not getting updated at this time
int cGirls::draw_with_default(sGirl* girl, int x, int y, int width, int height, int ImgType, bool random, int img)
{
	cImageList *images;
	/*
	 *	does the girl have her own pics for this image type
	 *	or do we need to use the default ones?
	 */
	if (girl->m_GirlImages->m_Images[ImgType].m_NumImages == 0) {
		images = m_DefImages->m_Images + ImgType;
	}
	else {
		images = girl->m_GirlImages->m_Images + ImgType;
	}
	/*
	 *	draw and return
	 */
	return images->DrawImage(x, y, width, height, random, img);
}

// `J` This is not used anywhere so it is not getting updated at this time
int cGirls::DrawGirl(sGirl* girl, int x, int y, int width, int height, int ImgType, bool random, int img)
{
	bool preg = false;
	if (girl->is_pregnant()) preg = true;
	while (1)
	{
		//               Loop thru case stmt(s) for image types; if current imagetype has no images, 
		//               substitute for next loop an image type that has/may have images.
		//               Only substitute more-general image types or those with > 0 image count, 
		//               ending with girl profile or default images, avoiding endless loop. 
		//				 (Simplified 'if' logic by using success/failure 'case" statements, 4-5-2013.)

		//				Also added capability to handle passed ImgType of pregnant subtypes

		/*
		 *		if you sell a girl from the dungeon, and then hotkey back to girl management
		 *		it crashes with girl->m_GirlImages == 0
		 *
		 *		so let's test for that here
		 */
		if (!girl || !girl->m_GirlImages) break;

		//			NOTE that this DrawGirl() section does not check if 'gallery' like 
		//			     GetImageSurface() does.

		switch (ImgType)
		{
			//				kept all cases to test for invalid Image Type.
		case IMGTYPE_TORTURE:
			if (preg && girl->m_GirlImages->m_Images[IMGTYPE_TORTURE + PREG_OFFSET].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_TORTURE + PREG_OFFSET].DrawImage(x, y, width, height, random, img);
			else if (preg && girl->m_GirlImages->m_Images[IMGTYPE_BDSM + PREG_OFFSET].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_BDSM + PREG_OFFSET].DrawImage(x, y, width, height, random, img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_TORTURE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_TORTURE].DrawImage(x, y, width, height, random, img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_BDSM].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_BDSM].DrawImage(x, y, width, height, random, img);
			else
				ImgType = IMGTYPE_PROFILE;		// Try this next loop
			break;

		case IMGTYPE_ANAL:
		case IMGTYPE_BDSM:
		case IMGTYPE_BEAST:
		case IMGTYPE_GROUP:
		case IMGTYPE_LESBIAN:
			//				Similar pregnant/non-pregnant 'success' condition and action; uses 'pregnancy offset'
			if (preg && girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType + PREG_OFFSET].DrawImage(x, y, width, height, random, img);
			else if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].DrawImage(x, y, width, height, random, img);
			else
				ImgType = IMGTYPE_SEX;		// Try this next loop
			break;

		case IMGTYPE_PROFILE:
			if (preg && girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].m_NumImages)
			{
				if (girl->m_newRandomFixed >= 0)
					random = true;
				return girl->m_GirlImages->m_Images[IMGTYPE_PREGNANT].DrawImage(x, y, width, height, random, img);
			}
			else if (girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_PROFILE].DrawImage(x, y, width, height, random, img);
			else
				//							Use default images, avoid endless loop
				return m_DefImages->m_Images[IMGTYPE_PROFILE].DrawImage(x, y, width, height, random, img);
			break;

		case IMGTYPE_PREGNANT:
		case IMGTYPE_DEATH:
			if (girl->m_GirlImages->m_Images[ImgType].m_NumImages == 0)
				return m_DefImages->m_Images[ImgType].DrawImage(x, y, width, height, random, img);
			else
				return girl->m_GirlImages->m_Images[ImgType].DrawImage(x, y, width, height, random, img);
			break;

		case IMGTYPE_SEX:
			if (preg && girl->m_GirlImages->m_Images[IMGTYPE_PREGSEX].m_NumImages)
				return girl->m_GirlImages->m_Images[IMGTYPE_PREGSEX].DrawImage(x, y, width, height, random, img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_SEX].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_SEX].DrawImage(x, y, width, height, random, img);
			else
				return m_DefImages->m_Images[IMGTYPE_SEX].DrawImage(x, y, width, height, random, img);
			break;

		case IMGTYPE_COMBAT:
		case IMGTYPE_MAID:
		case IMGTYPE_SING:
		case IMGTYPE_WAIT:
		case IMGTYPE_CARD:
		case IMGTYPE_BUNNY:
		case IMGTYPE_MILK:
			//				Similar'success' condition and action; 
			if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].DrawImage(x, y, width, height, random, img);
			else
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_ORAL:
		case IMGTYPE_ECCHI:
		case IMGTYPE_STRIP:
		case IMGTYPE_NUDE:
		case IMGTYPE_MAST:
		case IMGTYPE_TITTY:
		case IMGTYPE_HAND:
			//				Similar'success' condition and action  (but no alternative ImgType set); 
			if (girl->m_GirlImages->m_Images[ImgType].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[ImgType].DrawImage(x, y, width, height, random, img);
			break;

		default:
			//error!
			break;

			//				And many conditions return early
		}

		//		If not returned to calling module already, have failed to find ImgType images.
		//              If have not already, test a substitute image type that has/may have images,
		//					substitute ImgType and leave processing for nexp loop if simpler.
		//			(First switch testing success, 2nd setting replacement ImgType replaces complicated 'if's.

		switch (ImgType)
		{
		case IMGTYPE_ORAL:
			ImgType = IMGTYPE_SEX;
			break;

		case IMGTYPE_ECCHI:
			if (girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].DrawImage(x, y, width, height, random, img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].DrawImage(x, y, width, height, random, img);
			else
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_STRIP:
			if (girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].DrawImage(x, y, width, height, random, img);
			else if (girl->m_GirlImages->m_Images[IMGTYPE_NUDE].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_NUDE].DrawImage(x, y, width, height, random, img);
			else
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_NUDE:
			if (girl->m_GirlImages->m_Images[IMGTYPE_STRIP].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_STRIP].DrawImage(x, y, width, height, random, img);
			else 			if (girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].m_NumImages > 0)
				return girl->m_GirlImages->m_Images[IMGTYPE_ECCHI].DrawImage(x, y, width, height, random, img);
			else
				ImgType = IMGTYPE_PROFILE;
			break;

		case IMGTYPE_MAST:
			ImgType = IMGTYPE_NUDE;
			break;

		case IMGTYPE_TITTY:
			ImgType = IMGTYPE_ORAL;
			break;

		case IMGTYPE_HAND:
			ImgType = IMGTYPE_ORAL;
			break;

		default:
			//		Not an error here, just that next ImgType should be already set
			break;
		}
	}

	return -1;		// failure to find & draw image
}

void sGirl::OutputGirlRow(string* Data, const vector<string>& columnNames)
{
	for (unsigned int x = 0; x < columnNames.size(); ++x)
	{
		//for each column, write out the statistic that goes in it
		OutputGirlDetailString(Data[x], columnNames[x]);
	}
}

void sGirl::OutputGirlDetailString(string& Data, const string& detailName)
{
	//given a statistic name, set a string to a value that represents that statistic
	static stringstream ss;
	ss.str("");

	bool interrupted = false;	// `J` added
	if ((this) && (this)->m_YesterDayJob != (this)->m_DayJob &&
		(g_Clinic.is_Surgery_Job((this)->m_YesterDayJob) || (this)->m_YesterDayJob == JOB_REHAB) &&
		(((this)->m_WorkingDay > 0) || (this)->m_PrevWorkingDay > 0))
		interrupted = true;

	if (detailName == "Name")					{ ss << m_Realname; }
	else if (detailName == "Health")
	{
		if (get_stat(STAT_HEALTH) == 0)
			ss << gettext("DEAD");
		else
			ss << get_stat(STAT_HEALTH) << "%";
	}
	else if (detailName == "Age")
	{
		if (get_stat(STAT_AGE) == 100)
		{
			ss << gettext("???");
		}
		else
		{
			ss << get_stat(STAT_AGE);
		}
	}
	else if (detailName == "Libido")			{ ss << libido(); }
	else if (detailName == "Rebel")				{ ss << rebel(); }
	else if (detailName == "Looks")				{ ss << ((get_stat(STAT_BEAUTY) + get_stat(STAT_CHARISMA)) / 2) << "%"; }
	else if (detailName == "Tiredness")			{ ss << get_stat(STAT_TIREDNESS) << "%"; }
	else if (detailName == "Happiness")			{ ss << get_stat(STAT_HAPPINESS) << "%"; }
	else if (detailName == "Virgin")			{ ss << (g_Girls.CheckVirginity(this) ? gettext("Yes") : gettext("No")); }
	else if (detailName == "Weeks_Due")
	{
		if (is_pregnant())
		{
			cConfig cfg;
			int to_go = cfg.pregnancy.weeks_pregnant() - m_WeeksPreg;
			ss << to_go;
		}
		else
		{
			ss << gettext("---");
		}
	}
	else if (detailName == "PregCooldown")		{ ss << (int)m_PregCooldown; }
	else if (detailName == "Accommodation")
	{
		/* */if (m_AccLevel == 0)	ss << gettext("Very Poor");
		else if (m_AccLevel == 1)	ss << gettext("Adequate");
		else if (m_AccLevel == 2)	ss << gettext("Nice");
		else if (m_AccLevel == 3)	ss << gettext("Good");
		else if (m_AccLevel == 4)	ss << gettext("Wonderful");
		else if (m_AccLevel == 5)	ss << gettext("High Class");
		else /*                */	ss << gettext("Error");
	}
	else if (detailName == "Gold")
	{
		if (g_Gangs.GetGangOnMission(MISS_SPYGIRLS))
		{
			ss << m_Money;
		}
		else
		{
			ss << gettext("???");
		}
	}
	else if (detailName == "Pay")				{ ss << m_Pay; }
	
	// 'J' Girl Table job text
	else if (detailName == "DayJob" || detailName == "NightJob")
	{
		int DN_Job = m_DayJob;
		bool DN_Day = 1;
		if (detailName == "NightJob")
		{
			DN_Job = m_NightJob;
			DN_Day = 0;
		}
		if (DN_Job >= NUM_JOBS)
		{
			ss << gettext("None");
		}
		else if (DN_Job == JOB_REHAB)
		{
			if (g_Centre.GetNumGirlsOnJob(0, JOB_DRUGCOUNSELOR, DN_Day) > 0)
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (" << 3 - m_WorkingDay << ")";
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (?)***";
			}
		}
		else if (DN_Job == JOB_GETHEALING)
		{
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, DN_Day) > 0)
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job];
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " ***";
			}
		}
		else if (DN_Job == JOB_GETREPAIRS)
		{
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_MECHANIC, DN_Day) > 0 &&
				(g_Girls.HasTrait(this, "Construct") || g_Girls.HasTrait(this, "Half-Construct")))
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job];
			}
			else if (g_Girls.HasTrait(this, "Construct"))
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " ****";
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " !!";
			}
		}
		else if (DN_Job == JOB_GETABORT)
		{
			int wdays = (2 - (this)->m_WorkingDay);
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, DN_Day) > 0)
			{
				wdays = 1;
			}
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, DN_Day) > 0)
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (" << wdays << ")*";
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (?)***";
			}
		}
		else if (g_Clinic.is_Surgery_Job(DN_Job))
		{
			int wdays = (5 - (this)->m_WorkingDay);
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, DN_Day) > 0)
			{
				if (wdays >= 3)		{ wdays = 3; }
				else if (wdays > 1)	{ wdays = 2; }
				else				{ wdays = 1; }
			}
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, DN_Day) > 0)
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (" << wdays << ")*";
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (?)***";
			}
		}
		else if (g_Studios.is_Actress_Job(DN_Job) && g_Studios.CrewNeeded())
		{
			ss << g_Brothels.m_JobManager.JobName[DN_Job] << " **";
		}
		else
		{
			ss << g_Brothels.m_JobManager.JobName[DN_Job];
		}
		if (interrupted)
		{
			ss << " **";
		}
	}




	else if (detailName.find("STAT_") != string::npos)
	{
		string stat = detailName;
		stat.replace(0, 5, "");
		int code = sGirl::lookup_stat_code(stat);
		if (code != -1)
		{
			ss << get_stat(code);
		}
		else
		{
			ss << gettext("Error");
		}
	}
	else if (detailName.find("SKILL_") != string::npos)
	{
		string skill = detailName;
		skill.replace(0, 6, "");
		int code = sGirl::lookup_skill_code(skill);
		if (code != -1)
		{
			ss << get_skill(code);
		}
		else
		{
			ss << gettext("Error");
		}
	}
	else if (detailName.find("STATUS_") != string::npos)
	{
		string status = detailName;
		status.replace(0, 7, "");
		int code = lookup_status_code(status);
		if (code != -1)
		{
			if (m_States&(1 << code))
			{
				ss << gettext("Yes");
			}
			else
			{
				ss << gettext("No");
			}
		}
		else
		{
			ss << gettext("Error");
		}
	}
	else if (detailName == "is_pregnant")
	{
		if (is_pregnant())
		{
			cConfig cfg;
			int to_go = cfg.pregnancy.weeks_pregnant() - m_WeeksPreg;
			ss << gettext("Yes") << "(" << to_go << ")";
		}
		else
		{
			if (m_PregCooldown == 0)
				ss << gettext("No");
			else
				ss << gettext("No") << "(" << (int)m_PregCooldown << ")";
		}
	}
	else if (detailName == "is_slave")			{ ss << (is_slave() ? gettext("Yes") : gettext("No")); }
	else if (detailName == "carrying_human")	{ ss << (carrying_human() ? gettext("Yes") : gettext("No")); }
	else if (detailName == "is_addict")			{ ss << (is_addict() ? gettext("Yes") : gettext("No")); }
	else if (detailName == "has_disease")		{ ss << (has_disease() ? gettext("Yes") : gettext("No")); }
	else if (detailName == "is_mother")			{ ss << (is_mother() ? gettext("Yes") : gettext("No")); }
	else if (detailName == "is_poisoned")		{ ss << (is_poisoned() ? gettext("Yes") : gettext("No")); }
	else /*                            */		{ ss << gettext("Not found"); }
	Data = ss.str();
}

int sGirl::rebel()
{
	// return g_Girls.GetRebelValue(this, this->m_DayJob == JOB_MATRON); // `J` old version
	if (this->m_DayJob == JOB_INDUNGEON)	// `J` Dungeon "Matron" can be a Torturer from any brothel
		return g_Girls.GetRebelValue(this, (
		g_Brothels.GetNumGirlsOnJob(0, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(1, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(2, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(3, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(4, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(5, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(6, JOB_TORTURER, 0) > 0));
	else if (this->m_InMovieStudio)		return g_Girls.GetRebelValue(this, g_Studios.GetNumGirlsOnJob(0, JOB_DIRECTOR, 1) > 0);
	else if (this->m_InArena)			return g_Girls.GetRebelValue(this, g_Arena.GetNumGirlsOnJob(0, JOB_DOCTORE, 0) > 0);
	else if (this->m_InCentre)			return g_Girls.GetRebelValue(this, g_Centre.GetNumGirlsOnJob(0, JOB_CENTREMANAGER, 0) > 0);
	else if (this->m_InClinic)			return g_Girls.GetRebelValue(this, g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, 0) > 0);
	else if (this->m_InHouse)			return g_Girls.GetRebelValue(this, g_House.GetNumGirlsOnJob(0, JOB_HEADGIRL, 0) > 0);
	else if (this->m_InFarm)			return g_Girls.GetRebelValue(this, g_Farm.GetNumGirlsOnJob(0, JOB_FARMMANGER, 0) > 0);
	else								return g_Girls.GetRebelValue(this, g_Brothels.GetNumGirlsOnJob(this->where_is_she, JOB_MATRON, 0) > 0);
}

string sGirl::JobRatingLetter(int value)
{
	/* */if (value < -500)		return "X";	// Can not do this job
	else if (value >= 350)		return "   I   ";	// Incomparable
	else if (value >= 245)		return "  S  ";	// Superior
	else if (value >= 185)		return " A ";	// Amazing
	else if (value >= 145)		return "B"; // Better
	else if (value >= 100)		return "C";	// Can do it
	else if (value >= 70)		return "D";	// Don't bother
	else						return "E"; // Expect Failure
}