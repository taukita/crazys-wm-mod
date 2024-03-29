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
extern cInventory g_InvManager;

// `J` Job Farm - Laborers
bool cJobManager::WorkGardener(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKFARM;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a gardener on the farm.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	cConfig cfg;
	int enjoy = 0;
	int wages = 25;
	int tips = 0;
	int imagetype = IMGTYPE_PROFILE;
	int msgtype = Day0Night1;

	double jobperformance = JP_Gardener(girl, false);
	if (jobperformance >= 245)
	{
		ss << " She must be the perfect at this.";
		wages += 155;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this.";
		wages += 95;
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job.";
		wages += 55;
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.";
		wages += 15;
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.";
		wages -= 5;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
		wages -= 15;
	}
	ss << "\n\n";

	int roll = g_Dice.d100();

	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "Some of the patrons abused her during the shift.";
		enjoy -= 1;
	}
	else if (roll <= 25)
	{
		ss << "She had a pleasant time working.";
		enjoy += 3;
	}
	else
	{
		ss << "Otherwise, the shift passed uneventfully.";
		enjoy += 1;
	}


	// `J` Farm Bookmark - adding in items that can be gathered in the farm
#if 1

	int flowerpower = g_Dice % 3;
	/* */if (jobperformance < 70)	flowerpower -= 1;
	else if (jobperformance < 100)	flowerpower += 0;
	else if (jobperformance < 145)	flowerpower += 1;
	else if (jobperformance < 185)	flowerpower += 2;
	else if (jobperformance < 245)	flowerpower += 3;
	else /*                     */	flowerpower += 4;

	string additems[8] = { "", "", "", "", "", "", "", "" };
	int additemnum = 0;
	while (flowerpower > 0 && additemnum < 8)
	{
		string additem = "";
		switch (g_Dice % 14)
		{
		case 0:		if (flowerpower >= 5) { flowerpower -= 5;	additem = "Bouquet of Enchanted Roses"; } break;
		case 1:		if (flowerpower >= 5) { flowerpower -= 5;	additem = "Chatty Flowers"; }			  break;
		case 2:		if (flowerpower >= 4) { flowerpower -= 4;	additem = "Watermelon of Knowledge"; }	  break;
		case 3:		if (flowerpower >= 3) { flowerpower -= 3;	additem = "Mango of Knowledge"; }		  break;
		case 4:		if (flowerpower >= 3) { flowerpower -= 3;	additem = "Red Rose Extravaganza"; }	  break;
		case 5:		if (flowerpower >= 3) { flowerpower -= 3;	additem = "Vira Blood"; }				  break;
		case 6:		if (flowerpower >= 3) { flowerpower -= 3;	additem = "Whitewillow Sap"; }			  break;
		case 7:		if (flowerpower >= 2) { flowerpower -= 2;	additem = "Rose of Enchantment"; }		  break;
		case 8:		if (flowerpower >= 2) { flowerpower -= 2;	additem = "Sinspice"; }					  break;
		case 9:		if (flowerpower >= 2) { flowerpower -= 2;	additem = "Nut of Knowledge"; }			  break;
		case 10:	if (flowerpower >= 2) { flowerpower -= 2;	additem = "Willbreaker Spice"; }		  break;
		case 11:	if (flowerpower >= 1) { flowerpower -= 1;	additem = "Shroud Mushroom"; }			  break;
		case 12:	if (flowerpower >= 1) { flowerpower -= 1;	additem = "Wild Flowers"; }				  break;
		default:	flowerpower -= 1; break;
		}
		if (additem != "")
		{
			additems[additemnum] = additem;
			additemnum++;
		}
	}
	if (additemnum > 0)
	{
		msgtype = EVENT_GOODNEWS;
		ss << "\n\n" << girlName << " was able to harvest ";
		if (additemnum == 1)
		{
			ss << "one " << additems[0] << ".";
		}
		else
		{
			ss << additemnum << " items: ";
			for (int i = 0; i < additemnum; i++)
			{
				ss << additems[i];
				if (i == additemnum - 1) ss << ".";
				else ss << ", ";
			}
		}
		for (int i = 0; i < additemnum; i++)
		{
			sInventoryItem* item = g_InvManager.GetItem(additems[i]);
			if (item) g_Brothels.AddItemToInventory(item);
		}
	}






#endif


	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket()) wages = 0;    // You own her so you don't have to pay her.
	else
	{
		int roll_max = (g_Girls.GetStat(girl, STAT_INTELLIGENCE)
			+ g_Girls.GetStat(girl, SKILL_HERBALISM)
			+ g_Girls.GetSkill(girl, SKILL_FARMING));
		roll_max /= 6;
		wages += 10 + g_Dice%roll_max;
	}
	if (wages < 0) wages = 0;
	girl->m_Pay = wages;


	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	g_Girls.UpdateSkill(girl, SKILL_HERBALISM, (g_Dice%skill) + 2);
	// secondary (-1 for one then -2 for others)
	g_Girls.UpdateSkill(girl, SKILL_FARMING, max(0, (g_Dice % skill) - 1));
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, max(0, (g_Dice % skill) - 2));
	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, max(0, (g_Dice % skill) - 2));

	return false;
}


double cJobManager::JP_Gardener(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->herbalism() +
		// secondary - second 100
		((girl->farming() + girl->intelligence() + girl->constitution()) / 3) +
		// level bonus
		girl->level();

	//good traits
	if (g_Girls.HasTrait(girl, "Quick Learner"))  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		  jobperformance += 10;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy")) 		jobperformance -= 20; //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive")) 	jobperformance -= 20; //gets mad easy
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people	
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;


	return jobperformance;
}
