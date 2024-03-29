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
#include <sstream>
#include "cJobManager.h"
#include "cBrothel.h"

#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
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

extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Brothel - General - Matron_Job - Full_Time_Job
bool cJobManager::WorkMatron(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMATRON;
	girl->m_DayJob = girl->m_NightJob = JOB_MATRON;	// it is a full time job
	if (Day0Night1) return false;	// and is only checked once

	stringstream ss; string girlName = girl->m_Realname; ss << "Matron " << girlName;

	// `J` zzzzzz - this needs to be updated for building flow
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	cConfig cfg;

	int numgirls = brothel->m_NumGirls;
	int enjoy = 0;
	int conf = 0;
	int happy = 0;

	// Complications
	int check = g_Dice.d100();
	if (check < 10 && numgirls >(girl->service() + girl->confidence()) * 3)
	{
		enjoy -= (g_Dice % 6 + 5);
		conf -= 5; happy -= 10;
		ss << " was overwhelmed by the number of girls she was required to manage and broke down crying.";
	}
	else if (check < 10)
	{
		enjoy -= (g_Dice % 3 + 1);
		conf -= -1; happy -= -3;
		ss << " had trouble dealing with some of the girls.";
	}
	else if (check > 90)
	{
		enjoy += (g_Dice % 3 + 1);
		conf -= 1; happy -= 3;
		ss << " enjoyed helping the girls with their lives.";
	}
	else
	{
		enjoy += (g_Dice % 3 - 1);
		ss << " went about her day as usual.";
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	// Improve girl
	int xp = numgirls / 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 5; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			libido += 2;
	if (g_Girls.HasTrait(girl, "Lesbian"))				libido += numgirls / 20;

	girl->m_Pay = int(float(100.0 + (((girl->get_skill(SKILL_SERVICE) + girl->get_stat(STAT_CHARISMA) + girl->get_stat(STAT_INTELLIGENCE) + girl->get_stat(STAT_CONFIDENCE) + girl->get_skill(SKILL_MEDICINE) + 50) / 50)*numgirls) * cfg.out_fact.matron_wages()));

	if (conf>-1) conf += g_Dice%skill;
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, conf);
	g_Girls.UpdateStat(girl, STAT_HAPPINESS, happy);

	g_Girls.UpdateStat(girl, STAT_EXP, g_Dice%xp + 5);
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 2);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, g_Dice%libido);

	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 30, actiontype, "She has worked as a matron long enough that she has learned to be more Charismatic.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Psychic", 60, actiontype, "She has learned to handle the girls so well that you'd almost think she was Psychic.", Day0Night1);

	return false;
}

double cJobManager::JP_Matron(sGirl* girl, bool estimate)	// not used
{
	double jobperformance = 0.0;
	if (estimate)	// for third detail string
	{
		// `J` estimate - needs work
		jobperformance = ((girl->charisma() + girl->confidence() + girl->spirit()) / 3) +
			((girl->service() + girl->intelligence() + girl->medicine()) / 3) +
			girl->level();

		if (girl->is_slave()) jobperformance -= 1000;
	}
	else			// for the actual check		// not used
	{

	}

	return jobperformance;
}