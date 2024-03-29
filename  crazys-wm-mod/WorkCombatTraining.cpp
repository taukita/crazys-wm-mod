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
#include "cArena.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cBrothelManager g_Brothels;
extern cArenaManager g_Arena;
extern cMessageQue g_MessageQue;

// `J` Job Arena - Fighting - Learning_Job - Combat_Job
bool cJobManager::WorkCombatTraining(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_COMBAT;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (girl->m_Skills[SKILL_COMBAT] + girl->m_Skills[SKILL_MAGIC] + girl->m_Stats[STAT_AGILITY] +
		girl->m_Stats[STAT_CONSTITUTION] + girl->m_Stats[STAT_STRENGTH] >= 500)
	{
		ss << "There is nothing more she can learn here so she takes the rest of the day off.";
		girl->m_NightJob = girl->m_DayJob = JOB_ARENAREST;
		return false;	// not refusing
	}
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " trains for combat.\n\n";

	g_Girls.EquipCombat(girl);	// Ready for combat training
	cConfig cfg;
	int enjoy = 0;												// 
	int wages = 0;												// 
	int train = 0;												// main skill trained
	int tcom = girl->m_Skills[SKILL_COMBAT];					// Starting level - train = 1
	int tmag = girl->m_Skills[SKILL_MAGIC];						// Starting level - train = 2
	int tagi = girl->m_Stats[STAT_AGILITY];						// Starting level - train = 3
	int tcon = girl->m_Stats[STAT_CONSTITUTION];				// Starting level - train = 4
	int tstr = girl->m_Stats[STAT_STRENGTH];					// Starting level - train = 5
	bool gaintrait = false;										// posibility of gaining a trait
	int skill = 0;												// gian for main skill trained
	int dirtyloss = brothel->m_Filthiness / 100;				// training time wasted with bad equipment
	int sgCmb = 0, sgMag = 0, sgAgi = 0, sgCns = 0, sgStr = 0;	// gains per skill
	int roll_a = g_Dice.d100();									// roll for main skill gain
	int roll_b = g_Dice.d100();									// roll for main skill trained
	int roll_c = g_Dice.d100();									// roll for enjoyment



	/* */if (roll_a <= 5)	skill = 7;
	else if (roll_a <= 15)	skill = 6;
	else if (roll_a <= 30)	skill = 5;
	else if (roll_a <= 60)	skill = 4;
	else /*             */	skill = 3;
	/* */if (g_Girls.HasTrait(girl, "Quick Learner"))	{ skill += 1; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; }
	skill -= dirtyloss;
	ss << "The Arena is ";
	if (dirtyloss <= 0) ss << "clean and tidy";
	if (dirtyloss == 1) ss << "dirty and the equipment has not been put back in its place";
	if (dirtyloss == 2) ss << "messy. The equipment is damaged and strewn about the building";
	if (dirtyloss == 3) ss << "filthy and some of the equipment is broken";
	if (dirtyloss >= 4) ss << "in complete disarray and the equipment barely usable";
	ss << ".\n\n";
	if (skill < 1) skill = 1;	// always at least 1 

	do{		// `J` New method of selecting what job to do
		/* */if (roll_b < 20  && tcom < 100)	train = 1;	// combat
		else if (roll_b < 40  && tmag < 100)	train = 2;	// magic
		else if (roll_b < 60  && tagi < 100)	train = 3;	// agility
		else if (roll_b < 80  && tcon < 100)	train = 4;	// constitution
		else if (roll_b < 100 && tstr < 100)	train = 5;	// strength
		roll_b -= 10;
	} while (train == 0 && roll_b > 0);
	if (train == 0 || g_Dice.percent(5)) gaintrait = true;

	if (train == 1) { sgCmb = skill; ss << "She trains in general combat techniques.\n"; }	else sgCmb = g_Dice % 2;
	if (train == 2) { sgMag = skill; ss << "She trains to control her magic.\n"; }			else sgMag = g_Dice % 2;
	if (train == 3) { sgAgi = skill; ss << "She trains her agility.\n"; }					else sgAgi = g_Dice % 2;
	if (train == 4) { sgCns = skill; ss << "She works on her constitution.\n"; }			else sgCns = g_Dice % 2;
	if (train == 5) { sgStr = skill; ss << "She lifts weights to get stronger.\n"; }		else sgStr = g_Dice % 2;

	if (sgCmb + sgMag + sgAgi + sgCns + sgStr > 0)
	{
		ss << "She managed to gain:\n";
		if (sgCmb > 0) { ss << sgCmb << " Combat\n";		g_Girls.UpdateSkill(girl, SKILL_COMBAT, sgCmb); }
		if (sgMag > 0) { ss << sgMag << " Magic\n";			g_Girls.UpdateSkill(girl, SKILL_MAGIC, sgMag); }
		if (sgAgi > 0) { ss << sgAgi << " Agility\n";		g_Girls.UpdateStat(girl, STAT_AGILITY, sgAgi); }
		if (sgCns > 0) { ss << sgCns << " Constitution\n";	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, sgCns); }
		if (sgStr > 0) { ss << sgStr << " Strength\n";		g_Girls.UpdateStat(girl, STAT_STRENGTH, sgStr); }
	}

	int trycount = 20;
	while (gaintrait && trycount > 0)	// `J` Try to add a trait 
	{
		trycount--;
		switch (g_Dice % 10)
		{
		case 0:
			if (g_Girls.HasTrait(girl, "Fragile"))
			{
				g_Girls.RemoveTrait(girl, "Fragile");
				ss << "She has had to heal from so many injuries you can't say she is fragile anymore.";
				gaintrait = false;
			}
			else if (!g_Girls.HasTrait(girl, "Tough"))
			{
				g_Girls.AddTrait(girl, "Tough");
				ss << "She has become pretty Tough from her training.";
				gaintrait = false;
			}
			break;
		case 1:
			if (!g_Girls.HasTrait(girl, "Adventurer"))
			{
				g_Girls.AddTrait(girl, "Adventurer");
				ss << "She has been in enough tough spots to consider herself an Adventurer.";
				gaintrait = false;
			}
			break;
		case 2:
			if (g_Girls.HasTrait(girl, "Nervous") || g_Girls.HasTrait(girl, "Meek") || g_Girls.HasTrait(girl, "Dependant"))
			{
				if (g_Dice.percent(50) && g_Girls.HasTrait(girl, "Nervous"))
				{
					g_Girls.RemoveTrait(girl, "Nervous");
					ss << "She seems to be getting over her Nervousness with her training.";
					gaintrait = false;
				}
				else if (g_Dice.percent(50) && g_Girls.HasTrait(girl, "Meek"))
				{
					g_Girls.RemoveTrait(girl, "Meek");
					ss << "She seems to be getting over her Meakness with her training.";
					gaintrait = false;
				}
				else if (g_Dice.percent(50) && g_Girls.HasTrait(girl, "Dependant"))
				{
					g_Girls.RemoveTrait(girl, "Dependant");
					ss << "She seems to be getting over her Dependancy with her training.";
					gaintrait = false;
				}
			}
			else
			{
				if (g_Dice.percent(50) && !g_Girls.HasTrait(girl, "Aggressive"))
				{
					g_Girls.AddTrait(girl, "Aggressive");
					ss << "She is getting rather Aggressive from her enjoyment of combat.";
					gaintrait = false;
				}
				else if (g_Dice.percent(50) && !g_Girls.HasTrait(girl, "Fearless"))
				{
					g_Girls.AddTrait(girl, "Fearless");
					ss << "She is getting rather Fearless from her enjoyment of combat.";
					gaintrait = false;
				}
				else if (g_Dice.percent(50) && !g_Girls.HasTrait(girl, "Audacity"))
				{
					g_Girls.AddTrait(girl, "Audacity");
					ss << "She is getting rather Audacious from her enjoyment of combat.";
					gaintrait = false;
				}
			}
			break;
		case 3:
			if (!g_Girls.HasTrait(girl, "Strong"))
			{
				g_Girls.AddTrait(girl, "Strong");
				ss << "She is getting rather Strong from handling heavy weapons and armor.";
				gaintrait = false;
			}
			break;
		case 4:
			break;
		case 5:

			break;
		case 6:

			break;

		default:	break;	// no trait gained
		}
	}


		/*

		Small Scars
		Cool Scars
		Horrific Scars
		Bruises
		Idol
		Agile
		Fleet of Foot
		Clumsy
		Strong
		Merciless
		Delicate
		Brawler
		Assassin
		Masochist
		Sadistic
		Tsundere
		Twisted
		Yandere


		Missing Nipple

		Muggle
		Weak Magic
		Strong Magic
		Powerful Magic

		Broken Will
		Iron Will

		Eye Patch
		One Eye

		Shy
		Missing Teeth


		No Arms
		One Arm
		No Hands
		One Hand
		Missing Finger
		Missing Fingers

		No Feet
		No Legs
		One Foot
		One Leg
		Missing Toe
		Missing Toes


		Muscular
		Plump
		Great Figure



		*/



	//enjoyed the work or not
	/* */if (roll_c <= 10)	{ enjoy -= g_Dice % 3 + 1;	ss << "She did not enjoy her time training."; }
	else if (roll_c >= 90)	{ enjoy += g_Dice % 3 + 1;	ss << "She had a pleasant time training."; }
	else /*             */	{ enjoy += g_Dice % 2;		ss << "Otherwise, the shift passed uneventfully."; }
	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
	
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1);
	brothel->m_Filthiness += 2;	// fighting is dirty
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket())) { wages = 0; }
	else { wages = 25 + (skill * 5); } // `J` Pay her more if she learns more
	girl->m_Pay = wages;

	// Improve stats
	int xp = 5 + skill, libido = int(1 + skill / 2);

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ xp += 2; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ xp -= 2; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_CombatTraining(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance +=
			(100 - girl->m_Skills[SKILL_COMBAT]) +
			(100 - girl->m_Skills[SKILL_MAGIC]) +
			(100 - girl->m_Stats[STAT_AGILITY]) +
			(100 - girl->m_Stats[STAT_CONSTITUTION]) +
			(100 - girl->m_Stats[STAT_STRENGTH]);
	}
	else// for the actual check
	{
	}
	return jobperformance;
}
