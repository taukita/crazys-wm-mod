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
#include "cJobManager.h"
#include "cBrothel.h"
#include "cClinic.h"
#include "cMovieStudio.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Job Movie Studio - Actress
bool cJobManager::WorkFilmAnal(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	cConfig cfg;
	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50;
	int enjoy = 0;
	int finalqual = 0;
	int jobperformance = 0;

	g_Girls.UnequipCombat(girl);	// not for actress (yet)

	ss << girlName << " worked as an actress filming anal scenes.\n\n";

	int roll = g_Dice.d100();
	if (roll <= 10 && (g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel) || g_Girls.DisobeyCheck(girl, ACTION_SEX, brothel)))
	{
		ss << "She refused to do anal on film today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (roll <= 10) { enjoy -= g_Dice % 3 + 1;	ss << "She didn't like having a cock up her ass today.\n\n"; }
	else if (roll >= 90) { enjoy += g_Dice % 3 + 1;	ss << "She loved having her ass pounded today.\n\n"; }
	else /*            */{ enjoy += g_Dice % 2;		ss << "She had a pleasant day letting her co-star slip his cock into her butt.\n\n";
	}
	jobperformance = enjoy * 2;

	if (g_Girls.CheckVirginity(girl))
	{
		jobperformance += 20;
		ss << "She is a virgin.\n";
	}

	// remaining modifiers are in the AddScene function --PP
	finalqual = g_Studios.AddScene(girl, SKILL_ANAL, jobperformance);
	ss << "Her scene is valued at: " << finalqual << " gold.";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_ANAL, Day0Night1);

	// work out the pay between the house and the girl
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket())
	{
		wages = 0;	// You own her so you don't have to pay her.
	}
	else
	{
		wages += finalqual * 2;
	}
	girl->m_Pay = wages;

	// Improve stats
	int xp = 10, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_ANAL, g_Dice%skill + 1);

	g_Girls.UpdateEnjoyment(girl, ACTION_SEX, enjoy);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy);
	g_Girls.PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);

	return false;
}

double cJobManager::JP_FilmAnal(sGirl* girl, bool estimate)// not used
{
	return 0;
}