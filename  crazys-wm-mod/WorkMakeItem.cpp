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
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cFarm.h"


extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;




// `J` Farm Job - Producers
bool cJobManager::WorkMakeItem(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	string message = "";
	if (Preprocessing(ACTION_WORKCENTRE, girl, brothel, Day0Night1, summary, message))
		return true;

	int jobperformance = (g_Girls.GetSkill(girl, SKILL_CRAFTING) +
		g_Girls.GetSkill(girl, SKILL_SERVICE));



	// TODO need better dialog
	if (g_Dice % 100 <= 10)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCENTRE, -1, true);
		message = " She wasn't able to make anything.";
		girl->m_Events.AddMessage(message, IMGTYPE_CRAFT, Day0Night1);
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCENTRE, +3, true);
		message = " She enjoyed her time working and made two items.";
		girl->m_Events.AddMessage(message, IMGTYPE_CRAFT, Day0Night1);
		g_Brothels.add_to_goods(2);
	}

	// Improve girl
	int xp = 10, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	girl->m_Pay += 25;
	g_Gold.staff_wages(25);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_CRAFTING, g_Dice%skill + 1);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}